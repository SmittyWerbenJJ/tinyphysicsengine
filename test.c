#include "tinyphysicsengine.h"
#include <stdio.h>

#define F TPE_FRACTIONS_PER_UNIT

int testRotToQuat(
  TPE_Unit x, TPE_Unit y, TPE_Unit z, TPE_Unit angle,
  TPE_Unit expX, TPE_Unit expY, TPE_Unit expZ, TPE_Unit expW)
{
  printf("testing axis + rot -> quaternion ([%d,%d,%d] %d -> %d %d %d): ",
    x,y,z,angle,expW,expX,expY,expZ);

  TPE_Vec4 q, axis;

  TPE_vec4Set(&axis,x,y,z,0);
  TPE_rotationToQuaternion(axis,angle,&q);

  #define TOLERANCE 10

  if (q.x > expX + TOLERANCE || q.x < expX - TOLERANCE ||
      q.y > expY + TOLERANCE || q.y < expY - TOLERANCE ||
      q.z > expZ + TOLERANCE || q.z < expZ - TOLERANCE ||
      q.w > expW + TOLERANCE || q.w < expW - TOLERANCE)
  {
    printf("%d %d %d %d, ERROR",q.x,q.y,q.z,q.w);
    return;
  }
  
  puts("OK");
  return 1;

  #undef TOLERANCE
}

int ass(const char *what, int cond)
{
  printf("testing %s: %s\n",what,cond ? "OK" : "ERROR");
  return cond;
}

int main(void)
{
  #define ASS(w,c) if (!ass(w,c)) { return 0; } 
  ASS("shape ID",TPE_COLLISION_TYPE(TPE_SHAPE_SPHERE,TPE_SHAPE_CUBOID) == TPE_COLLISION_TYPE(TPE_SHAPE_CUBOID,TPE_SHAPE_SPHERE))

  TPE_Vec4 q1, q2, q3, axis;

  testRotToQuat(F,0,0,    0,    0,0,0,F);
  testRotToQuat(F,0,0,    F/4,  361,0,0,361);
  testRotToQuat(0,F,0,    F/4,  0,361,0,361);
  testRotToQuat(0,0,F,    F/2,  0,0,F,0);
  testRotToQuat(-F,F,F,   -F/8, 195,-195,-195,472);

  return 0;
}
