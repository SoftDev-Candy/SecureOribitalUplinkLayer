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
#include <QtMath>
#include <vector>

namespace
{
const char* OrbitRingVertexShader()
{
    return R"(
        #version 330 core

        layout(location = 0) in vec3 aPos;

        uniform mat4 uMVP;

        out vec3 vRingPos;

        void main()
        {
            vRingPos = aPos;
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";
}

const char* OrbitRingFragmentShader()
{
    return R"(
        #version 330 core

        in vec3 vRingPos;

        out vec4 FragColor;

        uniform vec3 uColor;

        void main()
        {
            float angle = atan(vRingPos.z, vRingPos.x);
            float dashPattern = fract((angle + 3.14159265) / 6.28318530 * 28.0);

            if (dashPattern > 0.18)
            {
                discard;
            }

            FragColor = vec4(uColor, 1.0);
        }
    )";
}

const char* SatelliteGlowVertexShader()
{
    return R"(
        #version 330 core

        layout(location = 0) in vec3 aPos;

        uniform mat4 uMVP;
        uniform float uPointSize;

        void main()
        {
            gl_Position = uMVP * vec4(aPos, 1.0);
            gl_PointSize = uPointSize;
        }
    )";
}

const char* SatelliteGlowFragmentShader()
{
    return R"(
        #version 330 core

        out vec4 FragColor;

        uniform vec3 uColor;

        void main()
        {
            vec2 uv = gl_PointCoord * 2.0 - 1.0;
            float dist = dot(uv, uv);

            if (dist > 1.0)
            {
                discard;
            }

            float alpha = smoothstep(1.0, 0.0, dist);
            FragColor = vec4(uColor, alpha * 0.9);
        }
    )";
}
}

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
        // OpenGL objects need the widget context to still be current while we destroy them.
        makeCurrent();
        earthMesh.Destroy();
        satelliteMesh.Destroy();

        if (orbitRingVbo.isCreated())
        {
            orbitRingVbo.destroy();
        }

        if (orbitRingVao.isCreated())
        {
            orbitRingVao.destroy();
        }

        if (orbitRingProgram != nullptr)
        {
            delete orbitRingProgram;
            orbitRingProgram = nullptr;
        }

        if (satelliteGlowVbo.isCreated())
        {
            satelliteGlowVbo.destroy();
        }

        if (satelliteGlowVao.isCreated())
        {
            satelliteGlowVao.destroy();
        }

        if (satelliteGlowProgram != nullptr)
        {
            delete satelliteGlowProgram;
            satelliteGlowProgram = nullptr;
        }

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

    // Build the shared shader once, then both Earth and satellite can use the same program.
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

    // Earth keeps its day texture inside the reusable mesh helper.
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

    // Build a tiny line shader because the Earth shader would throw a tantrum over plain vertices.
    orbitRingProgram = new QOpenGLShaderProgram(this);
    bool orbitRingVertexCreated = orbitRingProgram->addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        OrbitRingVertexShader());

    if (!orbitRingVertexCreated)
    {
        qDebug().noquote() << "Orbit ring vertex shader compilation failed:";
        qDebug().noquote() << orbitRingProgram->log();
    }

    bool orbitRingFragmentCreated = orbitRingProgram->addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        OrbitRingFragmentShader());

    if (!orbitRingFragmentCreated)
    {
        qDebug().noquote() << "Orbit ring fragment shader compilation failed:";
        qDebug().noquote() << orbitRingProgram->log();
    }

    bool orbitRingLinked = orbitRingProgram->link();

    if (!orbitRingLinked)
    {
        qDebug().noquote() << "Orbit ring shader link failed:";
        qDebug().noquote() << orbitRingProgram->log();
    }
    else
    {
        orbitRingVertexCount = 72;

        if (!orbitRingVao.create())
        {
            qDebug() << "Failed to create orbit ring VAO";
        }
        else
        {
            orbitRingVao.bind();

            if (!orbitRingVbo.create() || !orbitRingVbo.bind())
            {
                qDebug() << "Failed to create or bind orbit ring VBO";
            }
            else
            {
                // This buffer gets rewritten every frame so the trail can chase the satellite like unpaid rent.
                orbitRingVbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
                orbitRingVbo.allocate(orbitRingVertexCount * 3 * static_cast<int>(sizeof(float)));

                orbitRingProgram->bind();
                orbitRingProgram->enableAttributeArray(0);
                orbitRingProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
                orbitRingProgram->release();
                orbitRingVbo.release();
            }

            orbitRingVao.release();
        }
    }

    satelliteGlowProgram = new QOpenGLShaderProgram(this);
    bool satelliteGlowVertexCreated = satelliteGlowProgram->addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        SatelliteGlowVertexShader());

    if (!satelliteGlowVertexCreated)
    {
        qDebug().noquote() << "Satellite glow vertex shader compilation failed:";
        qDebug().noquote() << satelliteGlowProgram->log();
    }

    bool satelliteGlowFragmentCreated = satelliteGlowProgram->addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        SatelliteGlowFragmentShader());

    if (!satelliteGlowFragmentCreated)
    {
        qDebug().noquote() << "Satellite glow fragment shader compilation failed:";
        qDebug().noquote() << satelliteGlowProgram->log();
    }

    bool satelliteGlowLinked = satelliteGlowProgram->link();

    if (!satelliteGlowLinked)
    {
        qDebug().noquote() << "Satellite glow shader link failed:";
        qDebug().noquote() << satelliteGlowProgram->log();
    }
    else
    {
        if (!satelliteGlowVao.create())
        {
            qDebug() << "Failed to create satellite glow VAO";
        }
        else
        {
            satelliteGlowVao.bind();

            if (!satelliteGlowVbo.create() || !satelliteGlowVbo.bind())
            {
                qDebug() << "Failed to create or bind satellite glow VBO";
            }
            else
            {
                satelliteGlowVbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
                satelliteGlowVbo.allocate(3 * static_cast<int>(sizeof(float)));

                satelliteGlowProgram->bind();
                satelliteGlowProgram->enableAttributeArray(0);
                satelliteGlowProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
                satelliteGlowProgram->release();
                satelliteGlowVbo.release();
            }

            satelliteGlowVao.release();
        }
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

    // Earth draw pass
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

    // Set the transforms and lighting used by the Earth pass.
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

    // Orbit trail draw pass
    if (orbitRingProgram != nullptr && orbitRingVao.isCreated() && orbitRingVertexCount > 0)
    {
        std::vector<float> orbitTrailVertices;
        orbitTrailVertices.reserve(orbitRingVertexCount * 3);

        float trailSpan = 180.0f;

        // Open trail only, no full donut nonsense. It starts behind the satellite and keeps chasing it.
        for (int i = 0; i < orbitRingVertexCount; ++i)
        {
            float t = orbitRingVertexCount > 1
                ? static_cast<float>(i) / static_cast<float>(orbitRingVertexCount - 1)
                : 0.0f;
            float angleDeg = satellite.orbitAngle - t * trailSpan;
            float angleRad = qDegreesToRadians(angleDeg);
            float x = satellite.orbitRadius * qCos(angleRad);
            float z = satellite.orbitRadius * qSin(angleRad);

            orbitTrailVertices.push_back(x);
            orbitTrailVertices.push_back(0.0f);
            orbitTrailVertices.push_back(z);
        }

        orbitRingVbo.bind();
        orbitRingVbo.allocate(
            orbitTrailVertices.data(),
            static_cast<int>(orbitTrailVertices.size() * sizeof(float)));
        orbitRingVbo.release();

        orbitRingProgram->bind();
        orbitRingProgram->setUniformValue("uMVP", projection * view);
        orbitRingProgram->setUniformValue("uColor", QVector3D(0.62f, 0.69f, 0.78f));

        glLineWidth(2.0f);
        orbitRingVao.bind();
        glDrawArrays(GL_LINE_STRIP, 0, orbitRingVertexCount);
        orbitRingVao.release();
        orbitRingProgram->release();
        program->bind();
    }

    // Satellite draw pass
    // Reuse the same shader with a different model transform for the orbiting satellite.
    program->setUniformValue("uMVP", satelliteMvp);
    program->setUniformValue("uModel", satelliteModel);
    program->setUniformValue("uNormalMatrix", satelliteNormalMatrix);
    program->setUniformValue("uNightMap", 0);
    satelliteMesh.Draw(program, 0, "uDayMap");

    if (satelliteGlowProgram != nullptr && satelliteGlowVao.isCreated())
    {
        QVector3D glowPos(
            satelliteModel(0, 3),
            satelliteModel(1, 3),
            satelliteModel(2, 3));
        float glowVertex[3] = {glowPos.x(), glowPos.y(), glowPos.z()};

        satelliteGlowVbo.bind();
        satelliteGlowVbo.allocate(glowVertex, static_cast<int>(sizeof(glowVertex)));
        satelliteGlowVbo.release();

        // Tiny glow goblin so the satellite stops playing hide and seek in the void.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        satelliteGlowProgram->bind();
        satelliteGlowProgram->setUniformValue("uMVP", projection * view);
        satelliteGlowProgram->setUniformValue("uColor", QVector3D(0.82f, 0.92f, 1.0f));
        satelliteGlowProgram->setUniformValue("uPointSize", 20.0f);
        satelliteGlowVao.bind();
        glDrawArrays(GL_POINTS, 0, 1);
        satelliteGlowVao.release();
        satelliteGlowProgram->release();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    program->release();
    update();
}
