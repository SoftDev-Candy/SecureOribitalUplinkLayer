//
// Created by Candy on 4/27/2026.
//

#ifndef SOUL_SHADERSOURCE_HPP
#define SOUL_SHADERSOURCE_HPP

const char* VertexShader()
{
    return R"(
        #version 330 core

        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aUV;
        layout(location = 2) in vec3 aNormal;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";
}

const char* FragmentShader()
{
    return R"(
        #version 330 core

        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(0.1, 0.35, 1.0, 1.0);
        }
    )";
}

#endif //SOUL_SHADERSOURCE_HPP