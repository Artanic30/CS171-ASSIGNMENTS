#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    // 环境光照
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射光照
    // normalize片段法向量
    vec3 norm = normalize(Normal);
    // 计算光照方向单位向量：光源位置 - 片段位置
    vec3 lightDir = normalize(lightPos - FragPos);
    // 取片段单位法向量与光照方向单位向量点乘 =cos<光照与法向量夹脚> * 1 * 1 与 0.0 的最大值（背面没有漫反射）
    float diff = max(dot(norm, lightDir), 0.0);
    // 漫反射光照 = 上面的漫反射强度值与光照颜色乘积
    vec3 diffuse = diff * lightColor;

    // 镜面光照
    // 镜面光照系数设置为0.5
    float specularStrength = 0.5;
    // 视角方向单位向量 = 视角位置 - 片段位置
    vec3 viewDir = normalize(viewPos - FragPos);
    // 通过内置函数reflect反射角大小
    vec3 reflectDir = reflect(-lightDir, norm);
    // 取视角方向与反射角方向点乘结果的30次幂(反光度30)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 30);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}