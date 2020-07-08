#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 lightColor;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord); // set alle 4 vector values to 1.0
}