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
        uniform mat4 uModel;
        uniform mat3 uNormalMatrix;

        out vec2 vUV;
        out vec3 vWorldPos;
        out vec3 vNormal;

        void main()
        {
            vUV = aUV;

            vec4 worldPos = uModel * vec4(aPos, 1.0);
            vWorldPos = worldPos.xyz;

            vNormal = normalize(uNormalMatrix * aNormal);

            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";
}

const char* FragmentShader()
{
    return R"(
        #version 330 core

        in vec2 vUV;
        in vec3 vWorldPos;
        in vec3 vNormal;

        out vec4 FragColor;

        uniform sampler2D uDayMap;
        uniform sampler2D uNightMap;

        uniform vec3 uSunDir;
        uniform vec3 uViewPos;

        void main()
        {
            vec3 N = normalize(vNormal);
            vec3 L = normalize(-uSunDir);

            float dayAmount = dot(N, L);
            dayAmount = smoothstep(-0.15, 0.25, dayAmount);

            vec3 dayColor = texture(uDayMap, vUV).rgb;
            vec3 nightColor = texture(uNightMap, vUV).rgb;

            vec3 earthColor = mix(nightColor, dayColor, dayAmount);

            FragColor = vec4(earthColor, 1.0);
        }
    )";
}

#endif //SOUL_SHADERSOURCE_HPP