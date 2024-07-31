#version 330 core

#include common.glsl

in vec3 worldPos;
out vec4 FragColor;

// globals
uniform samplerCube _envMap;

void main() {		
    // vector from origin to cubemap
    vec3 normal = normalize(worldPos);
  
    vec3 irradiance = vec3(0.0);
    
    vec3 up = vec3 (0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    
    float stride = 0.025;
    float nrSamples = 0.0;
    // convolution of environment map
    for(float phi = 0.0; phi < 2.0 * PI; phi += stride)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += stride)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += texture(_envMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
  
    FragColor = vec4(irradiance, 1.0);
}