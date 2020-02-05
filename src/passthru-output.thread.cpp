#include <sys/epoll.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "passthru-output.thread.hpp"
#include "debug.hpp"

using namespace std;

#define MAX_EVENTS 8

EpollContext::EpollContext(const int from_fd, ostream &outStream, const std::string title) : position(0), source(from_fd), target(outStream), prefix(title)
{
}
EpollContext::~EpollContext() {}

void EpollContext::read()
{
	const ssize_t readed = ::read(source, buffer + position, INPUT_BUFFER_SIZE - position - 1);
	if (readed == 0)
	{
		// unwatch_process_output(source);
		// cerr<< "unwatch ? "<<prefix<<endl;
		return;
	}
	if (readed == -1)
	{
		cerr << "error read: " << errno << " " << strerror(errno) << endl;
		return;
	}

	position += readed;
	buffer[position] = '\0';
	// cerr << "(read) position = " << position << " (readout=" << (buffer + position) << ")" << endl;

	char *nl = buffer, *nextnl = strchr(nl, '\n');
	while (nextnl != NULL)
	{
		// cerr << "will flush_write: size=" << (nextnl - nl) << endl;
		*nextnl = '\0';
		target << prefix << ' ' << nl << std::endl;

		nl = nextnl + 1;
		nextnl = strchr(nl, '\n');
	}

	if (nl != buffer)
	{
		// cerr << "position=" << position << " -= " << (nl - buffer) << " (rem=" << buffer << ")" << endl;
		position = position - (nl - buffer);
		if (position != 0)
			memmove(buffer, nl, position);
		buffer[position] = '\0';
		// cerr << "(calc) position=" << position << " (rem=" << buffer << ")" << endl;
		if (position > INPUT_BUFFER_SIZE || position < 0)
			die("WTF");
	}
	else if (position == INPUT_BUFFER_SIZE)
	{
		position = 0;
		target << prefix << ' ' << nl << std::endl;
	}
}

void *processing_output(void *_ctx)
{
	wait_input_t *ctx = (wait_input_t *)_ctx;

	struct epoll_event events[MAX_EVENTS];

	if (pthread_mutex_unlock(ctx->wait) != 0)
		die("failed unlock ptread mutex.");

	auto running = ctx->running;
	auto epoll_fd = ctx->epoll_fd;
	while (*running)
	{
		// printf("polling for input...\r");
		const int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		// printf("there are %d ready events\r", event_count);
		for (int index = 0; index < event_count; index++)
		{
			((EpollContext *)events[index].data.ptr)->read();
		}
	}
	cerr << "read loop finished." << endl;
	return NULL;
}
