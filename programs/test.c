/** General automatic test for tinyphysicsengine, it should always pass. */

#include "../tinyphysicsengine.h"
#include <stdio.h>

#define ass(cond,text) { printf(text ": "); if (!(cond)) { puts("ERROR"); return 1; } else puts("OK"); }

TPE_Unit rampPoits[6] =
{
  0,0,
  -2400,1400,
  -2400,0
};

TPE_Vec3 envFunc(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START( TPE_envAABoxInside(p,TPE_vec3(0,1000,0),TPE_vec3(3000,2500,3000)),p )
  TPE_ENV_NEXT( TPE_envAATriPrism(p,TPE_vec3(100,200,-10),rampPoits,3000,2),p)
  TPE_ENV_NEXT( TPE_envBox(p,TPE_vec3(30,200,-10),TPE_vec3(500,600,700),TPE_vec3(10,20,30)), p)
  TPE_ENV_NEXT( TPE_envAABox(p,TPE_vec3(-100,300,200),TPE_vec3(500,600,700)), p)
  TPE_ENV_END
}

TPE_Vec3 envFunc2(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START( TPE_envSphereInside(p,TPE_vec3(100,20,-3),5000), p)
  TPE_ENV_NEXT( TPE_envGround(p,-500), p)

  if (TPE_ENV_BCUBE_TEST(p,maxD,TPE_vec3(300,-40,50),1100) )
  {
    TPE_ENV_NEXT( TPE_envCylinder(p, TPE_vec3(300,-40,50), TPE_vec3(-400,-200,-50), 751), p)
  }

  TPE_ENV_END
}

TPE_Vec3 envFuncBad(TPE_Vec3 p, TPE_Unit maxD)
{
  p.x += 200;
  return p;
}

TPE_Vec3 envFuncBad2(TPE_Vec3 p, TPE_Unit maxD)
{
  if (p.y > p.x)
    p.x = p.y;

  return p;
}

TPE_Vec3 envSimple(TPE_Vec3 p, TPE_Unit maxD)
{
  TPE_ENV_START( TPE_envAABoxInside(p,TPE_vec3(0,0,0),TPE_vec3(10000,10000,10000)), p)
  TPE_ENV_NEXT( TPE_envHalfPlane(p,TPE_vec3(0,-5000,0),TPE_vec3(500,500,0)), p)
  TPE_ENV_END
}

int main(void)
{
  puts("== testing tinyphysicsengine ==");

  {
    ass(TPE_vec2Angle(-100,0) == TPE_FRACTIONS_PER_UNIT / 2,"vec2 angle")

    TPE_Unit l;

    l = TPE_LENGTH(TPE_vec3Normalized(TPE_vec3(100,0,0)));
    ass(TPE_abs(l - TPE_FRACTIONS_PER_UNIT) < 5,"vec3 normalize")

    l = TPE_LENGTH(TPE_vec3Normalized(TPE_vec3(0,0,0)));
    ass(TPE_abs(l - TPE_FRACTIONS_PER_UNIT) < 5,"zero vec3 normalize")

    l = TPE_LENGTH(TPE_vec3Normalized(TPE_vec3(0,-1,0)));
    ass(TPE_abs(l - TPE_FRACTIONS_PER_UNIT) < 5,"small vec3 normalize")

    l = TPE_LENGTH(TPE_vec3Normalized(TPE_vec3(500000,300000,-700000)));

    ass(TPE_abs(l - TPE_FRACTIONS_PER_UNIT) < 5,"big vec3 normalize")
  }

  {
    TPE_Joint joints[16];
    TPE_Connection cons[16];

    joints[0] = TPE_joint(TPE_vec3(200,100,-400),300);
    joints[1] = TPE_joint(TPE_vec3(100,200,-400),300);
    joints[2] = TPE_joint(TPE_vec3(200,-400,200),300);
    joints[3] = TPE_joint(TPE_vec3(200,100,-400),400);

    cons[0].joint1 = 0; cons[0].joint2 = 1; cons[0].length = 1000;
    cons[1].joint1 = 1; cons[1].joint2 = 0; cons[1].length = 1000;
    cons[2].joint1 = 0; cons[2].joint2 = 1; cons[2].length = 1100;
    cons[3].joint1 = 2; cons[3].joint2 = 3; cons[3].length = 100;

    uint32_t jHashes[4], cHashes[4];

    for (int i = 0; i < 4; ++i)
    {
      jHashes[i] = TPE_jointHash(&joints[i]);
      cHashes[i] = TPE_connectionHash(&cons[i]);
    }

    for (int i = 0; i < 4; ++i)
      for (int j = i + 1; j < 4; ++j)
        ass(jHashes[i] != jHashes[j] && cHashes[i] != cHashes[j],"joints/connection hash");

    TPE_Body bodies[8];
    uint32_t bHashes[4];

    TPE_bodyInit(&bodies[0],joints,4,cons,4,300);
    TPE_bodyInit(&bodies[1],joints + 1,3,cons,4,300);
    TPE_bodyInit(&bodies[2],joints,4,cons,4,300);
    bodies[2].flags |= TPE_BODY_FLAG_SOFT | TPE_BODY_FLAG_NONROTATING;
    TPE_bodyInit(&bodies[3],joints,4,cons,4,200);

    for (int i = 0; i < 4; ++i)
      bHashes[i] = TPE_bodyHash(&bodies[i]);

    for (int i = 0; i < 4; ++i)
      for (int j = i + 1; j < 4; ++j)
        ass(bHashes[i] != bHashes[j],"body hash");

    TPE_World world;

    uint32_t wHashes[4];

    TPE_worldInit(&world,bodies,4,0);

    wHashes[0] = TPE_worldHash(&world);
    bodies[0].jointCount--;
    wHashes[1] = TPE_worldHash(&world);
    bodies[4] = bodies[0]; bodies[0] = bodies[3]; bodies[3] = bodies[4];
    wHashes[2] = TPE_worldHash(&world);
    world.bodyCount--;
    wHashes[3] = TPE_worldHash(&world);

    for (int i = 0; i < 4; ++i)
      for (int j = i + 1; j < 4; ++j)
        ass(wHashes[i] != wHashes[j],"world hash");
  }
 
  {
    puts("-- environment functions --");

    ass(TPE_testClosestPointFunction(envFunc,TPE_vec3(-3000,-3000,-3000),
      TPE_vec3(3000,3000,3000),32,40,0),"env function");

    ass(TPE_testClosestPointFunction(envFunc2,TPE_vec3(-2000,-1000,-5000),
      TPE_vec3(5000,6000,7000),32,30,0),"env function");

    ass(!TPE_testClosestPointFunction(envFuncBad,TPE_vec3(-1000,-1000,-1000),
      TPE_vec3(2000,3000,100),32,40,0),"env function bad");

    ass(!TPE_testClosestPointFunction(envFuncBad2,TPE_vec3(-1000,-2000,-200),
      TPE_vec3(1000,1000,2000),32,40,0),"env function bad");

  }

  {
    puts("-- simulation --");

    TPE_World w;
    TPE_Joint j[64];
    TPE_Connection c[128];
    TPE_Body b[4];

    j[0] = TPE_joint(TPE_vec3(0,0,0),320);
    TPE_bodyInit(&b[0],j,1,0,0,2124);
    TPE_bodyMoveBy(&b[0],TPE_vec3(-2000,3000,3000));

    TPE_makeBox(j + 1,c,800,900,850,320);
    TPE_bodyInit(&b[1],j + 1,8,c,16,1300);
    b[1].friction = 400;
    b[1].elasticity = 350;
    TPE_bodyMoveBy(&b[1],TPE_vec3(-2000,3800,1500));

    TPE_make2Line(j + 20,c + 32,1000,300);
    TPE_bodyInit(&b[2],j + 20,2,c + 32,1,1300);
    b[2].flags |= TPE_BODY_FLAG_NONROTATING;
    TPE_bodyMoveBy(&b[2],TPE_vec3(-3000,4100,-1500));

    TPE_makeCenterBox(j + 32,c + 64,600,500,510,300);
    TPE_bodyInit(&b[3],j + 32,9,c + 64,18,1200);
    b[3].flags |= TPE_BODY_FLAG_SOFT;
    TPE_bodyMoveBy(&b[3],TPE_vec3(-1500,3500,-3000));

    TPE_worldInit(&w,b,4,envSimple);

    puts("dropping bodies onto a ramp...");

    for (int i = 0; i < 300; ++i)
    {
      for (uint8_t j = 0; j < w.bodyCount; ++j)
        TPE_bodyApplyGravity(&w.bodies[j],8);

      TPE_worldStep(&w);
    }

    for (int i = 0; i < w.bodyCount; ++i)
    {
      ass(TPE_bodyGetCenterOfMass(&w.bodies[i]).x > 0,"x position > 0");
      ass(w.bodies[i].flags & TPE_BODY_FLAG_DEACTIVATED,"deactivated?");
    }
  }

  return 0;
}
