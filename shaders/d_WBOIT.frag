#version 460 core



layout(location = 0) out vec4 accumColor;  // RGBA16F
layout(location = 1) out vec4 revealage;   // R8

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

layout (std430, binding=1) buffer d_offsets
{
    vec4 d_OffsetPos[];
};

void main()
{
    vec4 color = texture(texture_diffuse1, TexCoord);

    // Insert your favorite weighting function here. The color-based factor
    // avoids color pollution from the edges of wispy clouds. The z-based
    // factor gives precedence to nearer surfaces.
    float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) * clamp(0.03 / (1e-5 + pow(gl_FragCoord.z / 200.0, 4.0)), 1e-2, 3e3);

    // Blend Func: GL_ONE, GL_ONE
    // Switch to premultiplied alpha and weight
    accumColor = vec4(color.rgb * color.a, color.a) * weight;

    // Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA
    revealage = vec4(color.a);    
}