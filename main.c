#include "memfuncs.h"
#include <stdio.h>

int main(){
	char* a = _malloc(4 * sizeof(char));
	for (char i = 0; i < 10; i++){
		printf("%d ", a[i]);
	}
	printf("\n");

	_realloc(a, 6);
	a[3] = 8;
	a[2] = 9;
	for (char i = 0; i < 6; i++){
		printf("%d ", a[i]);
	}
	printf("\n");
	_realloc(a, 2);
	for (char i = 0; i < 4; i++){
		printf("%d ", a[i]);
	}
	printf("\n");

	return 0;
}
