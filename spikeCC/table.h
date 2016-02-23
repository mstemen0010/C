/*
** File: table.h SCCS 1.2 - 8/4/95
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


#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fstream.h>
#include <stream.h>

#ifndef TRUE
	#define	TRUE	1
	#define FALSE	! TRUE
#endif

struct	tableElement 
{	
	char			*value;
	struct  tableElement	*next;
	struct	tableElement	*first;
};

class table 
{
	public:
							table(int newSize); 
							table(char & filename);
							table(char * filename);
					
							~table();

							int	find(char * key);
							int		add(char *item);

	private:
				unsigned 	long	makeHash(char * value);
							int		chain(tableElement * item, char *  value);
							int   	numLines(char &fileName);
							int   	numLines(char *fileName);
							int		build(fstream& file);
				numLines();


				int		arraySize;
				int		_numItems;
				int		_numLines;
				int		modValue;
				int		size;


				struct tableElement**	tbl;

};
