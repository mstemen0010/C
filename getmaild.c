#include<stdio.h>
#include<signal.h>
#include<sys/param.h>
#include<errno.h>
#include <sys/wait.h>
#include <signal.h>
#include<time.h>
#include <sys/stat.h>

#ifndef TRUE
#define TRUE 1
#define FALSE ! TRUE
#endif
extern int	errno;

#ifdef	SIGTSTP  /* true if BSD system */
#include <sys/file.h>
#include <sys/ioctl.h>
#endif

#define USAGE "usage: gmd [-t time in seconds] [-i in mail file] [-o out mail file]"

/* #include "systype.h" */


void sig_child(FILE *log );

void daemon_start(char *user, int ignsigcld, FILE *logfp);

void mailUser(FILE *logfp, char *name, char *msg);

void logMsg(FILE *logfp, char *msg);

void prtUsage(char *msg);



main(int argc, char *argv[])
{
	FILE        	*logfp;
	char 		cmd[BUFSIZ] = "";
	struct	stat	statbuf;
	int		status = 0;
	int		currFileSize = 0, oldFileSize = 0;
	int		firstTime = TRUE, opt, sleepTime = 300;
	time_t		startTime, currTime;
	extern	char	*optarg;
	extern  int	optind;
	char		mailIn[BUFSIZ] = "", mailOut[BUFSIZ] = "";
	char		logFile[BUFSIZ] = "";
	char		*userName, *userHome;
	char  		msg[BUFSIZ];


	if (argc > 6)
	{
		userName = (char *)getenv("USER");
		userHome = (char *)getenv("HOME");
		sprintf(logFile, "%s/%s_%s.log", userHome, userName, argv[0]);

		/* create or open the log file */
		
		if ( (logfp =  fopen(logFile, "w+")) == NULL)
		{
			sprintf(msg, "Unable to create log file: %s", logFile);
			logMsg(logfp, msg);
		}			



			while ((opt = getopt(argc, argv, "t:i:o:")) != -1)
		    	{
				switch(opt) {
					case 't':
						sleepTime = atoi(optarg);
						break;
					case 'i':
						if (optarg)
							strcpy(mailIn, optarg);
						break;
					case 'o':
						if (optarg)
							strcpy(mailOut, optarg);
						break;
					case '?':
					 	prtUsage("");
						break;
					default:
					 	prtUsage("error:");
						break;
		
				}	
			}
		
			daemon_start(userName, 1, logfp);
			mailUser(logfp, userName, "gmd started"); 
		
			status = stat(mailOut, &statbuf);
			if (status == -1)
			{
				sprintf(msg, "Unable to stat out mail file: %s", mailOut);
				logMsg(logfp, msg);
			}
		
			while (1)
			{
				if (firstTime)
				{
					status = stat(mailIn, &statbuf);
					if (status == 0)
					{
						currFileSize = statbuf.st_size; 
						firstTime = FALSE;
						sprintf (cmd, "/bin/cp %s %s", mailIn, mailOut);
						system(cmd);
						oldFileSize = currFileSize;
					}
					else
					{
						sprintf(msg, "Unable to stat in mail file: %s", mailIn);
						logMsg(logfp, msg);
					}
				}
				else
				{
					currTime = time(NULL);
					if ((currTime - startTime) >= 324000)
					{
						sprintf (cmd, "klog -pa `cat /home/mgs/.aikotoba`");
						system(cmd);
	
						logMsg(logfp, "gmd: got tokens");
						startTime = currTime;
					}
						
					status = stat(mailIn, &statbuf);

					if (status == 0)
					{
						currFileSize = statbuf.st_size; 
						if (oldFileSize != currFileSize)
						{
							sprintf (cmd, "/bin/cp %s %s", mailIn, mailOut);
							system(cmd);
							oldFileSize = currFileSize;
					
						}
					}
				}
				sleep(sleepTime);
			} /* end of while */

	} /* end of arg check */
	else
		prtUsage("invaild number of arguments");
} /* end of main */
	

void sig_child(FILE *log)
{
	 int	pid;
	 union  wait status;

#ifdef BSD
	printf("BSD defined\n");
	/*
	 * Use wait3() system call w/ the WNOHANG opt
	 */


	 while ((pid = wait3(&status, WNOHANG, (struct rusage *) 0)) > 0)
		;
#endif
}

/*
 * 	Detach a daemon process from login session context.
 */


 void daemon_start(char *userName, int ignsigcld, FILE *logfp)
 {
	register int	childpid, fd;
	char cmd[BUFSIZ] = "";
	char errmsg[BUFSIZ] ="";


	/* If we were started by init (process 1) from the /etc/inittab file 
	 * there's no need to detach.
	 * This test is unreliable due to an unavoidable ambiguity
	 * if the process is started by some other process and orphaned
	 * (i.e., if the parent process terminates before we are started).
	 */

	 if (getppid() == 1)
		goto out;

	/* 
	 * Ignore the terminal stop signals (BSD).
	 */

#ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif


	/*
	 * If we were not started in the background, fork and 
	 * let the parent exit. This also guarantees the first child 
	 * is not a process group leader.
	 */

	 if ( (childpid = fork()) < 0) {
		logMsg(logfp, "can't fork first child");
		}
	else if (childpid > 0) {
		exit(0); /* parent */
	}

/* First child process starts here. */
	/* 
	 * Disassociate from controlling terminal and process group.
	 * Ensure the process can't reacquire a new controlling terminal.
	 * 
	 */
#ifdef SIGTSTP /* BSD */

	if ( setpgrp(0, getpid()) == -1)
	{
		logMsg(logfp, "can't change process group");
	}

	if ( ( fd = open("/dev/tty", O_RDWR)) >= 0 )
	{
		ioctl(fd, TIOCNOTTY, (char *)NULL); /* lose controlling tty */
		close(fd);
	}
	else
	{
		logMsg(logfp, "can't lose tty");

	}

#else /* System V */

	if (setpgrp() == -1)
	{
		logMsg(logfp, "can't change process group");
	}
	signal(SIGHUP, SIG_IGN); /* immune from pgrp leader death */

	if ( (childpid = fork()) < 0)
	{
		logMsg(logfp, "can't fork second child");
	}
	else if (childpid > 0) {
		exit(0); /* first child */
	}

#endif

/* second child starts here*/ 

out:
	
	/* 
	 * Close any open file descriptors.
	 */

	 for (fd = 0; fd < NOFILE; fd++)
	 {
		/* close(fd); */
	 }

	errno = 0; /* probably got set to EBADF from a close */


	/* 
	 * Move the current directory to root, to make sure we 
	 * aren't on a mounted filesystem.
	 */

	 chdir("/");

	 /* 
	  * Clear any inherited file mode creation mask.
	  */

	  umask(0);

	  /* 
	   * See if the caller isn't interested in the exit status of its 
	   * children, and doesn't want to have them become zombies and
	   * clog up the system.
	   * With System V all we have to do is ignore the signal.
	   * With BSD, however, we have to catch each signal
	   * and execute the wait3() system call.
	   */

	if (ignsigcld)
	{
#ifdef SIGTSTP

		signal(SIGCLD, sig_child ); /* BSD */
#else
		signal(SIGCLD, SIG_IGN); /* System V */

#endif
	}

} /* end of daemon_start */


void mailUser(FILE *logfp, char *user, char *msg)
{
	char cmd[BUFSIZ] = "";
	char *mailHost;

	mailHost = (char *)getenv("MAILHOST");


	sprintf(cmd, "echo \" %s \" | mail -s gmdinfo:%d %s@%s " , msg, getpid(), user, mailHost );
	system(cmd);
}

void logMsg(FILE *logfp, char *msg)
{
	char			*date = "";
	time_t			startTime, currTime;

	startTime = time(NULL);
	if (startTime)
		date = ctime(&startTime);
	fprintf(logfp, "%s - %s\n", date, msg);
	fflush(logfp);
}

void prtUsage(char *msg)
{
	fprintf(stdout, "%s\n \n %s\n", msg, USAGE);
	exit(0);
}
