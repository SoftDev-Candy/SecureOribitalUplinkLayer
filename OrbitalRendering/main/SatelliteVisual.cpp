//
// Created by Candy on 5/8/2026.
//

#include "SatelliteVisual.hpp"
#include <QtMath>

void SatelliteVisual::Update(float dt)
{
    // Just keep pushing the satellite around the ring every frame.
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

QVector3D SatelliteVisual::GetOrbitPosition() const
{
    // This is the normal "where is the satellite right now" question.
    return GetOrbitPositionForAngle(orbitAngle);
}

QVector3D SatelliteVisual::GetOrbitPositionForAngle(float angleDeg) const
{
    // Phase is like saying "start this one somewhere else" so the sky is less awkward.
    float totalAngleDeg = angleDeg + orbitPhaseDeg;
    float angleRad = qDegreesToRadians(totalAngleDeg);
    float x = orbitRadius * qCos(angleRad);
    float z = orbitRadius * qSin(angleRad);

    // Build the orbit on the flat XZ plane first, then tilt the whole thing.
    QVector3D flatOrbitPos(x, 0.0f, z);
    QMatrix4x4 tiltMatrix;
    tiltMatrix.setToIdentity();
    tiltMatrix.rotate(orbitTiltDeg, 1.0f, 0.0f, 0.0f);

    return tiltMatrix.map(flatOrbitPos);
}

QMatrix4x4 SatelliteVisual::GetModelMatrix() const
{
    QVector3D orbitPos = GetOrbitPosition();

    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(orbitPos);

    // Give the mesh a little spin and also lean it with the orbit plane so it feels less pasted on.
    model.rotate(orbitTiltDeg, 1.0f, 0.0f, 0.0f);
    model.rotate(-(orbitAngle + orbitPhaseDeg), 0.0f, 1.0f, 0.0f);
    model.scale(scale);

    return model;
}
