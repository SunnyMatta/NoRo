#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;

// Material Maps
uniform sampler2D u_AlbedoMap;   
uniform sampler2D u_NormalMap;   
uniform sampler2D u_ORMMap;      // R: Ambient Occlusion, G: Roughness, B: Metallic
uniform sampler2D u_EmissiveMap; 
uniform sampler2D u_ShadowMap;   

// Lighting & Scene
uniform vec3 u_CamPos;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;     // Use high values for HDR (e.g., vec3(10.0))
uniform vec3 u_EmissiveFactor; // Multiplier for glow intensity
uniform mat4 u_LightSpaceMatrix;

const float PI = 3.14159265359;

// --- PBR MATH HELPERS ---

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    return a2 / (PI * pow(NdotH2 * (a2 - 1.0) + 1.0, 2.0));
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// --- SHADOWS ---

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    return shadow / 9.0;
}

// --- MAIN ---

void main() {
    // 1. Sample textures and convert to Linear Space
    vec3 albedo = pow(texture(u_AlbedoMap, TexCoords).rgb, vec3(2.2));
    vec3 nSample = texture(u_NormalMap, TexCoords).rgb;
    vec3 orm = texture(u_ORMMap, TexCoords).rgb;
    vec3 emissive = pow(texture(u_EmissiveMap, TexCoords).rgb, vec3(2.2)) * u_EmissiveFactor;

    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;

    // 2. Setup Vectors
    vec3 N = normalize(TBN * (nSample * 2.0 - 1.0));
    vec3 V = normalize(u_CamPos - WorldPos);
    vec3 L = normalize(u_LightPos - WorldPos);
    vec3 H = normalize(V + L);

    // 3. Reflectance at normal incidence
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // 4. Cook-Torrance BRDF Specular
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);      
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 numerator    = D * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    // 5. Diffuse vs Specular ratio (Energy Conservation)
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  

    // 6. Final Lighting
    float NdotL = max(dot(N, L), 0.0);        
    float shadow = ShadowCalculation(u_LightSpaceMatrix * vec4(WorldPos, 1.0), N, L);

    vec3 ambient = vec3(0.03) * albedo * ao;
    
    // Shadow only affects Direct Light (Diffuse + Specular)
    vec3 directLight = (kD * albedo / PI + specular) * u_LightColor * NdotL;
    vec3 result = ambient + (1.0 - shadow) * directLight + emissive;

    // 7. HDR Tonemapping (Reinhard) & Gamma Correction
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0/2.2)); 

    FragColor = vec4(result, 1.0);
}