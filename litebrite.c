#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define SCALE 32768.0


void printfBack(char wordToPrint[BUFSIZ], long time);

static short seed = 1;


short rand1(void);

void  srand1(short x);

int randomizePlusN(int startNum, int maxNum);

void 	flipIt(int numTimes);

void strobeIt(int numTimes, char *strobChar, int Width, int delay);

void danceIt(int numTimes, char *danceChar, int Width, int delay);

void clockIt(int time);

void pongIt(int numTimes, char *ballChar, int Width, int delay);

void talkIt(int numTimes, char *wordString, int delay);

void writeIt(int numTimes, char *wordString, int delay);

void slotIt(int numTimes, int width, int delay); 

void justDoIt(int 	entertainType, int optiArg, char *optcArg, int numTimes, int width, int delay);

void clearPrompt();

void usage(char *msg);

enum argTypes
{
	False,
	True,
	Pong = 'p',
	Flip = 'f',
	Talk = 't',
	Dance = 'd',
	Strobe = 's',
	Clock = 'c',
	Slot = 'l',
	Write = 'w',
	Width = 'W',
	Delay = 'D',  
	numTimes = 'I'
};

main(int argc, char *argv[])
{
	extern 	char *optarg;
	extern 	int	optind;
			int	opt;
			int i = 0;
			int width = 10;
			int	numtimes = 1;
			int delayTime = 0;
			char *passString = NULL;
			int	 passInt = 0;
			int	entertainType = 0;

	if (argc > 2)
	{
    	while ((opt = getopt(argc, argv, "p:f:t:d:s:t:W:I:D:c:w:l:")) != -1)
    	{
    		switch(opt) {
	
				case numTimes:
					if (optarg)
						numtimes = atoi(optarg);
				break;
	
				case Delay:
					if (optarg)
						delayTime = atoi(optarg);
				break;

				case Width:
					if (optarg)
						width = atoi(optarg);
				break;
					
				case Flip:
					
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Flip;
					if(optarg)
						passInt = atoi(optarg);
					else
						passInt = 10;
				break;
	
				case Pong:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Pong;
					if(optarg)
						passString = optarg;
					else
						*passString = 'X';
				break;

				case Clock:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Clock;
					if(optarg)
						passInt = atoi(optarg);
					else
						passInt = 60;
				break;
				
				case Dance:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Dance;
					if(optarg)
						passString = optarg;
					else
						*passString = 'X';
				break;
	
				case Write:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Write;
					if(optarg)
						passString = optarg;
				break;

				case Talk:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Talk;
					if(optarg)
						passString = optarg;
				break;
	
				case Slot:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Slot;
					if(optarg)
						passInt = atoi(optarg);
					else
						passInt = 3;
				break;
				case Strobe:
					if (entertainType)
						usage("You may only select one type");
					else
						entertainType = Strobe;
					if (optarg)
						passString = optarg;
					else
						*passString = '-';
				break;

				
			}
		}
		justDoIt(entertainType, passInt, passString, numtimes, width, delayTime );
		clearPrompt();
		sleep(1);
	}
	else
		usage(NULL);

}


void justDoIt(int 	entertainType, int optiArg, char *optcArg, int numTimes, int width, int delayTime)
{
	clearPrompt();
	switch(entertainType)
	{
		case Pong:
			pongIt(numTimes, optcArg, width, delayTime);
		break;

		case Dance:
			danceIt(numTimes, optcArg, width, delayTime);
		break;

		case Clock:
			clockIt(optiArg);
		break;

		case Flip:
			flipIt(optiArg);
		break;
	
		case Strobe:
			strobeIt(numTimes, optcArg, width, delayTime);
		break;
		case Talk:
			talkIt(numTimes, optcArg, delayTime);
		break;
		case Write:
			writeIt(numTimes, optcArg, delayTime);
		break;
		case Slot:
			slotIt(numTimes, optiArg, delayTime);
		break;
	}
}

void slotIt(int numTimes, int width, int delay)
{
    int needR[80];
    int rArray[80];
    int r1, r2, r3, column, duration;
    int needR1 = 1, needR2 = 1, needR3 = 1;
    char tempStr[BUFSIZ];
    char tempNum[BUFSIZ];
    char firstLine[BUFSIZ] = "";
    int n, i, j, index, times;
	int	allMatch = True;
   
	
	for (times = 0; times < numTimes; times++)
	{
	    for (i = 0; i < width; i++)
	        needR[i] = 1;
	    srand1((short) time(NULL));
	 
	/*	
	    for (i = 0; i < (width * 5 + 2); i++)
	        strcat(firstLine, "|");
	     printf("%s\n", firstLine);
		*/
	    for (column = 0; column < width; column++)
	    {
	        /*  duration = randomizePlusN(20, 5); */
	        duration = 25;
	        for (i = 0; i < duration; i++)
	        {
	   
	            strcpy(tempStr, "");
	            for (j = 0; j < width; j++ )
	            {
	                if (needR[j])
	                    rArray[j] = randomizePlusN(32, 6);
	                strcat(tempStr, "||");
	                sprintf(tempNum, " %c ", rArray[j]);
	                strcat(tempStr, tempNum);
				}
	            strcat(tempStr, "||");
	            printfBack(tempStr, 28/width);
	        }
			
	        if (column == index)
	            needR[index] = 0;
	    
	        index++;
	    }
	
		for(j=1; j< width; j++)	
		{
			if (! (rArray[j] == rArray[j - 1]))
				allMatch = False;
		}
		if (allMatch)
		{
			strcat( tempStr, "  JACKPOT !!");
	        printfBack(tempStr, 25);
		}
	    sleep(delay);
	}
    
}

void writeIt(int numTimes, char *wordString, int delay)
{
	char lenString[BUFSIZ];
	char line[BUFSIZ];
	char strBuf[BUFSIZ];
	int	 duration = 0;
	int	 currLength = 0;
	int  maxLength = 0;
	int	 numWords = 0;
	int  pad = 0;
	int	 sequence = 0;
	int	 times = 0;
	char *token;
	char *lenToken;

	for (times = 0; times < numTimes; times++)
	{
		/* find the longest string */
		strcpy(lenString, wordString);
		lenToken = strtok(lenString, " ");
		while (lenToken)
		{
			numWords++;
			currLength = strlen(lenToken);
			if (currLength > maxLength)
				maxLength = currLength;
			lenToken = strtok(NULL, " ");
		}
		strcpy(line, wordString);
		token = strtok(line, " ");
	    while(token)
	    {
			strcpy(strBuf, token);	
			currLength = strlen(strBuf);
			for (pad = currLength; pad <= maxLength; pad++)
				strcat(strBuf, " ");
	        printfBack(strBuf, 10);
			token = strtok(NULL, " ");
		}
	    sleep(delay);
	}
}

void talkIt(int numTimes, char *wordString, int delay)
{
	char lenString[BUFSIZ];
	char line[BUFSIZ];
	char strBuf[BUFSIZ];
	int	 duration = 0;
	int	 currLength = 0;
	int  maxLength = 0;
	int	 faceLength = 0;
	int	 numWords = 0;
	int  pad = 0;
	int	 sequence = 0;
	int	 times = 0;
	char *token;
	char *lenToken;
	char *face1 = ":-O ";
	char *face2 = ":-| ";
	char *face3 = ":-) ";
	char *faceArray[3];

	faceArray[0] = face1;
	faceArray[1] = face2;
	faceArray[2] = face3;

	for (times = 0; times < numTimes; times++)
	{
		/* find the longest string */
		strcpy(lenString, wordString);
		lenToken = strtok(lenString, " ");
		while (lenToken)
		{
			numWords++;
			currLength = strlen(lenToken);
			if (currLength > maxLength)
				maxLength = currLength;
			lenToken = strtok(NULL, " ");
		}
		strcpy(line, wordString);
		token = strtok(line, " ");
		faceLength = strlen(face1);
	    while(token)
	    {
			for (duration = 0; duration <= ((strlen(token))/3); duration++)
			{
				for (sequence = 0; sequence <= 1; sequence++) 
				{
					strcpy(strBuf, faceArray[sequence]);
					for (pad = 0; pad < maxLength; pad++)
						strcat(strBuf, " ");
	            	printfBack(strBuf, 11);
				}
			}
			strcpy(strBuf, faceArray[1]);
			strcat(strBuf, token);	
			currLength = strlen(strBuf);
			for (pad = currLength; pad <= (maxLength + faceLength); pad++)
				strcat(strBuf, " ");
	        printfBack(strBuf, 60);
			token = strtok(NULL, " ");
		}
		strcpy(strBuf, faceArray[2]);
		for (pad = 0; pad < maxLength; pad++)
			strcat(strBuf, " ");
	    printfBack(strBuf, 60);
	    sleep(delay);
	}
}

void pongIt(int numTimes, char *ballChar, int width, int delay)
{
    int     cycle = 0;
    int     pos = 0;
    int     index = 0;
    int     i = 0;
    char    strBuf[BUFSIZ] = "";
 
    /* make sure we have a clear line */
 
    for (i = 0; i < numTimes; i++)
    {
        strcpy(strBuf, "");
        index = 1;
        for (cycle = 1; cycle < width * 2; cycle++)
        {
            if (cycle <= width )
            {
                strcpy(strBuf, "");
                for (pos = 1; pos <= width; pos++)
                {
                    if (pos == index)
                        strcat(strBuf, ballChar);
                    else
                        if (pos == 1 )
                            strcat(strBuf, "|");
                        else if (pos == width)
                            strcat(strBuf, "|");
                        else
                            strcat(strBuf, " ");
                }
                printfBack(strBuf, 5);
                index++;
            }
            else
            {
                strcpy(strBuf, "");
                for (pos = 1; pos <= width; pos++)
                {
                    if (pos == index)
                        strcat(strBuf, ballChar);
                    else
                        if (pos == 1 )
                            strcat(strBuf, "|");
                        else if (pos == width)
                            strcat(strBuf, "|");
                        else
                            strcat(strBuf, " ");
                }
                printfBack(strBuf, 5);
                index--;
            }
               
        }
	    sleep(delay);
    }
}
void clockIt(int time) 
{
    int     duration = 0;
    int     cycle = 0;
    int     pos = 0;
    int     i = 0;
	int		width = 10;
	int		timeNumber = 0;
	int		runTimeSeconds = 0;
	int		runTimeMinutes = 0;
	int		runTimeHours = 0;
	int		seconds = 0;
	char     tempString[BUFSIZ] = "";
	char     timeString[BUFSIZ] = "";
    char    strBuf[BUFSIZ] = "";
     
	if (time > 0)
	{
		for (duration = 0; duration < time; duration++)
		{
	        strcpy(strBuf, "");
	        // for (cycle = 1; cycle < width; cycle++)
		   //		sginap(6);
	        strcpy(strBuf, "");
			if (seconds > 0)
			{
				runTimeHours = seconds/3600;
				runTimeMinutes = seconds/60;
				runTimeSeconds = seconds%60;
			}

		    sprintf(tempString, "%02d:%02d:%02d", runTimeHours, runTimeMinutes, runTimeSeconds);
		    strcat(strBuf, tempString);
	        printfBack(strBuf, 1);
			sleep(1);
			seconds++;
		}
	}
	else
	{
		seconds = abs(time);
		for (duration = abs(time); duration > 0 ; duration--)
		{
	        strcpy(strBuf, "");
	        // for (cycle = 1; cycle < width; cycle++)
		    // sginap(60000);
	        strcpy(strBuf, "");
			if (seconds > 0)
			{
				runTimeHours = seconds/3600;
				runTimeMinutes = seconds/60;
				runTimeSeconds = seconds%60;
			}
	        sprintf(tempString, "%02d:%02d:%02d", runTimeHours, runTimeMinutes, runTimeSeconds);
	        strcat(strBuf, tempString);
	        printfBack(strBuf, 1);
			sleep(1);
			seconds--;
		}
	}
}

void danceIt(int numTimes, char *danceChar, int width, int delay)
{
	int		cycle = 0;
	int		pos = 0;
	int		index = 0;
	int		i = 0;
    char    strBuf[BUFSIZ] = "";
 
    /* make sure we have a clear line */
 
    for (i = 0; i < numTimes; i++)
    {
        strcpy(strBuf, "");
		index = 1;
        for (cycle = 1; cycle < width * 2; cycle++)
        {
			if (cycle <= width )
			{
        		strcpy(strBuf, "");
        		for (pos = 1; pos <= width; pos++)
				{
					if (pos == index)
            			strcat(strBuf, danceChar);
            		else
            			strcat(strBuf, " ");
				}
        		printfBack(strBuf, 5);
				index++;
			}
			else
			{
        		strcpy(strBuf, "");
        		for (pos = 1; pos <= width; pos++)
				{
					if (pos == index)
            			strcat(strBuf, danceChar);
            		else
            			strcat(strBuf, " ");
				}
        		printfBack(strBuf, 5);
				index--;
			}
 
        }
	    sleep(delay);
    }
}

void strobeIt(int numTimes, char *strobChar, int width, int delay)
{
	int		i = 0;
	int		index = 0;
	int		cycle = 0;
	int		pos = 0;
	char	strBuf[BUFSIZ] = "";

	/* make sure we have a clear line */

	for (i = 1; i <= numTimes; i++)
	{
		strcpy(strBuf, "");

		index = 0;
		for (cycle = 0; cycle <= width; cycle++)
		{
			if (cycle <= (width/2)) /* first half */
			{
				strcpy(strBuf, "");
				for (pos = 1; pos <= width; pos++)
				{
					if ( (pos >= ((width/2) - index)) && (pos <= ((width/2) + index + 1)))
						strcat(strBuf, strobChar);
					else
						strcat(strBuf, " ");
				}
				printfBack(strBuf, 20);
				index++;
			}
			else /* second half */
			{
				strcpy(strBuf, "");
				for (pos = 1; pos <= width; pos++)
				{
					if ( (pos <= ((width/2) + index)) && (pos >= ((width/2) - index + 1)))
						strcat(strBuf, strobChar);
					else
						strcat(strBuf, " ");
				}
				printfBack(strBuf, 20);
				index--;
			}
		}
	    sleep(delay);
	}
}
void flipIt(int numTimes)
{

	int i = 0;

	printf("\n");
	fflush(stdout);

	for (i = 0; i <= numTimes; i++)
	{
		printfBack("|", 10);
		printfBack("/", 10);
		printfBack("-", 10);
		printfBack("\\", 10);
	}
}

void clearPrompt()
{
	int	index = 0;

	for (index = 0; index <= 40; index++)
	{
		printf("%c", 8);
	}
}
void printfBack(char wordToPrint[BUFSIZ], long time)
{
	int		lenWord = 0; 
	int		index = 0;

	lenWord = strlen(wordToPrint);

	printf("%s", wordToPrint);	
	fflush(stdout);
	sleep(time);

	for (index = 0; index <= lenWord; index++)
	{
		printf("%c", 8);
		/* printf("%c", 127); */
		fflush(stdout);
	}
	fflush(stdout);

}

void usage(char *msg)
{
	if (msg)
		printf("%s\n\n", msg);
	printf("usage: litebrite [type (-f,-t,-s,-d,-p) argument] [options (-w,-i)]\n\n" );
	printf("type can be one of the following:\n\n");
	printf("-l (n) where \"n\" is the number of \"slots\" on the slot machine\n");
	printf("-f (n) where \"n\" is the of times to flip the bar\n");
	printf("-w (s) where \"s\" is a quoted string of characters that is written to the screen\n");
	printf("-t (s) where \"s\" is a quoted string of characters that the talking man says \n");
	printf("-p (c) where \"c\" is the ball character in the pong game\n"); 
	printf("-d (c) where \"c\" is the scanning character\n"); 
	printf("-s (c) where \"c\" is the strobing character\n"); 
	printf("-c (n) where \"n\" the number of seconds to count up or down (negative value for \"n\") \n\n");

	printf("In addition you may use the following as options:\n");
	printf("-I (n) where \"n\" is the number of iterations \n"); 
	printf("-W (n) where \"n\" is the field width of some of the effects(-p, -d, s)\n"); 
	printf("-D (n) where \"n\" is delay in seconds between the iterations \n\n"); 
	
	exit(1);
}

int randomizePlusN(int startNum, int maxNum)
{
 
    float   roll;
 
    
    roll = ((float) rand1()/SCALE + 1.0) * maxNum / 2.0 + 1.0;
    
    return(startNum + roll);
 
 
}
 
short rand1(void)
{
    seed = (seed * 25173 + 13849) % 65536;
    return(seed);
}
   
void srand1(short x)
{
    seed = x;
}
