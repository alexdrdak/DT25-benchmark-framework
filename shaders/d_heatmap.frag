#version 460 core

uniform usampler2D heatmapTex;
out vec4 FragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    uint count = texture(heatmapTex, uv).r;

    FragColor = vec4(count/256.0, count/256.0, count/256.0, 1.0); // Normalize and colorize
}