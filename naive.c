#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NORMAL "\033[1;0m"
#define HIGHLIGHT "\033[1;1m"
#define GREEN "\033[1;32m"

static void return_to_user(char *buf)
{
	/* ... */
}

static void process_buffer(char *buf)
{
	printf("processing data: %s%s%s\n", HIGHLIGHT, buf, NORMAL);
	sleep(1);
}

static void done(void)
{
	printf(GREEN "DONE" NORMAL "\n");
}

static void do_work(int num, char **bufs)
{
	char buffer[256];
	static int i;

	for (i = 1; i < num; i++) {
		/* copy buf[i] to internal buffer */
		strcpy(buffer, bufs[i]);

		/* return bus[i] to user */
		return_to_user(bufs[i]);

		/* process buffer data */
		process_buffer(buffer);
	}
}

int main(int argc, char **argv)
{
	do_work(argc, argv);
	done();

	return 0;
}

