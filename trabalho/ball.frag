#version 410 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPosition;

out vec4 FragColor;

uniform mat4 View; // View matrix
uniform mat4 Model; // Model matrix
uniform mat4 Projection; // Projection matrix

// Material properties parsed from .mtl
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Light structures for each type of light
struct AmbientLight {
    vec3 ambient;
};
struct DirectionalLight {
    vec3 direction;
    vec3 ambient, diffuse, specular;
};
struct PointLight {
    vec3 position;
    vec3 ambient, diffuse, specular;
    float constant, linear, quadratic;
};
struct SpotLight {
    vec3 position, spotDirection;
    vec3 ambient, diffuse, specular;
    float constant, linear, quadratic;
    float spotCutoff, spotExponent;
};

#define NUM_POINT_LIGHTS 1
uniform AmbientLight   uAmbientLight;
uniform DirectionalLight uDirLight;
uniform PointLight      uPointLights[NUM_POINT_LIGHTS];
uniform SpotLight       uSpotLight;

uniform Material uMaterial;
uniform sampler2D textureSampler;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform vec3 lightColor;

<<<<<<< Updated upstream
=======
uniform bool uUseLighting;
uniform bool useAmbient;
uniform bool useDirectional;
uniform bool usePoint;
uniform bool useSpotlight;

vec3 CalcAmbient(AmbientLight L, vec3 Ka) {
    return L.ambient * Ka;
}

vec3 CalcDirectional(DirectionalLight L, vec3 N, vec3 V, vec3 Kd, vec3 Ks, float shininess) {
    vec3 Ldir = normalize(- (View * vec4(L.direction,0)).xyz);
    float diff = max(dot(N,Ldir),0.0);
    vec3 R = reflect(-Ldir, N);
    float spec = pow(max(dot(R,V),0.0), shininess);
    return L.ambient * Kd                // ambiente
         + diff     * L.diffuse * Kd     // difusa
         + spec     * L.specular * Ks;   // especular
}

vec3 CalcPoint(PointLight L, vec3 P, vec3 N, vec3 V, vec3 Kd, vec3 Ks, float shininess) {
    vec3 posEye = (View * vec4(L.position,1)).xyz;
    vec3 Ldir   = normalize(posEye - P);
    float dist  = length(posEye - P);
    float att   = 1.0 / (L.constant + L.linear*dist + L.quadratic*dist*dist);
    float diff  = max(dot(N,Ldir),0.0);
    vec3 R      = reflect(-Ldir, N);
    float spec  = pow(max(dot(R,V),0.0), shininess);
    return att * (L.ambient * Kd
                + diff * L.diffuse * Kd
                + spec * L.specular * Ks);
}

vec3 CalcSpot(SpotLight L, vec3 P, vec3 N, vec3 V, vec3 Kd, vec3 Ks, float shininess) {
    vec3 posEye = (View * vec4(L.position,1)).xyz;
    vec3 Ldir   = normalize(posEye - P);
    float theta = dot(normalize(-Ldir), normalize(L.spotDirection));
    if(theta > cos(radians(L.spotCutoff))) {
        float diff = max(dot(N,Ldir),0.0);
        vec3 R     = reflect(-Ldir,N);
        float spec = pow(max(dot(R,V),0.0), shininess);
        float dist = length(posEye - P);
        float att  = 1.0 / (L.constant + L.linear*dist + L.quadratic*dist*dist);
        float intensity = pow(theta, L.spotExponent);
        return att * intensity * (
            L.ambient  * Kd +
            diff * L.diffuse * Kd +
            spec * L.specular * Ks
        );
    }
    return vec3(0.0);
}

>>>>>>> Stashed changes
void main() {
    if (!uUseLighting) {
        FragColor = vec4(0.0);
        return;
    }

<<<<<<< Updated upstream
    // Ambient test lighting
    float ambientStrength = 0.8; // ambient light intensity
    vec3 ambient = ambientStrength * texColor;
=======
    vec3 N  = normalize(Normal);
    vec3 V  = normalize(viewPosition - FragPosition);
    vec3 Kd = uMaterial.diffuse;
    vec3 Ks = uMaterial.specular;
    vec3 tex = texture(textureSampler, TexCoord).rgb;
>>>>>>> Stashed changes

    vec3 color = uMaterial.ambient * 0.0; // Initialize color to zero

    // Calculate lighting contributions
    if (useAmbient)
        color += CalcAmbient(uAmbientLight, uMaterial.ambient);
    if (useDirectional)
        color += CalcDirectional(uDirLight, N, V, Kd, Ks, uMaterial.shininess);
    if (usePoint) {
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            color += CalcPoint(uPointLights[i], FragPosition, N, V, Kd, Ks, uMaterial.shininess);
        }
    }
    if (useSpotlight)
        color += CalcSpot(uSpotLight, FragPosition, N, V, Kd, Ks, uMaterial.shininess);

    color *= tex;
    FragColor = vec4(color, 1.0);
}