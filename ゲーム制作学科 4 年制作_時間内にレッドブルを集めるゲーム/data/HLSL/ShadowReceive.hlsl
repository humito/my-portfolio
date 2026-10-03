//=============================================================================
//投影テクスチャシャドウ[ShadowCast.hlsl]
//Author:HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************

float4x4 g_WorldViewProjection;			//ワールド * ビュー * プロジェクション
float4x4 g_WorldViewProjectionLight;	//ワールド * ビュー * プロジェクション(ライト基準)
float4x4 g_WorldViewProjectionLightTex;	//ライト基準の行列変換をテクスチャ座標に変換したもの

//正規化したライトベクトル
float4 g_LightDir;

//マテリアルの色
float4 g_Color;

//プレイヤー座標
float4 g_PlayerPos;

//テクスチャサンプラー
sampler g_TexSampler0 : register(s0);

//テクスチャシャドーマップ
sampler g_TexSampler1 : register(s1) = sampler_state{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************
//バーテックスシェーダーからピクセルシェーダーへ渡すための構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float4 col		: COLOR0;	//マテリアル
	float2 tex		: TEXCOORD0;//テクスチャ座標
	float4 lightUV	: TEXCOORD1;//Zバッファのテクセル座標
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VertexShader3D(in float4 pos : POSITION,	//頂点の座標
					in float4 nor : NORMAL,		//法線ベクトル
					in float2 tex : TEXCOORD0,	//テクスチャ座標
					out VS_OUTPUT outVertex)	//出力用頂点構造体
{
	//頂点座標の行列変換
	outVertex.pos = mul(pos, g_WorldViewProjection);

	//テクスチャ座標
	outVertex.tex = tex;

	//ランバート拡散照明の計算
	float3 L = -g_LightDir;
	float3 N = normalize(nor.xyz);
	outVertex.col = dot(N, L) * g_Color;

	//Zバッファサーフェイスの深度情報を取得するためのテクセル座標
	outVertex.lightUV = mul(pos, g_WorldViewProjectionLightTex);
}
//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PixelShader3D(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	//深度情報計算
	float z = inVertex.lightUV.z / inVertex.lightUV.w;
	z = clamp(z, 0.0f, 0.8f) - 0.25f;

	//投影テクスチャシャドーマップ適応
	float4 shadow = tex2D(g_TexSampler1, inVertex.lightUV);//(1 - z) + z;

	//出力用ピクセルカラー設定
	outColor = tex2D(g_TexSampler0, inVertex.tex) * inVertex.col * shadow;
}
//EOF