// 頂點的資料輸入格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

// Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD0;
};

texture PositionTex;

sampler PositionSampler = sampler_state
{
	Texture = <PositionTex>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture VelocityTex;

sampler VelocitySampler = sampler_state
{
	Texture = <VelocityTex>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture NoiseTex;

sampler NoiseSampler = sampler_state
{
	Texture = <NoiseTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;   
	AddressU  = Wrap;
	AddressV  = Wrap;
};

//
// Pass through vertex shader
//
VS_OUTPUT VS_Passthrough(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = float4(In.Position, 1.0f);
	Out.Texcoord = In.Texcoord;

	return Out;
}

float3 Force;
float2 LifeRange;
float2 SpeedRange;
float2 SizeRange;
float4 Rand[3];
float  fTimeAdvance;
float  fTan;

struct PS_OUTPUT
{
	float4 Position : COLOR0;
	float4 Velocity : COLOR1;
};

// `發射出新的 particle`
void EmitParticle(out float4 position, out float4 velocity, float2 texcoord)
{
	// `從貼圖中取出亂數, CPU 所傳入的 Rand[0] 會提供亂數的 seed 效果.`
	float4 rand0 = tex2D(NoiseSampler, texcoord + Rand[0].xy);
	rand0 = frac(rand0 + Rand[1]);
	
	float4 rand1 = tex2D(NoiseSampler, texcoord + Rand[0].zw);
	rand1 = frac(rand1 + Rand[2]);
	
	// `下面會使用亂數來設定 particle 屬性`
	float  fSize = SizeRange.x + rand1.z * SizeRange.y;
	// `位置跟大小`
	position = float4(0.0f, 0.0f, 0.0f, fSize);
	// `方向跟速度`
	float4 dir = (rand0 - 0.5f) * 2.0f;
	float2 xz = normalize(dir.xz) * fTan * rand1.x;
	float  fLife = LifeRange.x +  LifeRange.y * rand0.w;
	float  fSpeed = SpeedRange.x + rand1.y * SpeedRange.y;
	
	velocity.xyz = normalize(float3(xz.x, 1.0f, xz.y)) * fSpeed;
	velocity.w = fLife;
}

//
// Pixel Shader
//
PS_OUTPUT PS_Simulate(VS_OUTPUT In)
{
	PS_OUTPUT Out;
	
	// `從貼圖中取出速度值跟壽命值`
	float4 velocity = tex2D(VelocitySampler, In.Texcoord);
	velocity.w = velocity.w - fTimeAdvance;
	
	if ( velocity.w <= 0.0f )
	{
		// `壽命終止, 再產生新的 particle .`	
		EmitParticle(Out.Position, Out.Velocity, In.Texcoord);
	}
	else
	{
		// `從貼圖中取出位置`
		float4 position = tex2D(PositionSampler, In.Texcoord);

		// `對 particle 做運動計算`
		Out.Position.xyz = position.xyz + velocity.xyz * fTimeAdvance;
		Out.Position.w = position.w;
		
		Out.Velocity.xyz = velocity.xyz + Force;
		Out.Velocity.w = velocity.w;
	}

	return Out;
}

technique Simulate
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Simulate();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}
