//
// Created by Candy on 4/24/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Orbitview.h" resolved

#include "orbitview.hpp"
#include "Shader/ShaderSource.hpp"
#include<QDebug>
#include<iostream>
#include<QMatrix4x4>
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
    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
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
    m_program = new QOpenGLShaderProgram(this);
    bool VertexCreated = m_program->addShaderFromSourceCode(
    QOpenGLShader::Vertex,
    VertexShader());

    if (!VertexCreated)
    {
        qDebug().noquote() << "Vertex shader compilation failed:";
        qDebug().noquote() << m_program->log();
    }

    bool FragmentCreated = m_program->addShaderFromSourceCode(
        QOpenGLShader::Fragment,
        FragmentShader());

    if (!FragmentCreated)
    {
        qDebug().noquote() << "Fragment shader compilation failed:";
        qDebug().noquote() << m_program->log();
    }

    bool linked = m_program->link(); // Link the shaders to the GPU

    if (!linked)
    {
        qDebug().noquote() << "Shader program link failed:";
        qDebug().noquote() << m_program->log();
    }

    m_program->bind(); //Bind that big ting

    //Need to tell the GPU how to read the data
    m_program->enableAttributeArray(0); // position
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));

    m_program->enableAttributeArray(1); // UV
    m_program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 8 * sizeof(float));

    m_program->enableAttributeArray(2); // normal
    m_program->setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, 8 * sizeof(float));
    //Release me 💅 The pain make it stahp//
    glEnable(GL_DEPTH_TEST);
    m_vao.release();
    m_vbo.release();
    m_program->release();
}

void Orbitview::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Orbitview::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_program)
        return;

    m_program->bind(); //Bind that big ting
    m_vao.bind();

    QMatrix4x4 model;
    model.setToIdentity();

    //scaling the size of the object
    model.scale(0.3f);

    QMatrix4x4 view;
    view.setToIdentity();

    // Move camera/object relationship back so we can see the mesh
    view.translate(0.0f, 0.0f, -5.0f);

    QMatrix4x4 projection;
    projection.setToIdentity();

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.0f;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 mvp = projection * view * model;

    m_program->setUniformValue("uMVP", mvp);

    glDrawElements(GL_TRIANGLES, meshindexCount, GL_UNSIGNED_INT, nullptr);
    m_vao.release();
    m_program->release();
}