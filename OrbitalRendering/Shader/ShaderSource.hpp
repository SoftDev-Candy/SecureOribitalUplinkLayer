//
// Created by Candy on 4/27/2026.
//

#ifndef SOUL_SHADERSOURCE_HPP
#define SOUL_SHADERSOURCE_HPP

const char* VertexShader()
{

return R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
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
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
    )";

}

#endif //SOUL_SHADERSOURCE_HPP