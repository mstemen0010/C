#INCLUDE <stdio.h>
#include <strings.h>

main(int argc, char *argv[])
{
	char buffer1[BUFSIZ * 2] = "";
	char *wordptr = NULL;
	int counter = 0;
	char *envval = "STARTCHAR";
	char *tempCounter;
	char *acounterPtr = "";
	char acounter[BUFSIZ];

	if (getenv(envval))
	{		
		acounterPtr = (char *)getenv(envval); 
		counter = toascii(*acounterPtr);
	}
	else
		counter = 65; /* acsii "A" */
	

	sprintf(acounter, "%c", counter);
	printf ("<A HREF = \"#%s\">", acounter);
	fflush(stdout);
	while (read(fileno(stdin), buffer1, sizeof(buffer1)))
	{
		wordptr = buffer1;
		while (*wordptr != '\0')
		{
			if (*wordptr != '\n')
			{
				putc(*wordptr, stdout);
				fflush(stdout);
			}
			else
			{
				counter++;
				sprintf(acounter, "%c", counter);
				printf("</A><BR>\n<A HREF = \"#%s\">", acounter);   
				fflush(stdout);
			}
			*wordptr++;
		}
	}
	printf("</A><BR>\n"); 
	fflush(stdout);

}

