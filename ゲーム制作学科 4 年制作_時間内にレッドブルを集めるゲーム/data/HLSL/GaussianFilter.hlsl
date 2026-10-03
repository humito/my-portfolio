//=============================================================================
//ガウスフィルター[GaussianFilter.hlsl]
//Author:HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************

//オブジェクトのテクスチャー
sampler g_TexSampler : register(s0);//テクスチャサンプラー

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
//ピクセルシェーダー(X方向へのブラー)
//=============================================================================
void PixelShaderXBlur(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	//X方向へのガウスフィルタ
	float4 cut = tex2D(g_TexSampler, float2(inVertex.tex.x - 0.008f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x - 0.006f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x - 0.004f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x - 0.002f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, inVertex.tex) * 0.2f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x + 0.002f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x + 0.004f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x + 0.006f, inVertex.tex.y)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x + 0.008f, inVertex.tex.y)) * 0.1f;

	outColor = cut;
}
//=============================================================================
//ピクセルシェーダー(Y方向へのブラー)
//=============================================================================
void PixelShaderYBlur(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	//Y方向へのガウスフィルタ
	float4 cut = tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y - 0.008f)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y - 0.006f)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y - 0.004f)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y - 0.002f)) * 0.1f;
	cut += tex2D(g_TexSampler, inVertex.tex) * 0.2f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y + 0.002f)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y + 0.004f)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y + 0.006f)) * 0.1f;
	cut += tex2D(g_TexSampler, float2(inVertex.tex.x, inVertex.tex.y + 0.008f)) * 0.1f;

	outColor = cut;
}
//EOF