#include <iostream>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	printf("starting clangd\n");

	if(fork() == -1)
	{
		printf("Could not fork\n");
		return 1;
	}



	return 0;
}

