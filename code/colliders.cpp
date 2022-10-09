#include "colliders.h"
#include <cmath>
#include <iostream>


/*
 * Plane
 */

bool ColliderPlane::testCollision(const Particle* p) const {
    return (planeN.dot(p->pos)+planeD)*(planeN.dot(p->prevPos)+planeD) <= 0;
}

void ColliderPlane::resolveCollision(Particle* p, double kElastic, double kFriction) const {
    p->pos -= (1 + kElastic)*(planeN.dot(p->pos) + planeD)*planeN;
    p->vel -= (1 + kElastic)*(planeN.dot(p->vel))*planeN;
    Vec3 vn = planeN.dot(p->vel)*planeN;
    Vec3 vt = p->vel - vn;
    p->vel -= kFriction*vt;
}

// SPHERIC
bool ColliderSpheric::testCollision(const Particle* p) const {
    Vec3 h = p->pos - center;
    return (h.dot(h) <= pow(radius, 2));
}

void ColliderSpheric::resolveCollision(Particle* p, double kElastic, double kFriction) const {
    Vecd tangentPlane = p->prevPos - center;
    tangentPlane /= tangentPlane.norm();
    double d = -tangentPlane.dot(p->prevPos);
    p->pos = p->prevPos - (1 + kElastic)*(tangentPlane.dot(p->pos) + d)*tangentPlane;
    p->vel -= (1 - kElastic)*(tangentPlane.dot(p->vel))*tangentPlane;
    Vecd tangentVector = p->vel - (tangentPlane.dot(p->vel)*tangentPlane);
    p->vel -= kFriction*tangentVector;
}
