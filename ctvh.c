#include <stdio.h>

main()
{
	char s[BUFSIZ];
	long hval;
	while (gets(s))
	{

		hval = strtol(s, (char**)NULL, 16);
		printf("Dec: %d Octal: %o\n", hval, hval, hval);	
	}

}
