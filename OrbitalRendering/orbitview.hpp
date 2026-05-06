//
// Created by Candy on 4/24/2026.
//

#ifndef SOUL_ORBITVIEW_HPP
#define SOUL_ORBITVIEW_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include "render/MeshData.hpp"


QT_BEGIN_NAMESPACE

namespace Ui
{
    class Orbitview;
}

QT_END_NAMESPACE

class Orbitview : public QOpenGLWidget , protected QOpenGLFunctions
{
    Q_OBJECT

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer ebo{QOpenGLBuffer::IndexBuffer};
    QOpenGLShaderProgram *program;


public:
    //Constructor
    explicit Orbitview(QWidget *parent = nullptr);

    //Destructor
    ~Orbitview() override;

    //Initialize openGl Function set up rendering , load shader etc ///
    void initializeGL()override;
    //update projection matrix and other size relate issue
    void resizeGL(int w ,int h)override;
    //Basically Draw the scene
    void paintGL()override;

   //Adding Mesh Data here
    MeshData mesh;
    int meshindexCount = 0;

    //Adding rotation//
    float earthRotation = 0.0f;

    //Day and night texture to showcase texture performance
    QOpenGLTexture* dayTexture = nullptr;
    QOpenGLTexture* nightTexture = nullptr;



private:
};


#endif //SOUL_ORBITVIEW_HPP