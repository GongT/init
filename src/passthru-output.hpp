#pragma once

#include <sys/epoll.h>
#include "process-create.hpp"
#include "passthru-output.thread.hpp"

typedef struct ContextBundle
{
	int pid;
	EpollContext *context0;
	EpollContext *context1;
} context_bundle_t;

class OutputCollector
{
private:
	volatile bool running;
	pthread_mutex_t *lock;
	std::list<context_bundle_t> contextStore;
	int epoll_fd;

	pthread_t output_processing_thread;

	std::list<context_bundle_t>::iterator findProcessHandle(int pid);
	EpollContext *_enable(const int source, ostream &target, const string title);

public:
	OutputCollector();
	~OutputCollector();

	bool enable(const ProcessHandle *source);
	bool disable(const ProcessHandle *source);

	void startup();
	void shutdown();
};

extern OutputCollector outputCollector;
