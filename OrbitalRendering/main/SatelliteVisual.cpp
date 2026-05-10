//
// Created by Candy on 5/8/2026.
//

#include "SatelliteVisual.hpp"
#include <QtMath>

void SatelliteVisual::Update(float dt)
{
    orbitAngle += orbitSpeed * dt;

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
    float angleRad = qDegreesToRadians(orbitAngle);
    float x = orbitRadius * qCos(angleRad);
    float z = orbitRadius * qSin(angleRad);

    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(x, 0.0f, z);
    model.rotate(-orbitAngle, 0.0f, 1.0f, 0.0f);
    model.scale(scale);

    return model;
}
