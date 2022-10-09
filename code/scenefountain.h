#ifndef SCENEFOUNTAIN_H
#define SCENEFOUNTAIN_H
#define _USE_MATH_DEFINES

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <list>
#include "scene.h"
#include "widgetfountain.h"
#include "particlesystem.h"
#include "integrators.h"
#include "colliders.h"
#include <cmath>

class SceneFountain : public Scene
{
    Q_OBJECT

public:
    SceneFountain();
    virtual ~SceneFountain();

    virtual void initialize();
    virtual void reset();
    virtual void update(double dt);
    virtual void paint(const Camera& cam);

    virtual void mousePressed(const QMouseEvent* e, const Camera& cam);
    virtual void mouseMoved(const QMouseEvent* e, const Camera& cam);

    virtual void getSceneBounds(Vec3& bmin, Vec3& bmax) {
        bmin = Vec3(-50, -10, -50);
        bmax = Vec3( 50, 100, 50);
    }
    virtual unsigned int getNumParticles() { return system.getNumParticles(); }

    virtual QWidget* sceneUI() { return widget; }

public slots:
    void updateSimParams();

protected:
    WidgetFountain* widget = nullptr;

    QOpenGLShaderProgram* shader = nullptr;
    QOpenGLVertexArrayObject* vaoSphereS = nullptr;
    QOpenGLVertexArrayObject* vaoFloor   = nullptr;
    QOpenGLVertexArrayObject* vaoCube = nullptr;
    QOpenGLVertexArrayObject* vaoBigSphere = nullptr;
    QOpenGLVertexArrayObject* vaoCollisionSphere = nullptr;

    Particle* bigSphereParticle;
    Particle* collisionSphereParticle;

    unsigned int numFacesCube = 6;
    unsigned int numFacesSphereS = 0;
    unsigned int numFacesBigSphere = 0;
    unsigned int numFacesCollisionSphere = 0;

    IntegratorEuler integrator;
    IntegratorMidpoint integratorMidPoint_;
    ParticleSystem system;
    std::list<Particle*> deadParticles;
    ForceConstAcceleration* fGravity;
    ForceGravitationalAtraction* fGravitationalAtraction;


    // Collider Cube
    ColliderPlane colliderFloor;
    ColliderSpheric colliderSpheric;
    //ColliderCube colliderCube_;

    double kBounce, kFriction;
    double emitRate;
    double maxParticleLife;

    Vec3 fountainPos;
    int mouseX, mouseY;
};

#endif // SCENEFOUNTAIN_H
