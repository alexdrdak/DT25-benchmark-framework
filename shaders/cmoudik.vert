#version 460 core

%{D_DEFINES}%

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

layout (std430, binding=1) buffer d_offsets
{
    vec4 d_OffsetPos[];
};

uniform int d_ssbo_index;

void main()
{   
    mat4 b_vm;
    
    #ifdef D_INSTANCED    
        vec4 vt = view * transform * vec4(d_OffsetPos[gl_InstanceID].xyz, 1.0);
    #elif defined (D_INDIRECT)
        vec4 vt = view * transform * vec4(d_OffsetPos[gl_BaseInstance].xyz, 1.0);        
    #else
        vec4 vt = view * transform * vec4(d_OffsetPos[d_ssbo_index].xyz, 1.0);
    #endif
        
    b_vm[0] = vec4(1.0, 0.0, 0.0, 0.0);
    b_vm[1] = vec4(0.0, 1.0, 0.0, 0.0);
    b_vm[2] = vec4(0.0, 0.0, 1.0, 0.0);
    b_vm[3] = vec4(vt[0], vt[1], vt[2], 1.0);       
    
    #ifdef D_INSTANCED    
        gl_Position = projection * b_vm * vec4(aPos,1.0);    
    #elif defined (D_INDIRECT)
    gl_Position = projection * b_vm * vec4(aPos,1.0);
    #else
        gl_Position = projection * b_vm * vec4(aPos, 1.0);
    #endif
    
    TexCoord = aTexCoord;
}