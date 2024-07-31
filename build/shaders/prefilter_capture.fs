#version 330 core

#include common.glsl
#include brdf.glsl

in vec3 worldPos;
out vec4 FragColor;

// globals
uniform samplerCube _envMap;
uniform float roughness;

void main() {
    vec3 N = normalize(worldPos);
    vec3 R = N;
    vec3 V = R;

    const uint NSAMPLES = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0); 

    for(uint i = 0u; i < NSAMPLES; i++) {
        vec2 Xi = Hammersley(i, NSAMPLES);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float ndotl = max(dot(N, L), 0.0);
        if(ndotl > 0.0) {
            prefilteredColor += texture(_envMap, L).rgb * ndotl;
            totalWeight      += ndotl;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}