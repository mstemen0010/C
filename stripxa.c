#include <stdio.h>
#include <strings.h>

main(int argc, char *argv[])
{
	char buffer1[BUFSIZ] = "";
	char *wordptr = NULL;
	
	fflush(stdout);
	while (read(fileno(stdin), buffer1, sizeof(buffer1)))
	{
		if (stillGood == NEXT_TO_LAST)
			stillGood = LAST;
		wordptr = buffer1;
		while (*wordptr != '\0')
		{
			if (*wordptr != '\n')
			{
				if (isascii(*wordptr))
				{
					putc(*wordptr, stdout);
					fflush(stdout);
				}
				else
					putc(' ', stdout);

			}
			else
			{
				printf("\n");   
				fflush(stdout);
			}
			*wordptr++;
		}
		stillGood = read(fileno(stdin), buffer1, sizeof(buffer1))
		if (stillGood)
	}
	fflush(stdout);

}

