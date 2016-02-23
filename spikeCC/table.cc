/*
** File: table.cc SCCS 1.2 - 8/4/95
**
** Author:   Matthew Stemen
**
**
** 
** Created:  19 June 1995
**  
**
**
** Purpose: Monitors and reports any and all remote logins to the 
**          host it is run from.  This activity is reported either 
**          using Sound, Images(icons), and Tabular reports or any
**          combination thereof.
*/


#include "table.h"

table::table(int newSize)
{
	_numItems = 0;
	size = newSize;
	_numLines = newSize;
	tbl = (tableElement**)malloc(size * sizeof(tableElement)); 
	for (int i = 0; i <= size; i++)
		tbl[i] = new tableElement;
}

table::table(char* filename)
{
	fstream  fs(filename, ios::in);	
	_numItems = 0;
	_numLines = numLines(filename); 
	size = _numLines;
	build(fs);	
}
table::table(char& filename)
{
	fstream  fs((char *)filename, ios::in);	
	_numItems = 0;
	_numLines = numLines(filename); 
	size = _numLines;
	build(fs);	
}

table::~table()
{
	for (int i = 0; i < _numItems; i++)
	{	
		tableElement *nx;
		for (tableElement* n = tbl[i]; n;n=nx)
		{
			if (n)
			{
				if (n->next)
					nx=n->next;
				if (n->value)
					delete n->value;
				delete n;
			}
		}
	}
	delete tbl;
}

int table::build (fstream& file) 
{
	char		inputs[BUFSIZ];
	int				success = TRUE;

	tbl = (tableElement**)malloc(size * sizeof(tableElement)); 
	for (int i = 0; i <= size; i++)
		tbl[i] = new tableElement;
	file.seekg(0);
	int index = 0;
	while(file.getline(inputs, BUFSIZ - 1))
			success = add(inputs);
	return(success);

}
table::add ( char* item) 
{
	unsigned	long	Tkey;
				int	 	status = FALSE;	

	Tkey = 0;
	Tkey = makeHash(item);

	if (tbl[Tkey]->value) // see if the element is being used
	{
		// yes it is, chain it
		status = chain(&*tbl[Tkey], item);
	}
	else
	{
		// no it is not, add it
		tbl[Tkey]->value = new char[strlen(item) + 1];
		if (tbl[Tkey]->value)
		{
			strcpy(tbl[Tkey]->value, item);
			tbl[Tkey]->next = tbl[Tkey];
			tbl[Tkey]->first = tbl[Tkey];
			status = TRUE;
		}

	}
	_numItems++;
	return(status);


}

int table::find ( char * item) 
{
	long	tHash = makeHash(item); 
	int		found = FALSE;

	char *tName;

	if (tbl[tHash]->first) // see if this element is occupied
	{
		tName = tbl[tHash]->value;
		if (strcmp(item, tName) == 0)
			found = TRUE;
		else // it was not in the first pos, so now we walk the chain
		{
			// see if there is a chain, if there isn't, the first person should
			// be the begining element
			if (tbl[tHash] != tbl[tHash]->first)
			{
				// walk the chain
				struct tableElement *currentElement = tbl[tHash]->first;
				while (! found)
				{
					tName = currentElement->value;
					if (strcmp(item, tName) == 0)
						found = TRUE;
					else
					{
						currentElement = currentElement->next;
						if (! currentElement || (currentElement == currentElement->next));
							break;
					}
				} // end while
			}
		} // end else walk chain
	}
	return(found);
} // end table::find

table::chain ( tableElement* item,  char* value) 
{
	int	status = FALSE;
	tableElement* newElement = new tableElement;	

	if(newElement)
	{
		newElement->value = new char[strlen(value) + 1];
		strcpy(newElement->value, value);
		newElement->value = value;
		newElement->next = item->first;
		item->first = newElement;
		status = TRUE;
	}
	return(status);
}

int table::numLines (  char *fileName) 
{
	int 	lCount = 0;
	char	inputs[BUFSIZ];

	fstream in(fileName, ios::in);
	
	while(in.getline(inputs, BUFSIZ - 1))
		lCount++;

	in.close();
	return(lCount);
}
	
int table::numLines (  char &fileName) 
{
	int 	lCount = 0;
	char	inputs[BUFSIZ];

	fstream in((char *)fileName, ios::in);
	
	while(in.getline(inputs, BUFSIZ - 1))
		lCount++;

	in.close();
	return(lCount);
}
	
unsigned long table::makeHash (char * key)
{
	register	unsigned 	long	Tkey;
	register	unsigned			Code;
							int		index = 0;

	Tkey = 1;
	// pos adds weight to the position a letter in the word
	int	pos = (int)key[index] + 7; // 7 is just a prime number
	int len = strlen(key);
	if (len)
		len--;
	for (index = 0; index < len; index++)
	{
		Code = ((unsigned) key[index]* (pos += ((int)key[index] ))) * 3;
		Tkey += (Code + 11);
	}
	return(Tkey % _numLines);
	
}


	
	

