#version 330 core

#include common.glsl
#include uniforms.glsl

in Interpolator {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
} input;
out vec4 FragColor;

// globals
uniform float _farPlane = 25.0f;

// PBR stuff
uniform samplerCube _irrMap;
uniform samplerCube _prefilterMap;
uniform sampler2D _brdfLUT;

// Shadow stuff
uniform sampler2D _shadowMap[n_lights];
uniform samplerCube _shadowCubeMap[n_lights];

uniform float _smoothness = 0.1f;
uniform float _metallic = 0.0f;
uniform sampler2D _mainTex;
uniform sampler2D _detailTex;

struct Light {
    vec3 color;
    vec3 dir;
    vec3 pos;
};

float CalculateDirShadow(sampler2D shadowMap, vec4 shadowCoords, vec3 normal, vec3 lightDir) {
    vec3 coords = shadowCoords.xyz / shadowCoords.w;
    coords = coords * 0.5 + 0.5;
    float currentDepth = coords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // Soft shadows using PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, coords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

const vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float CalculatePointShadow(samplerCube shadowCube, vec3 normal, vec3 lightPos) {
    vec3 lightVec = input.worldPos - lightPos;
    float currentDepth = length(lightVec);
    float bias = max(0.05 * (1.0 - dot(normal, lightVec)), 0.005);

    // Soft shadows using PCF
    float shadow = 0.0;
    int samples  = 20;
    float diskRadius = 0.05;
    for(int i = 0; i < samples; i++) {
        float pcfDepth = texture(shadowCube, lightVec + sampleOffsetDirections[i] * diskRadius).r;
        pcfDepth *= _farPlane;   // undo mapping [0;1]
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;   
    }
    shadow /= float(samples);
    return shadow;
}  

vec3 BRDF_PBR(vec3 albedo, vec3 specularTint, float smoothness, vec3 normal, vec3 viewDir, Light light) {
    float roughness = 1 - smoothness;
    vec3 halfVector = normalize(light.dir + viewDir);
    float ndotl = DotClamped(normal, light.dir);
    float ndotv = DotClamped(normal, viewDir);

    // cook-torrance brdf
    float NDF = DistributionGGX(normal, halfVector, roughness);
    float G = GeometrySmith(normal, viewDir, light.dir, roughness);
    vec3 F = FresnelSchlick(DotClamped(halfVector, viewDir), specularTint);

    vec3 top = NDF * G * F;
    float bot = 4.0 * ndotv * ndotl + 0.0001;
    vec3 specular = top / bot;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - _metallic;	   

    vec3 color = (kD * albedo / PI + specular) * light.color * ndotl;
    return color;
}

Light CreateDirLight(vec3 dir, vec3 color) {
    Light light;
    light.dir = normalize(dir);
    light.color = color;
    return light;
}

Light CreatePointLight(vec3 pos, vec3 color) {
    vec3 lightVec = pos - input.worldPos;
    float attenuation = 1 / (1 + length(lightVec));
    Light light;
    light.pos = pos;
    light.dir = normalize(lightVec);
    light.color = color * attenuation;
    return light;
}

void main() {
    float luminance = texture(_detailTex, input.uv).r;
    vec3 albedo = texture(_mainTex, input.uv).rgb;
    
    vec3 specularTint;
    float oneMinusReflectivity;
    albedo = DiffuesAndSpecullarFromMetallic(albedo, pow(_metallic, 1.0f / GAMMA), specularTint, oneMinusReflectivity);
    float roughness = 1.0 - _smoothness;
    vec3 normal = normalize(input.normal);
    vec3 viewDir = normalize(_camPos - input.worldPos);
    vec3 reflectDir = reflect(-viewDir, normal);

    // Apply lights
    vec4 shadowCoords;
    Light light;
    vec3 color = vec3(0.0f);
    for (int i = 0; i < n_lights; i++) {
        shadowCoords = _shadowMatrix[i] * vec4(input.worldPos, 1.0);
        light = CreateDirLight(_dirlight_dir[i], _dirlight_color[i]);
        color += BRDF_PBR(albedo, specularTint, _smoothness, normal, viewDir, light) * (1.0 - CalculateDirShadow(_shadowMap[i], shadowCoords, normal, light.dir));
    }
    for (int i = 0; i < n_lights; i++) {
        light = CreatePointLight(_pointlight_pos[0], _pointlight_color[0]);
        color += BRDF_PBR(albedo, specularTint, _smoothness, normal, viewDir, light) * (1.0 - CalculatePointShadow(_shadowCubeMap[0], normal, light.pos));
    }
    
    // Apply ambient lighting from background
    vec3 F = FresnelSchlickRoughness(DotClamped(normal, viewDir), specularTint, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - _metallic;	  
    vec3 irradiance = texture(_irrMap, normal).rgb;
    vec3 diffuse = albedo * irradiance;
    vec3 prefilteredColor = textureLod(_prefilterMap, reflectDir, roughness * MAX_LOD).rgb;
    vec2 brdf = texture(_brdfLUT, vec2(DotClamped(normal, viewDir), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * 0.1;
    color += ambient;

    color = pow(color, vec3(1.0 / GAMMA));  
    FragColor = vec4(color, 1.0);
}
