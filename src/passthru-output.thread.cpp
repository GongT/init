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

bool EpollContext::read()
{
	const ssize_t readed = ::read(source, buffer + position, INPUT_BUFFER_SIZE - position - 1);
	if (readed == 0)
	{
		// cerr << "(epoll) readed 0: " << this->prefix << endl;
		return false;
	}
	if (readed == -1)
	{
		// ERROR_LOG << "(epoll) error read: " << this->prefix << " errno:" << errno << ", errstr:" << strerror(errno) << endl;
		return false;
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
	return true;
}

void *processing_output(void *_ctx)
{
	cerr << "(epoll) read loop create." << endl;
	wait_input_t *ctx = (wait_input_t *)_ctx;
	auto running = ctx->running;
	auto epoll_fd = ctx->epoll_fd;

	struct epoll_event events[MAX_EVENTS];

	if (pthread_mutex_unlock(ctx->wait) != 0)
		die("failed unlock ptread mutex.");

	cerr << "(epoll) read loop start." << *running << endl
		 << endl
		 << endl;

	while (*running)
	{
		// cerr << "(epoll) polling for input..." << endl;
		const int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		// cerr << "(epoll) there are " << event_count << " ready events" << endl;
		for (int index = 0; index < event_count; index++)
		{
			auto p = ((EpollContext *)events[index].data.ptr);
			const bool r = p->read();
			if (!r)
			{
				// cerr << "(epoll) remove fd " << p->fd() << " by error (maybe it closed)." << endl;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, p->fd(), NULL) != 0)
				{
					ERROR_LOG("Warn: failed to remove epoll event '" << p->fd());
				}
			}
		}
	}
	cerr << "(epoll) read loop finished." << endl;
	return NULL;
}
