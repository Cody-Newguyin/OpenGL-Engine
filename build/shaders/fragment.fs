#version 330 core

#include common.glsl
#include uniforms.glsl

in Input {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
    vec4 tangent;
} i;

struct Interpolaters {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
    vec4 tangent;
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
uniform sampler2D _bumpMap;

struct Light {
    vec3 color;
    vec3 dir;
    vec3 pos;
};

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

float CalculateDirShadow(sampler2D shadowMap, vec4 shadowCoords, vec3 lightDir) {
    vec3 coords = shadowCoords.xyz / shadowCoords.w;
    coords = coords * 0.5 + 0.5;
    float currentDepth = coords.z;
    float bias = max(0.05 * (1.0 - dot(input.normal, lightDir)), 0.005);

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

float CalculatePointShadow(samplerCube shadowCube, vec3 lightPos) {
    vec3 lightVec = input.worldPos - lightPos;
    float currentDepth = length(lightVec);
    float bias = max(0.05 * (1.0 - dot(input.normal, lightVec)), 0.005);

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

vec3 PBR_Ambient(vec3 albedo, vec3 specularTint, float smoothness, vec3 normal, vec3 viewDir) {
    float roughness = 1 - smoothness;
    vec3 F = FresnelSchlickRoughness(DotClamped(input.normal, viewDir), specularTint, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - _metallic;	

    vec3 irradiance = texture(_irrMap, normal).rgb;
    vec3 diffuse = albedo * irradiance;

    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 prefilteredColor = textureLod(_prefilterMap, reflectDir, roughness * MAX_LOD).rgb;
    vec2 brdf = texture(_brdfLUT, vec2(DotClamped(normal, viewDir), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * 0.1;
    return ambient;
}

void InitializeFragmentNormal() {
    #ifdef BUMP_MAP
        vec2 texelSize = 1.0 / textureSize(_bumpMap, 0);
        float scale = 3.0;

        vec2 du = vec2(texelSize.x * 0.5, 0);
        float u1 = texture(_bumpMap, i.uv - du).r ;
        float u2 = texture(_bumpMap, i.uv + du).r ;

        vec2 dv = vec2(0, texelSize.y * 0.5);
        float v1 = texture(_bumpMap, i.uv - dv).r ;
        float v2 = texture(_bumpMap, i.uv + dv).r ; 

        vec3 tangentSpaceNormal = normalize(vec3(u1 - u2, 1, v1 - v2));

        // tangentSpaceNormal = vec3(0.0, 1.0, 0.0);
        vec3 tangent = vec3(1.0, 0.0, 0.0);
        vec3 normal = vec3(0.0, 1.0, 0.0);
        // vec3 binormal = vec3(0.0, 1.0, 0.0);
        vec3 binormal = cross(input.normal, input.tangent.xyz) * input.tangent.w;
        // vec3 binormal = cross(input.normal, input.tangent)

        input.normal = normalize(
            tangentSpaceNormal.x * input.tangent.xyz +
            tangentSpaceNormal.y * input.normal +
            tangentSpaceNormal.z * binormal
        );
        // input.normal = normalize(i.normal);
    #endif
}

void InitializeInput() {
    input.uv = i.uv;
    input.normal = normalize(i.normal);
    input.worldPos = i.worldPos;
    input.tangent = normalize(i.tangent);

    InitializeFragmentNormal();
}

void main() {
    InitializeInput();

    #ifdef BUMP_MAP
        vec3 albedo = vec3(0.1, 0.6, 0.1);
    #else
        vec3 albedo = texture(_mainTex, input.uv).rgb;
    #endif

    vec3 specularTint;
    float oneMinusReflectivity;
    albedo = DiffuesAndSpecullarFromMetallic(albedo, pow(_metallic, 1.0f / GAMMA), specularTint, oneMinusReflectivity);
    vec3 viewDir = normalize(_camPos - input.worldPos);

    // Apply lights
    vec4 shadowCoords;
    Light light;
    vec3 color = vec3(0.0f);
    for (int i = 0; i < n_lights; i++) {
        shadowCoords = _shadowMatrix[i] * vec4(input.worldPos, 1.0);
        light = CreateDirLight(_dirlight_dir[i], _dirlight_color[i]);
        color += BRDF_PBR(albedo, specularTint, _smoothness, input.normal, viewDir, light) * (1.0 - CalculateDirShadow(_shadowMap[i], shadowCoords, light.dir));
    }
    for (int i = 0; i < n_lights; i++) {
        light = CreatePointLight(_pointlight_pos[0], _pointlight_color[0]);
        color += BRDF_PBR(albedo, specularTint, _smoothness, input.normal, viewDir, light) * (1.0 - CalculatePointShadow(_shadowCubeMap[0], light.pos));
    }
    color += PBR_Ambient(albedo, specularTint, _smoothness, input.normal, viewDir);

    color = pow(color, vec3(1.0 / GAMMA));  
    FragColor = vec4(color, 1.0);
}
