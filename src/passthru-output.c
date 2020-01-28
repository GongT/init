#include <pthread.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/epoll.h>
#include "my-types.h"
#include "process-create.h"

int channel_in_fd = -1;
int epoll_fd;
pthread_t output_processing_thread;
volatile bool running = true;

#define BUFFER_SIZE 2048
#define MAX_EVENTS 8

typedef struct EPollContext
{
	int source;
	int target;
	char buffer[BUFFER_SIZE + 1];
	ssize_t position;
	const char *prefix;
	const size_t prefix_len;
} m_epoll_ctx_t;

typedef struct WaitInput
{
	int fd;
	pthread_mutex_t *wait;
} wait_input_t;

bool unwatch_process_output(const int source)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, source, NULL))
	{
		printf_stderr("Warn: failed to remove epoll event.");
		return false;
	}
	else
	{
		// printf_stderr("remove epoll event.\n");
		return true;
	}
}

bool watch_process_output(const char *prefix, const int source, const int target)
{
	struct epoll_event *event = malloc(sizeof(struct epoll_event));
	event->events = EPOLLIN;

	m_epoll_ctx_t r = {
		.source = source,
		.target = target,
		.buffer = "",
		.position = 0,
		.prefix = prefix,
		.prefix_len = prefix ? strlen(prefix) : 0,
	};

	event->data.ptr = ALLOC_STRUCT(r);
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, source, event))
	{
		printf_stderr("Fatal: failed to add epoll event.");
		kill_all_quit();
		return false;
	}
	return true;
}
void flush_write(const m_epoll_ctx_t *const r, const char *buffer, const size_t buff_size)
{
	if (r->prefix)
		write(r->target, r->prefix, r->prefix_len);
	write(r->target, buffer, buff_size);
	write(r->target, "\n", 1);
}

void buffer_read(m_epoll_ctx_t *r)
{
	const ssize_t readed = read(r->source, r->buffer + r->position, BUFFER_SIZE - r->position);
	if (readed == 0)
	{
		// unwatch_process_output(r->source);
		// printf_stderr("unwatch some ",r->prefix);
		return;
	}
	if (readed == -1)
	{
		printf_stderr("error read: %d %s", errno, strerror(errno));
		return;
	}

	r->position += readed;
	r->buffer[r->position] = '\0';
	// printf("(read) r->position = %d (readout=%s)\n", r->position, r->buffer + r->position);

	char *nl = r->buffer, *nextnl = strchr(nl, '\n');
	while (nextnl != NULL)
	{
		// printf_stderr("will flush_write: size=%d", nextnl - nl);
		flush_write(r, nl, nextnl - nl);

		nl = nextnl + 1;
		nextnl = strchr(nl, '\n');
	}

	if (nl != r->buffer)
	{
		// printf_stderr("position=%d -= %d (rem=%s)\n", r->position, (nl - r->buffer), r->buffer);
		r->position = r->position - (nl - r->buffer);
		if (r->position != 0)
			memmove(r->buffer, nl, r->position);
		r->buffer[r->position] = '\0';
		// printf_stderr("(calc) position=%d (rem=%s)\n", r->position, r->buffer);
		if (r->position > BUFFER_SIZE || r->position < 0)
			die("WTF");
	}
	else if (r->position == BUFFER_SIZE)
	{
		r->position = 0;
		flush_write(r, r->buffer, BUFFER_SIZE);
	}
}

void *processing_output(void *_ctx)
{
	wait_input_t *ctx = (wait_input_t *)_ctx;

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		die("Failed to create epoll file descriptor\n");

	struct epoll_event events[MAX_EVENTS];

	pthread_mutex_unlock(ctx->wait);
	while (running)
	{
		// printf("polling for input...\r");
		const int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		// printf("there are %d ready events\r", event_count);
		for (int index = 0; index < event_count; index++)
		{
			buffer_read(events[index].data.ptr);
		}
	}
	printf_stderr("read loop finished.\n");
	return NULL;
}

void shutdown_debug_output()
{
	running = false;
	// close(channel_in_fd);
	channel_in_fd = -1;
	pthread_cancel(output_processing_thread);
	pthread_join(output_processing_thread, NULL);
	printf("successful quit event loop.\n");
}

void create_debug_output()
{
	pthread_mutex_t lock;
	if (pthread_mutex_init(&lock, NULL) != 0)
		die("failed create mutex lock");

	wait_input_t ctx = {
		.wait = &lock,
	};

	pthread_mutex_lock(&lock);
	pthread_create(&output_processing_thread, NULL, processing_output, &ctx);

	pthread_mutex_lock(&lock);
	pthread_mutex_destroy(&lock);
}
