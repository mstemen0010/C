h60585
s 00019/00001/00680
d D 1.4 95/06/21 14:05:41 mgs 4 3
c 
e
s 00286/00094/00395
d D 1.3 95/06/21 14:02:36 mgs 3 2
c 
e
s 00050/00019/00439
d D 1.2 95/06/20 09:05:02 mgs 2 1
c Added behavior flags and fix log enteries to allow for multiple spikes running.
e
s 00458/00000/00000
d D 1.1 95/06/20 07:56:26 mgs 1 0
c date and time created 95/06/20 07:56:26 by mgs
e
u
U
f e 0
t
T
I 4

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


E 4
I 1
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
D 4

E 4
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


D 3

E 3
#define	USERNOTFOUND	0
#define	USERFOUND	1
#define PROCDIR "/proc"
I 3

E 3
I 2
/* define "behavior flags" */
D 3
#define HEEL		0
#define	GUARD		1
#define NOBEHAVIOR	2
E 3
I 3
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
E 3
E 2


/* globals */

extern int 	errno;
int		warnWSound = FALSE;
int		warnWMail = FALSE;
int		warnWLog = FALSE;
int		warnWConsole = FALSE;
I 2
D 3
int		behavior = NOBEHAVIOR, guardFlag = FALSE, heelFlag = FALSE;
E 3
I 3
long	activeBehavior = NOBEHAVIOR, passiveBehavior = NOBEHAVIOR;
long	guardType = NOGUARDTYPE;	
E 3
E 2

void SIG_CHILD(FILE *log );

D 3
int makeRounds(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp); 
E 3
I 3
int makeRounds(char *spikeUser, char *watchWho, char *watchWhat, FILE *logfp);
E 3

int sniffUser(uid_t scent, uid_t currentUid);

D 2
void bark(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp);
E 2
I 2
D 3
void bark(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp, char *msg);
E 3
I 3
void bark(char *spikeUser, char *watchWho, uid_t watchWhoUid, FILE *logfp, char *msg);
E 3
E 2

void daemon_start(char *user, int ignsigcld, FILE *logfp);

void mailUser(FILE *logfp, char *name, char *msg);

void logMsg(FILE *logfp, char *msg);

void prtUsage(char *msg);

void sig_child(FILE *log );

main (int argc, char *argv[])
{
	extern	char	*optarg;
D 3
	extern	int	optind;
E 3
I 3
	extern	int		optind;
E 3
	struct	stat	statbuf;
	struct	passwd	*pwstruct;
D 3
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
I 2
	int		status, guard = TRUE;
E 3
I 3
	char			*spikeUser, *spikeUserHome;
	FILE 			*logfp;
	char			mailTo;
	char			msg[BUFSIZ];	
	char			logFile[BUFSIZ];	
	char 			cmd[BUFSIZ] ="";
	int				sleepTime = 300;
	int				opt; 
	time_t			startTime, currTime;
	int				status, guard = TRUE;
	char			watchWhat[BUFSIZ] = "";
	char			watchWho[BUFSIZ] = "";
	uid_t			watchWhoUid = 0;
E 3
E 2



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
D 2
		while ((opt = getopt(argc, argv, "n:u:t:smlc")) != -1)
E 2
I 2
D 3
		while ((opt = getopt(argc, argv, "n:u:t:smlchg")) != -1)
E 3
I 3
		while ((opt = getopt(argc, argv, "j:g:u:d:t:smlchp")) != -1)
E 3
E 2
		{
			switch(opt) {
I 3
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
E 3

D 3
				case 'u':
E 3
I 3
				case GUARD:

E 3
					if (optarg)
					{
D 3
							watchedForUid = (uid_t)atol(optarg);
							pwstruct = getpwuid(watchedForUid);
							if (pwstruct->pw_name)
								strcpy(watchedForUser, pwstruct->pw_name);
E 3
I 3
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
								strcpy(watchWhat, optarg);
						
						}			
						else
						{
								sprintf(msg, "Unable to stat \"%s\" ", optarg);
								perror("error was:");
								prtUsage(msg);
						}

E 3
					}
					else
					{
D 3
						prtUsage("Invalid Argument");
E 3
I 3
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
E 3
				 	}		
I 3

E 3
				break;
D 3
				case 'n':
E 3
I 3

				case HUNT: 
E 3
					if (optarg)
					{
D 3
						strcpy(watchedForUser, optarg);
						pwstruct = getpwnam(watchedForUser);
						watchedForUid = pwstruct->pw_uid;
E 3
I 3
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
E 3
					}
					else
					{
D 3
						prtUsage("Invalid Argument");
E 3
I 3
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
E 3
				 	}		
				break;
D 3
				case 't':
E 3
I 3

				case TIME:
E 3
					if (optarg)
						sleepTime = atoi(optarg);
					else
					{
D 3
						prtUsage("Invalid Argument");
E 3
I 3
						sprintf(msg, "Invalid Argument: %s", optarg);
						prtUsage(msg);
E 3
				 	}		
				break;

D 3
				case 's':
E 3
I 3
				case WSOUND:
E 3
					warnWSound = TRUE;
				break;

D 3
				case 'm':
E 3
I 3
				case WMAIL:
E 3
					warnWMail = TRUE;
				break;

D 3
				case 'l':
E 3
I 3
				case WLOG:
E 3
					warnWLog = TRUE;
				break;

D 3
				case 'c':
E 3
I 3
				case WCONSOLE:
E 3
					warnWConsole = TRUE;
				break;

I 2
D 3
				case 'h':
					heelFlag = TRUE;
					if (heelFlag && guardFlag)
						prtUsage("You may not set both -g and -h");
					else
						behavior = HEEL;
				break;

				case 'g':
					guardFlag = TRUE;
					if (heelFlag && guardFlag)
						prtUsage("You may not set both -g and -h");
					else
						behavior = GUARD;
				break;
E 3

E 2
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

D 3
		sprintf(msg, "spike started on user:%s uid:%d", watchedForUser, watchedForUid);
D 2
		fprintf(stdout, "spike started on user:%s uid:%d", watchedForUser, watchedForUid);
		mailUser(logfp, spikeUser, msg); 
		/* check to see in uses mail file exists */
E 2
I 2
		fprintf(stdout, "spike started on user:%s uid:%d\n", watchedForUser, watchedForUid);
E 3
I 3
		sprintf(msg, "spike started on user:%s uid:%d\n", watchWho, watchWhoUid);
		fprintf(stdout, "%s",  msg);
E 3
		fflush(stdout); 
		/* inform the user we have started */
D 3
		bark(spikeUser, watchedForUser, watchedForUid, logfp, msg);
E 3
I 3
		bark(spikeUser, watchWho, watchWhoUid, logfp, msg);
E 3
E 2

D 2
		while (TRUE)
E 2
I 2
		while (guard)
E 2
		{
D 2
			makeRounds(spikeUser, watchedForUser, watchedForUid, logfp);
			sleep(sleepTime);
E 2
I 2
D 3
			if ( (makeRounds(spikeUser, watchedForUser, watchedForUid, logfp)) == USERFOUND)
				if (behavior == HEEL)
E 3
I 3
			if ( (makeRounds(spikeUser, watchWho, watchWhat, logfp)) == USERFOUND)
				if (passiveBehavior == HEEL)
E 3
					guard = FALSE;
				else	
					sleep(sleepTime);
E 2
		}

	} /* end, argc check */
	else
		prtUsage("");

													
} /* end of main */



D 3
int makeRounds(char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp)
E 3
I 3
int makeRounds(char *spikeUser, char *watchWho, char *watchWhat, FILE *logfp)
E 3
{

D 3
	DIR 		*dirp;
E 3
I 3
	struct	passwd	*pwstruct;
E 3
	struct	dirent	*direntp;
	struct	stat	statbuf;
D 3
	char		uid[BUFSIZ] = "";
	int		smellUser = USERNOTFOUND, status = 0;
	char		userIdStr[32] = ""; 
	char		filePath[BUFSIZ] = "";
	char		msg[BUFSIZ] = "";
E 3
I 3
	DIR 			*dirp;
	char			uid[BUFSIZ] = "";
	int				smellUser = USERNOTFOUND, status = 0;
	char			userIdStr[32] = ""; 
	char			filePath[BUFSIZ] = "";
	char			msg[BUFSIZ] = "";
	uid_t			watchWhoUid = 0;
E 3

D 3
	dirp = opendir(PROCDIR);


	if (dirp != NULL)
E 3
I 3
	if ( (guardType == GFILESYS) && (activeBehavior == HUNT))
E 3
	{
D 3
	 	while ( (direntp = readdir( dirp )) != NULL )
E 3
I 3
		dirp = opendir(watchWhat);

		pwstruct = getpwnam(watchWho);
		if (pwstruct)
E 3
		{
D 3
			if (direntp->d_name)
E 3
I 3
			watchWhoUid = pwstruct->pw_uid;

			if (dirp != NULL)
E 3
			{
D 3
				sprintf(filePath, "%s/%s", PROCDIR, direntp->d_name);
				status = stat(filePath, &statbuf);	
				smellUser = sniffUser(watchedForUid, statbuf.st_uid);
				if (smellUser)
E 3
I 3
	 			while ( (direntp = readdir( dirp )) != NULL )
E 3
				{
D 2
					bark(spikeUser, watchedForUser, watchedForUid, logfp);
E 2
I 2
D 3
					sprintf(msg, "found user %s (uid: %d)", watchedForUser, watchedForUid);
					bark(spikeUser, watchedForUser, watchedForUid, logfp, msg);
E 2
					break;
				}
E 3
I 3
					if (direntp->d_name)
					{
						sprintf(filePath, "%s/%s", watchWhat, direntp->d_name);
						status = stat(filePath, &statbuf);	
						smellUser = sniffUser(watchWhoUid, statbuf.st_uid);
						if (smellUser)
						{
							sprintf(msg, "Spike: found user %s (uid: %d)", watchWho, watchWhoUid);
							bark(spikeUser, watchWho, watchWhoUid, logfp, msg);
							break;
						}
E 3
			
I 3
					}
				}

				closedir(dirp);
			}
			else
			{
				sprintf(msg, "unable to open directory: %s", watchWhat);
				logMsg(logfp, msg);
E 3
			}
		}
D 3
		closedir(dirp);
	}
	else
	{
		sprintf(msg, "unable to open directory: %s", PROCDIR);
		perror("");
		logMsg(logfp, msg);
	}
E 3
I 3
		else
		{
			sprintf(msg, "unable to get uid for: %s", watchWho);
			logMsg(logfp, msg);
		}
	} /* end, GFILESYS && HUNT */

	if ((guardType == GUTMP) && (activeBehavior == HUNT))
	;

	if ((guardType == GFILESYS) && (activeBehavior == GUARD))
	;
E 3

I 3
	if ((guardType == GUTMP) && (activeBehavior == GUARD))
	;


E 3
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

D 2
void bark( char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp)
E 2
I 2
D 3
void bark( char *spikeUser, char *watchedForUser, uid_t watchedForUid, FILE *logfp, char *msg)
E 3
I 3
void bark( char *spikeUser, char *watchWho, uid_t watchWhoUid, FILE *logfp, char *msg)
E 3
E 2
{
D 2
	char	msg[BUFSIZ] = "";
E 2
	FILE	*confp;

D 2
	sprintf(msg, "found user %s (uid: %d)", watchedForUser, watchedForUid);
E 2
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
D 2
			sprintf(msg, "Unable to open /dev/console");
			logMsg(logfp, msg);
E 2
I 2
			logMsg(logfp, "Unable to open /dev/console");
E 2
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
D 2
	char            *date = "";
E 2
I 2
	char            *rawdate = "";
	char            date[BUFSIZ] = "";
E 2
	time_t          startTime, currTime;
I 2
	char		outMsg[BUFSIZ] ="";
E 2
	startTime = time(NULL);
	if (startTime)
D 2
		date = (char *)ctime(&startTime);
	fprintf(logfp, "%s - %s\n", date, msg);
E 2
I 2
		rawdate = (char *)ctime(&startTime);
		bcopy (rawdate, date, (strlen(rawdate) - 1));
	sprintf(outMsg, "spike:%d@%s - %s", getpid(), date, msg);
	fprintf(logfp, "%s\n", outMsg);
E 2
	fflush(logfp);
} /* end, logUser */

void prtUsage(char *msg)
{
D 3
	fprintf(stdout, "%s\n \n", msg);
D 2
	fprintf (stdout, "usage: spike [-u (uid) or -n (user name) ] [-t (sleep time)] [options] \n");
E 2
I 2
	fprintf (stdout, "usage: spike  [-u (uid) or -n (user name) ] [-t (sleep time)]  [options] \n");
E 3
I 3
	fprintf(stdout, "\n \n%s\n \n", msg);
	fprintf (stdout, "usage: spike  [-j (uid or name) | -g (file of list of `authorized` users)]   [-t (sleep time)]  [options] \n");
E 3
E 2
	fprintf (stdout, "options\n"); 
	fprintf (stdout, "------\n"); 
	fprintf (stdout, "-s	warn with sound (bark) \n");
	fprintf (stdout, "-m	warn with mail  ( sent to your mailbox) \n");
	fprintf (stdout, "-l	warn with log entry (kept in your home directory) \n");
D 2
	fprintf (stdout, "-c	warn to your console window  \n\n");
E 2
I 2
D 3
	fprintf (stdout, "-c	warn to your console window  \n");
E 3
I 3
	fprintf (stdout, "-c	warn to your console window  \n\n");

E 3
	fprintf (stdout, "-h	warn you once, then exits \n");
D 3
	fprintf (stdout, "-g	warn you once, then continues to warn again, and again \n\n");
E 2
	fprintf (stdout, "NOTE: All of the options may be specified in any combinnation.  \n ");
I 2
	fprintf (stdout, "NOTE: spike may not be given conflicting behavior, you may use \"-h\" or \"-g\".  \n ");
E 3
I 3
	fprintf (stdout, "-p	warn you once, then continues to warn again, and again \n\n");
	fprintf (stdout, "One of the following must be provided\n-------------------------------------\n");
	fprintf (stdout, "-d(directory)	Must be valid directory to watch. \n");
	fprintf (stdout, "-u(utmp filename)	utmp file to watch, must be valid directory to watch. Required with  \n\n");
	
	fprintf (stdout, "NOTE: All of the options may be specified in any combinnation.\n");
	fprintf (stdout, "      spike may not be given conflicting behavior, you may use \"-h\" or \"-g\".  \n \n");
E 3
E 2

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
E 1
