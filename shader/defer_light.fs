#version 330 core
out vec4 fragColor;
in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssaoTex;
uniform int useSsao;

struct Light {
    vec3 position;
    vec3 color;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    float specular = texture(gAlbedoSpec, texCoord).a;

    // if (useSsao == 1)
    //     fragColor = vec4(texture(ssaoTex, texCoord).rgb, 1.0);
    // else
    //     fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // return;

    vec3 ambient = useSsao == 1 ?
        texture(ssaoTex, texCoord).r * 0.4 * albedo :
        albedo * 0.4;
    vec3 lighting = ambient;
    vec3 viewDir = normalize(viewPos - fragPos);
    for (int i = 0; i < NR_LIGHTS; ++i)
    {
        vec3 lightDir = normalize(lights[i].position - fragPos).rgb;
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].color;
        lighting += diffuse*0.5;
    }
    fragColor = vec4(lighting, 1.0);
}