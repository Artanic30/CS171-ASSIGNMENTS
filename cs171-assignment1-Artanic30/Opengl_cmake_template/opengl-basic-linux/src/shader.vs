#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   // FragPos: 模块世界坐标
   FragPos = vec3(model * vec4(aPos, 1.0));
   // 法向量从物品本地坐标系转换为世界坐标系
   Normal = mat3(transpose(inverse(model))) * aNormal;
}