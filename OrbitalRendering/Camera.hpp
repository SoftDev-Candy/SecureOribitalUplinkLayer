//
// Created by Candy on 5/2/2026.
//

#ifndef SOUL_CAMERA_HPP
#define SOUL_CAMERA_HPP

#include<QMatrix4x4>
#include <QVector3D>


class Camera
{
public:
    Camera();

    QMatrix4x4 GetViewMatrix()const;

    QMatrix4x4 GetProjectionMatrix(float aspect) const;

    void AddYaw(float yaw_delta);
    void AddPitch(float pitch_delta);
    void AddZoom(float zoom_delta);
    void CameraControls();

    //Reset the camera back to its original state
    void CameraReset();

private:
    //Target for the camera
    QVector3D cam_target = QVector3D(0.0f,0.0f,0.0f);

    //Distance from the object
    float cam_distance = 4.0f;

    //horizontal orbit around Earth
    float cam_yaw = 0.0f;

    //Vertical orbital angle
    float cam_pitch = 0.0f;

    //Width of camera lens
    float cam_fov = 45.0f;

    float cam_minDistance = 1.5f;
    float cam_maxDistance = 20.0f;




};


#endif //SOUL_CAMERA_HPP