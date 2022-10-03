#include "forces.h"

void ForceConstAcceleration::apply() {
    for (Particle* p : particles) {
        // TODO
        p->force+=p->mass * this->getAcceleration();
    }
}
