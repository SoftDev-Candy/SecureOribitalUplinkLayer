//
// Created by Candy on 4/24/2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Orbitview.h" resolved

#include "orbitview.hpp"
#include "Shader/ShaderSource.hpp"
#include<QDebug>
#include<iostream>



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

    //Create a vertices for a triangle --//TODO -- Remove this later no further use only for learning purposes
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    m_vbo.allocate(vertices , sizeof(vertices)); //Upload data to the GPU

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
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0,GL_FLOAT,0,3,3*sizeof(float));

    //Release me 💅 The pain make it stahp//
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
    m_program->bind(); //Bind that big ting
    m_vao.bind();

    glDrawArrays(GL_TRIANGLES , 0, 3);

    m_vao.release();
    m_program->release();
}