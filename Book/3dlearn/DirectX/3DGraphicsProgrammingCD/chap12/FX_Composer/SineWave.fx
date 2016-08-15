float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
float4x4 WorldXf : World < string UIWidget="None"; >;
float4x4 ViewIXf : ViewInverse < string UIWidget="None"; >;

float Timer : TIME < string UIWidget = "None"; >;

float Height
<
    string UIWidget="slider";
    string UIName = "Height";
    float UIMin = 0.0;
    float UIMax = 1.0;
    float UIStep = 0.05;
> = 0.2f;

float Speed
<
    string UIWidget="slider";
    string UIName = "Speed";
    float UIMin = 0.0;
    float UIMax = 10.0;
    float UIStep = 0.1;
> = 1.0f;

float Waves
<
    string UIWidget="slider";
    string UIName = "Waves";
    float UIMin = 0.0;
    float UIMax = 10.0;
    float UIStep = 0.1;
> = 2.0f;

struct VS_INPUT
{
    float4 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
    VS_OUTPUT Out;
	  
    float x = (In.Position.x + Timer * Speed) * Waves;
    float height = sin(x) * Height;
    float3 position = In.Position.xyz;
    
    position.y += height;
    
    Out.Position = mul(float4(position, 1.0f), WorldViewProj);
    Out.Color = (sin(x) + 1.0f) * 0.5f;
    
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
