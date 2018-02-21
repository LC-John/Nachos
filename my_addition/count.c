#include "count.h"

int Count(int n)
{	
	int i = 0;
	for (i = n; i > 0; i--)
	{
		printf("%d\n", i);
		sleep(1);
	}
}
