//
// Created by Candy on 5/8/2026.
//

#ifndef SOUL_SATELLITEVISUAL_HPP
#define SOUL_SATELLITEVISUAL_HPP

#include <QMatrix4x4>

class SatelliteVisual
{
public:
    float orbitRadius = 2.1f;
    float orbitAngle = 0.0f;
    float orbitSpeed = 20.0f;
    float scale = 0.00001f;

    // Moves the satellite forward around its orbit using the frame time step.
    void Update(float dt);

    // Builds the model matrix for the satellite from its orbit settings.
    QMatrix4x4 GetModelMatrix() const;
};


#endif //SOUL_SATELLITEVISUAL_HPP
