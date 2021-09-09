#include "systemcalls.h"
#include <stdlib.h>	
#include <syslog.h>	
#include <sys/stat.h>	
#include <unistd.h>	//fork()
#include <sys/types.h>	//waitpid()
#include <sys/wait.h>	//waitpid()
#include <string.h>

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
    printf("COMMAND_STATUS is %s %d\n",cmd,status);
    if(status == 0)
    {
	syslog(LOG_DEBUG,"%s command executed successfully\n",cmd);
	return true;
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
#if 0
    printf("COUINT IS %d\n",count);
    printf("COMMAND 0 - IS %s\n",command[0]);
    printf("COMMAND 1 - IS %s\n",command[1]);
    printf("COMMAND 2 - IS %s\n",command[2]);
#endif
    int status;
    pid_t pid;
    pid = fork();

    //printf("PID VALUE IS %d\n",pid);
   
    if(pid == -1)
    {
	//printf("Does this execute?\n");
	return false;
    }
   
    char *argv[count];
    
    for(int i = 0; i < count;i++)
    {
	argv[i] = command[i+1];
    }

    int ret_status;
    //printf("what is this? %s\n",command[1]);
    ret_status = execv(command[0],argv);
    printf("ret_status %d\n",ret_status);
    
    if(ret_status == -1 )
    {
	printf("exec is failing\n");
 #if 0
    	if(waitpid(pid,&status,0) == -1)
    	{
	    printf("waitpid::");
	    return false;
	}
	else if(WIFEXITED(status))
	{
	    printf("status =%d\n",WEXITSTATUS(status));
	    syslog(LOG_DEBUG,"Normal termination with exit status = %d\n",WEXITSTATUS(status));
	    return WEXITSTATUS(status);
	}
	else
	{
	    printf("Am I here?\n");
	    /* DO nothing */
	}
	printf("here?");
#endif
	return false;
    }
   
#if 1

    if(waitpid(pid,&status,0) == -1)
    {
	printf("waitpid::");
	return false;
    }
    else if(WIFEXITED(status))
    {
	printf("status =%d\n",WEXITSTATUS(status));
	syslog(LOG_DEBUG,"Normal termination with exit status = %d\n",WEXITSTATUS(status));
	return WEXITSTATUS(status);
    }
    else
    {
	/* DO nothing */
    }
#endif

    va_end(args);

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
