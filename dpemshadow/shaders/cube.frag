#version 330 core
out vec4 outColor;

in vec3 worldPos;
in vec2 texCoord;
in vec3 worldNormal;

uniform vec3 color;
uniform sampler2D texture1;
uniform sampler2D texture2;
// 光
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
// 阴影
uniform float lightNear;
uniform float lightFar;
uniform sampler2D frontMap;
uniform sampler2D backMap;


// 点光阴影
vec3 PointLight(){
    vec3 texColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2).rgb;
    vec3 normal = normalize(worldNormal);
    vec3 ambient = lightColor * 0.01;
    vec3 lightDir = normalize(lightPos - worldPos);
    // 漫反射
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor * texColor;
    // 高光反射
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float gloss = 32.0;
    float spec = pow(max(dot(normal, halfDir), 0.0), gloss);
    vec3 specular = spec * lightColor;

    return ambient + diffuse + specular;
}

float PointLightShadow(){
    float shadow = 0.8;
    float bias = 0.005;
    vec3 pos = worldPos - lightPos;
    float len = length(pos);
    pos = pos / len;
    float curDepth = (len - lightNear) / (lightFar - lightNear); 
    float depth;
    if (pos.z < 0){
        vec2 uv = pos.xy / (-pos.z + 1.0) * 0.5 + 0.5; // 映射到 [0, 1]
        depth = texture(backMap, uv).r;
    }
    else{
        vec2 uv = pos.xy / (pos.z + 1.0) * 0.5 + 0.5;
        depth = texture(frontMap, uv).r;
    }
    return curDepth - bias > depth ? shadow : 0.0;
}

void main()
{
    vec3 result = PointLight() * (1.0 - PointLightShadow());
    // gama correction
    result.rgb = pow(result.rgb, vec3(0.4545));
    outColor = vec4(result, 1.0);

}