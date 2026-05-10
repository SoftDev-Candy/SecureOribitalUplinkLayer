//
// Created by Candy on 5/10/2026.
//

#include "TexturedMesh.hpp"
#include <QDebug>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "ObjLoader.hpp"

bool TexturedMesh::Initialize(const std::string& objPath, const QString& texturePath, QOpenGLShaderProgram* program)
{
    Destroy();

    if (program == nullptr)
    {
        qDebug() << "TexturedMesh needs a valid shader program";
        return false;
    }

    if (!LoadObjToMeshData(objPath, mesh))
    {
        qDebug() << "Failed to load mesh:" << QString::fromStdString(objPath);
        return false;
    }

    if (mesh.indices.empty())
    {
        qDebug() << "Mesh has no indices:" << QString::fromStdString(objPath);
        mesh.vertices.clear();
        mesh.indices.clear();
        return false;
    }

    QImage image(texturePath);
    if (image.isNull())
    {
        qDebug() << "Failed to load mesh texture:" << texturePath;
        mesh.vertices.clear();
        mesh.indices.clear();
        return false;
    }

    image = image.convertToFormat(QImage::Format_RGBA8888);
    texture = new QOpenGLTexture(QImage(image).flipped());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);

    if (!vao.create())
    {
        qDebug() << "Failed to create mesh VAO";
        Destroy();
        return false;
    }

    vao.bind();

    if (!vbo.create() || !vbo.bind())
    {
        qDebug() << "Failed to create or bind mesh VBO";
        vao.release();
        Destroy();
        return false;
    }

    if (!ebo.create() || !ebo.bind())
    {
        qDebug() << "Failed to create or bind mesh EBO";
        vao.release();
        vbo.release();
        Destroy();
        return false;
    }

    vbo.allocate(mesh.vertices.data(), static_cast<int>(mesh.vertices.size() * sizeof(float)));
    ebo.allocate(mesh.indices.data(), static_cast<int>(mesh.indices.size() * sizeof(unsigned int)));
    indexCount = static_cast<int>(mesh.indices.size());

    program->bind();
    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 8 * sizeof(float));

    program->enableAttributeArray(2);
    program->setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, 8 * sizeof(float));

    vao.release();
    vbo.release();
    ebo.release();
    program->release();

    return true;
}

void TexturedMesh::Draw(QOpenGLShaderProgram* program, int textureUnit, const char* uniformName)
{
    if (program == nullptr || texture == nullptr || indexCount == 0 || !vao.isCreated())
    {
        return;
    }

    QOpenGLContext* current = QOpenGLContext::currentContext();
    if (current == nullptr)
    {
        return;
    }

    texture->bind(textureUnit);
    program->setUniformValue(uniformName, textureUnit);

    vao.bind();
    current->functions()->glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    vao.release();
}

void TexturedMesh::Destroy()
{
    if (texture != nullptr)
    {
        delete texture;
        texture = nullptr;
    }

    if (ebo.isCreated())
    {
        ebo.destroy();
    }

    if (vbo.isCreated())
    {
        vbo.destroy();
    }

    if (vao.isCreated())
    {
        vao.destroy();
    }

    mesh.vertices.clear();
    mesh.indices.clear();
    indexCount = 0;
}
