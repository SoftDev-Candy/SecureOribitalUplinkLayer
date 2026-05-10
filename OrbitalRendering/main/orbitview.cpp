//
// Created by Candy on 4/24/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Orbitview.h" resolved

#include "main/orbitview.hpp"
#include "../Shader/ShaderSource.hpp"
#include <QDebug>
#include <QImage>
#include <QMatrix4x4>
#include <QOpenGLTexture>

void Orbitview::wheelEvent(QWheelEvent *event)
{
    float zoomAmount = event->angleDelta().y() > 0 ? -0.5f : 0.5f;

    _camera.AddZoom(zoomAmount);

    update();
}

void Orbitview::mousePressEvent(QMouseEvent* event)
{
    lastMousePos = event->pos();
}

void Orbitview::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - lastMousePos;
    lastMousePos = event->pos();

    if (event->buttons() & Qt::LeftButton)
    {
        _camera.AddYaw(delta.x() * 0.3f);
        _camera.AddPitch(-delta.y() * 0.3f);

        update();
    }
}

Orbitview::Orbitview(QWidget *parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

Orbitview::~Orbitview()
{
    if (context() != nullptr)
    {
        makeCurrent();
        earthMesh.Destroy();
        satelliteMesh.Destroy();

        if (nightTexture != nullptr)
        {
            delete nightTexture;
            nightTexture = nullptr;
        }

        doneCurrent();
    }
}

void Orbitview::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);

    program = new QOpenGLShaderProgram(this);
    bool vertexCreated = program->addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        VertexShader());

    if (!vertexCreated)
    {
        qDebug().noquote() << "Vertex shader compilation failed:";
        qDebug().noquote() << program->log();
    }

    bool fragmentCreated = program->addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        FragmentShader());

    if (!fragmentCreated)
    {
        qDebug().noquote() << "Fragment shader compilation failed:";
        qDebug().noquote() << program->log();
    }

    bool linked = program->link();

    if (!linked)
    {
        qDebug().noquote() << "Shader program link failed:";
        qDebug().noquote() << program->log();
        return;
    }

    if (!earthMesh.Initialize(
        "C:/SOUL/assets/sphere-cylcoords-1k.obj",
        "C:/SOUL/assets/Texture/earth albedo.jpg",
        program))
    {
        qDebug() << "Failed to initialize Earth mesh";
        return;
    }

    // Load the extra night texture here because the Earth shader samples two maps.
    QImage nightImage("C:/SOUL/assets/Texture/2k_earth_nightmap.jpg");
    if (nightImage.isNull())
    {
        qDebug() << "Failed to load night texture";
    }
    else
    {
        nightImage = nightImage.convertToFormat(QImage::Format_RGBA8888);
        nightTexture = new QOpenGLTexture(QImage(nightImage).flipped());
        nightTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        nightTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        nightTexture->setWrapMode(QOpenGLTexture::Repeat);
    }

    if (!satelliteMesh.Initialize(
        "C:/SOUL/assets/SAT1.obj",
        "C:/SOUL/assets/Texture/10477_Satellite_v1_Diffuse.jpg",
        program))
    {
        qDebug() << "Failed to initialize satellite mesh";
    }

    glEnable(GL_DEPTH_TEST);
    program->release();
}

void Orbitview::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Orbitview::paintGL()
{
    earthRotation += 0.03f;

    //TODO - Calculate Earth's proper rotation here using Quaternion.///

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program)
        return;

    program->bind();

    QMatrix4x4 model;
    model.setToIdentity();

    //scaling the size of the object
    model.scale(0.009f);

    //Rotate the object here
    model.rotate(earthRotation,0.0,1.0,0.0f);

    QMatrix4x4 view = _camera.GetViewMatrix();

    //This calculates the aspect ratio
    float aspect = height() > 0 ? float(width()) / float(height()) : 1.0f;

    QMatrix4x4 projection =_camera.GetProjectionMatrix(aspect);

    QMatrix4x4 mvp = projection * view * model;

    QMatrix3x3 normalMatrix = model.normalMatrix();

    //Set Uniforms
    program->setUniformValue("uMVP", mvp);
    program->setUniformValue("uModel", model);
    program->setUniformValue("uNormalMatrix", normalMatrix);

    program->setUniformValue("uSunDir", QVector3D(-1.0f, 0.0f, 0.0f).normalized());
    program->setUniformValue("uViewPos", QVector3D(0.0f, 0.0f, 3.0f));

    if (nightTexture != nullptr)
    {
        nightTexture->bind(1);
        program->setUniformValue("uNightMap", 1);
    }

    earthMesh.Draw(program, 0, "uDayMap");

    satellite.Update(1.0f / 60.0f);
    QMatrix4x4 satelliteModel = satellite.GetModelMatrix();
    QMatrix4x4 satelliteMvp = projection * view * satelliteModel;
    QMatrix3x3 satelliteNormalMatrix = satelliteModel.normalMatrix();

    program->setUniformValue("uMVP", satelliteMvp);
    program->setUniformValue("uModel", satelliteModel);
    program->setUniformValue("uNormalMatrix", satelliteNormalMatrix);
    program->setUniformValue("uNightMap", 0);
    satelliteMesh.Draw(program, 0, "uDayMap");

    program->release();
    update();
}
