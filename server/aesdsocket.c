/* References:
 * https://www.geeksforgeeks.org/socket-programming-cc/
 * https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
 * https://www.tutorialkart.com/c-programming/c-delete-file/#:~:text=To%20delete%20a%20file%20using,returns%20a%20non%2Dzero%20value.
 * Beej
 * https://www.tutorialspoint.com/c_standard_library/c_function_strchr.htm
 * https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-sigprocmask-examine-change-thread
 * https://stackoverflow.com/questions/25261/set-and-oldset-in-sigprocmask
 * https://pubs.opengroup.org/onlinepubs/009604599/functions/stdin.html
 * https://www.freebsd.org/cgi/man.cgi?query=queue&apropos=0&sektion=0&manpath=FreeBSD+10.2-RELEASE&arch=default&format=html
 * Linux notes for reference
 * Author : Rishab Shah
 */

//standard headers required
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include <arpa/inet.h>  //inet_ntop
#include <fcntl.h>      //file operations
#include <signal.h>     //signal
#include <syslog.h>     //syslog

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>

#include <pthread.h>    //threads
#include <sys/queue.h>  //linked list to store thread id's

#define DAEMON_CODE           (1)
#define PORT_NO				        (9000)
#define PORT_BIND			        ("1234")
#define BACK_LOG			        (10)
#define BUFFER_CAPACITY       (100)
#define FILE_PATH_TO_WRITE    ("/var/tmp/aesdsocketdata")
#define MULTIPLIER_FACTOR     (2)


typedef struct
{
    //int threadIdx;
    int thread_accept_fd;
    bool thread_completion_status;
}threadParams_t;

struct slist_data_s
{
  threadParams_t thread_data;
  SLIST_ENTRY(slist_data_s) entries;
};

int file_des = 0;
int server_socket_fd = 0;
int client_accept_fd = 0;
sigset_t x;
int g_Signal_handler_detection = 0;

/* Function prototypes */
void *get_in_addr(struct sockaddr *sa);
void socket_termination_signal_handler(int signo);
void exit_handling();


/* Start of program */
int main(int argc, char *argv[])
{
  syslog(LOG_DEBUG,"-------------START OF PROGRAM-------------------");
#if DAEMON_CODE
  int set_daemon = 0;
  if(argc > 1)
  {
    if((strcmp(argv[1],"-d")) == 0)
    {
      set_daemon = 1;
    }
  }
#endif

  //daemon
  pid_t pid = 0;
  openlog(NULL, 0, LOG_USER);
   
  /* Signal handler */ // Why? Running in while loop client connection
  if(signal(SIGINT,socket_termination_signal_handler) == SIG_ERR)
  {
    perror("SIGINT registeration");
    return -1;
  }
  
  if(signal(SIGTERM,socket_termination_signal_handler) == SIG_ERR)
  {
    perror("SIGTERM registeration");
    return -1;
  }
  
  /* Add signals to be masked */
  int ret_sig_stat_1 = 0,ret_sig_stat_2 = 0,ret_sig_stat_3 = 0;
  
  ret_sig_stat_1 = sigemptyset(&x); 
  ret_sig_stat_2 = sigaddset(&x,SIGINT);
  ret_sig_stat_3 = sigaddset(&x,SIGTERM);
  if( (ret_sig_stat_1 == -1) || (ret_sig_stat_2 == -1) || (ret_sig_stat_3 == -1)  ) 
  {
    perror("sig signal set");
    return -1;
  }

  /* Socket */  
  server_socket_fd = socket(AF_INET,SOCK_STREAM,0);
  if(server_socket_fd == -1)
  {
    perror("server_socket_fd");  
    return -1; 
  }
    
  /* socketopt, Allows reuse */
  int server_setsockopt_fd = 0;
  int opt = 1;
    
  // Forcefully attaching socket to the port 8080
  server_setsockopt_fd = setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR  | SO_REUSEPORT,
                                        &opt, sizeof(opt));
  if(server_setsockopt_fd == -1)
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  
  /* Bind */
  int server_bind_fd = 0;
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(PORT_NO);
  
  server_bind_fd = bind(server_socket_fd,(struct sockaddr*)&server_address,sizeof(server_address));
  if(server_bind_fd == -1)
  {
    perror("server bind fd");
    return -1;
  }
  
 #if DAEMON_CODE 
  /* Daemon creation*/
  if(set_daemon == 1)
  {
    syslog(LOG_DEBUG,"daemon\n");
    pid = fork();
    if(pid == -1)
    {
      perror("fork failed");
      return -1;
    }
    else if(pid > 0)
    {
      //parent context
      syslog(LOG_DEBUG,"CHILD PID = %d\n",pid);
      exit(EXIT_SUCCESS);
    }
    
    /* create new session and process grp*/
    if(setsid()== -1)
    {
      perror("set sid failure");
      return -1;
    }
    
    //change cd to root
    if(chdir("/") == -1)
    {
      perror("chdir");
      return -1;
    }
    
    //close all open files (in,out,error)
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO); 

    open("/dev/null",O_RDWR);
    dup(0);
    dup(0);
  }
  #endif
  
  //In parent context and daemon mode
  //SLIST_HEAD(slisthead,slist_data_s)head;
  
  /* listen */
  int server_listen_fd = 0;
  
  server_listen_fd = listen(server_socket_fd,BACK_LOG);
  if(server_listen_fd == -1)
  {
    perror("server_listen_fd");
    return -1;
  }
  
  /* file open logic */
  int ret_status = 0;
  mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  char* filename = FILE_PATH_TO_WRITE;
  file_des = open(filename, (O_RDWR | O_CREAT), mode);
  if(file_des == -1) //file_ptr
  {
    syslog(LOG_ERR,"file creation,opening failure\n");
    perror("file creation");
    return -1;
  }
  
  /* local declarations */
  int current_data_pos = 0;
  //int read_status = 0;
  int no_of_bytes_rcvd = 0;
  bool run_status = true;
  
  char temp_buffer[BUFFER_CAPACITY];
  memset(temp_buffer, '\0', BUFFER_CAPACITY);
    
  while(run_status)
  { 
    if(g_Signal_handler_detection == 1)
    {
      exit_handling();
      break;
    }  
  
    syslog(LOG_DEBUG,"run_status");
    /* Accept */

    socklen_t server_address_len = 0;
    server_address_len = sizeof(server_address);
    char s[INET6_ADDRSTRLEN];
    memset(&s[0],'\0',sizeof(s));
    
    client_accept_fd = accept(server_socket_fd, (struct sockaddr *)&server_address,&server_address_len);
    /* Accepted connection from client */
    inet_ntop(AF_INET,get_in_addr((struct sockaddr *)&server_address),s,sizeof(s));
    syslog(LOG_DEBUG, "Accepted connection from %s", s);
    if(client_accept_fd == -1)
    {
      syslog(LOG_DEBUG, "executed");
      perror("client_accept_fd");
      if(g_Signal_handler_detection == 1)
      {
        exit_handling();
        break;
      }  
    }
    
    #if 1
    /* sig status */
    int sig_status = 0;
    sig_status = sigprocmask(SIG_BLOCK, &x, NULL);
    if(sig_status == -1)
    {
      perror("sig_status - 1");
    }
    #endif
    syslog(LOG_DEBUG,"client_accept_fd completed");
        
    char* writer_file_buffer_ptr = NULL;
    int write_buffer_size = BUFFER_CAPACITY;
    writer_file_buffer_ptr = (char *)malloc(sizeof(char)*BUFFER_CAPACITY);
    if(writer_file_buffer_ptr == NULL)
    {
      syslog(LOG_ERR,"writer_file_buffer_ptr failure\n");
      perror("writer_file_buffer");
    }
    memset(writer_file_buffer_ptr,'\0',BUFFER_CAPACITY);
    
    int exit_write_loop = 0;
      
    /* read */
    int curr_location = 0;
    //char *new_line_check_write = NULL;
    
    int read_buffer_size = BUFFER_CAPACITY;
    char *new_line_read = NULL;
    
    int send_status = 0;
    
    //changed logic to work only on temp_buffer from previous writer_file_buffer_ptr.
    //writer_file_buffer_ptr vaused 40,000 errors as a memory beyind its scope was accessed.
    //converted to do to avoid multiple break conditions and variable handling
    //while((no_of_bytes_rcvd = (recv(client_accept_fd, writer_file_buffer_ptr + curr_location, BUFFER_CAPACITY, 0))))
    do
    {    
      no_of_bytes_rcvd = recv(client_accept_fd, temp_buffer/*writer_file_buffer_ptr + curr_location*/, sizeof(temp_buffer), 0);
      
      if(!no_of_bytes_rcvd || (strchr(temp_buffer, '\n') != NULL))
      {
        //Take a break when "\n" is detected (i..e termination conditon) or 
        //when the no of bytes received are zero
        exit_write_loop = 1;
      }
         
      if((no_of_bytes_rcvd+curr_location) >= write_buffer_size)
      {
        
        write_buffer_size *= MULTIPLIER_FACTOR;
         
        syslog(LOG_DEBUG,"write_buffer_size = %d\n",write_buffer_size);

        char* tmpptr = (char *)realloc(writer_file_buffer_ptr, (sizeof(char) * write_buffer_size) );
        
        if(tmpptr == NULL)
        {
          perror("write realloc failure");
          syslog(LOG_DEBUG,"here");
          free(writer_file_buffer_ptr);
          writer_file_buffer_ptr = NULL;
          free(tmpptr);
          tmpptr = NULL;
          run_status = false;
        }
        
        writer_file_buffer_ptr = tmpptr;

        syslog(LOG_DEBUG,"assignment succesful after realloc");
      }
      
      //seperate memory to store the data
      //the whole working is done on static buffer now
      memcpy(&writer_file_buffer_ptr[curr_location], temp_buffer, no_of_bytes_rcvd);
      
      curr_location = curr_location + no_of_bytes_rcvd;
            
    }while(exit_write_loop == 0);
    
    exit_write_loop = 0;
    
    ret_status = write(file_des,writer_file_buffer_ptr,curr_location);
    syslog(LOG_DEBUG,"return status  = %d\n",ret_status);
    if(ret_status == -1)
    {
      syslog(LOG_ERR,"file writing failure\n");
      perror("file writing");
    }
    
    /* present number of byts in file */
    current_data_pos = lseek(file_des,0,SEEK_CUR);
    syslog(LOG_DEBUG,"position is %d\n",current_data_pos);
    
    lseek(file_des,0,SEEK_SET);
    
    int bytes_sent = 0;
    int bytes_read = 0;
    int read_buffer_loc = 0; 
    int store_previous_new_line = 0;
    
    while(bytes_sent < current_data_pos)
    {
      syslog(LOG_ERR,"read iteration\n");
      read_buffer_loc = 0;
        
      char* read_file_buffer_ptr = NULL;
      read_file_buffer_ptr = (char *)malloc(sizeof(char)*BUFFER_CAPACITY);

      if(read_file_buffer_ptr == NULL)
      {
        perror("read malloc failure");
        syslog(LOG_DEBUG,"read alloc failure");
      }
      
      //Converted to do while for ease of variable handling
      //while(1)
      do
      {
        /* read one byte at a time for line check */
        bytes_read = read(file_des,read_file_buffer_ptr + read_buffer_loc ,sizeof(char));
        if(bytes_read == -1)
        {
          perror("bytes_read");
          syslog(LOG_DEBUG,"bytes_read ilure");
        }
        
        /* accumulation of one by one read */
        read_buffer_loc = read_buffer_loc + bytes_read;

        if(read_buffer_loc > 1)
        {
          new_line_read = strchr(read_file_buffer_ptr,'\n');           
        }

        if(read_buffer_size+store_previous_new_line< (current_data_pos))
        {
          read_buffer_size = read_buffer_size + (current_data_pos-store_previous_new_line);
        
          char* tmpptr  = realloc(read_file_buffer_ptr, sizeof(char)*read_buffer_size);
          if(tmpptr == NULL)
          {
            perror("read realloc failure");
            free(read_file_buffer_ptr);
            read_file_buffer_ptr = NULL;
            free(tmpptr);
            tmpptr = NULL;
          }        
          read_file_buffer_ptr = tmpptr;
        }   
        
      }while(new_line_read == NULL);
 
      store_previous_new_line = read_buffer_loc;
      
      send_status = send(client_accept_fd,read_file_buffer_ptr,read_buffer_loc,0);
      //syslog(LOG_DEBUG,"send_status (send return) = %d\n",send_status);
      if(send_status == -1)
      { 
        perror("error in sending to client");
      }

      free(read_file_buffer_ptr);
      read_file_buffer_ptr = NULL;
      
      bytes_sent = bytes_sent + read_buffer_loc;
      //syslog(LOG_DEBUG,"bytes_sent variable value = %d\n",bytes_sent);
    }

    #if 1
    sig_status = sigprocmask(SIG_UNBLOCK, &x, NULL);
    if(sig_status == -1)
    {
      perror("sig_status - 2");
    }
    #endif  
     
    free(writer_file_buffer_ptr);
    writer_file_buffer_ptr = NULL; 
    
    close(client_accept_fd);
    syslog(LOG_DEBUG, "Closed connection from %s", s);
    syslog(LOG_DEBUG,"writer_file_buffer_ptr free\n");
   
  }

  syslog(LOG_DEBUG,"exit reached\n");
  syslog(LOG_DEBUG,"-------------END OF PROGRAM-------------------");
  return 0;
}

//get socket address, IPV4 or IPV6
void *get_in_addr(struct sockaddr *sa)
{
  if(sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/* handling sigint and sigterm graceful exit */
void socket_termination_signal_handler(int signo)
{

  syslog(LOG_DEBUG,"Caught signal, exiting\n");
 
  if(shutdown(server_socket_fd,SHUT_RDWR))
  {
    perror("Failed on shutdown");
    syslog(LOG_ERR,"Could not close socket fd in signal handler: %s",strerror(errno));
  }

  g_Signal_handler_detection = 1;

}

/* commmon part handling for normal and sigint,sigterm exit */
void exit_handling()
{ 
  //closed any pending operations
  //close open sockets
  //delete the FILE created
  syslog(LOG_DEBUG,"exit_handling");
  int ret_status = 0;
  ret_status = remove(FILE_PATH_TO_WRITE);
  syslog(LOG_DEBUG,"ret_status - remove:: %d\n",ret_status);
  
  close(client_accept_fd);
  close(server_socket_fd);
  close(file_des);
  closelog();
  
}





/* EOF */
