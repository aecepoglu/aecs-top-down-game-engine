#ifndef MOVE_H
#define MOVE_H

typedef bool (moveFun)( struct Map *, struct object*);

moveFun moveForward;
moveFun moveBackword;
moveFun turnLeft;
moveFun turnRight;
moveFun eat;
moveFun pickUp;
moveFun kick;

#endif
