#ifndef _COMMON_GLSL
#define _COMMON_GLSL

// Define Macros
#define PI 3.14159265359
#define GAMMA 2.2
#define MAX_LOD 4.0f

float DotClamped(vec3 vector0, vec3 vector1) {
    return max(dot(vector0, vector1), 0.0);
}

float OneMinusReflectivityFromMetallic(float metallic) {
    float oneMinusReflectivity = 0.96f - metallic * 0.96f;
    return oneMinusReflectivity;
}

vec3 DiffuesAndSpecullarFromMetallic(vec3 albedo, float metallic, out vec3 specularTint, out float oneMinusReflectivity) {
    specularTint = mix(vec3(0.04f), albedo, metallic);
    oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
    return albedo * oneMinusReflectivity;
}

// Normal distribution function 
// Approximates surface aligning with the halfway vecor based on smoothness
float DistributionGGX(vec3 normal, vec3 halfVector, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float ndoth = DotClamped(normal, halfVector);
    float ndoth2 = ndoth * ndoth;

    float top = a2;
    float bot = (ndoth2 * (a2 - 1.0) + 1.0);
    bot = PI * bot * bot;
    return top / bot;
}

// Geometry function
// Approximates the surface overshadowing itself
float GeometrySchlickGGX(float ndotv, float roughness) {
    float r = roughness + 1;
    float k = (r * r) / 8.0;

    float top = ndotv;
    float bot = ndotv * (1.0 - k) + k;

    return top / bot;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float k) {
    return GeometrySchlickGGX(DotClamped(normal, viewDir), k) * 
           GeometrySchlickGGX(DotClamped(normal, lightDir), k);
}

// Fresnel function
// Approximates ratio of surface relfection at varying surface angles
vec3 FresnelSchlick(float cosTheta, vec3 specularTint) {
    return specularTint + (vec3(1.0) - specularTint) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

#endif