#include "colliders.h"
#include <cmath>
#include <iostream>


/*
 * Plane
 */

bool ColliderPlane::testCollision(const Particle* p) const {
    // TODO
    return (planeN.dot(p->pos)+planeD)*(planeN.dot(p->prevPos)+planeD) <= 0;
}

void ColliderPlane::resolveCollision(Particle* p, double kElastic, double kFriction) const {
    // TODO
    p->pos -= (1 + kElastic)*(planeN.dot(p->pos) + planeD)*planeN;
    p->vel -= (1 + kElastic)*(planeN.dot(p->vel))*planeN;
    Vec3 vn = (planeN.dot(p->vel))*planeN;
    Vec3 vt = p->vel - vn;
    p->vel -= kFriction*vt;
}
