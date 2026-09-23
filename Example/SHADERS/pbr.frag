#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_ORMMap;
uniform sampler2D u_EmissiveMap;
uniform sampler2D u_ShadowMap;
uniform sampler2D u_EnvMap;

uniform vec3 u_EmissiveFactor;
uniform vec3 u_CamPos;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};
uniform Light u_Lights[16];
uniform int u_LightCount;

const float PI = 3.14159265359;
const vec2 invAtan = vec2(0.1591, 0.3183);

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    return GeometrySchlickGGX(max(dot(N, V), 0.0), roughness) * GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    //keep shadow at 0.0 if outside the far plane of the light's frustum
    if(projCoords.z > 1.0)
        return 0.0;

    //closest depth value from light's perspective
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    //material Properties
    vec3 albedo = pow(texture(u_AlbedoMap, TexCoords).rgb, vec3(2.2));
    vec3 orm = texture(u_ORMMap, TexCoords).rgb;
    float ao = orm.r;
    //float ao = 1;
    float roughness = orm.g;
    float metallic = orm.b;


    vec3 N = normalize(TBN * (texture(u_NormalMap, TexCoords).rgb * 2.0 - 1.0));
    vec3 V = normalize(u_CamPos - WorldPos);

    //calculate base reflectivity
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < u_LightCount; ++i) {
        vec3 L = normalize(u_Lights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(u_Lights[i].position - WorldPos);
        float attenuation = u_Lights[i].intensity / (distance * distance + 1.0);
        vec3 radiance = u_Lights[i].color * attenuation;

        // Cook-Torrance BRDF
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = D * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float shadow = 0.0;
        if(i == 0) {
            shadow = ShadowCalculation(FragPosLightSpace);
        }

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

    vec3 R = reflect(-V, N);

    vec2 envUV = SampleSphericalMap(R);
    vec3 envColor = texture(u_EnvMap, envUV).rgb;
    vec3 ambient = envColor * albedo * ao;
    vec3 emissive = pow(texture(u_EmissiveMap, TexCoords).rgb, vec3(2.2)) * u_EmissiveFactor;

    vec3 color = ambient + Lo + emissive;

    // HDR Tone Mapping
    color = color / (color + vec3(1.0));
    // Gamma Correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
