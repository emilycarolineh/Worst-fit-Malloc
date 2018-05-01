//Emily Higgs

#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#include "mymalloc.c"

int main() {

	int *a = my_worstfit_malloc(8 * sizeof(int));

	a[0] = 7;
	a[7] = 0;
	a[6] = 1;
	a[1] = 6;
	a[5] = 2;
	a[2] = 5;
	a[3] = 4;
	a[4] = 3;

	int i;

	printf("Simple malloc test. Should count down from 7 to 0.\n");

	for(i = 0; i < 8; i++) {
		printf("%d\t", a[i]);
	}

	my_free(a);

	//Int array of size 20.
	int *b = my_worstfit_malloc(5 * sizeof(int));

	b[0] = 100;
	b[1] = 200;
	b[2] = 300;
	b[3] = 400;
	b[4] = 500;
	
	printf("\nSimple free test. Should count from 100 to 500 by hundreds.\n");

	for(i = 0; i < 5; i++) {
		printf("%d\t", b[i]);
	}

	//String of size 10. 
	int *c = my_worstfit_malloc(11 * sizeof(int));

	c[0] = 0;
	c[1] = 1;
	c[2] = 2;
	c[3] = 3;
	c[4] = 4;
	c[5] = 5;
	c[6] = 6;
	c[7] = 7;
	c[8] = 8;
	c[9] = 9;

	int *d = my_worstfit_malloc(100 * sizeof(int));

	for(i = 0; i < 100; i++)
		d[i] = i;
	
	int *e = my_worstfit_malloc(2 * sizeof(int));

	e[0] = 21;
	e[1] = 121;

	my_free(d);
	my_free(b);


	int *f = my_worstfit_malloc(6 * sizeof(int));

	f[0] = 700;
	f[1] = 800;
	f[2] = 900;
	f[3] = 1100;
	f[4] = 1200;
	f[5] = 1300;


	printf("\nIf these two values match, worst fit determination works: %d, %d\n", f[0], d[0]);

	my_free(e);

	int *g = my_worstfit_malloc(200 * sizeof(int));

	g[0] = 3000;
	g[1] = 3001;
	g[2] = 3002;


	int *h = my_worstfit_malloc(10 * sizeof(int));

	my_free(g);
	my_free(c);
	my_free(f);

	int *j = my_worstfit_malloc(2 *sizeof(int));

	j[0] = 42;
	j[1] = 43;

	printf("Testing both-neighbor coalescing: %d, %d\n", j[0], b[0]);

	return 0;
}

