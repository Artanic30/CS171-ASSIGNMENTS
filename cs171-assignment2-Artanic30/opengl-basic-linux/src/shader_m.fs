#version 330 core
out vec4 FragColor;


struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define NR_POINT_LIGHTS 4

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} fs_in;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture1;
uniform sampler2D texture2;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main()
{
    vec3 norm = texture(texture2, fs_in.TexCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    // vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    // 第一阶段：定向光照
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // 第二阶段：点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    result += CalcPointLight(pointLights[i], norm, fs_in.TangentFragPos, viewDir);

    FragColor = vec4(result, 1.0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Fill these functions. And use them in main.                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// calculates the color when using a directional light.

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)

{
    vec3 lightDir = normalize(-(fs_in.TBN * light.direction));
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 30.0);
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(texture1, fs_in.TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture1, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture1, fs_in.TexCoords));
    return (ambient + diffuse + specular);
}


// calculates the color when using a point light.

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(fs_in.TBN * light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 30.0);
    // 衰减
    float distance    = length(fs_in.TBN * light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(texture1, fs_in.TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture1, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture1, fs_in.TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


