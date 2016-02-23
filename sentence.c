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

		*wordptr = toupper(*wordptr);
		wordptr++;
			 
		if (isupper(*wordptr))
		{
			while (*wordptr != '\0')
			{

				if (index > 1)
				{
					lastptr = &buffer2[index - 1];
					if (*lastptr != '.' && *lastptr != '!' && *lastptr != '?')
						*wordptr = tolower(*wordptr);
				}
				else
					*wordptr = tolower(*wordptr);
				wordptr++;
				index++;
			}
		}
		else
		{
			while (*wordptr != '\0')
			{
				if (index > 1)
				{
					lastptr = &buffer2[index - 1];
					if (*lastptr == '.' || *lastptr == '!' || *lastptr == '?')
						*wordptr = toupper(*wordptr);
				}
				wordptr++;
				index++;
			}
		}

	}
	for (i = 0; i <= index; i++)
		wordptr--;
	printf("%s", wordptr);

}

