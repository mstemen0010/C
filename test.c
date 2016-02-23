#include <stdio.h>

#ifndef TRUE
#define TRUE    1
#define FALSE   ! TRUE
#endif

struct person
{
	char 			value[10];
	struct	person	*nextPerson;
	struct  person  *firstPerson;
} info;

unsigned long makeHash(char *key, int modVal);

struct person** buildPersonList(unsigned numberOfPeople);

unsigned numLines(char *fileName);

int	findHash(char *key, int modVal, struct person **personList);

main()
{
	FILE					*infp;
	double 					inputd[50];
	long 					inputl[50];
	char					inputs[BUFSIZ];
	char					inputs2[BUFSIZ];
	char					outStr[BUFSIZ];
	double					inputl2;
	int						index = 0, numread = 0, found = FALSE;
	int						numLinked = 0;
	unsigned				lines = 0;
	struct		person		**whoList;
	struct 		person		*newPerson;

	lines = numLines("junk");

	whoList = buildPersonList(lines);

	
	if ((infp = fopen("junk", "r")) != NULL)
	{
		while (fgets(inputs, BUFSIZ, infp))
		{
			index = makeHash(inputs, lines);
			/* check to see if they are in our hashed list */
			if (! whoList[index]->nextPerson)
			{
				whoList[index] = (struct person *)malloc ((strlen(inputs) + 1) * sizeof(whoList[index]->value));
				if (whoList[index])
				{
					strcpy(whoList[index]->value, inputs);
					whoList[index]->nextPerson = whoList[index];
					whoList[index]->firstPerson = whoList[index];
				}
			}
			else /* otherwise chain them out */
			{
				newPerson = (struct person *)malloc(sizeof(struct person));
				if (newPerson)
				{
					strcpy(newPerson->value, inputs);
					newPerson->nextPerson = whoList[index]->firstPerson;
				}
				whoList[index]->firstPerson = newPerson;
				numLinked++;
			}		
		}
		close(infp);

		printf("Enter a string: ");
		fgets(inputs2, BUFSIZ, stdin);
		found = findHash(inputs2, lines, &*whoList);



	}
}

int	findHash(char *key, int modVal, struct person **personList)
{
	unsigned	long 	tHash = 0;
				char	*tName = "";
				int		found = FALSE;
	struct		person	*currentPerson;



	tHash = makeHash(key, modVal);

	if (personList[tHash]->firstPerson)
	{
		tName = personList[tHash]->value;
		if (strcmp(key, tName) == 0)
		{
			found = TRUE;	
		}
		else
		{
			/* see if there is a chain */
			if ( personList[tHash] != personList[tHash]->firstPerson) 
			{
				/* walk the chain */
				currentPerson = personList[tHash]->firstPerson;
				while (! found)
				{
					tName = currentPerson->value;
					if (strcmp(key, tName) == 0)
						found = TRUE;
					else
					{
						currentPerson = currentPerson->nextPerson;
						if(! currentPerson || (currentPerson == currentPerson->nextPerson))
							break;
					}
				}
			}
				
		}

	}
	return(found);
}


unsigned long makeHash(char *key, int modVal)
{
	register	unsigned	long	Tkey;
	register	unsigned 	long	Hkey;
	register	unsigned 			Code;
	
	long 	lKey;
	double 	dKey;
	int		iKey;
	int		index = 0;
	int		digit = 0;
	int		length = 0;
	long	score;
	int		pos = 0;
	int		factor = 11;


	length = strlen(key) - 1;
	Tkey = 1;
	pos = (int) key[index] + 7;
	
	for (index = 0; index < length; index++)
	{
		Code = ((unsigned) key[index] * (pos +=  (int)key[index] )) * 3;
		Tkey += Code +  11;
	}
	Hkey = Tkey % modVal;
	/* printf("%u\n", Hkey); */
	return (Hkey);
}

unsigned numLines(char *fileName)
{
	FILE		*fp;
	unsigned	lCount = 0;
	char		inputs[BUFSIZ];

	if ((fp = fopen(fileName, "r")) != NULL)
	{
		while (fgets(inputs, BUFSIZ, fp))
			lCount++;
		fclose(fp);
	}
	return (lCount);
}


struct person **buildPersonList( unsigned numberOfPeople)
{
	int						index = 0;
	struct		person		**list;
	list = (struct person **)malloc(numberOfPeople*sizeof(struct person));
	for (index = 0; index <= numberOfPeople; index++)
		list[index] = (struct person *)malloc (sizeof(struct person));
	return(list);

}
