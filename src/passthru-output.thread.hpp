#include <pthread.h>
#include <string>
#include <cstring>

#define INPUT_BUFFER_SIZE 2048

typedef struct WaitInput
{
	const int epoll_fd;
	volatile bool *running;
	pthread_mutex_t *wait;
} wait_input_t;

class EpollContext
{
protected:
	char buffer[INPUT_BUFFER_SIZE];
	ssize_t position;

	const int source;
	std::ostream &target;
	const std::string prefix;

public:
	EpollContext(const int from_fd, std::ostream &outStream, const std::string title);
	~EpollContext();

	bool read();
	int fd() { return source; };
};

void *processing_output(void *_ctx);
