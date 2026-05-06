//
// Created by Candy on 4/24/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Orbitview.h" resolved

#include "orbitview.hpp"
#include "Shader/ShaderSource.hpp"
#include<QDebug>
#include<iostream>
#include<QMatrix4x4>
#include <QImage>
#include <QOpenGLTexture>
#include "render/ObjLoader.hpp"


Orbitview::Orbitview(QWidget *parent) : QOpenGLWidget(parent)
{
}

Orbitview::~Orbitview()
{
}

void Orbitview::initializeGL()
{
    initializeOpenGLFunctions();

    //Load and initialize texture here
    QImage dayImage("C:/SOUL/assets/Texture/earth albedo.jpg");
    QImage nightImage("C:/SOUL/assets/Texture/earth night_lights_modified.png");

    //Throw error if texture obj is NUll
    if (dayImage.isNull())
    {
        qDebug() << "Failed to load day texture";
    }
    else
    {
        dayImage = dayImage.convertToFormat(QImage::Format_RGBA8888);
        dayTexture = new QOpenGLTexture(dayImage.mirrored());
        dayTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        dayTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        dayTexture->setWrapMode(QOpenGLTexture::Repeat);
    }

    //check if nighttexture is NULL if not bull then initialize it in the right format
    if (nightImage.isNull())
    {
        qDebug() << "Failed to load night texture";
    }
    else
    {
        nightImage = nightImage.convertToFormat(QImage::Format_RGBA8888);
        nightTexture = new QOpenGLTexture(nightImage.mirrored());
        nightTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        nightTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        nightTexture->setWrapMode(QOpenGLTexture::Repeat);
    }

    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);

    //Load obj here and load it now if not loaded god help us all

    LoadObjToMeshData("C:/SOUL/assets/Earth.obj",mesh);

   bool Vao_created = m_vao.create();//Creates the OpenGL VAO

    if (!Vao_created)
    {
        std::cerr<<"Failed to created VAO";
        return;
    }

    if (m_vao.isCreated())
    {
        m_vao.bind();//Start recording the VAO
    }

    //Create and bind VBO here
    bool Vbo_created =  m_vbo.create();      // creates GPU buffer
    bool Vbo_bind =  m_vbo.bind();        // make this buffer active

    //Checks for VBO create and Bind
    if (!Vbo_created)
    {
        std::cerr<<"Failed to created VBO";
        return;
    }
    if (!Vbo_bind)
    {
        std::cerr<<"Failed to bind VBO";
        return;
    }


    // After VBO
    bool Ebo_created = ebo.create();
    bool Ebo_bind = ebo.bind();

    if (!Ebo_created || !Ebo_bind) {
        std::cerr << "Failed to create/bind EBO";
        return;
    }

    m_vbo.allocate(mesh.vertices.data(), mesh.vertices.size() * sizeof(float));
    ebo.allocate(mesh.indices.data(), mesh.indices.size() * sizeof(unsigned int));

    meshindexCount = mesh.indices.size();

    std::cout << "Vertices: " << mesh.vertices.size() << "\n";
    std::cout << "Indices: " << meshindexCount << "\n";
    //
    program = new QOpenGLShaderProgram(this);
    bool VertexCreated = program->addShaderFromSourceCode(
    QOpenGLShader::Vertex,
    VertexShader());

    if (!VertexCreated)
    {
        qDebug().noquote() << "Vertex shader compilation failed:";
        qDebug().noquote() << program->log();
    }

    bool FragmentCreated = program->addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        FragmentShader());

    if (!FragmentCreated)
    {
        qDebug().noquote() << "Fragment shader compilation failed:";
        qDebug().noquote() << program->log();
    }

    bool linked = program->link(); // Link the shaders to the GPU

    if (!linked)
    {
        qDebug().noquote() << "Shader program link failed:";
        qDebug().noquote() << program->log();
    }

    program->bind(); //Bind that big ting

    //Need to tell the GPU how to read the data
    program->enableAttributeArray(0); // position
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));

    program->enableAttributeArray(1); // UV
    program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 8 * sizeof(float));

    program->enableAttributeArray(2); // normal
    program->setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, 8 * sizeof(float));
    //Release me 💅 The pain make it stahp//
    glEnable(GL_DEPTH_TEST);
    m_vao.release();
    m_vbo.release();
    program->release();
}

void Orbitview::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Orbitview::paintGL()
{
    earthRotation += 0.2f;

    //TODO - Calculate Earth's proper rotation here using Quaternion.///

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program)
        return;

    program->bind(); //Bind that big ting
    m_vao.bind();

    QMatrix4x4 model;
    model.setToIdentity();

    //scaling the size of the object
    model.scale(0.3f);

    //Rotate the object here
    model.rotate(earthRotation,0.0,1.0,0.0f);

    QMatrix4x4 view;
    view.setToIdentity();

    // Move camera/object relationship back so we can see the mesh
    view.translate(0.0f, 0.0f, -5.0f);

    QMatrix4x4 projection;
    projection.setToIdentity();

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.0f;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 mvp = projection * view * model;

    QMatrix3x3 normalMatrix = model.normalMatrix();

    //Set Uniforms
    program->setUniformValue("uMVP", mvp);
    program->setUniformValue("uModel", model);
    program->setUniformValue("uNormalMatrix", normalMatrix);

    program->setUniformValue("uSunDir", QVector3D(-1.0f, 0.2f, -0.3f).normalized());
    program->setUniformValue("uViewPos", QVector3D(0.0f, 0.0f, 3.0f));
    program->setUniformValue("uMVP", mvp);

    //Bind Textures here
    if (dayTexture)
    {
        dayTexture->bind(0);
        program->setUniformValue("uDayMap", 0);
    }

    if (nightTexture)
    {
        nightTexture->bind(1);
        program->setUniformValue("uNightMap", 1);
    }

    update();

    glDrawElements(GL_TRIANGLES, meshindexCount, GL_UNSIGNED_INT, nullptr);
    m_vao.release();
    program->release();
}