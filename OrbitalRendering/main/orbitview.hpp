//
// Created by Candy on 4/24/2026.
//

#ifndef SOUL_ORBITVIEW_HPP
#define SOUL_ORBITVIEW_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QPoint>
#include <QWheelEvent>
#include <QMouseEvent>
#include "../render/TexturedMesh.hpp"
#include "Camera.hpp"
#include "SatelliteVisual.hpp"


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
    QOpenGLShaderProgram *program = nullptr;
    QOpenGLShaderProgram *orbitRingProgram = nullptr;
    QOpenGLShaderProgram *satelliteGlowProgram = nullptr;
    QOpenGLVertexArrayObject orbitRingVao;
    QOpenGLBuffer orbitRingVbo{QOpenGLBuffer::VertexBuffer};
    int orbitRingVertexCount = 0;
    QOpenGLVertexArrayObject satelliteGlowVao;
    QOpenGLBuffer satelliteGlowVbo{QOpenGLBuffer::VertexBuffer};

protected:
    // Zooms the camera in or out using the mouse wheel.
    void wheelEvent(QWheelEvent *event) override;
    // Saves the mouse position so drag rotation can start from the right spot.
    void mousePressEvent(QMouseEvent* event) override;
    // Rotates the camera while the left mouse button is dragged.
    void mouseMoveEvent(QMouseEvent* event) override;

public:
    // Builds the OpenGL widget and turns on the mouse input used by the camera.
    explicit Orbitview(QWidget *parent = nullptr);

    // Cleans up the OpenGL resources owned by the widget before shutdown.
    ~Orbitview() override;

    // Creates the shader program, uploads the Earth mesh, and loads the textures it needs.
    void initializeGL()override;
    // Keeps the OpenGL viewport matched to the widget size.
    void resizeGL(int w ,int h)override;
    // Draws the Earth and satellite with the current camera and shader uniforms.
    void paintGL()override;

    //Adding rotation//
    float earthRotation = 0.0f;

    //Earth mesh and day texture live together here so Orbitview stays smaller
    TexturedMesh earthMesh;

    //Satellite mesh uses the same shader for now so it can be added with minimal changes
    TexturedMesh satelliteMesh;

    //Night texture stays separate because the Earth shader still samples two maps
    QOpenGLTexture* nightTexture = nullptr;

    //Camera object
    Camera _camera;

    SatelliteVisual satellite;

    QPoint lastMousePos;

private:
};


#endif //SOUL_ORBITVIEW_HPP
