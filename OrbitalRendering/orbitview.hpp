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
    QOpenGLShaderProgram *m_program;


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



private:
};


#endif //SOUL_ORBITVIEW_HPP