/*
** File: %M% SCCS %I% - %G%
**
** Author:   Matthew Stemen
**
**
** 
** Created:  18 July 1995
**  
**
**
** Purpose: Monitors and reports any and all remote logins to the 
**          host it is run from.  This activity is reported either 
**          using Sound, Images(icons), and Tabular reports or any
**          combination thereof.
*/


#include "spike.h" 

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


spike::spike(int numOpts, char * options[])
{
	extern	char	*optarg;
	extern	int		optind;
	struct	stat	statbuf;
	int				opt; 
	char 			message[BUFSIZ];
    char 			tName[BUFSIZ] = "";

	_active = Nobehavior;
	_passive = Nobehavior;
	_guardType = Noguardtype;

	daemonName("spike");

	if (numOpts > 7) /* check args for min. number */
	{

		while ((opt = getopt(numOpts, options, "j:g:f:u:d:t:s:mlchp")) != -1)
		{
			switch(opt) {
				case Utmp:
					guardType((long )Gutmp);
					if (optarg)
					{
						if (! stat(optarg, &statbuf))
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFREG)
							{
								char message[BUFSIZ];
								sprintf(message, "\"%s\" is NOT a valid utmp file", optarg);
								usage(message);
							}
							else
								watchWhat(optarg);
						}			
						else
						{
								sprintf(message, "Unable to stat \"%s\" ", optarg);
								perror("error was:");
								usage(message);
						}

					}
					else
					{
						sprintf(message, "Invalid Argument: %s", optarg);
						usage(message);
				 	}		
				break;

				case Filesys:
					char *pathName = "";
					guardType((long )Gfilesys);
					if (optarg)
					{
						if (strcmp(optarg, ".") == 0)
						{
							pathName = getcwd(NULL, BUFSIZ - 1);
						}
						else if (strcmp(optarg, "~") == 0)
						{
							pathName = getenv("HOME");
						}
						else
							pathName = optarg;
							
						if (! stat(pathName, &statbuf))
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFDIR)
							{
								sprintf(message, "\"%s\" is NOT a vaild directory", pathName);
								usage(message);
							}
							else
								watchWhat(pathName);
						
						}			
						else
						{
								sprintf(message, "Unable to stat \"%s\" ", pathName);
								perror("error was:");
								usage(message);
						}

					}
					else
					{
						sprintf(message, "Invalid Argument: %s", optarg);
						usage(message);
				 	}		
				break;
					
				case Heel:
					passive((long )Heel);
				break;

				case Prowl:
					passive((long )Prowl);
				break;

				case Guard:
					if (optarg)
					{	
						active((long)Guard);
						if (! stat(optarg, &statbuf))
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFREG)
							{
								sprintf(message, "\"%s\" is NOT a vaild user file", optarg);
								usage(message);
							}
							else
							{
								guardWhat(optarg);
								guardTable = new table(guardWhat());
							}

							
						}
					}
					break;
						
						
					

				case Hunt: 
					if (optarg)
					{
						active((long )Hunt);
						_watchedUser = new passwd;
						watchedUser(optarg);
					}
					else
					{
						usage("Invalid Argument");
				 	}		
				break;

				case Time:
					if (optarg)
						timerVal(atoi(optarg));
					else
					{
						usage("Invalid Argument");
				 	}		
				break;

				case Wsound:
					if (optarg)
					{	
						warnWSound(True);
						_barkSound = NULL;
						if (! stat(optarg, &statbuf))
						{
							if ( (statbuf.st_mode & S_IFMT) != S_IFREG)
							{
								sprintf(message, "\"%s\" is NOT a vaild sound file", optarg);
								usage(message);
							}
							else
							{
								barkSound(optarg);
							}

							
						}
						else
							usage("You must supply a sound file (.au) with the -s");
					}
					else
						usage("You must supply a sound file (.au) with the -s");
					break;

				case Wmail:
					warnWMail(True);
				break;

				case Wlog:
    				// char *tName = new char[BUFSIZ];
    				sprintf(tName, "%s/%s_%s.log", usrHome(), usrName(), daemonName());
    				logFileName(tName);
    				logFile(new fstream(logFileName(), ios::app));
    				if ( ! ( logFile()->good())) 
        				cout << "Unable to open log file:\"" << logFileName() << "\" \n";
					else
						warnWLog(True);
				break;

				case Wconsole:
					warnWConsole(True);
				break;


				case '?':

					usage("");

				break;

				default:

					usage("error:");

				break;
			} /* end, opt switch */
		} /* end, while getopt */
		if ((passive() != Heel) && (passive() != Prowl))
			usage("You must select either -h or -p");
		if ((active() != Guard) && (active() != Hunt))
			usage("You must select either -g or -j");
		if ((guardType() != Gutmp) && (guardType() != Gfilesys))
			usage("You must select either -u or -d");
		if (timerVal() < 0)
			usage("You must set a timer value");
		if (timerVal() == 0)
			usage("You must set a timer value greater than 0");
	}
	else
		usage("");
} /* end of setup */
spike::~spike()
{
	delete guardTable;
	delete[] _watchedUser;
}
char *spike::uidCnvt(uid_t uidToCnvt)
{
	
	passwd	*temppwstruct = NULL;
	char 	*tempName = NULL;

   	temppwstruct = getpwuid(uidToCnvt);
	if(temppwstruct)
		tempName = temppwstruct->pw_name;

	return(tempName);
	
}
passwd *spike::uidCnvt(char *usrNameToCnvt)
{
   	passwd 	*temppwstruct = NULL;
	char	*tempName = NULL;	
	char 	 userName[BUFSIZ];
	

   	temppwstruct = getpwnam(usrNameToCnvt);
	if ( ! temppwstruct )
	{
		// we may have been passed a UID
		tempName = 	uidCnvt((uid_t)atol(usrNameToCnvt));
		if (tempName)
		{
			strcpy(userName, tempName);
   			temppwstruct = getpwnam(userName);
		}
	}

	return(temppwstruct);
}

void spike::watchedUser(char * user)
{
	
	char	message[BUFSIZ];


	if (! (_watchedUser = uidCnvt(user) ))
	{
		sprintf(message, "No such user as : %s", user);
		usage(message);
	}
}
void spike::run()
{
	char message[BUFSIZ] ="";
	if (active() == Hunt)
		sprintf(message, "Spike started[pid %d], looking for user:%s uid:%d in \"%s\"\n", getpid(),watchedUserName() ,watchedUid(), watchWhat() );
	else
		sprintf(message, "Spike started[pid:%d], guarding \"%s\", using \"%s\" as the authorization file\n ", getpid(),watchWhat() ,guardWhat() );
	msg(message);
	bark();
	while(makeRounds())
	{
		sleep((unsigned) timerVal());
	}			
}

int spike::makeRounds()
{

	int		keepRunning = TRUE;
	DIR 	*dirp;
	int		smellUser = USERNOTFOUND;

	if (guardType() == Gfilesys)
	{
		dirp = opendir(watchWhat());
		if (dirp)
		{
			if (sniff(dirp))
			{	
				if (passive() == Heel)
					keepRunning = False;
				else
					keepRunning = True;
			}
		} 
		closedir(dirp);

	}
	if (guardType() == Gutmp)
	{
				FILE 	*utmpFp;
		struct	utmp	ulist;

		if (utmpFp = fopen(watchWhat(), "r")  )
		{
      		while ( fread((char *)&ulist, sizeof(ulist), 1, utmpFp) ) 
			{
				if (sniff(ulist))
				{
					char message[BUFSIZ];
					if (active() == Hunt)
						sprintf(message, "Spike: found user %s (uid: %d) in \"%s\" ", _watchedUser->pw_name, _watchedUser->pw_uid, watchWhat());
					if (active() == Guard)
					{
						passwd *temppwstruct = uidCnvt((char *)ulist.ut_name); 
						if (temppwstruct)
							sprintf(message, "Spike: found user %s (uid: %d)", ulist.ut_name, temppwstruct->pw_uid);
						else
							sprintf(message, "Spike: found user %s (unable to get uid)", ulist.ut_name);
					}
					msg(message);
					bark();
					if (passive() == Heel)
					{
						keepRunning = False;
					}
					else
						keepRunning = True;
				}
			} 

		}
	}	

	return(keepRunning);
				
} /* end, makeRounds */				


int spike::sniff(utmp & utmpStruct)
{
	int status = USERNOTFOUND;

	if (utmpStruct.ut_name)	
	{
		if ( strcmp (utmpStruct.ut_name, ""))
		{
			switch(active())
			{
				case Hunt:
					if (utmpStruct.ut_name)
					{
						status = sniff(utmpStruct.ut_name);
					}
				break;

				case Guard:
					 if ( ! (guardTable->find(utmpStruct.ut_name)) ) 	
						status = USERFOUND;
				break;
			}
		}	
	}
	return(status);
}
int spike::sniff(char * currentUsrName)
{
	int status = USERNOTFOUND;
	if (currentUsrName)
	{
		int difference = 0;
		difference = strcmp(currentUsrName, (char *)watchedUserName());
		if (! difference)
			status = USERFOUND;
	}
	return(status);
}
int spike::sniff(uid_t & currentUid)
{
	int status = USERNOTFOUND;
	if (watchedUid() == currentUid) 
	{
		status = USERFOUND;
	}
	return(status);
} /* end, sniffUser */

int spike::sniff(DIR * directory)
{
			int 	status = USERNOTFOUND;
	struct 	dirent 	*direntp;
	struct 	stat	statbuf;
			char	filePath[BUFSIZ];
			int		fileCount = 0;
			
	rewinddir(directory);
	while ( ( direntp = readdir(directory)) )
		fileCount++;
			

	table *intTable = new table(fileCount);
	
	rewinddir(directory);
	switch(active())
	{
		case Hunt:
			while ( ( direntp = readdir(directory)) )
			{
				if (direntp->d_name)
				{
					sprintf(filePath,"%s/%s", watchWhat(), direntp->d_name);
					if (stat(filePath, &statbuf) == 0)
					{
						if ((status = sniff(statbuf.st_uid)))
						{
							char message[BUFSIZ];
							// sprintf(message, "Spike: found user %s (uid: %d)", uidCnvt(statbuf.st_uid), _watchedUser->pw_uid);
							sprintf(message, "Spike: found user %s (uid: %d) in \"%s\" ", _watchedUser->pw_name, _watchedUser->pw_uid, watchWhat());
							msg(message);
							bark();
							break;
						}
					}
				}
			}
		break;
		
		case Guard:

			while (( direntp = readdir(directory)) )
			{
				if(direntp->d_name)
				{
					sprintf(filePath,"%s/%s", watchWhat(), direntp->d_name);
					if (stat(filePath, &statbuf) == 0)
					{
						// need to convert uid to usr name
						char *currentUserPtr;
						char currentUser[BUFSIZ];

						if (currentUserPtr = uidCnvt(statbuf.st_uid));
						{
							strcpy(currentUser, currentUserPtr);
					   		if (! guardTable->find(currentUser)) 	
							{
								if (! intTable->find(currentUser))
								{
									intTable->add(currentUser);
									char *userName = uidCnvt(statbuf.st_uid); 
									char message[BUFSIZ];
									if (userName)
										sprintf(message, "Spike: found user %s (uid: %d)", userName, statbuf.st_uid);
									msg(message);
									bark();
								}
							}
						}
					}		
				}
		
			}
		break;
	}
		delete intTable;
	return(status);

} /* end, sniffUser */

void spike::bark()
{

	if (warnWSound())
	{
		char cmd[BUFSIZ] = "";
		sprintf(cmd, "cat %s > /dev/audio", barkSound());
		system(cmd);
	}
	if (warnWLog())
	{
		time_t startTime;
		startTime = time(NULL);
        char* rawdate = (char *)ctime(&startTime);
        char date[BUFSIZ] = "";
        bcopy (rawdate, date, (strlen(rawdate) - 1));
        char tmessage[BUFSIZ] = "";
        sprintf(tmessage, "%s:%d@%s - %s\n",daemonName(), getpid(), date, msg());
        msg(tmessage);
		logMsg();
	}
	if (warnWMail())
		mailUser(msg());
	if (warnWConsole())
	{
		fstream console("/dev/console", ios::out);
		if ( ! console.good())
		{
			msg( "Unable to open /dev/console");
			logMsg( );
		}
		else
		{
			console << msg() << "\n";
			console.flush();
			console.close();
		}
	} /* end, watchConsole */

} /* end, bark */


void spike::usage(char *msg)
{
	cout << "\n" << "\n" << msg << "\n" << " \n";
	cout << "usage: spike  [-j (uid or name) or -g (file of list of `authorized` users)]\n";
	cout << "[-d (directory name) or -u (utmp file name)]  [-t (sleep time)]\n" ;
  	cout << "[-h (stop after warning) or -p (continue to warn until program is ended)]\n";
	cout << "[options]\n";
	cout << "options\n"; 
	cout << "------\n"; 
	cout << "-s	warn with sound (bark) \n";
	cout << "-m	warn with mail  ( sent to your mailbox) \n";
	cout << "-l	warn with log entry (kept in your home directory) \n";
	cout << "-c	warn to your console window  \n\n";

	cout << "NOTE: All of the arguments may be specified in any combination.\n";
	cout << "      spike may not be given conflicting behavior, you may use \"-h\" or \"-g\".  \n \n";

	exit(0);
} /* end, usage */
