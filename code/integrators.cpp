#include "integrators.h"


void IntegratorEuler::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd x1 = x0 + dt*dx;
    system.setState(x1);
}


void IntegratorSymplecticEuler::step(ParticleSystem &system, double dt) {
    // TODO
    Vecd vt = system.getVelocities();
    Vecd pt = system.getPositions();
    Vecd vtdt = vt + dt*system.getAccelerations();
    Vecd ptdt = pt + dt*vtdt;
    system.setPositions(ptdt);
    system.setVelocities(vtdt);
}


void IntegratorMidpoint::step(ParticleSystem &system, double dt) {
    // TODO
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd h = x0 + (dt/2)*dx;

    // Derivate
    system.setState(h); // Update state to get derivated
    Vecd hd = system.getDerivative();
    system.setState(x0 + dt*hd);
}

void IntegratorRungeKuttaK2::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd h = x0 + dt*dx;

    // Derivate
    system.setState(h); // Update state!
    Vecd hd = system.getDerivative();
    system.setState(x0 + dt*hd);
}

void IntegratorVerlet::step(ParticleSystem &system, double dt) {
    // TODO
    const double k = 0.99;
    Vecd pt = system.getPositions();
    Vecd vt = system.getVelocities();
    Vecd ptdt = pt + k*(vt*dt) + system.getAccelerations()*dt*dt;

    Vecd xt = system.getState();
    Vecd dx = system.getDerivative();
    Vecd xtdt = xt + dt*dx;
    Vecd vtdt = (xtdt - xt)/dt;

    //system.setVelocities(vtdt);
    system.setPositions(ptdt);
};
