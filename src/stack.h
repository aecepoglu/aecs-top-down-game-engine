#ifndef STACK_H
#define STACK_H

#define STACK_PUSH(list,item,tail) list[ (tail)++] = item
#define STACK_POP(list,item,tail) item = list[ --(tail)]
#define STACK_IS_FULL(tail,size) ((tail)==size)
#define STACK_IS_EMPTY(tail) ((tail)==0)

#endif /*STACK_H*/
