typedef char *ptr_stack;

#define pop() ((void *)(((uint16_t *)stack)--)[0])

#define push(X) ((++(uint16_t *)stack)[0]=X)

#define top() ((void *)*(uint16_t *)stack)
