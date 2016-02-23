#include <stdio.h>
#include <strings.h>

main(int argc, char *argv[])
{
	char buffer1[BUFSIZ] = "";
	char *wordptr = NULL;
	int	  npCount = 0;
	
	fflush(stdout);
	while (read(fileno(stdin), buffer1, sizeof(buffer1)))
	{
		wordptr = buffer1;
		while (*wordptr != '\0')
		{
			if (*wordptr != '\n')
			{
				if (! isascii(*wordptr))
					npCount++;

			}
			*wordptr++;
		}
	}
	printf ("Found %d nonprintable characters\n", npCount);
	fflush(stdout);

}

