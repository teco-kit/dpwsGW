#ifndef _PTR_STACK_H
#define _PTR_STACK_H 1
#ifndef CCSC
typedef void ** ptr_stack;

//#define new_ptr_stack(stack,dom) void * stack[20]={dom};

#define new_ptr_stack(s,d) void **s; void* my_stack[20]; s=&my_stack[0] ; if(pop()); push(d)

#if 1
#define pop() (*(stack--))
#else
static void *stack_pop(void ***stack) {
	void *ret=*(*stack);
	**stack=0;
	(*stack)--;
	return ret;
}


#define pop() stack_pop(&stack)
#endif
#if 1
#define push(X) (*(++stack) = X)
#else
static void *stack_push(void ***stack, void * X) {
	return *(++(*stack)) = X;
}

#define push(X) stack_push(&(stack),X)
#endif

#define top() *stack
#else

typedef char *ptr_stack;

#define pop() ((void *)(((uint16_t *)stack)--)[0])

#define push(X) ((++(uint16_t *)stack)[0]=X)

#define top() ((void *)*(uint16_t *)stack)

#define new_ptr_stack(s,d) void **s; char my_stack[20]; s=&my_stack[0] ; pop(); push(d)

#endif
#endif
