/*
** File: daemon.h SCCS 1.2 - 8/4/95
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


#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


#include<sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <sys/param.h>
#include <sys/unistd.h>

#include<sys/file.h>
#include <sys/ioctl.h>


#ifndef DAEMON_H
#define DAEMON_H

#ifndef TRUE
	#define 	TRUE	1
	#define		FALSE	! TRUE
#endif

class daemon 
{
	public:
		     	    	daemon();
				virtual	~daemon();
		     	void	start(const int ignsigcld = 1);
	protected:
		virtual void 	run(void) = 0 ; // pure
				char	*usrName();
				void	usrName(const char *newName );
				char	*usrHome();
				void	usrHome(const char *newHome);
				const char * 	daemonName();
				void	daemonName(const char *newdeamonName);
				const char * 	msg();
				void	msg(const char *newMsg);

				signed	timerVal();
				void	timerVal(const signed newTime);
				


		virtual void 	logMsg();
		virtual int 	mailUser(const char *message);
		virtual int 	makeRounds(void) = 0 ; // pure
	    virtual	fstream *	logFile() = 0; // pure
		virtual	void	logFile(fstream *newLog) = 0; // pure
		virtual	char 	*logFileName() = 0; // pure
		virtual	void	logFileName(const char *newLogName) = 0; // pure

				unsigned	_timerVal;
				char	*_usrName;
				char	*_usrHome;
				fstream	*_logFile;
				char	*_logFileName;
				char	*_msg;
				char    *_daemonName;



	private:
				SIG_PF sig_child();

};
#endif

/* inline methods */

inline  const char *   daemon::daemonName(){ return _daemonName;}
inline  void	daemon::daemonName(const char *newdaemonName )
{ 
		char *tmpHold = NULL;
	if (_daemonName)
		tmpHold = _daemonName;
	_daemonName =  new char[strlen(newdaemonName) + 1];
	if (_daemonName)
		strcpy(_daemonName, newdaemonName);
	if (tmpHold)
		delete[] tmpHold;
}
inline  const char *   daemon::msg(){ return _msg;}
inline  void	daemon::msg(const char *newMsg )
{ 
		char *tmpHold = NULL;
	if (_msg)
		tmpHold = _msg;
	_msg =  new char[strlen(newMsg) + 1];
	if (_msg)
		strcpy(_msg, newMsg);
	if (tmpHold)
		delete[] tmpHold;
}
inline	char*	daemon::usrName(){return _usrName;}

inline	void	daemon::usrName(const char *newName)
{	
		char *tmpHold = NULL;
	if (_usrName)
		tmpHold = _usrName;
	_usrName = new char[strlen((char *)newName) + 1];
	if (_usrName)
		strcpy(_usrName, (char *)newName);
	if (tmpHold)
		delete[] tmpHold;
}
inline	char*	daemon::usrHome(){return _usrHome;}
inline	void	daemon::usrHome(const char *newHome)
{	
	char *tmpHold = NULL;
	if (_usrHome)
		tmpHold = _usrHome;
	_usrHome = new char[strlen(newHome) + 1];
	if (_usrHome)
		strcpy(_usrHome, newHome);
	if (tmpHold)
		delete[] tmpHold;
}
inline	signed	daemon::timerVal(){ return _timerVal;}
inline	void	daemon::timerVal(const signed newTime){ _timerVal = newTime;}
