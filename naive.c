#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RANDOMIZE_VA_SPACE "/proc/sys/kernel/randomize_va_space"

static void assert_no_aslr(void)
{
	FILE *fd;
	int err = 0;
	char randomize_va_space;

	fd = fopen(RANDOMIZE_VA_SPACE, "r");
	if (!fd) {
		printf("could not open " RANDOMIZE_VA_SPACE "\n");
		exit(1);
	}

	if (fread(&randomize_va_space, sizeof(char), 1, fd) != 1) {
		printf("could not read " RANDOMIZE_VA_SPACE  "\n");
		err = 1;
	}

	fclose(fd);

	if (randomize_va_space != '0') {
		if (!err) {
			printf(RANDOMIZE_VA_SPACE ": %c\n issue (as root):\n\n"
				"    echo 0 > " RANDOMIZE_VA_SPACE "\n\n",
				randomize_va_space);
		}
		exit(1);
	}
}

static void vulnerable_func(char *src)
{
	char dst[256];

	strcpy(dst, src);
	printf("Got: %s\n", dst);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("usage: ./naive <arg>\n");
		return -1;
	}

	assert_no_aslr();
	vulnerable_func(argv[1]);
	return 0;
}

