/*

Point Light Sample

*/

float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
float4x4 WorldXf : World < string UIWidget="None"; >;
float4x4 ViewIXf : ViewInverse < string UIWidget="None"; >;

float3 Light0Pos : Position <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Position";
    string Space = "World";
> = {-0.5f,2.0f,1.25f};

float3 Light0Diffuse : Diffuse <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Diffuse";
    string UIWidget = "Color";
> = {1.0f, 1.0f, 1.0f};

float3 Light0Specular : Specular <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Specular";
    string UIWidget = "Color";
> = {1.0f, 1.0f, 1.0f};

float3 AmbientLight : Ambient <
    string UIName =  "Ambient Light";
    string UIWidget = "Color";
> = {0.07f, 0.07f, 0.07f};

float SpecExpon : SpecularPower <
    string UIName =  "Specular Power";
    string UIWidget = "slider";
    float UIMin = 1.0;
    float UIMax = 128.0;
    float UIStep = 1.0;
> = 55.0;

float3 Attenuation <
    string UIWidget = "Color";
    string UIName =  "Light0 Attenuation";
> = {0.0f, 1.0f, 0.0f};

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: Normal;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
  float3 WorldNormal : TEXCOORD0;
  float3 WorldPosition : TEXCOORD1;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
    VS_OUTPUT Out;
	
    Out.Position = mul(float4(In.Position.xyz, 1.0f), WorldViewProj);
    Out.WorldNormal = mul( In.Normal, (float3x3) WorldXf);
    Out.WorldPosition = mul( In.Position, WorldXf).xyz;
    
    return Out;
 }

float4 mainPS(VS_OUTPUT In) : COLOR 
{
    float3 vWorldNormal = In.WorldNormal;
    float3 vWorldPosition = In.WorldPosition;
	   
    float3 vAmbient = AmbientLight;
	
    float3 vDiff = Light0Pos - vWorldPosition;
    float  fDistance = length(vDiff);
    float3 vDir = vDiff / fDistance;
	
    float3 vDiffuse = Light0Diffuse * saturate(dot(vDir, vWorldNormal));
	
    float3 vCameraPosition = ViewIXf[3].xyz;
    float3 vCameraDir = normalize(vCameraPosition - vWorldPosition);
    float3 vHalfDir = normalize(vDir + vCameraDir);
    float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
    float  fSpecular = pow(fSpecularCosine, SpecExpon);
    float3 vSpecular = Light0Specular * fSpecular;
	
    float fLightAttenuation = dot(float3(1.0f, fDistance, fDistance * fDistance), Attenuation);
    
    float4 Color;
    Color.rgb = vAmbient + (vDiffuse + vSpecular) / fLightAttenuation;
    Color.a = 1.0f;
    
    return Color;
}

technique technique0
{
    pass p0 
    {
        VertexShader = compile vs_3_0 mainVS();
        PixelShader = compile ps_3_0 mainPS();
    }
}
