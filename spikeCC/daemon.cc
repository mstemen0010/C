/*
** File: daemon.cc SCCS 1.2 - 8/4/95
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


#include "daemon.h"

daemon::daemon()
{
	usrName( getenv("USER") ); 
	usrHome( getenv("HOME") );
	timerVal(-1);
	_msg = new char[BUFSIZ];
	
}
daemon::~daemon()
{
	delete[] _logFileName;
	delete[] _usrName;
	delete[] _usrHome;
	delete[] _msg;

}
void	daemon::start(const int ignsigcld)
{
   /* If we were started by init (process 1) from the /etc/inittab file
	* there's no need to detach.
	* This test is unreliable due to an unavoidable ambiguity
	* if the process is started by some other process and orphaned
	* (i.e., if the parent process terminates before we are started).
	*/  


	if ((getppid()) == 1)
		goto out;

	// Ignore the terminal stop signals (BSD).
	
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

	int childpid;	
	if ( (childpid = fork()) < 0) 
	{
		msg("can't fork first child");
		logMsg();
	}
	else if (childpid > 0) 
		exit(0); // parent
	
	// First child process starts here. 
	
	/*
	 * Disassociate from controlling terminal and process group.
	 * Ensure the process can't reacquire a new controlling terminal.
	 */
	
	#ifdef SIGTSTP // BSD 
		#ifndef _BSD_COMPAT // non SGI
			if ( BSDsetpgrp( 0, getpid()) == -1)
			{
				msg("can't change process group");
				logMsg();
			}
		#else
			if ( setpgrp( 0, getpid()) == -1)
			{
				msg("can't change process group");
				logMsg();
			}
		#endif		
		int fd;
		if (  (fd = open("/dev/tty/", O_RDWR)) >= 0)
		{
			ioctl(fd, TIOCNOTTY, (char *)NULL); /* lose controlling tty */
			close(fd);
		}
		else
		{
			msg("can't lose tty");
			logMsg();
		}
	#else // System V 
		if (setpgrp() == -1)
		{
			msg("can't change process group");
			logMsg();
		}
		signal(SIGHUP, SIG_IGN); /* immune from pgrp leader death */
		if ( (childpid = fork()) < 0)
		{
			msg("can't fork second child");
			logMsg();
		}
		else if (childpid > 0) 
			exit(0); /* first child */
	#endif   
	
	/* second child starts here*/

	out:
	
		 // Close any open file descriptors.
	
		 for (fd = 0; fd < NOFILE; fd++)
				;
			// close(fd); 
	
		errno = 0; // probably got set to EBADF from a close 
	
		/*
		 * Move the current directory to root, to make sure we
		 * aren't on a mounted filesystem.
		 */
	
		chdir("/");
							  
	 	// Clear any inherited file mode creation mask.
	
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
				signal(SIGCLD, sig_child() ); /* BSD */
			#else
				signal(SIGCLD, SIG_IGN); /* System V */
			#endif
		}
		//		cout << "Got here \n";
	//			cout.flush();
} /* end of daemon_start */


int daemon::mailUser(const char *message)
{
	char cmd[BUFSIZ];
    sprintf(cmd, "`echo \" %s \" | mail -s %sinfo:%d %s` ", message ,daemonName(), getpid(), usrName() );
    return (system(cmd));
} /* end, mailUser */

SIG_PF daemon::sig_child()
{

#ifdef BSD
    printf("BSD defined\n");
    /*
     * Use wait3() system call w/ the WNOHANG opt
     */
 
 
     int    pid;
     union  wait status;
     while ((pid = wait3(&status, WNOHANG, (struct rusage *) 0)) > 0)
        ;
#endif
	return((SIG_PF)1);
}
void daemon::logMsg()
{

    if (msg())
    {
        logFile()->write(msg(), strlen(msg()) );
        logFile()->flush();
    }
} /* end, logUser */

