#include <stdio.h>

main()
{
	char s[BUFSIZ];
	long hval;
	while (gets(s))
	{
		if (isalpha(s[0]) ||  isalpha(s[1]))
		{

			hval = strtol(s, (char**)NULL, 16);
			printf("Hex: %x Dec: %d Octal: %o\n", hval, hval, hval);	
		}
		else
			printf("Hex: %x Dec: %d Octal: %o\n", atoi(s), atoi(s),atoi(s));	
	}

}
