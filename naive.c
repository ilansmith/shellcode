#include <unistd.h>

int main(int argc, char **argv)
{
	char *vars[2];

	vars[0] = "/bin/sh";
	vars[1] = NULL;
	execve(vars[0], vars, NULL);

	return 0;
}

