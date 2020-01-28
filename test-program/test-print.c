#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "require <string tag> <sleep ms> argument.\n");
		return 1;
	}
	uint32_t n = 0;
	sscanf(argv[2], "%u", &n);
	while (1)
	{
		printf("%s", argv[1]);
		for (char a = 0; a < 10; a++)
		{
			nanosleep(&(struct timespec){
						  .tv_sec = 0,
						  .tv_nsec = 10 * 1000 * 1000,
					  },
					  NULL);
			printf("%d", a);
			fflush(stdout);
		}
		printf("\n======\n------\n");
		nanosleep(&(struct timespec){
					  .tv_sec = 0,
					  .tv_nsec = n * 1000 * 1000,
				  },
				  NULL);
	}
}