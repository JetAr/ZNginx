float4x4 World;
float4x4 View;
float4x4 Projection;

float4 AmbientColor = float4(1, 1, 1, 1);
float AmbientIntensity = 0.1;

float4 DiffuseColor = float4(1, 1, 1, 1);
float DiffuseIntensity = 1.0;
float3 DiffuseLightDirection = float3(-1, -1, 1);

float4 SpecularColor = float4(1, 1, 1, 1);
float SpecularIntensity = 1.0;

float3 CameraPosition = float3(0, 0, 0);

texture texture0;
sampler2D textureSampler0 = sampler_state
{
    Texture = (texture0);
    MinFilter = Linear;
    MagFilter = Linear;
};

texture texture1;
sampler2D textureSampler1 = sampler_state
{
    Texture = (texture1);
    MinFilter = Linear;
    MagFilter = Linear;
};

texture texture2;
sampler2D textureSampler2 = sampler_state
{
    Texture = (texture2);
    MinFilter = Linear;
    MagFilter = Linear;
};

texture texture3;
sampler2D textureSampler3 = sampler_state
{
    Texture = (texture3);
    MinFilter = Linear;
    MagFilter = Linear;
};
 
struct VertexShaderInput
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 TextureCoordinate0 : TEXCOORD0;
    float2 TextureCoordinate1 : TEXCOORD1;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
	float3 Normal : COLOR1;
    float2 TextureCoordinate0 : TEXCOORD0;
    float2 TextureCoordinate1 : TEXCOORD1;
	float3 ViewVector : TEXCOORD2;
};

VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;

    float4 worldPosition = mul(input.Position, World);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);

    output.Normal = mul(input.Normal, World);

    output.TextureCoordinate0 = input.TextureCoordinate0;
    output.TextureCoordinate1 = input.TextureCoordinate1;
	
	output.ViewVector = worldPosition - CameraPosition;

    return output;
}

float4 PixelShaderFunction(VertexShaderOutput input) : COLOR0
{
	float3 normal = normalize(input.Normal);
	float3 light = normalize(DiffuseLightDirection);
    float4 diffuse = DiffuseColor * DiffuseIntensity * dot(normal, -light);
	
	float4 blendFactor = tex2D(textureSampler3, input.TextureCoordinate1);
	float4 textureColor = tex2D(textureSampler0, input.TextureCoordinate0) * blendFactor.x;
	textureColor = tex2D(textureSampler1, input.TextureCoordinate0) * blendFactor.y + textureColor * (1.0f - blendFactor.y);
	textureColor = tex2D(textureSampler2, input.TextureCoordinate0) * blendFactor.z + textureColor * (1.0f - blendFactor.z);
	
	float3 viewVector = normalize(input.ViewVector);
	float3 reflection = -reflect(light, normal);
	float Specular = SpecularIntensity * SpecularColor * pow(max(0, dot(reflection, viewVector)), 12) * textureColor.a;
	
	float4 blendColor = saturate(textureColor * diffuse + AmbientColor * AmbientIntensity + Specular);
    blendColor.a = 1;

    return blendColor;
}

technique TerrainBlend
{
    pass Pass1
    {
        VertexShader = compile vs_1_1 VertexShaderFunction();
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}
