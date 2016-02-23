#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#define USAGE "usage: fetchline [-n line number] | [-s string to search for] [filename]"
#define FIND_LINE 1
#define FIND_STR  2

void usage(char *errMessage);
main(int argc, char *argv[])
{
			FILE	*infp;
			char	*fileName, *fetchNum;
			char	str2Fetch[BUFSIZ];
			int	line2Fetch = 0, currentLine = 0;
			int	searchType = NULL, opt;
			int found = 0;
			char 	lineRead[BUFSIZ], errmsg[BUFSIZ];
	struct		stat	statbuff;
	static		int	errorno;
	extern		char	*optarg;
	extern 		int	optind;



	if (argc > 3)
	{
		if (argv[1])
		{
			fileName = argv[1];
			if (argv[2])
			{
				/* look at args */
				while ((opt = getopt(argc, argv, "s:n:")) != -1)
				{
					switch(opt)
					{
						case 's':
							searchType = FIND_STR;	
							if (optarg)
								strcpy (str2Fetch, optarg);
							else
								line2Fetch = atoi(optarg);
							break;
	
						case 'n':
							searchType = FIND_LINE;
							if (optarg)
								line2Fetch = atoi(optarg);
							else
								usage("missing argument");
							break;
	
						case '?':
							usage(NULL);
							break;
					} /* end switch */
				} /* end while */

				if (searchType)
				{
					/* get the remaining arg */
					if (argv[argc -1])
					{
						strcpy(fileName, argv[argc -1]);
					}
					else
						usage("missing filename");
						
					if (stat(fileName, &statbuff) == 0)
					{
						switch (statbuff.st_mode & S_IFMT)
						{
							case S_IFREG:
								if (infp = fopen (fileName, "r"))
								{
									switch (searchType) 
									{
										case FIND_LINE:
											/* 
											while (currentLine++ != line2Fetch)
											{
												fgets(lineRead, BUFSIZ, infp);
											}
											*/
											while (fgets(lineRead, BUFSIZ, infp))
											{
												if (currentLine >= line2Fetch)
												{
													found = 1;
													break;
												}
												currentLine++;
											}
											if (found)
												fprintf(stdout, "%s", lineRead);
											else
												fprintf(stdout, "%d", currentLine);
											fclose(infp);
											break;

										case FIND_STR:
											while (fgets(lineRead, BUFSIZ, infp))
											{
												if (strstr(lineRead, str2Fetch))
													fprintf(stdout, "%s", lineRead);
											}
											fclose(infp);
											break;
									}
								}
								else
								{
									sprintf(errmsg, "Unable to open file:\"%s\"", fileName);
									perror(errmsg);
								}
								break;
	
							default:
								sprintf(errmsg, "\"%s\" is not a regular file.", fileName);
								usage(errmsg);
								break;
						} /* end switch */
	
					} /* end if stat */
					else
						perror("Unable to open file:");
								
						
				} /* end if searchType */
				else
					usage("invaild search type");
			} /* end if argv[2] */
			else
				usage(NULL);

		} /* end if argv[1] */
		else
			usage(NULL);
	} /* end if argc > 1 */
	else
		fprintf(stderr, "%s\n", USAGE);


	return 1; 
}
void usage(char *errMessage)
{
	if (errMessage)
		fprintf(stderr, "\n\n%s", errMessage);
	fprintf(stderr, "\n\n%s\n\n", USAGE);
	exit (-1);
}

		
