#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>

/* defines */

#ifndef TRUE
#define TRUE 	1
#define FALSE 	! TRUE
#endif



#define	USERNOTFOUND	0
#define	USERFOUND	1
#define PROCDIR "/proc"


/* globals */

extern int 	errno;
int		warnWSound = FALSE;
int		warnWMail = FALSE;
int		warnWLog = FALSE;
int		warnWConsole = FALSE;

void SIG_CHILD(FILE *log );

int makeRounds(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp); 

int sniffUser(uid_t scent, uid_t currentUid);

void bark(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp);

void daemon_start(char *user, int ignsigcld, FILE *logfp);

void mailUser(FILE *logfp, char *name, char *msg);

void logMsg(FILE *logfp, char *msg);

void prtUsage(char *msg);

void sig_child(FILE *log );

main (int argc, char *argv[])
{
	extern	char	*optarg;
	extern	int	optind;
	struct	stat	statbuf;
	struct	passwd	*pwstruct;
	char		*spikeUser, *spikeUserHome;
	char		watchedForUser[32];
	FILE 		*logfp;
	char		mailTo;
	char		msg[BUFSIZ];	
	char		logFile[BUFSIZ];	
	char 		cmd[BUFSIZ] ="";
	int		sleepTime = 300;
	int		opt; 
	time_t		startTime, currTime;
	uid_t		watchedForUid;



	if (argc > 4) /* check args for min. number */
	{
		spikeUser = (char *)getenv("USER");
		spikeUserHome = (char *)getenv("HOME");
		sprintf(logFile, "%s/%s_spike.log", spikeUserHome, spikeUser);

		/* create or open the log file */
		if ( (logfp = fopen(logFile, "a+")) == NULL)
		{
			sprintf(msg, "Unable to create log file: %s", logFile);
			logMsg(logfp, msg);

		}
		while ((opt = getopt(argc, argv, "n:u:t:smlc")) != -1)
		{
			switch(opt) {

				case 'u':
					if (optarg)
					{
							watchedForUid = (uid_t)atol(optarg);
							pwstruct = getpwuid(watchedForUid);
							if (pwstruct->pw_name)
								strcpy(watchedForUser, pwstruct->pw_name);
					}
					else
					{
						prtUsage("Invalid Argument");
				 	}		
				break;
				case 'n':
					if (optarg)
					{
						strcpy(watchedForUser, optarg);
						pwstruct = getpwnam(watchedForUser);
						watchedForUid = pwstruct->pw_uid;
					}
					else
					{
						prtUsage("Invalid Argument");
				 	}		
				break;
				case 't':
					if (optarg)
						sleepTime = atoi(optarg);
					else
					{
						prtUsage("Invalid Argument");
				 	}		
				break;

				case 's':
					warnWSound = TRUE;
				break;

				case 'm':
					warnWMail = TRUE;
				break;

				case 'l':
					warnWLog = TRUE;
				break;

				case 'c':
					warnWConsole = TRUE;
				break;

				case '?':

					prtUsage("");

				break;

				default:

					prtUsage("error:");

				break;
			} /* end, opt switch */
		} /* end, while getopt */

		/* start this program as a daemon */
		daemon_start(spikeUser, 1, logfp);

		sprintf(msg, "spike started on user:%s uid:%d", watchedForUser, watchedForUid);
		fprintf(stdout, "spike started on user:%s uid:%d", watchedForUser, watchedForUid);
		mailUser(logfp, spikeUser, msg); 
		/* check to see in uses mail file exists */

		while (TRUE)
		{
			makeRounds(spikeUser, watchedForUser, watchedForUid, logfp);
			sleep(sleepTime);
		}

	} /* end, argc check */
	else
		prtUsage("");

													
} /* end of main */



int makeRounds(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp)
{

	DIR 		*dirp;
	struct	dirent	*direntp;
	struct	stat	statbuf;
	char		uid[BUFSIZ] = "";
	int		smellUser = USERNOTFOUND, status = 0;
	char		userIdStr[32] = ""; 
	char		filePath[BUFSIZ] = "";
	char		msg[BUFSIZ] = "";

	dirp = opendir(PROCDIR);


	if (dirp != NULL)
	{
	 	while ( (direntp = readdir( dirp )) != NULL )
		{
			if (direntp->d_name)
			{
				sprintf(filePath, "%s/%s", PROCDIR, direntp->d_name);
				status = stat(filePath, &statbuf);	
				smellUser = sniffUser(watchedForUid, statbuf.st_uid);
				if (smellUser)
				{
					bark(spikeUser, watchedForUser, watchedForUid, logfp);
					break;
				}
			
			}
		}
		closedir(dirp);
	}
	else
	{
		sprintf(msg, "unable to open directory: %s", PROCDIR);
		perror("");
		logMsg(logfp, msg);
	}

	return(smellUser);
				
} /* end, makeRounds */				


int sniffUser(uid_t scent, uid_t currentUid)
{
	int status = USERNOTFOUND;

	if (scent == currentUid) 
	{
		status = USERFOUND;
	}
	return(status);

} /* end, sniffUser */

void bark( char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp)
{
	char	msg[BUFSIZ] = "";
	FILE	*confp;

	sprintf(msg, "found user %s (uid: %d)", watchedForUser, watchedForUid);
	if (warnWSound)
		; /* opt not installed yet */
	if (warnWLog)
		logMsg(logfp, msg);
	if (warnWMail)
		mailUser(logfp, spikeUser, msg);
	if (warnWConsole)
	{
		if ( (logfp = fopen("/dev/console", "a")) == NULL)
		{
			sprintf(msg, "Unable to open /dev/console");
			logMsg(logfp, msg);
		}
		else
		{
			fprintf(confp, "%s\n", msg);
			fflush(confp);
			fclose(confp);
		}
	} /* end, watchConsole */

} /* end, bark */

void mailUser(FILE *logfp, char *user, char *msg)
{
    	char cmd[BUFSIZ] = "";
	char *mailHost;
	mailHost = (char *)getenv("MAILHOST");

	sprintf(cmd, "echo \" %s \" | mail -s spikeinfo:%d %s " , msg, getpid(), user );
	system(cmd);
} /* end, mailUser */

void logMsg(FILE *logfp, char *msg)
{
	char            *date = "";
	time_t          startTime, currTime;
	startTime = time(NULL);
	if (startTime)
		date = (char *)ctime(&startTime);
	fprintf(logfp, "%s - %s\n", date, msg);
	fflush(logfp);
} /* end, logUser */

void prtUsage(char *msg)
{
	fprintf(stdout, "%s\n \n", msg);
	fprintf (stdout, "usage: spike [-u (uid) or -n (user name) ] [-t (sleep time)] [options] \n");
	fprintf (stdout, "options\n"); 
	fprintf (stdout, "------\n"); 
	fprintf (stdout, "-s	warn with sound (bark) \n");
	fprintf (stdout, "-m	warn with mail  ( sent to your mailbox) \n");
	fprintf (stdout, "-l	warn with log entry (kept in your home directory) \n");
	fprintf (stdout, "-c	warn to your console window  \n\n");
	fprintf (stdout, "NOTE: All of the options may be specified in any combinnation.  \n ");

	exit(0);
} /* end, prtUsage */
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
