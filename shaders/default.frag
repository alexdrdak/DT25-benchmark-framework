#version 460 core
out vec4 FragColor;

%{D_DEFINES}%

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

void main()
{
    #ifdef D_WIREFRAMED
        FragColor = vec4(1.0);
    #else
        FragColor = texture(texture_diffuse1, TexCoord);
    #endif
}