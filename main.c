#include "memfuncs.h"
#include <stdio.h>
#include <pthread.h>

#define BLOCKS_AMOUNT 3
#define ITERATIONS_AMOUNT 1 
#define THREADS_AMOUNT 9 
pthread_mutex_t mutexx = PTHREAD_MUTEX_INITIALIZER;

typedef struct block{
	size_t size;
	int* ptr;
}block;

typedef struct parameters{
	int number;
	block* blocks;
}parameters;

void* alloc(void* params){
	parameters* args = (parameters*) params;
	int number = args->number;
	block* blocks = args->blocks;
	size_t sizes[BLOCKS_AMOUNT] = {16, 1024, 1024*1024};
	for(int i = 0; i < BLOCKS_AMOUNT; i++){
		blocks[i + BLOCKS_AMOUNT*number].size = sizes[i];
		blocks[i + BLOCKS_AMOUNT*number].ptr = _malloc(sizes[i]);
		printf("%p %lu\n", blocks[i + BLOCKS_AMOUNT*number].ptr, sizes[i]);
	}
	return NULL;
}

void* fill(void* params){
	parameters* args = (parameters*) params;
	block* blocks = args->blocks;
	int number = args->number;
	int n = number % (THREADS_AMOUNT / BLOCKS_AMOUNT);
	for(int i = 0; i < BLOCKS_AMOUNT; i++){
		int* ptr = blocks[i + BLOCKS_AMOUNT * n].ptr;
		*ptr = number + 1;
	}
	return NULL;
}

void* save(void* params){
	parameters* args = (parameters*) params;
	block* blocks = args->blocks;
	int number = args->number;
	int n = number % (THREADS_AMOUNT / BLOCKS_AMOUNT);
	FILE *fp = fopen("data.txt", "a");
	if(fp != NULL){
		for(int i = 0; i < BLOCKS_AMOUNT; i++){
			pthread_mutex_lock(&mutexx);
			fprintf(fp, "%d %lu\n", *blocks[i + BLOCKS_AMOUNT*n].ptr, blocks[i].size);
			pthread_mutex_unlock(&mutexx);
			_free(blocks[i + BLOCKS_AMOUNT*n].ptr);
		}
	}
	fclose(fp);
	return NULL;		
}

int main(){
	block* blocks = (block*) _malloc(sizeof(block)*THREADS_AMOUNT);
	parameters args[THREADS_AMOUNT]; 
	for (int i = 0; i < THREADS_AMOUNT; i++){
		args[i].blocks = blocks;
		args[i].number = i;
	}

	pthread_t threads[THREADS_AMOUNT];
	for(int j = 0; j < ITERATIONS_AMOUNT; j++){
		for (int i = 0; i < THREADS_AMOUNT/3; i++){
			if(pthread_create(&threads[i], NULL, 
				(void*) alloc, (void*)&args[i])){
				printf("creating thread error\n");
				return 1;
			}
		}

		for (int i = 0; i < THREADS_AMOUNT/3; i++){
			if(pthread_join(threads[i], NULL)){
				printf("joining thread error\n");
				return 1;
			};	
		}

		for (int i = THREADS_AMOUNT/3; i < 2*THREADS_AMOUNT/3; i++){
			if(pthread_create(&threads[i], NULL, 
				(void*) fill, (void*)&args[i])){
				printf("creating thread error\n");
				return 1;
			}
		}

		for (int i = THREADS_AMOUNT/3; i < 2*THREADS_AMOUNT/3; i++){
			if(pthread_join(threads[i], NULL)){
				printf("joining thread error\n");
				return 1;
			};	
		}
		
		for (int i = 2*THREADS_AMOUNT/3; i < THREADS_AMOUNT; i++){
			if(pthread_create(&threads[i], NULL, 
				(void*) save, (void*) &args[i])){
				printf("creatin thread error\n");
				return 1;
			}
		}

		for (int i = 2*THREADS_AMOUNT/3; i < THREADS_AMOUNT; i++){
			if(pthread_join(threads[i], NULL)){
				printf("joining thread error\n");
				return 1;
			}
		}
	}

	_free(blocks);
		
	return 0;
}
