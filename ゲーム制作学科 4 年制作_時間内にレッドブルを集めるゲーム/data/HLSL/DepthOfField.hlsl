//=============================================================================
//被写界深度フィルター[DepthOfField.hlsl]
//Author:HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************

//オブジェクトのテクスチャー
sampler g_TexSampler0 : register(s0);//ブラー付きのレンダリング
sampler g_TexSampler1 : register(s1);//通常のレンダリング
sampler g_TexSampler2 : register(s2);//バックバッファ

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************
//バーテックスシェーダーからピクセルシェーダーへ渡すための構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float2 tex		: TEXCOORD0;//テクスチャ座標
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VertexShader3D(in float4 pos : POSITION,	//頂点の座標
					in float4 col : COLOR0,		//色情報
					in float2 tex : TEXCOORD0,	//テクスチャ座標
					out VS_OUTPUT outVertex)	//出力用頂点構造体
{
	outVertex.pos = pos;
	outVertex.tex = tex;
}
//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PixelShader3D(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	//深度情報を取得
	float fBlend = tex2D(g_TexSampler2, inVertex.tex).a;

	//1.0fを超えないようにする
	fBlend = (fBlend > 1.0f) ? (fBlend - 1.0f) : fBlend;
	//fBlend = fBlend - min(1.0f, fBlend);

	//フォーカスの色情報を設定
	outColor = tex2D(g_TexSampler0, inVertex.tex) * fBlend
		+ tex2D(g_TexSampler1, inVertex.tex) * (1.0f - fBlend);
	outColor.a = 1.0f;
}
//EOF