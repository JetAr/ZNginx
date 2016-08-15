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
	string Object = "AmbientLight0";
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

float ConstantAttenuation : ConstantAttenuation <
	string Object = "PointLight0";
	string UIWidget = "Color";
	string UIName =  "Light0 Constant Attenuation";
> = 1.0f;

float LinearAttenuation : LinearAttenuation <
	string Object = "PointLight0";
	string UIWidget = "Color";
	string UIName =  "Light0 Linear Attenuation";
> = 1.0f;

float QuadraticAttenuation : QuadraticAttenuation <
	string Object = "PointLight0";
	string UIWidget = "Color";
	string UIName =  "Light0 Quadratic Attenuation";
> = 1.0f;

bool bLight0Diffuse <
	string UIName = "Diffuse Lighting";
	string UIWidget = "checkbox";
> = true;

bool bLight0Specular <
	string UIName = "Specular Lighting";
	string UIWidget = "checkbox";
> = true;

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

float3 DiffuseLighting(float3 vSurfaceNormal, float3 vLightDir)
{
	float3 vDiffuse = Light0Diffuse * saturate(dot(vSurfaceNormal, vLightDir));
	return vDiffuse;
}

float3 SpecularLighting(float3 vSurfacePosition, float3 vSurfaceNormal, float3 vLightDir)
{
	float3 vCameraPosition = ViewIXf[3].xyz;
	float3 vCameraDir = normalize(vCameraPosition - vSurfacePosition);
	float3 vHalfDir = normalize(vLightDir + vCameraDir);
	float  fSpecularCosine = saturate(dot(vHalfDir, vSurfaceNormal));
	float  fSpecular = pow(fSpecularCosine, SpecExpon);
	float3 vSpecular = Light0Specular * fSpecular;

	return vSpecular;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
	float3 vWorldNormal = In.WorldNormal;
	float3 vWorldPosition = In.WorldPosition;

	float4 Color = float4(0,0,0,1);

	Color.rgb = AmbientLight;

	float3 vDiff = Light0Pos - vWorldPosition;
	float  fDistance = length(vDiff);
	float3 vDir = vDiff / fDistance;

	float3 Attenuation = float3(ConstantAttenuation, LinearAttenuation, QuadraticAttenuation);
	float fLightAttenuation = dot(float3(1.0f, fDistance, fDistance * fDistance), Attenuation);

	if ( bLight0Diffuse )
	{
		Color.rgb += DiffuseLighting(vWorldNormal, vDir) / fLightAttenuation;
	}

	if ( bLight0Specular )
	{
		Color.rgb += SpecularLighting(vWorldPosition, vWorldNormal, vDir) / fLightAttenuation;
	}

	return Color;
}

technique technique0
{
	pass p0 
	{
		VertexShader = compile vs_2_0 mainVS();
		PixelShader = compile ps_2_0 mainPS();
		CULLMODE = NONE;
	}
}
