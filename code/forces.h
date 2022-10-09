#ifndef FORCES_H
#define FORCES_H

#include <vector>
#include "particle.h"

class Force
{
public:
    Force(void) {}
    virtual ~Force(void) {}

    virtual void apply() = 0;

    void addInfluencedParticle(Particle* p) {
        particles.push_back(p);
    }

    void setInfluencedParticles(const std::vector<Particle*>& vparticles) {
        particles = vparticles;
    }

    void clearInfluencedParticles() {
        particles.clear();
    }

    std::vector<Particle*> getInfluencedParticles() const {
        return particles;
    }

protected:
    std::vector<Particle*>	particles;
};


class ForceConstAcceleration : public Force {
public:
    ForceConstAcceleration() { acceleration = Vec3(0,0,0); }
    ForceConstAcceleration(const Vec3& a) { acceleration = a; }
    virtual ~ForceConstAcceleration() {}

    virtual void apply();

    void setAcceleration(const Vec3& a) { acceleration = a; }
    Vec3 getAcceleration() const { return acceleration; }

protected:
    Vec3 acceleration;
};

class ForceGravitationalAtraction : public Force {
public:
    ForceGravitationalAtraction() {
        mass1 = 0.0; mass2 = 0.0;
        pos1 = Vec3(0,0,0); pos2 = Vec3(0,0,0);
        gravitationalConstant = -6.67E-11;
    }

    ForceGravitationalAtraction(double m1, double m2, Vec3 &p1, Vec3 &p2) {
        this->mass1 = m1; this->mass2 = m2;
        this->pos1 = p1; this->pos2 = p2;
    }
    virtual ~ForceGravitationalAtraction() {}
    virtual void apply();

    void setValues(double m1, Vec3 &p1, double m2, Vec3 &p2) {
        this->mass1 = m1; this->mass2 = m2;
        this->pos1 = p1; this->pos2 = p2;
    }

    Vec3 getGravitacionalAtractionForce() {
        Vec3 distanceVector = (this->pos1 - this->pos2);
        double distanceModule = this->getDistanceBetween2Vectors(this->pos1, this->pos2);
        Vec3 unitVector = distanceVector/distanceModule;
        return gravitationalConstant*((this->mass1*this->mass2)/(distanceModule*distanceModule))*unitVector;
    }

private:
    double getDistanceBetween2Vectors(Vec3 vector1, Vec3 vector2) {
        Vec3 distanceVector = (vector2 - vector1);
        return sqrt(distanceVector.x()*distanceVector.x() + distanceVector.y()*distanceVector.y() + distanceVector.z()*distanceVector.z());
    }

protected:
    double mass1;
    double mass2;
    double gravitationalConstant;
    Vec3 pos1;
    Vec3 pos2;
};


#endif // FORCES_H
