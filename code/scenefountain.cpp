#include "scenefountain.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>

#include <random>


SceneFountain::SceneFountain() {
    widget = new WidgetFountain();
    connect(widget, SIGNAL(updatedParameters()), this, SLOT(updateSimParams()));
    widget->setGenerateParticles(0); // Default
}


SceneFountain::~SceneFountain() {
    if (widget)     delete widget;
    if (shader)     delete shader;
    if (vaoFloor)   delete vaoFloor;
    if (vaoSphereS) delete vaoSphereS;
    if (vaoBigSphere) delete vaoBigSphere;
    if (vaoCollisionSphere) delete vaoCollisionSphere;
    if (fGravity)   delete fGravity;
    if (fGravitationalAtraction) delete fGravitationalAtraction;
}


void SceneFountain::initialize() {
    // load shader
    shader = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");

    // create floor VAO
    Model quad = Model::createQuad();
    vaoFloor = glutils::createVAO(shader, &quad);
    glutils::checkGLError();

    // create sphere VAOs
    Model sphere = Model::createIcosphere(1);
    vaoSphereS = glutils::createVAO(shader, &sphere);
    numFacesSphereS = sphere.numFaces();
    glutils::checkGLError();


    // BIG SPHERE
    Model BigSphere = Model::createIcosphere(2);
    vaoBigSphere = glutils::createVAO(shader, &BigSphere);
    numFacesBigSphere = BigSphere.numFaces();
    glutils::checkGLError();
    bigSphereParticle = new Particle();
    bigSphereParticle->pos = Vec3(40,80,0);
    bigSphereParticle->radius = 10;
    bigSphereParticle->mass = 6E14; // Must be massive!

    // COLLISION SPHERE
    Model CollisionBigSphere = Model::createIcosphere(3);
    vaoCollisionSphere = glutils::createVAO(shader, &CollisionBigSphere);
    numFacesCollisionSphere = CollisionBigSphere.numFaces();
    glutils::checkGLError();
    collisionSphereParticle = new Particle();
    collisionSphereParticle->pos = Vec3(0,40,0);
    collisionSphereParticle->radius = 10;
    collisionSphereParticle->color = Vec3(0,0,1);


    // create forces
    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);
    fGravitationalAtraction = new ForceGravitationalAtraction();
    system.addForce(fGravitationalAtraction);

    // scene
    fountainPos = Vec3(0, 80, 0);
    colliderFloor.setPlane(Vec3(0, 1, 0), 0);
    colliderSpheric.setColliderSphere(collisionSphereParticle->pos, collisionSphereParticle->radius);
}


void SceneFountain::reset()
{
    // update values from UI
    updateSimParams();

    // reset random seed
    Random::seed(1337);

    // erase all particles
    fGravity->clearInfluencedParticles();
    fGravitationalAtraction->clearInfluencedParticles();
    system.deleteParticles();
    deadParticles.clear();
}


void SceneFountain::updateSimParams()
{
    // get gravity from UI and update force
    double g = widget->getGravity();
    fGravity->setAcceleration(Vec3(0, -g, 0));

    // get other relevant UI values and update simulation params
    kBounce = widget->getkBounce();
    kFriction = widget->getkFriction();
    maxParticleLife = 7.0;
    emitRate = 200;
}


void SceneFountain::paint(const Camera& camera) {

    QOpenGLFunctions_3_3_Core* glFuncs = nullptr;
    glFuncs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());

    shader->bind();

    // camera matrices
    QMatrix4x4 camProj = camera.getPerspectiveMatrix();
    QMatrix4x4 camView = camera.getViewMatrix();
    shader->setUniformValue("ProjMatrix", camProj);
    shader->setUniformValue("ViewMatrix", camView);

    // lighting
    const int numLights = 1;
    const QVector3D lightPosWorld[numLights] = {QVector3D(100,500,100)};
    const QVector3D lightColor[numLights] = {QVector3D(1,1,1)};
    QVector3D lightPosCam[numLights];
    for (int i = 0; i < numLights; i++) {
        lightPosCam[i] = camView.map(lightPosWorld[i]);
    }
    shader->setUniformValue("numLights", numLights);
    shader->setUniformValueArray("lightPos", lightPosCam, numLights);
    shader->setUniformValueArray("lightColor", lightColor, numLights);

    // draw floor
    vaoFloor->bind();
    QMatrix4x4 modelMat;
    modelMat.scale(100, 20, 100);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // BIG SPHERE (GRAVITATIONAL ATTRACTION)
    vaoBigSphere->bind();
    modelMat = QMatrix4x4();
    modelMat.translate(bigSphereParticle->pos[0], bigSphereParticle->pos[1], bigSphereParticle->pos[2]);
    modelMat.scale(bigSphereParticle->radius);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", GLfloat(0), GLfloat(0), GLfloat(0));
    shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("matshin", 100.f);
    glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesBigSphere, GL_UNSIGNED_INT, 0);

    // COLLISION SPHERE
    vaoCollisionSphere->bind();
    modelMat = QMatrix4x4();
    modelMat.translate(collisionSphereParticle->pos[0], collisionSphereParticle->pos[1], collisionSphereParticle->pos[2]);
    modelMat.scale(collisionSphereParticle->radius);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", GLfloat(collisionSphereParticle->color[0]), GLfloat(collisionSphereParticle->color[1]), GLfloat(collisionSphereParticle->color[2]));
    shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("matshin", 100.f);
    glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesCollisionSphere, GL_UNSIGNED_INT, 0);


    // draw the different spheres
    vaoSphereS->bind();
    for (const Particle* particle : system.getParticles()) {
        Vec3   p = particle->pos;
        Vec3   c = particle->color;
        double r = particle->radius;

        modelMat = QMatrix4x4();
        modelMat.translate(p[0], p[1], p[2]);
        modelMat.scale(r);
        shader->setUniformValue("ModelMatrix", modelMat);

        shader->setUniformValue("matdiff", GLfloat(c[0]), GLfloat(c[1]), GLfloat(c[2]));
        shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
        shader->setUniformValue("matshin", 100.f);

        glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesSphereS, GL_UNSIGNED_INT, 0);
    }

    shader->release();
}


void SceneFountain::update(double dt) {

    // emit new particles, reuse dead ones if possible
    int emitParticles = std::max(1, int(std::round(emitRate * dt)));
    for (int i = 0; i < emitParticles; i++) {
        Particle* p;
        if (!deadParticles.empty()) {
            // reuse one dead particle
            p = deadParticles.front();
            deadParticles.pop_front();
        }
        else {
            // create new particle
            p = new Particle();
            system.addParticle(p);

            // don't forget to add particle to forces that affect it
            fGravity->addInfluencedParticle(p);
            if (widget->attractionGravitatoryForceFlag()) fGravitationalAtraction->addInfluencedParticle(p);
        }

        p->color = Vec3(153/255.0, 217/255.0, 234/255.0);
        p->radius = 0.7;
        p->life = maxParticleLife;


        // NORMAL
        if (widget->getGenerateParticles() == 0) {
            double x = Random::get(-10.0, 10.0);
            double y = 0;
            double z = Random::get(-10.0, 10.0);
            p->pos = Vec3(x, y, z) + fountainPos;
            p->vel = Vec3(0,0,0);

        } else {
            std::random_device rd;
            std::uniform_real_distribution<double> dist(0, 1);
            std::mt19937 gen(rd());

            //SEMI-SPHERE
            double alpha = 2*M_PI*(dist(gen) - 0.5);
            double beta = M_PI*(dist(gen))/2;
            p->pos = Vec3(cos(alpha)*cos(beta), sin(beta), sin(alpha)*cos(beta));
            p->vel = 25*Vec3(p->pos.x(), p->pos.y(), p->pos.z());
        }


        // WATERFALL

       if (widget->attractionGravitatoryForceFlag()) fGravitationalAtraction->setValues(p->mass, p->pos, bigSphereParticle->mass, bigSphereParticle->pos);

    }

    // integration step
    system.updateForces();
    Vecd ppos = system.getPositions();
    integrator.step(system, dt);
    //integratorMidPoint_.step(system, dt);
    system.setPreviousPositions(ppos);

    // collisions
    for (Particle* p : system.getParticles()) {
        if (colliderFloor.testCollision(p)) {
            colliderFloor.resolveCollision(p, kBounce, kFriction);
            // Change to red with every collision (floor)
            p->color = Vec3(1, 0, 0);
        }

        if (colliderSpheric.testCollision(p)) {
            colliderSpheric.resolveCollision(p, kBounce, kFriction);
            // Change to green with every collision (colliderSphericParticle)
            p->color = Vec3(0, 1, 0);
        }
    }

    // check dead particles
    for (Particle* p : system.getParticles()) {
        if (p->life > 0) {
            p->life -= dt;
            if (p->life < 0) {
                deadParticles.push_back(p);
            }
        }
    }
}

void SceneFountain::mousePressed(const QMouseEvent* e, const Camera&)
{
    mouseX = e->pos().x();
    mouseY = e->pos().y();
}

void SceneFountain::mouseMoved(const QMouseEvent* e, const Camera& cam)
{
    int dx = e->pos().x() - mouseX;
    int dy = e->pos().y() - mouseY;
    mouseX = e->pos().x();
    mouseY = e->pos().y();

    Vec3 disp = cam.worldSpaceDisplacement(dx, -dy, cam.getEyeDistance());

    // example
    if (e->modifiers() & Qt::ControlModifier) {
        // ctrl + click
        fountainPos += disp;
    }
    else if (e->modifiers() & Qt::AltModifier) {
        // option + click!
        collisionSphereParticle->pos += disp;
        colliderSpheric.setColliderSphere(collisionSphereParticle->pos, 10);
    } else {
        // Right mouse's button!
        bigSphereParticle->pos += disp;
    }
}
