matrix ViewProj;
matrix WorldViewProj;

texture ReflectMap;
texture WaveTexture;

float Time;

sampler rs = sampler_state
{
	Texture = <ReflectMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler ws = sampler_state
{
	Texture = <WaveTexture>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position	  : POSITION;
    float4 viewcoord  : TEXCOORD0;
    float2 wavecoord  : TEXCOORD1;
};

VS_OUTPUT WaterVS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.position = mul(input.position, WorldViewProj);
    
    output.viewcoord = mul(input.position, ViewProj);
    
	float translation = (fmod(Time, 20) - 10) * 0.1;
    output.wavecoord.xy = input.texcoord.xy * 10 + float2(translation, 0);
         
    return output;
}

struct PS_INPUT
{
    float4 viewcoord : TEXCOORD0;
    float2 wavecoord : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 color : COLOR;
};

PS_OUTPUT WaterPS(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT)0; 
	
    half3 bumptex = tex2D(ws, input.wavecoord.xy).xyz;
    bumptex = normalize(bumptex - 0.5); 
    half3 reflbump = bumptex.xyz * half3(0.02, 0.02, 1.0);  
  
    float4 projcoord = input.viewcoord / input.viewcoord.w;
    projcoord.x = 0.5 * projcoord.x + 0.5;
    projcoord.y = -0.5 * projcoord.y + 0.5;

    output.color = tex2D(rs, projcoord.xy + reflbump.xy); 
	output.color.a = 0.8;

    return output;
}

technique WaterTech
{
    pass P0
    {
		AlphaBlendEnable = TRUE;
        DestBlend = INVSRCALPHA;
        SrcBlend = SRCALPHA;
		
        VertexShader = compile vs_2_0 WaterVS();
        PixelShader  = compile ps_2_0 WaterPS();
    }    
}
