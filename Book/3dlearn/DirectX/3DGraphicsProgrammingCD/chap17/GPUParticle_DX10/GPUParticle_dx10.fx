// ���I����ƿ�J�榡
struct sParticle
{
	float4 Position : POSITION;
	float4 Velocity : VELOCITY;
};

Texture2D NoiseTex;

SamplerState NoiseSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

//
// Pass through vertex shader
//
sParticle VS(sParticle In)
{
	return In;
}

float3 Force;
float2 LifeRange;
float2 SpeedRange;
float2 SizeRange;
float4 Rand[3];
float  fTimeAdvance;
float  fTan;

void EmitParticle(out float4 position, out float4 velocity, float2 texcoord)
{
	// �q Lookup Texture �����X�ü�
	float4 rand0 = NoiseTex.SampleLevel(NoiseSampler, Rand[0].xy + texcoord, 0);
	rand0 = frac(rand0 + Rand[1]);

	float4 rand1 = NoiseTex.SampleLevel(NoiseSampler, Rand[0].zw + texcoord, 0);
	rand1 = frac(rand1 + Rand[2]);
	
	// �ζüƨӨM�w��V�A�t�סA�j�p�C
	float2 dir = (rand0.xy - 0.5f) * 2.0f;
	float2 xz  = normalize(dir.xy) * fTan * rand0.z;
	float  fLife  = LifeRange.x + LifeRange.y * rand0.w;
	float  fSpeed = SpeedRange.x + rand1.x * SpeedRange.y;
	float  fSize  = SizeRange.x + rand1.y * SizeRange.y;
	
	position = float4(0.0f, 0.0f, 0.0f, fSize);
	
	velocity.xyz = normalize(float3(xz.x, 1.0f, xz.y)) * fSpeed;
	velocity.w = fLife;
}

//
// Geometry Shader
//
[maxvertexcount(1)]
void GS_Simulate(point sParticle input[1], uint index : SV_PrimitiveID, inout PointStream<sParticle> stream )
{
	sParticle Out;
	
	float4 position = input[0].Position;
	float4 velocity = input[0].Velocity;

	if ( velocity.w <= 0.0f )
	{
		// �ةR�פF, �n���ͤ@�ӷs�� Particle.	
		
		// �ھڳ��I���s���ӨM�w�n�ϥΪ� Lookup Texture �y�СC
		int iy = index/256;
		int ix = index & 0xff;
		
		float2 texcoord = float2((float)ix/256.0f, (float)iy/256.0f);
		EmitParticle(Out.Position, Out.Velocity, texcoord);
	}
	else
	{
		// �ͩR���٨S����

		// ���� Particle 
		Out.Position.xyz = position.xyz + velocity.xyz * fTimeAdvance;
		Out.Position.w = position.w;
		// �M�J���O	
		Out.Velocity.xyz = velocity.xyz + Force;
		// ���ֹةR
		Out.Velocity.w = velocity.w - fTimeAdvance;
	}

	stream.Append(Out);
}

GeometryShader StreamOut = ConstructGSWithSO( CompileShader( gs_4_0, GS_Simulate() ), "POSITION.xyzw; VELOCITY.xyzw" );

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
    DepthFunc = LESS_EQUAL;
};

technique10 Simulate
{
	pass p0 
	{
		SetVertexShader( CompileShader(vs_4_0, VS() ) );
		SetGeometryShader( StreamOut );
		SetPixelShader( NULL );
		SetDepthStencilState(DisableDepth, 0);
	}
}
