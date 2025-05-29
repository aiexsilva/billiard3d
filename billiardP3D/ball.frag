#version 410 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPosition;

out vec4 FragColor;

uniform mat4 View;
uniform mat4 Model;
uniform mat4 Projection;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

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

uniform AmbientLight    uAmbientLight;
uniform DirectionalLight uDirLight;
uniform PointLight       uPointLights[NUM_POINT_LIGHTS];
uniform SpotLight        uSpotLight;

uniform Material uMaterial;
uniform sampler2D textureSampler;
uniform vec3 viewPosition;
uniform vec3 lightColor;

uniform bool uUseLighting;
uniform bool useAmbient;
uniform bool useDirectional;
uniform bool usePoint;
uniform bool useSpotlight;
uniform bool useAmbientMinimap;

vec3 CalcAmbient(AmbientLight L, vec3 Ka) {
    return L.ambient * Ka;
}

vec3 CalcDirectional(DirectionalLight L, vec3 N, vec3 V, vec3 Kd, vec3 Ks, float shininess) {
    vec3 Ldir = normalize(-L.direction);
    float diff = max(dot(N, Ldir), 0.0);
    vec3 R    = reflect(-Ldir, N);
    float spec= pow(max(dot(R, V), 0.0), shininess);
    return L.ambient * Kd
         + diff      * L.diffuse * Kd
         + spec      * L.specular * Ks;
}

vec3 CalcPoint(PointLight L, vec3 P, vec3 N, vec3 V, vec3 Kd, vec3 Ks, float shininess) {
    vec3 Ldir = normalize(L.position - P);
    float dist= length(L.position - P);
    float att = 1.0 / (L.constant + L.linear*dist + L.quadratic*dist*dist);
    float diff= max(dot(N, Ldir), 0.0);
    vec3 R    = reflect(-Ldir, N);
    float spec= pow(max(dot(R, V), 0.0), shininess);
    return att * (L.ambient * Kd
                + diff    * L.diffuse  * Kd
                + spec    * L.specular * Ks);
}

vec3 CalcSpot(SpotLight L, vec3 P, vec3 N, vec3 V, vec3 Kd, vec3 Ks, float shininess) {
    vec3 Ldir  = normalize(L.position - P);
    float theta= dot(normalize(-Ldir), normalize(L.spotDirection));
    if (theta > cos(radians(L.spotCutoff))) {
        float diff      = max(dot(N, Ldir), 0.0);
        vec3 R         = reflect(-Ldir, N);
        float spec      = pow(max(dot(R, V), 0.0), shininess);
        float dist      = length(L.position - P);
        float att       = 1.0 / (L.constant + L.linear*dist + L.quadratic*dist*dist);
        float intensity = pow(theta, L.spotExponent);
        return att * intensity * (
            L.ambient * Kd +
            diff      * L.diffuse * Kd +
            spec      * L.specular* Ks
        );
    }
    return vec3(0.0);
}

void main() {

    vec3 N  = normalize(Normal);
    vec3 V  = normalize(viewPosition - FragPosition);
    vec3 Kd = uMaterial.diffuse;
    vec3 Ks = uMaterial.specular;
    vec3 Ka = uMaterial.ambient;
    vec3 tex = texture(textureSampler, TexCoord).rgb;
    vec3 color = uMaterial.ambient * lightColor;

    if (!uUseLighting) {
        if(useAmbientMinimap){
            color += CalcDirectional(uDirLight, N, V, Kd, Ks, uMaterial.shininess);
            color *= tex;
            FragColor = vec4(color, 1.0);
        } else {
            FragColor = vec4(0.0);
        }
        return;
    }

    if (!(useAmbient || useDirectional || usePoint || useSpotlight)) {
        color = vec3(0.0);
    }

    if (useAmbient){
        color += CalcAmbient(uAmbientLight, Ka);
    }
    if (useDirectional){
        color += CalcDirectional(uDirLight, N, V, Kd, Ks, uMaterial.shininess);
    }
    if (usePoint) {
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
            color += CalcPoint(uPointLights[i], FragPosition, N, V, Kd, Ks, uMaterial.shininess);
    }
    if (useSpotlight){
        color += CalcSpot(uSpotLight, FragPosition, N, V, Kd, Ks, uMaterial.shininess);
    }

    color *= tex;
    FragColor = vec4(color, 1.0);
}