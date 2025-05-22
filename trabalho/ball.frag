#version 410 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPosition;

out vec4 FragColor;

// Material properties parsed from .mtl
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material uMaterial;
uniform sampler2D textureSampler;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform vec3 lightColor;

uniform bool uUseLighting;
uniform bool useAmbient;
uniform bool useDiffuse;
uniform bool useSpecular;

void main()
{   
    if(!uUseLighting){
        FragColor = vec4(0.0);
    }else{
        vec3 result = vec3(0.0); 
        //ambient
        if(useAmbient){
            // Ambient lighting: Light intensity * Material's ambient color
            float ambientStrength = 1.0;  //adjust for brighter or darker balls
            vec3 ambient = ambientStrength * lightColor * uMaterial.ambient;

            result += ambient;  
        }

        if (useDiffuse){ // diffuse

            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPosition - FragPosition);

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;

            result += diffuse;
        }

        if (useSpecular){ // specular

            float specularStrength = 0.5;

            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPosition - FragPosition);

            vec3 viewDirection = normalize(viewPosition - FragPosition);
            vec3 reflectDirection = reflect(-lightDir, norm);

            float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
            vec3 specular = specularStrength * spec * lightColor;  


            result += specular;

        }
        vec3 texColor = texture(textureSampler, TexCoord).rgb;
        result *= texColor;
        FragColor = vec4(result, 1.0);
    }
}