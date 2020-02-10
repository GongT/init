#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "debug.hpp"
#include "process-create.hpp"
#include "passthru-output.hpp"
#include "process-collection.hpp"

OutputCollector outputCollector;

OutputCollector::OutputCollector() : running(false), lock(new pthread_mutex_t)
{
}

OutputCollector::~OutputCollector()
{
	shutdown();
	delete lock;
}

void OutputCollector::shutdown()
{
	if (!running)
		return;
	running = false;
	pthread_cancel(output_processing_thread);
	pthread_join(output_processing_thread, NULL);
	std::cerr << "successful quit event loop." << std::endl;
}

void OutputCollector::startup()
{
	running = true;

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		die("Failed to create epoll file descriptor\n");

	if (pthread_mutex_init(lock, NULL) != 0)
		die("failed create mutex lock");

	if (pthread_mutex_lock(lock) != 0)
		die("fail lock pthread mutex");

	wait_input_t ctx{
		epoll_fd,
		&running,
		lock,
	};
	pthread_create(&output_processing_thread, NULL, processing_output, &ctx);

	if (pthread_mutex_lock(lock) != 0)
		die("fail lock pthread mutex");

	pthread_mutex_destroy(lock);
}
std::list<context_bundle_t>::iterator OutputCollector::findProcessHandle(int pid)
{
	for (auto itr = contextStore.begin(); itr != contextStore.end(); itr++)
	{
		if (itr->pid == pid)
		{
			return itr;
		}
	}
	return contextStore.end();
}

bool OutputCollector::disable(const ProcessHandle *process)
{
	auto ptr = findProcessHandle(process->pid());
	if (ptr == contextStore.end())
	{
		cerr << "Warn: disable() multiple call (process=" << process->pid() << ")." << endl;
		return false;
	}

	int err;
	if ((err = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, process->fd0(), NULL)) != 0)
	{
		cerr << "Warn: failed to remove epoll event 'fd0'. (" << err << ") " << strerror(err) << endl;
	}
	if ((err = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, process->fd1(), NULL)) != 0)
	{
		cerr << "Warn: failed to remove epoll event 'fd1'. (" << err << ") " << strerror(err) << endl;
	}

	delete ptr->context0;
	ptr->context0 = NULL;
	delete ptr->context1;
	ptr->context1 = NULL;

	bool rm = false;
	for (auto itr = contextStore.begin(); itr != contextStore.end(); itr++)
	{
		if (itr->pid == process->pid())
		{
			contextStore.erase(itr);
			rm = true;
			break;
		}
	}

	if (!rm)
		std::cerr << "Warn: context store miss-sync." << std::endl;

	return true;
}

EpollContext *OutputCollector::_enable(const int source, ostream &target, const string title)
{
	EpollContext *ctx = new EpollContext(source, target, title);

	struct epoll_event *event = new struct epoll_event;
	event->events = EPOLLIN;
	event->data.ptr = ctx;

	int err;
	if ((err = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, source, event)) == 0)
		return ctx;

	cerr << "Fatal: failed to add epoll event. (" << err << ") " << strerror(err) << endl;
	processCollection.killAll();
	return NULL;
}
bool OutputCollector::enable(const ProcessHandle *process)
{
	auto ptr = findProcessHandle(process->pid());
	if (ptr != contextStore.end())
	{
		cerr << "Warn: enable() multiple call (process=" << process->pid() << ")." << endl;
		return false;
	}
	auto c1 = _enable(process->fd0(), cout, process->source().title());
	if (c1 == NULL)
		return false;
	auto c2 = _enable(process->fd1(), cerr, process->source().title());
	if (c1 == NULL)
		return false;

	contextStore.push_back(context_bundle_t{process->pid(), c1, c2});

	return true;
}