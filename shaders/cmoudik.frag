#version 460 core
out vec4 FragColor;

%{D_DEFINES}%

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

layout(early_fragment_tests) in; // optional,
layout (binding = 0, r32ui) uniform uimage2D heatmap;

layout (std430, binding=1) buffer d_offsets
{
    vec4 d_OffsetPos[];
};

float near = 0.1;
float far  = 2.0;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
};

void main()
{
#ifdef D_HEATMAPWRITE
    imageAtomicAdd(heatmap, ivec2(gl_FragCoord.xy), 10);
#endif

#ifdef D_WIREFRAMED
    FragColor = vec4(1.0);
#else
    FragColor = texture(texture_diffuse1, TexCoord);
#endif
}