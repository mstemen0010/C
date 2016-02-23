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
#include <utmp.h>
#include <unistd.h>
#include <bstring.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include "daemon.h"
#include "table.h"

enum behavior
{
	False,
	True,
	Heel = 'h',
	Prowl = 'p',
	Hunt = 'j',
	Guard = 'g',
	Mask = 127,
	Time = 't',
	Filesys = 'd',
	Utmp = 'u',
	Wmail = 'm',
	Wsound = 's',
	Wlog = 'l',
	Wconsole = 'c',
	Gutmp = 'y',
	Gfilesys = 'z'
};

const int	Nobehavior = Mask;
const int	Noguardtype = Mask;


class spike : public virtual daemon
{
	public:
		spike(int numOptions, char *options[]);
		~spike();

		virtual int	makeRounds();
		virtual fstream *  logFile(); 
        virtual void    logFile(fstream *newLog); 
        virtual char    *logFileName(); 
        virtual void    logFileName(const char *newLogName); 
		void setup(int numOptions, char *options[]);
		void bark();
		char* barkSound();
		void barkSound(char *newSound);         	
		long active();
		void active(long newBehavior);         	
		long passive();
		void passive(long newBehavior);         	
		long & guardType();
		void guardType(long newBehavior);         	
		void usage(char * msg);
		int warnWSound();
		void warnWSound(const int & newLogic);         	
		int warnWMail();
		void warnWMail(const int & newLogic);         	
		int warnWLog();
		void warnWLog(const int & newLogic);         	
		int warnWConsole();
		void warnWConsole(const int & newLogic);         	
		uid_t  watchedUid() const;
		char  *watchedUserName() const;
		char *uidCnvt(uid_t uidToCnvt);
		passwd *uidCnvt(char *usrNameToCnvt);
		void	run();
		
	private:
		int sniff( uid_t & current);
		int sniff( DIR * directory);
		int sniff( utmp & utmpStruct);
		int sniff( char * currentUserName);
		passwd * watchedUser();
		void watchedUser(char * user);
		char *watchWhat();
		void watchWhat(char * what);
		char *guardWhat();
		void guardWhat(char * what);
		table  *guardTable;
		long 	_active;
		long 	_passive;
		long   _guardType;
		int 	_warnWSound;
		int 	_warnWMail;
		int 	_warnWLog;
		int 	_warnWConsole;
		passwd * _watchedUser;
		char 	*_watchWhat;
		char 	*_guardWhat;
		char	*_barkSound;

};

inline long spike::active(){ return _active; }
inline void spike::active(long newBehavior)
{ 
	_active = _active & newBehavior; 
	if (_active == (Hunt & Guard))
		usage("You may not set both -j and -g");
}
inline long spike::passive(){ return _passive; }
inline void spike::passive(long newBehavior)
{ 
	_passive = _passive & newBehavior; 
	if (_passive == (Heel & Prowl))
		usage("You may not set both -h and -p");
}
inline long & spike::guardType(){ return _guardType; }
inline void spike::guardType(long newType)
{ 
	_guardType = _guardType & newType; 
	if (_guardType == (Gutmp & Gfilesys))
		usage("You may not set both -u and -f");
}
inline	char * spike::watchWhat(){ return _watchWhat;}
inline	char * spike::guardWhat(){ return _guardWhat;}
inline  void spike::guardWhat(char * what)
{
	char *tmpHold = NULL;
	if(_watchWhat)
		tmpHold = _guardWhat;
	_guardWhat = new char[strlen(what) + 1];
	if (_guardWhat)
		strcpy(_guardWhat, what);
	if (tmpHold)
		delete[] tmpHold;
}
inline	char * spike::barkSound(){ return _barkSound;}
inline  void spike::barkSound(char * newSound)
{
	char *tmpHold = NULL;
	if(_barkSound)
		tmpHold = _barkSound;
	_barkSound = new char[strlen(newSound) + 1];
	if (_barkSound)
		strcpy(_barkSound, newSound);
	if (tmpHold)
		delete[] tmpHold;
}
inline  void spike::watchWhat(char * what)
{
	char *tmpHold = NULL;
	if(_watchWhat)
		tmpHold = _watchWhat;
	_watchWhat = new char[strlen(what) + 1];
	if (_watchWhat)
		strcpy(_watchWhat, what);
	if (tmpHold)
		delete[] tmpHold;
}
inline int spike::warnWSound(){ return _warnWSound;} 
inline void spike::warnWSound(const int & newLogic)
{ 
	if(newLogic == True) 
		_warnWSound = newLogic;
	else 
		_warnWSound = False;
} 
inline int spike::warnWMail(){ return _warnWMail;} 
inline void spike::warnWMail(const int & newLogic)
{ 
	if (newLogic == True) 
		_warnWMail = newLogic;
	else 
		_warnWMail = False;
} 
inline int spike::warnWLog(){ return _warnWLog;} 
inline void spike::warnWLog(const int & newLogic)
{ 
	if(newLogic == True) 
		_warnWLog = newLogic;
	else 
		_warnWLog = False;
} 
inline int spike::warnWConsole(){ return _warnWConsole;} 
inline void spike::warnWConsole(const int & newLogic)
{ 
	if(newLogic == True) 
		_warnWConsole = newLogic;
	else 
		_warnWConsole = False;
} 
inline uid_t spike::watchedUid() const{return (_watchedUser->pw_uid);}
inline char * spike::watchedUserName() const{return (_watchedUser->pw_name);}
inline  fstream* spike::logFile(){return  _logFile;}
inline  char*   spike::logFileName(){return _logFileName;}
inline  void    spike::logFile(fstream *newLogFile)
{
    fstream *tmpHold = NULL;
    if (_logFile)
        tmpHold = _logFile;
    if (logFileName())
	{
    	_logFile = newLogFile; 
    	if (tmpHold)
        	delete[] tmpHold;
	}
}  
inline  void    spike::logFileName(const char *newLogName)
{
    char *tmpHold = NULL;
    if (_logFileName)
        tmpHold = _logFileName;
    _logFileName = new char[strlen(newLogName) + 1];
    if (_logFileName)
        strcpy(_logFileName, newLogName);
    if (tmpHold)
        delete[] tmpHold;
}  
