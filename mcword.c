#include <stdio.h>
#include <strings.h>

main(int argc, char *argv[])
{
	char buffer1[BUFSIZ] = "";
	char buffer2[BUFSIZ] = "";
	char *wordptr = NULL;
	char *token, *lastptr;
	int  i, index = 0;
	
	
	while (read(fileno(stdin), buffer1, sizeof(buffer1)))
	{
			strcpy(buffer2, buffer1);
			wordptr = buffer1;	
			wordptr++;
			while (*wordptr != '\0')
			{

				lastptr = &buffer2[index]; 
				if (isupper(*lastptr))
					*wordptr = tolower(*wordptr);
				wordptr++;
				index++;
			}
			for (i = 0; i <= index; i++)
				wordptr--;
			printf("%s", wordptr);
	}

}

