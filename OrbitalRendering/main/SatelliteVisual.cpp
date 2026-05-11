//
// Created by Candy on 5/8/2026.
//

#include "SatelliteVisual.hpp"
#include <QtMath>

void SatelliteVisual::Update(float dt)
{
    // Advance the satellite around the orbit in degrees so the values stay easy to tune.
    orbitAngle += orbitSpeed * dt;

    // Keep the angle in a simple 0..360 range so it does not grow forever over time.
    if (orbitAngle >= 360.0f)
    {
        orbitAngle -= 360.0f;
    }
    else if (orbitAngle < 0.0f)
    {
        orbitAngle += 360.0f;
    }
}

QMatrix4x4 SatelliteVisual::GetModelMatrix() const
{
    // Convert the stored orbit angle into a point on a flat orbit around Earth.
    float angleRad = qDegreesToRadians(orbitAngle);
    float x = orbitRadius * qCos(angleRad);
    float z = orbitRadius * qSin(angleRad);

    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(x, 0.0f, z);
    // Turn the model a little as it moves so it does not look completely static in orbit.
    model.rotate(-orbitAngle, 0.0f, 1.0f, 0.0f);
    model.scale(scale);

    return model;
}
