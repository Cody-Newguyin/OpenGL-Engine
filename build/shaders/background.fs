#version 330 core

in vec3 worldPos;
out vec4 FragColor;

// globals
uniform samplerCube _envMap;

void main() {    
    vec3 color = texture(_envMap, worldPos).rgb;
    FragColor = vec4(color, 1.0);
}