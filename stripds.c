#include <stdio.h>
#include <strings.h>

main(int argc, char *argv[])
{
	char buffer1[BUFSIZ] = "";
	char *wordptr = NULL;
	
	printf ("<P>\n");
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
				printf("<BR>\n");   
				fflush(stdout);
			}
			*wordptr++;
		}
	}
	printf("<BR>\n");   
	printf ("<\/P>\n");
	fflush(stdout);

}

