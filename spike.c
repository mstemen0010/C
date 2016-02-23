
/*
** File: %M% SCCS %I% - %G%
**
** Author:   Matthew Stemen
**
**
** 
** Created:  19 June 1995
**  
**
**
** Purpose: Monitors and reports any and all remote logins to the 
**          host it is run from.  This activity is reported either 
**          using Sound, Images(icons), and Tabular reports or any
**          combination thereof.
*/


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

/* define "behavior flags" */
#define HEEL		'h'
#define	PROWL		'p'
#define HUNT		'j' 
#define GUARD		'g'
#define MASK		 127 /* mask, all bits on */
#define NOBEHAVIOR	 MASK
#define NOGUARDTYPE	 MASK

/* others */
#define TIME		't' /* these are defined based on values from cmd line
args */
#define FILESYS		'd' /* " */
#define UTMP		'u' /* " */
#define WMAIL		'm' /* " */
#define WSOUND		's' /* " */
#define WLOG		'l' /* " */
#define WCONSOLE	'c' /* " */
#define GUTMP       'y' /* arbitrary value */
#define GFILESYS    'z' /* arbitrary value */
#define MAXCANWATCH 5000


/* globals */

extern int 	errno;
int		warnWSound = FALSE;
int		warnWMail = FALSE;
int		warnWLog = FALSE;
int		warnWConsole = FALSE;
long	activeBehavior = NOBEHAVIOR, passiveBehavior = NOBEHAVIOR;
long	guardType = NOGUARDTYPE;	
struct  person 
{
	long	name;
	struct	person	*next;
} info;

struct persun 	*guardWho[MAXCANWATCH];

void SIG_CHILD(FILE *log );

int makeRounds(char *spikeUser, char *watchWho, char *watchWhat, FILE *logfp);

int smellFileSys(uid_t scent, DIR *dirp, char *watchWhat);

void bark(char *spikeUser, char *watchWho, uid_t watchWhoUid, FILE *logfp, char *msg);

void daemon_start(char *user, int ignsigcld, FILE *logfp);

void mailUser(FILE *logfp, char *name, char *msg);

void logMsg(FILE *logfp, char *msg);

void prtUsage(char *msg);

void sig_child(FILE *log );

main (int argc, char *argv[])
{
	extern	char	*optarg;
	extern	int		optind;
	struct	stat	statbuf;
	struct	passwd	*pwstruct;
	char			*spikeUser, *spikeUserHome;
	FILE 			*logfp, *utmpfp;
	char			mailTo;
	char			msg[BUFSIZ];	
	char			logFile[BUFSIZ];	
	char 			cmd[BUFSIZ] ="";
	char			line[BUFSIZ] = "";
	int				sleepTime = 300;
	int				opt; 
	time_t			startTime, currTime;
	int				status, guard = TRUE, index = 0;
	char			watchWhat[BUFSIZ] = "";
	char			watchWho[BUFSIZ] = "";
	char 			guardWhat[BUFSIZ] = "";
	uid_t			watchWhoUid = 0;



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
		while ((opt = getopt(argc, argv, "j:g:u:d:t:smlchp")) != -1)
		{
			switch(opt) {
				case UTMP:
					guardType = guardType & GUTMP; 
					if (guardType == (GUTMP & GFILESYS))
						prtUsage("You may not set both -u and -d");
					
					if (optarg)
					{
						status = stat(optarg, &statbuf);	
						if (! status)
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFREG)
							{
								sprintf(msg, "\"%s\" is NOT a valid utmp file", optarg);
								prtUsage(msg);
							}
							else
							{
								strcpy(watchWhat, optarg);
							}
						
						}			
						else
						{
								sprintf(msg, "Unable to stat \"%s\" ", optarg);
								perror("error was:");
								prtUsage(msg);
						}

					}
					else
					{
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
				 	}		
				break;

				case FILESYS:
					guardType = guardType & GFILESYS; 
					if (guardType == (GUTMP & GFILESYS))
						prtUsage("You may not set both -u and -d");
					
					if (optarg)
					{
						status = stat(optarg, &statbuf);	
						if (! status)
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFDIR)
							{
								sprintf(msg, "\"%s\" is NOT a vaild directory", optarg);
								prtUsage(msg);
							}
							else
								strcpy(watchWhat, optarg);
						
						}			
						else
						{
								sprintf(msg, "Unable to stat \"%s\" ", optarg);
								perror("error was:");
								prtUsage(msg);
						}

					}
					else
					{
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
				 	}		
				break;
					
				case HEEL:
					passiveBehavior = passiveBehavior & HEEL;
					if (passiveBehavior == (HEEL & PROWL) )
						prtUsage("You may not set both -p and -h");
				break;

				case PROWL:
					passiveBehavior = passiveBehavior &  PROWL;
					if (passiveBehavior == (HEEL & PROWL))
						prtUsage("You may not set both -p and -h");
				break;

				case GUARD:

					if (optarg)
					{
						activeBehavior = activeBehavior & GUARD;
						if (activeBehavior == (HUNT & GUARD))
							prtUsage("You may not set both -j and -g");
						status = stat(optarg, &statbuf);	
						if (! status)
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFREG)
							{
								sprintf(msg, "\"%s\" is NOT a vaild file", optarg);
								prtUsage(msg);
							}
							else
							{
								if ((utmpfp = fopen(optarg, "r")) == NULL)
								{
									sprintf(msg, "Unable to open %s for read", optarg);
									logMsg(logfp, msg);
								}
								else
								{
									while (fread(&guardWho[index], sizeof(long),1,  utmpfp))
									{
										index++;
									}
									fclose(utmpfp);
								}
							}	
						}			
						else
						{
								sprintf(msg, "Unable to stat \"%s\" ", optarg);
								perror("error was:");
								prtUsage(msg);
						}

					}
					else
					{
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
				 	}		

				break;

				case HUNT: 
					if (optarg)
					{
						activeBehavior = activeBehavior & HUNT;
						if (activeBehavior == (HUNT & GUARD))
							prtUsage("You may not set both -j and -g");
						pwstruct = getpwnam(optarg);
						if (pwstruct)
						{
							watchWhoUid = pwstruct->pw_uid;
							if (watchWhoUid == 0)
							{
								/* the above is set to "0" (root), then we
								 * check to make sure that spike was run on
								 * root. If not we were given an invalid user
								 * name to look for 
								 */
								if ( strcmp(optarg, "root") && strcmp(optarg, "ROOT") && strcmp(optarg,"0")) 
								{
									printf("%s\n", *optarg);
									sprintf(msg, "No such user as : %s", optarg);
									prtUsage(msg);
								}
							}		 
							strcpy(watchWho, pwstruct->pw_name);
						}
					       /* 
						* if the above did not work, we were probably
						* pass a uid 
						*/
						else
						{
							pwstruct = getpwuid(atol(optarg));
							if (pwstruct)
							{
								watchWhoUid = pwstruct->pw_uid;
								if (watchWhoUid == 0)
								{
									/* the above is set to "0" (root), then we
								 	* check to make sure that spike was run on
								 	* root. If not we were given an invalid user
								 	* name to look for 
								 	*/
									if ( *optarg != 'root' && *optarg != 'ROOT' && *optarg != '0') 
									{
										sprintf(msg, "No such user as : %s", optarg);
										prtUsage(msg);
									}
								}		 
								strcpy(watchWho, pwstruct->pw_name);
							}
					       		/* 
							 * if neither of the above worked, we were probably
							 * passed bad info 
							 */
							else
							{
								sprintf(msg, "No such uid as : %s", optarg);
								prtUsage(msg);
							}
						}
					}
					else
					{
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
				 	}		
				break;

				case TIME:
					if (optarg)
						sleepTime = atoi(optarg);
					else
					{
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
				 	}		
				break;

				case WSOUND:
					warnWSound = TRUE;
				break;

				case WMAIL:
					warnWMail = TRUE;
				break;

				case WLOG:
					warnWLog = TRUE;
				break;

				case WCONSOLE:
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
		/* daemon_start(spikeUser, 1, logfp); */

		sprintf(msg, "spike started on user:%s uid:%d\n", watchWho, watchWhoUid);
		fprintf(stdout, "%s",  msg);
		fflush(stdout); 
		/* inform the user we have started */
		bark(spikeUser, watchWho, watchWhoUid, logfp, msg);

		while (guard)
		{
			if ( (makeRounds(spikeUser, watchWho, watchWhat, logfp)) == USERFOUND)
				if (passiveBehavior == HEEL)
					guard = FALSE;
				else	
					sleep(sleepTime);
		}

	} /* end, argc check */
	else
		prtUsage("");

													
} /* end of main */



int makeRounds(char *spikeUser, char *watchWho, char *watchWhat, FILE *logfp)
{

	struct	passwd	*pwstruct;
	DIR 			*dirp;
	char			uid[BUFSIZ] = "";
	int				smellUser = USERNOTFOUND, status = 0;
	char			userIdStr[32] = ""; 
	char			msg[BUFSIZ] = "";
	uid_t			watchWhoUid = 0;

	if ( (guardType == GFILESYS) && (activeBehavior == HUNT))
	{
		dirp = opendir(watchWhat);

		pwstruct = getpwnam(watchWho);
		if (pwstruct)
		{
			watchWhoUid = pwstruct->pw_uid;

			if (dirp != NULL)
			{
				smellUser = smellFileSys(watchWhoUid, dirp, watchWhat);
				if (smellUser)
				{
					sprintf(msg, "Spike: found user %s (uid: %d)", watchWho, watchWhoUid);
					bark(spikeUser, watchWho, watchWhoUid, logfp, msg);
				}
				closedir(dirp);
			}
			else
			{
				sprintf(msg, "unable to open directory: %s", watchWhat);
				logMsg(logfp, msg);
			}
		}
		else
		{
			sprintf(msg, "unable to get uid for: %s", watchWho);
			logMsg(logfp, msg);
		}
	} /* end, GFILESYS && HUNT */

	if ((guardType == GUTMP) && (activeBehavior == HUNT))
	{
		if ((utmpfp = fopen(watchWhat, "r")))
		{
			while(fread((char *)&ulist, sizeof(ulist), 1, utmpfp))
			{
				for (index = 0; index <=  


	;
	}

	if ((guardType == GFILESYS) && (activeBehavior == GUARD))
	;

	if ((guardType == GUTMP) && (activeBehavior == GUARD))
	;


	return(smellUser);
				
} /* end, makeRounds */				


int smellFileSys(uid_t scent, DIR *dirp, char *watchWhat)
{
	int				smell = USERNOTFOUND; 
	struct	dirent	*direntp;
	struct	stat	statbuf;
	char			filePath[BUFSIZ] = "";

	switch (activeBehavior)
	{
		case HUNT:
			while ( (direntp = readdir( dirp )) != NULL )
			{
					if (direntp->d_name)
					{
							sprintf(filePath, "%s/%s", watchWhat, direntp->d_name);
							if (stat(filePath, &statbuf) == 0)	
							{

								if (scent == statbuf.st_uid)
								{
									smell = USERFOUND;
									break;
								}
							}
					
					}
			}
		break;

		case GUARD:

		break;
	}
	return(smell);

} /* end, smellFileSys */

void bark( char *spikeUser, char *watchWho, uid_t watchWhoUid, FILE *logfp, char *msg)
{
	FILE	*confp;

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
			logMsg(logfp, "Unable to open /dev/console");
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
	char            *rawdate = "";
	char            date[BUFSIZ] = "";
	time_t          startTime, currTime;
	char		outMsg[BUFSIZ] ="";
	startTime = time(NULL);
	if (startTime)
		rawdate = (char *)ctime(&startTime);
		bcopy (rawdate, date, (strlen(rawdate) - 1));
	sprintf(outMsg, "spike:%d@%s - %s", getpid(), date, msg);
	fprintf(logfp, "%s\n", outMsg);
	fflush(logfp);
} /* end, logUser */

void prtUsage(char *msg)
{
	fprintf(stdout, "\n \n%s\n \n", msg);
	fprintf (stdout, "usage: spike  [-j (uid or name) | -g (file of list of `authorized` users)]   [-t (sleep time)]  [options] \n");
	fprintf (stdout, "options\n"); 
	fprintf (stdout, "------\n"); 
	fprintf (stdout, "-s	warn with sound (bark) \n");
	fprintf (stdout, "-m	warn with mail  ( sent to your mailbox) \n");
	fprintf (stdout, "-l	warn with log entry (kept in your home directory) \n");
	fprintf (stdout, "-c	warn to your console window  \n\n");

	fprintf (stdout, "-h	warn you once, then exits \n");
	fprintf (stdout, "-p	warn you once, then continues to warn again, and again \n\n");
	fprintf (stdout, "One of the following must be provided\n-------------------------------------\n");
	fprintf (stdout, "-d(directory)	Must be valid directory to watch. \n");
	fprintf (stdout, "-u(utmp filename)	utmp file to watch, must be valid directory to watch. Required with  \n\n");
	
	fprintf (stdout, "NOTE: All of the options may be specified in any combinnation.\n");
	fprintf (stdout, "      spike may not be given conflicting behavior, you may use \"-h\" or \"-g\".  \n \n");

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
