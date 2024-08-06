#version 330 core

in vec3 worldPos;

uniform vec3 _pointlight0_pos;

out float FragColor;

void main() {
    float lightDistance = length(worldPos - _pointlight0_pos);

    FragColor = lightDistance / 25.0f;
}  