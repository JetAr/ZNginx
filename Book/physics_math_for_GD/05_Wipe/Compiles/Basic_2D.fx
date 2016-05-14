// ---------------------------------------------------------
// Basic_2D.fx
// Simple2D�V�F�[�_
// ---------------------------------------------------------


// �e�N�X�`��
Texture2D Tex2D : register( t0 );		// �e�N�X�`��

// �e�N�X�`���T���v��
SamplerState MeshTextureSampler : register( s0 )
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// �萔�o�b�t�@
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};


// VertexShader���͌`��
struct VS_INPUT {
    float4 v4Position	: POSITION;		// �ʒu
    float4 v4Color		: COLOR;		// �F
    float2 v2Tex		: TEXTURE;		// �e�N�X�`�����W
};

// VertexShader�o�͌`��
struct VS_OUTPUT {
    float4 v4Position	: SV_POSITION;	// �ʒu
    float4 v4Color		: COLOR;		// �F
    float2 v2Tex		: TEXTURE;		// �e�N�X�`�����W
};

// ���_�V�F�[�_
VS_OUTPUT VS( VS_INPUT Input )
{
    VS_OUTPUT	Output;

    Output.v4Position = mul( Input.v4Position, View );
    Output.v4Color = Input.v4Color;
    Output.v2Tex = Input.v2Tex;

    return Output;
}

// �s�N�Z���V�F�[�_
float4 PS( VS_OUTPUT Input ) : SV_TARGET {
    return Tex2D.Sample( MeshTextureSampler, Input.v2Tex ) * Input.v4Color;
}

