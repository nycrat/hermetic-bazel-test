// A minimal Bullet Physics example: a sphere falls onto a static ground plane
// and we print its height as the simulation advances.
//
// Run with: bazel run //main:bullet-demo

#include <cstdio>

#include <btBulletDynamicsCommon.h>

int main() {
  // --- Collision configuration and world ----------------------------------
  btDefaultCollisionConfiguration collision_config;
  btCollisionDispatcher dispatcher(&collision_config);
  btDbvtBroadphase broadphase;
  btSequentialImpulseConstraintSolver solver;
  btDiscreteDynamicsWorld world(&dispatcher, &broadphase, &solver, &collision_config);
  world.setGravity(btVector3(0, -9.81f, 0));

  // --- Static ground plane ------------------------------------------------
  btCollisionShape* ground_shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
  btRigidBody::btRigidBodyConstructionInfo ground_info(0, nullptr, ground_shape);
  btRigidBody* ground = new btRigidBody(ground_info);
  world.addRigidBody(ground);

  // --- Falling sphere -----------------------------------------------------
  const btScalar radius = 1.0f;
  const btScalar mass = 1.0f;
  btCollisionShape* sphere_shape = new btSphereShape(radius);

  btVector3 inertia(0, 0, 0);
  sphere_shape->calculateLocalInertia(mass, inertia);

  btTransform start;
  start.setIdentity();
  start.setOrigin(btVector3(0, 10, 0));
  btDefaultMotionState* sphere_motion = new btDefaultMotionState(start);

  btRigidBody::btRigidBodyConstructionInfo sphere_info(
      mass, sphere_motion, sphere_shape, inertia);
  btRigidBody* sphere = new btRigidBody(sphere_info);
  world.addRigidBody(sphere);

  // --- Step the simulation and report the height --------------------------
  const btScalar dt = 1.0f / 60.0f;
  std::printf("t=0.00s  height=%.3f\n", sphere->getCenterOfMassPosition().y());
  for (int step = 1; step <= 120; ++step) {
    world.stepSimulation(dt, 1);
    if (step % 20 == 0) {
      std::printf("t=%.2fs  height=%.3f\n", step * dt, sphere->getCenterOfMassPosition().y());
    }
  }

  // --- Cleanup ------------------------------------------------------------
  world.removeRigidBody(sphere);
  world.removeRigidBody(ground);
  delete sphere;
  delete sphere_motion;
  delete sphere_shape;
  delete ground;
  delete ground_shape;

  return 0;
}
