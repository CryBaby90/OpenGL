#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

//材质
uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{             
    vec3 N = normalize(Normal); 
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    for(int i = 0; i < 4; ++i)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);

        float distance    = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance); //为了gamma矫正
        vec3 radiance     = lightColors[i] * attenuation; 

        //Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);       
        float G   = GeometrySmith(N, V, L, roughness);  
        vec3 F  = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
        vec3 specular     = nominator / denominator;  

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic; // 如果表面是金属的，我们会把系数kD变为0

        float NdotL = max(dot(N, L), 0.0); 

        //比较重要的是我们没有把kS乘进去我们的反射率方程中，
        //这是因为我们已经在specualr BRDF中乘了菲涅尔系数F了，
        //因为kS等于F，因此我们不需要再乘一次。
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ambient lighting (we now use IBL as the ambient term)
    //由于环境光来自半球内围绕法线 N 的所有方向，因此没有一个确定的半向量来计算菲涅耳效应。
    //为了模拟菲涅耳效应，我们用法线和视线之间的夹角计算菲涅耳系数
    //之前我们是以受粗糙度影响的微表面半向量作为菲涅耳公式的输入，
    //但我们目前没有考虑任何粗糙度，表面的反射率总是会相对较高.所以加入roughness
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 color   = ambient + Lo;  

    //HDR tonemapping
    color = color / (color + vec3(1.0));
    //gamma矫正
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
