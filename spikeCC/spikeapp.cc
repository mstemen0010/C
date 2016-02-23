#include "spike.h"

int main(int argc, char *argv[])
{
	spike *mySpike;
	mySpike = new spike(argc, argv);
	 mySpike->start();
	mySpike->run(); 
	
	delete[] mySpike;
	return 1;
}
	
