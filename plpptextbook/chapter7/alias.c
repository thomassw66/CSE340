#include <stdio.h>
#include <stdlib.h>




int main() {
	int *x, *y;
	x = (int *) malloc(sizeof(int));
	*x = 1;
	y = x;
	*y = 2;
	printf("%d\n", *x);
}