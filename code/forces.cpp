#include "forces.h"
#include <iostream>

void ForceConstAcceleration::apply() {
    for (Particle* p : particles) {
        // TODO
        p->force += p->mass * this->getAcceleration();
    }
}

void ForceGravitationalAtraction::apply() {
    for (Particle* p : particles) {
        p->force += this->getGravitacionalAtractionForce();
    }
}
