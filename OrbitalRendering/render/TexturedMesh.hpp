//
// Created by Candy on 5/10/2026.
//

#ifndef SOUL_TEXTUREDMESH_HPP
#define SOUL_TEXTUREDMESH_HPP

#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QString>
#include <string>
#include "MeshData.hpp"

class QOpenGLShaderProgram;

class TexturedMesh
{
public:
    // Loads an OBJ, uploads its mesh data, and creates the texture used when this mesh is drawn.
    bool Initialize(const std::string& objPath, const QString& texturePath, QOpenGLShaderProgram* program);

    // Binds the mesh texture and draws the mesh with the shader program that is already active.
    void Draw(QOpenGLShaderProgram* program, int textureUnit, const char* uniformName);

    // Releases the buffers, texture, and stored mesh data owned by this mesh.
    void Destroy();

private:
    MeshData mesh;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer ebo{QOpenGLBuffer::IndexBuffer};
    QOpenGLTexture* texture = nullptr;
    int indexCount = 0;
};

#endif //SOUL_TEXTUREDMESH_HPP
