#include "tinyphysicsengine.h"
#include <stdio.h>

#define F TPE_FRACTIONS_PER_UNIT
  
#define TOLERANCE 10

int tolerance(TPE_Unit x, TPE_Unit expX)
{
  return (x <= (expX + TOLERANCE)) && (x >= (expX - TOLERANCE)); 
}

int testRotToQuat(
  TPE_Unit x, TPE_Unit y, TPE_Unit z, TPE_Unit angle,
  TPE_Unit expX, TPE_Unit expY, TPE_Unit expZ, TPE_Unit expW)
{
  printf("testing axis + rot -> quaternion ([%d,%d,%d] %d -> %d %d %d %d): ",
    x,y,z,angle,expX,expY,expZ,expW);

  TPE_Vec4 q, axis;

  TPE_vec4Set(&axis,x,y,z,0);
  TPE_rotationToQuaternion(axis,angle,&q);

  if (!tolerance(q.x,expX) ||
      !tolerance(q.y,expY) ||
      !tolerance(q.z,expZ) || 
      !tolerance(q.w,expW)) 
  {
    printf("bad (%d %d %d %d)\n",q.x,q.y,q.z,q.w);
    return 0;
  }
  
  puts("OK");
  return 1;
}

int ass(const char *what, int cond)
{
  printf("testing %s: %s\n",what,cond ? "OK" : "ERROR");
  return cond;
}

int testColl(const TPE_Body *b1, const TPE_Body *b2,
  TPE_Unit expRet, TPE_Unit expX, TPE_Unit expY, TPE_Unit expZ,
  TPE_Unit expNX, TPE_Unit expNY, TPE_Unit expNZ)
{
  printf("testing collision detection, %d %d: ",b1->shape,b2->shape);

  TPE_Vec4 p, n;

  TPE_Unit ret = TPE_bodyCollides(b1,b2,&p,&n);

  if (!tolerance(ret,expRet) ||
      !tolerance(p.x,expX) ||
      !tolerance(p.y,expY) ||
      !tolerance(p.z,expZ) ||
      !tolerance(n.x,expNX) ||
      !tolerance(n.y,expNY) ||
      !tolerance(n.z,expNZ))
    return 0;

  puts("OK");

  return 1;
}

int main(void)
{
  #define ASS(what) if (!what) { puts("ERROR"); return 0; } 

  {
    ASS(ass("shape ID",TPE_COLLISION_TYPE(TPE_SHAPE_SPHERE,TPE_SHAPE_CUBOID) == TPE_COLLISION_TYPE(TPE_SHAPE_CUBOID,TPE_SHAPE_SPHERE)))

    TPE_Vec4 q1, q2, q3, axis, p;

    ASS(testRotToQuat(F,0,0,    0,    0,0,0,F));
    ASS(testRotToQuat(F,0,0,    F/4,  361,0,0,361));
    ASS(testRotToQuat(0,F,0,    F/4,  0,361,0,361));
    ASS(testRotToQuat(0,0,F,    F/2,  0,0,F,0));
    ASS(testRotToQuat(-F,F,F,   -F/8, 112,-112,-112,472));

TPE_rotationToQuaternion(TPE_vec4(256,256,256,0),512,&q1);

p = TPE_vec4(512,0,0,0);

TPE_rotatePoint(&p,q1);

TPE_PRINTF_VEC4(p);
return 0;
  }

  {
    TPE_Body b1, b2;
    TPE_Vec4 collPoint, collNorm;
  
    TPE_bodyInit(&b1);
    TPE_bodyInit(&b2);

    b1.shape = TPE_SHAPE_SPHERE;
    b2.shape = TPE_SHAPE_SPHERE;
    
    b1.shapeParams[0] = TPE_FRACTIONS_PER_UNIT;
    b2.shapeParams[1] = TPE_FRACTIONS_PER_UNIT;

    b1.position = TPE_vec4(TPE_FRACTIONS_PER_UNIT,TPE_FRACTIONS_PER_UNIT / 2,0,0);
    b2.position = TPE_vec4(TPE_FRACTIONS_PER_UNIT + TPE_FRACTIONS_PER_UNIT / 2,TPE_FRACTIONS_PER_UNIT / 2,0,0);
    
    ASS(testColl(&b1,&b2,256,640,256,0,512,0,0));
    ASS(testColl(&b2,&b1,256,640,256,0,-512,0,0));


  }

  return 0;
}
