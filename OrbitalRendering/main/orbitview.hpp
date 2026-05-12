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
#include <array>
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

    struct SatRenderData
    {
        // Name is what we line up with the UI table and the SQLite rows.
        QString name;
        // Visual holds the orbit math and model transform knobs.
        SatelliteVisual visual;
    };

private:
    // Shader for the textured Earth and the tiny satellite mesh.
    QOpenGLShaderProgram *program = nullptr;
    // Separate little shader because orbit trails are just colored lines, not textured models.
    QOpenGLShaderProgram *orbitRingProgram = nullptr;
    // Separate point shader so satellites do not vanish into the void the second we blink.
    QOpenGLShaderProgram *satelliteGlowProgram = nullptr;
    // These buffers are reused for each satellite trail instead of making three separate copies.
    QOpenGLVertexArrayObject orbitRingVao;
    QOpenGLBuffer orbitRingVbo{QOpenGLBuffer::VertexBuffer};
    int orbitRingVertexCount = 0;
    // Same idea here, one tiny glow point buffer reused for whichever satellite we are drawing.
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
    // Stores which satellite the operator picked so we can highlight it in the scene.
    void SetSelectedSatellite(const QString& satelliteName);
    // Stores the current link status for the selected satellite so we can tint it a bit.
    void SetSatelliteLinkStatus(const QString& linkStatus);

    // Tiny Earth spin value so the planet does not just sit there looking suspicious.
    float earthRotation = 0.0f;

    //Earth mesh and day texture live together here so Orbitview stays smaller
    TexturedMesh earthMesh;

    //Satellite mesh uses the same shader for now so it can be added with minimal changes
    TexturedMesh satelliteMesh;

    //Night texture stays separate because the Earth shader still samples two maps
    QOpenGLTexture* nightTexture = nullptr;

    // Camera object
    Camera _camera;

    // These are the three little orbit gremlins that match SAT_1, SAT_2, and SAT_3 in the backend.
    std::array<SatRenderData, 3> satellites;
    QString selectedSatelliteName;
    QString satelliteLinkStatus = "Disconnected";

    // Mouse drag remembers the last click spot so orbit controls do not jump around like chaos.
    QPoint lastMousePos;

private:
};


#endif //SOUL_ORBITVIEW_HPP
