uniform float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
uniform float4x4 WorldXf : World < string UIWidget="None"; >;

uniform float3 Light0Pos : Position <
	string Object = "PointLight0";
	string UIName = "PointLight0 Position";
	string Space = "World";
> = {0.0f,2.0f,0.0f};

uniform float3 Light0Color : Diffuse <
	string Object = "PointLight0";
	string UIName = "PointLight0 Color";
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
	float4 Color	: COLOR;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.Position = mul(float4(In.Position.xyz, 1.0f), WorldViewProj);

	float3 vWorldNormal = mul( In.Normal, (float3x3) WorldXf);
	float3 vWorldPosition = mul( In.Position, WorldXf).xyz;
	float3 vDir = normalize(Light0Pos - vWorldPosition);

	Out.Color.rgb = Light0Color * saturate(dot(vDir, vWorldNormal));
	Out.Color.a = 1.0f;

	return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
    return In.Color;
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
