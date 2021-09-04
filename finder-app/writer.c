/*
 *  References: 
 *  https://vi.stackexchange.com/questions/7975/how-can-i-change-the-indent-size
 *  https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp
 *  https://pubs.opengroup.org/onlinepubs/7908799/xsh/libgen.h.html
 *  https://codeforwin.org/2018/03/c-program-check-file-or-directory-exists-not.html
 *  https://pubs.opengroup.org/onlinepubs/009695299/functions/mkdir.html
 *  https://pubs.opengroup.org/onlinepubs/007908799/xsh/sysstat.h.html
 *  https://stackoverflow.com/questions/1658476/c-fopen-vs-open
 *  https://linux.die.net/man/3/syslog
 *  https://linux.die.net/man/8/syslogd
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>	    /* dirname,basename,mkdir */
#include <unistd.h>	    /* access() */
#include <sys/stat.h>	  /* directory ecistence check */
#include <errno.h>	    /* perror() */
#include <stdarg.h>     /* syslog functionality */
#include <syslog.h>     /* for syslog open/close functioanlity */

//function prototype
int checkdirpathexistence(const char* path);
int check_file_existence(const char *path);
int create_file_and_write(const char* path, const char* string_to_write, const char* file_name);

//Start of main program
int main(int argc,char *argv[])
{
  openlog(NULL,0,LOG_USER);
  
  //Check for required number of arguments
  if(argc != 3)
  {
    syslog(LOG_ERR,"ERROR: Invalid number of arguments\n");
    syslog(LOG_ERR,"Total number of arguments should be 2\n");
    syslog(LOG_ERR,"The order of the arguments should be:\n");
    syslog(LOG_ERR,"[./writer] [arg 1] [arg 2]\n");
    syslog(LOG_ERR,"  1)Name of the file along with the directory. Ex - /home/rishab/abcd.txt\n");
    syslog(LOG_ERR,"  2)String to be written inside the file mentioned in arguement 1 i.e. inside abcd.txt\n"); 
    exit(1);
  }

  syslog(LOG_DEBUG,"The string is argv[1] = %s argv[2] %s\n",argv[1],argv[2]);

  char* directory_path_rcvd = argv[1]; 
  char* string_to_be_written = argv[2];
   
  char file_creation_path_req[200];
  memset(file_creation_path_req,'\0',sizeof(file_creation_path_req));
  strcpy(file_creation_path_req,argv[1]);
  
  char* base_name_extracted = basename(directory_path_rcvd);
  char* dir_name_extracted = dirname(directory_path_rcvd);

  syslog(LOG_DEBUG,"BASE NAME :: %s\n",base_name_extracted);
  syslog(LOG_DEBUG,"DIR NAME :: %s\n",dir_name_extracted);
  
  //Check for directory existence and create one if it does not exist
  int ret;
  ret = checkdirpathexistence(dir_name_extracted);
  if(ret != 1)
  {
    syslog(LOG_ERR,"directory creation failed\n");
    perror("directory functionality:");
    exit(1);
  }
  
  //create file
  create_file_and_write(file_creation_path_req,string_to_be_written,base_name_extracted);
  
  //Check if file exist
  ret = check_file_existence(file_creation_path_req);
  if(ret == 0)
  {
    syslog(LOG_DEBUG,"file created successfully\n");
    printf("file created successfully\n");
  }
  
  closelog();
  
  return 0;
}

int checkdirpathexistence(const char* path)
{
  struct stat stats;

  //create file
  stat(path,&stats);

  if(S_ISDIR(stats.st_mode))
  {
    //directory exists
    return 1;
  }
  
  printf("inside - 4 is %s\n",path);
  
  int status;
  char command[10];
  memset(command,'\0',sizeof(command));
  
  char command_executed[300];
  memset(command_executed,'\0',sizeof(command_executed));

  strcpy(command,"mkdir -p ");
  strncpy(command_executed,command,strlen(command));
  strncat(command_executed,path,strlen(path));
  
  printf("final string is %s\n",command_executed);
  
  status = system(command_executed);
  if(status == 0)
  {
    syslog(LOG_DEBUG,"directory created successfully\n");
    printf("directory created successfully\n");
  }

  return 1;

}

int check_file_existence(const char *path)
{
  int status;
  status = access(path,F_OK);
  
  if(status != 0)
  {
    syslog(LOG_ERR,"file %s does not exist\n",path);
    perror("file existence error:");
    return 1;
  }

  return 0;
}


int create_file_and_write(const char* path, const char* string_to_write, const char* file_name)
{
  FILE* file_ptr;
  int ret_status;
  
  file_ptr = fopen(path,"w");
 
  if(!file_ptr)
  {
    syslog(LOG_ERR,"file creation,opening failure\n");
    perror("file creation:");
    exit(1);
  }

  ret_status = fwrite(string_to_write,sizeof(char),strlen(string_to_write),file_ptr);
  syslog(LOG_DEBUG,"Writing %s to %s\n",string_to_write,file_name);
 
  if(!ret_status)
  {
    syslog(LOG_ERR,"file writing failure\n");
    perror("file writing:");
    fclose(file_ptr);
    exit(1);
  }
  
  fclose(file_ptr);
  
  return 0;
}

/* EOF */
