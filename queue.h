#define QUEUE_PUSH( list, item, head, tail, size) do { list[tail] = item; tail=(tail+1)%size; } while(0)
#define QUEUE_POP( list, head, tail, size) list[ head]; head=(head+1)%size;
#define QUEUE_IS_EMPTY( head, tail) (head == tail)
