#ifndef MEMBLOCKS_H
#define MEMBLOCKS_H

#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

typedef struct mem_block *t_block;
#define META_SIZE 40

struct mem_block{
	size_t size; //8
	t_block next_block; //8
	t_block prev_block; //8
	int is_free; //4 not bool to make meta n*8
	int padding; //4 also for meta n*8
	void* ptr; //8
	char data[1]; //ptr on not meta data	
};

t_block find_block(t_block *last_block, size_t size, void *first_block){
	t_block block = first_block;
	while(block && !(block->is_free && block->size >= size)){
		*last_block = block;
		block = block->next_block;
	}
	return block;
}

t_block extend_heap(t_block last_block, size_t size){
	t_block block;
	block = sbrk(0);
	if(sbrk(META_SIZE + size) == (void*)-1)
		return NULL;
	block->size = size;
	block->next_block = NULL;
	if(last_block)
		last_block->next_block = block;
	block->is_free = 0;
	return block;
}

void split_block(t_block block, size_t size){
	        t_block new_block;
		new_block->data[1] = block->data[1] + size;
	        new_block->size = block->size - size - META_SIZE;
	        new_block->next_block = block->next_block;
	        new_block->is_free = 1;
	        block->size = size;
	        block->next_block = new_block;
}

size_t align8(size_t size){
        if(size & 0x7 == 0){
                return size;
        }
        return ((size >> 3) + 1) << 3;
}

t_block get_block(void *p){
        char *temp;
	        temp = p;
        return ( p = temp -= META_SIZE);
}

bool is_address_valid(void *p, void *first_block){
        if(first_block){
                if (p > first_block && p < sbrk(0)){
 	               return p == (get_block(p))->ptr;
                }
        }
        return 0;
}

t_block fusion(t_block block){
        if(block->next_block && block->next_block->is_free){
	        block->size += META_SIZE + block->next_block->size;
                block->next_block = block->next_block->next_block;
                if(block->next_block){
	                block->next_block->prev_block = block;
                }
        }
        return block;
}

void copy_block(t_block old_block, t_block new_block){
	size_t *old_data, *new_data;
	old_data = old_block->ptr;
	new_data = new_block->ptr;
	for(size_t i = 0; (i*8) < old_block->size && (i*8) < new_block->size; i++){
		new_data[i] = old_data[i];
	}
}

#endif


