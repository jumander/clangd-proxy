#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include<sys/wait.h>
#include<sys/prctl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

int main(int argc, char* argv[])
{
	printf("starting clangd\n");

	// Create pipes for parent and child process to communicate
	// read from 0 write to 1
	int pipeToProg[2];
	int pipeToProxy[2];

	if (pipe(pipeToProg))
	{
		printf("failed to create pipe to clangd\n");
		return 1;
	}
	if (pipe(pipeToProxy))
	{
		printf("failed to create pipe to proxy\n");
		return 1;
	}
	printf("pipes successfully created\n");



	pid_t pid = fork();
	if (pid == -1)
	{
		printf("Could not fork\n");
		return 1;
	}

	if (pid == 0)
	{
		printf("child here\n");
		dup2(pipeToProg[0], STDIN_FILENO);
		printf("first pipe redirected\n");
		dup2(pipeToProxy[1], STDOUT_FILENO);
		printf("second pipe redirected\n");
		dup2(pipeToProxy[1], STDERR_FILENO);
		printf("pipes redirected\n");

		// Send kill signal to child if parent dies
		prctl(PR_SET_PDEATHSIG, SIGTERM);
		printf("Sigterm control signal set\n");

		// Close unused ends
		close(pipeToProg[1]);
		close(pipeToProxy[0]);
		printf("close pipes\n");

		std::string message = "";
		int constexpr size = 4096;
		char buffer[size];
	
		printf("starting to read\n");
		while (!feof(STDIN_FILENO))
		{
			printf("try to read\n");
			fgets(buffer, size, STDIN_FILENO);
			message += buffer;
		}

		std::cout << "Message from proxy " << message << std::endl;


		return 0;
	}
	else 
	{
		// Close unused ends
		close(pipeToProg[0]);
		close(pipeToProxy[1]);

		sleep(1);
		printf("parent hello\n"); 
		std::string toChild = "Sending this clangd";
		write(pipeToProg[1], toChild.c_str(), toChild.size()); 

		sleep(2);

		printf("done\n");

		int status;
		kill(pid, SIGKILL); //send SIGKILL signal to the child process
		waitpid(pid, &status, 0);
	}



	return 0;
}

