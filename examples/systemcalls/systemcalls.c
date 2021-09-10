#include <stdlib.h>	
#include <syslog.h>	
#include <sys/stat.h>	
#include <unistd.h>	//fork()
#include <sys/types.h>	//waitpid()
#include <sys/wait.h>	//waitpid()
#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the commands in ... with arguments @param arguments were executed 
 *   successfully using the system() call, false if an error occurred, 
 *   either in invocation of the system() command, or if a non-zero return 
 *   value was returned by the command issued in @param.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success 
 *   or false() if it returned a failure
*/
    int status;
    status = system(cmd);

    //printf("COMMAND_STATUS is %s %d\n",cmd,status);

    if(status == 0)
    {
	syslog(LOG_DEBUG,"%s command executed successfully\n",cmd);
	return true;
    }
    else
    {
	//do wait and stuff to check the issues with the child
    }

    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the 
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    //command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *   
*/

    int ret_status = 0;
    int status;

    pid_t pid;
    pid = fork();
 
    if(pid == -1)
    {
	//Since, new process creation failed, the program is still
	//in parent context
	perror("fork");
	return false;
    }
    else if(pid == 0)
    {
	//Indicates that the child has been created succesfully
	//Now, we can proceed ahead to replace the address space of
	//the process for executing command
	printf("I am the child, my pid is %d\n",pid);
	//converting list into array for execv input
#if 0
	char *argv[count];
	for(int i = 0; i < count;i++)
      	{
	    argv[i] = command[i+1];
	}
#endif	
	ret_status = execv(command[0],&command[0]);
	
	if(ret_status == -1 )
	{
	    perror("execv");
	    return false;
	}
	else
	{
	    //ideally should not reach here
	    printf("reached here meaning: some issue with exec in child's context\n");
	    exit(-1);
	    //return false;
	}
    }
    else if(pid > 0)
    {
	printf("I am the parent in the main process context -- %d\n",pid);
	//put wait logic here
	//when execv is successful the child wont return till completion or might have returned
	//but parent is waiting for its exit status
#if 1
	if(waitpid(pid,&status,0) == -1)
	{
	    perror("waitpid");
	    return false;
	}

	if(WIFEXITED(status))
	{
	    //printf("or here -- %d\n\n\n\n\n\n",status);
	    //return false;
	    //return WEXITSTATUS(status);
	    if(WEXITSTATUS(status)!= 0)
	    {
		return false;
	    }
	}
#endif

	//printf("-----------ret_status = %d\n",ret_status);
#if 0
	pid = wait(&status);
	if(pid == -1)
	{
	    perror("wait");
	    return false;
	}

	printf("print in ths context of wait (parent) == %d\n",pid);

	if(WIFEXITED(status))
	{
	    printf("INHERE::ret_status = %d	",ret_status);
	    printf("Normal termination with exit status = %d\n",WEXITSTATUS(status));
	    return WEXITSTATUS(status);
	}

	if(WIFSIGNALED(status))
	{
	    printf("Killed by signal = %d%s\n",WTERMSIG(status),WCOREDUMP(status) ? " (Dumped core)" : "");
	}

	if(WIFSTOPPED(status))
	{
	    printf("Stopped by signal = %d\n",WSTOPSIG(status));
	}
	if(WIFCONTINUED(status))
	{
	    printf("Continued\n");
	}
#endif

    }
    else
    {
	printf("should not reach here - parent or child\n");
    }

    va_end(args);
    printf("Do you print????\n");
    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.  
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *   
*/

    va_end(args);
    
    return true;
}
