//Emily Higgs

#include <unistd.h>
#pragma pack(1)

struct node  *first = NULL;		//Pointer to the first node in the list
struct node  *last = NULL;		//Pointer to the last node in the list


struct node {		
		int chunk_size;				//The size of this chunk of memory
		char free;					//Whether it is free or empty [1 = occupied, 0 = free]
		struct node  *next;  		//A pointer to the next node
		struct node  *previous;		//A pointer to the previous node.
};

void *my_worstfit_malloc(int size) {

	if(size < 1) {
		return NULL;
	}

	//If the first node in the list is null, then the linked list is empty. 
	if(first == NULL) { 

		//Create a pointer to a new node struct.
		struct node *new_node = (struct node *)sbrk(sizeof(struct node)); 

		//Fill in all the node field values.
		new_node->chunk_size = size;
		new_node->free = 1;
		new_node->next = NULL;
		new_node->previous = NULL;


		first = new_node;
		last = new_node;

		return(sbrk(size));

	}
	//Otherwise, search through the linked list in search of the largest free space.
	else {

		//Pointers: One to the node being examined and one to the free space with the worst fit.
		//The int represents the size of the largest free space present.  
		struct node *current = first;
		struct node *worst = NULL;
		int space = -1;

		//Search the entire linked list for the largest free space. 
		while(1) {

			//If the current node is free, check it.
			if(current->free == 0) {

				//If the current node has more space than the worst node, make it the new worst node and update space.
				if(space < current->chunk_size) {
					worst = current;
					space = current->chunk_size;
				}
			}

			//Move current to the next node in the list.
			if(current->next != NULL)
				current = current->next;
			else
				break;
		}


		//If the space is equal to the requested size, just modify the node and return a pointer to this free space.
		if(space == size) {
			worst->free = 1;
			return(worst + sizeof(struct node));
		}
		//If there are no free spaces within the heap or the space isn't large enough for the allocation and
		//for a new node upon splitting, add a new node at the end of the list (current). 
		else if(space < 0 || (size + sizeof(struct node) + 1) > space) { 
	
			//Create a pointer to the new node.
			struct node *new_node = (struct node *) sbrk(sizeof(struct node));

			//Set its attributes, including its previous reference to the last node in the list. 
			new_node->chunk_size = size;
			new_node->free = 1;
			new_node->next = NULL;
			new_node->previous = current;

			//Set the next reference of the previous last node in the list to the new node.
			current->next = new_node;

			last = new_node; 
			
			void *pointer = (void *)sbrk(size);

			printf("\nNew Node Info: pointer: %p, chunk: %d, previous: %p\n", pointer, new_node->chunk_size, new_node->previous);

			return(pointer);

		}
		//If the space is large enough for creating a new node upon splitting, split!
		else {

			//Create a new node for the remaining free space.
			struct node *new_node = worst + sizeof(struct node) + size;

			//Set the fields of the new node. 
			new_node->chunk_size = space - sizeof(struct node) - size;
			new_node->free = 0;
			new_node->next = worst->next;
			new_node->previous = worst;

			(worst->next)->previous = new_node;

			//Update the fields of the worst node to reflect this new node.
			worst->free = 1;
			worst->next = new_node;
			worst->chunk_size = size;


			if(last == worst)
				last = new_node;

			void *pointer = worst;

			//Return the start of the requested space.
			return(pointer + sizeof(struct node));
		}
	}
}

void my_free(void *ptr) {

	//Grab a pointer to this space's node. 
	struct node *ptr_node = ptr - sizeof(struct node);

	if(ptr_node->free == 0) {
		return;
	}
	
	//If the region of memory is the only one on the heap (only node), back up brk and update first/last.
	if(ptr_node == first && ptr_node == last) {
		sbrk(-(sizeof(struct node) + ptr_node->chunk_size));
		first = NULL;
		last = NULL;

		return;
	}
	//If the region of memory is the first on the heap, then coalescing can only happen with the next node. 
	else if(ptr_node == first) {

		struct node *next_node = ptr_node->next;

		//If the next node is free, coalesce them. 
		if(next_node != NULL && next_node->free == 0) {

			//Add the next node and its empty chunk to the node in question's chunk size. 
			ptr_node->chunk_size += sizeof(struct node) + next_node->chunk_size;

			//Move the next pointer along.
			ptr_node->next = next_node->next;

			(next_node->next)->previous = ptr_node;
		}

		//Mark this node as empty. 
		ptr_node->free = 0;

		return;
	}
	//If the region of memory is last on the heap, then coalescing can only happen with the previous node.
	else if(ptr_node == last) {

		struct node *previous_node = ptr_node->previous;

		//If the previous node is free, brk needs to move to before the previous node. 
		if(previous_node->free == 0) {

			if(previous_node != first) {
				last = previous_node->previous;
				(previous_node->previous)->next = NULL;
			}
			else {
				first = NULL;
				last = NULL;
			}


			sbrk((ptr_node->chunk_size + (2 * sizeof(struct node)) + previous_node->chunk_size) * -1);
		}
		else {
			last = previous_node;
			previous_node->next = NULL;

			sbrk((ptr_node->chunk_size + sizeof(struct node))* -1);
		}

		return;
	}
	//In this case, the node is neither the first nor the last. Coalescing is possible with next and/or previous node(s). 
	else {
		struct node *previous_node = ptr_node->previous;
		struct node *next_node = ptr_node->next;

		//Both neighbors are free. Coalesce with both.
		if(previous_node->free == 0 && next_node->free == 0) {
			//Not strictly necessary; just in case.
			ptr_node->free = 0;

			previous_node->next = next_node->next;
			(next_node->next)->previous = previous_node;
			
			//Add the space taken up by ptr and next to the chunk size of the previous node. 
			previous_node->chunk_size += (2 * sizeof(struct node)) + ptr_node->chunk_size + next_node->chunk_size;
		}
		//Previous node is free.
		else if(previous_node->free == 0) {
			//Not strictly necessary; just in case.
			ptr_node->free = 0;

			previous_node->next = ptr_node->next;
			previous_node->chunk_size += sizeof(struct node) + ptr_node->chunk_size;

			next_node->previous = previous_node;
		}
		//Next node is free. 
		else if(next_node->free == 0) {
			ptr_node->next = next_node->next;
			ptr_node->chunk_size += sizeof(struct node) + next_node->chunk_size;

			ptr_node->free = 0;
			(next_node->next)->previous = ptr_node;
			
		}
		//Both neighbors are occupied.
		else 
			ptr_node->free = 0;

		return;
	}
}
