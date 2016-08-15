float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
float4x4 WorldXf : World < string UIWidget="None"; >;

float3 Light0Pos : Position <
	string Object = "PointLight0";
	string UIName =  "PointLight0 Position";
	string Space = "World";
> = {-0.5f,2.0f,1.25f};

float3 Light0Color : Diffuse <
	string Object = "PointLight0";
	string UIName =  "PointLight0 Color";
	string UIWidget = "Color";
> = {1.0f, 1.0f, 1.0f};

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: Normal;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 WorldNormal	: TEXCOORD0;
	float3 WorldPosition: TEXCOORD1;
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
	float4 color;
	float3 vDir = normalize(Light0Pos - In.WorldPosition);

	color.rgb = Light0Color * saturate(dot(vDir, In.WorldNormal));
	color.a = 1.0f;

	return color;
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
