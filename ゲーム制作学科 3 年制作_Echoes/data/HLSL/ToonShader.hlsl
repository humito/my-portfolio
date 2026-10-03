//=============================================================================
// トゥーンシェーダー[ToonShader.hlsl]
// Author : HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************
//ワールド、ビュー、射影座標変換マトリックス
float4x4 m_WVP;

//ワールドマトリクス
float4x4 m_World;

//照明の方向ベクトル
float4 m_LightDir = float4(-0.37261237f, -0.2f, 0.801783741f, 1.0f);
float4 m_LightDir2 = float4(0.267261237f, -0.534522474f, -0.801783741f, 1.0f);

//オブジェクトの色(Diffuse)
float4 m_Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

//サンプラー
sampler tex0 : register(s0);//オブジェクトのテクスチャー
sampler tex1 : register(s1);//トゥーンマップのテクスチャー

//****************************************************************
//シェーダー構造体定義
//****************************************************************
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 Col1 : COLOR0;
	float4 Col2 : COLOR1;
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
VS_OUTPUT VS(	float4 Pos : POSITION,
				float4 Normal : NORMAL,
				float2 Tex : TEXCOORD0)
{
	VS_OUTPUT Out;

	Out.Pos = mul(Pos,m_WVP);
	Out.Tex = Tex;

	Normal = mul(float4(Normal.xyz, 0.0f), m_World);
	Out.Normal = normalize(Normal.xyz);

	return Out;
}

//=============================================================================
//ピクセルシェーダー
//=============================================================================
PS_OUTPUT PS(VS_OUTPUT In) : COLOR0
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	//ハーフランバート拡散照明によるライティング処理
	float p = max(dot(In.Normal, -m_LightDir.xyz), dot(In.Normal, -m_LightDir2.xyz));
	p = p * 0.5f + 0.5f;
	p = p * p;

	//トゥーンシェーダー処理
	float4 Col = tex2D(tex1, float2(p, 0.0f));

			//色情報格納
	Out.Col1 = Col * tex2D(tex0, In.Tex) * m_Color;
	Out.Col1.w = 1.0f;
	Out.Col2.a = 2.0f;

	return Out;
}

//=============================================================================
//テクニックのセット
//=============================================================================
technique TShader
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS();
		PixelShader = compile ps_2_0 PS();
	}
}
//EOF