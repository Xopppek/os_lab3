#ifndef MEMFUNCS_H
#define MEMFUNCS_H

#include "memblocks.h"
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* first_block = NULL;

void* _malloc(size_t size){
	t_block block, last_block;
	size_t size8;

	pthread_mutex_lock(&mutex);

	size8 = align8(size);
	if(first_block){
		last_block = first_block;
		block = find_block(&last_block, size8, first_block);
		if(block){
			if((block->size - size8) >= (META_SIZE + 8)){
				split_block(block, size8);
			}
			block->is_free = 0;
		}
		else{
			block = extend_heap(last_block, size8);
			if(!block){
				pthread_mutex_unlock(&mutex);
				return NULL;
			}
		}
	}
	else{
		block = extend_heap(NULL, size8);
		if(!block){
			pthread_mutex_unlock(&mutex);
			return NULL;
		}
		first_block = block;
	}
	pthread_mutex_unlock(&mutex);
	block->ptr = &(block->data);
	return block->data;
};

void* _calloc(size_t amount, size_t size){
	size_t *elements;
	elements = _malloc(size);
	if(elements)
		for (size_t i = 0; i < size; i++)
			elements[i] = 0;
	return elements; 
};

void _free(void* ptr){
	t_block block;
	pthread_mutex_lock(&mutex);
	if (is_address_valid(ptr, first_block)){
		block = get_block(ptr);
		block->is_free = 1;
		if(block->prev_block && block->prev_block->is_free){
			block = fusion(block->prev_block);
		}
		if(block->next_block){
			fusion(block);
		}
		else{
			if(block->prev_block)
				block->prev_block->next_block = NULL;
			else
				first_block = NULL;
			brk(block);
		}
	}
	pthread_mutex_unlock(&mutex);
};

void* _realloc(void* ptr, size_t size){
	pthread_mutex_lock(&mutex);
	size_t size8;
	t_block block, new_block;
	void* newptr;
	if(!ptr){
		pthread_mutex_unlock(&mutex);
		return _malloc(size);
	}
	if (is_address_valid(ptr, first_block)){
		size8 = align8(size);
		block = get_block(ptr);
		if(block->size >= size8){
			if(block->size - size8 >= META_SIZE + 8)
				split_block(block, size8);
		}else{
			if(block->next_block && block->next_block->is_free && 
			   block->size + META_SIZE + block->next_block->size >= size8){
				fusion(block);
				if(block->size - size8 >= META_SIZE + 8)
					split_block(block, size8);
			}else{
				newptr = _malloc(size8);
				if(!newptr){
					pthread_mutex_unlock(&mutex);
					return NULL;
				}
				new_block = get_block(newptr);
				copy_block(block, new_block);
				pthread_mutex_unlock(&mutex);
				_free(ptr);
				return newptr;
			}
		}
		pthread_mutex_unlock(&mutex);
		return ptr;
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
};

#endif
