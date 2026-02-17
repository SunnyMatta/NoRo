#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in mat3 TBN;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

// Temporary Toggle Uniforms
uniform bool useTextures; 
uniform bool useNormalMap;

void main() {
    // 1. Handle Diffuse
    vec3 color;
    if (useTextures) {
        color = texture(diffuseMap, TexCoords).rgb;
    } else {
        color = vec3(0.5, 0.5, 0.5); // Default Grey
    }

    // 2. Handle Normals
    vec3 worldNormal;
    if (useNormalMap) {
        vec3 normal = texture(normalMap, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        worldNormal = normalize(TBN * normal);
    } else {
        // Fallback to the geometric normal (the Z-axis of the TBN)
        worldNormal = normalize(TBN[2]); 
    }

    // Simple lighting for testing
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(worldNormal, lightDir), 0.2); // 0.2 min light

    FragColor = vec4(color * diff, 1.0);
}