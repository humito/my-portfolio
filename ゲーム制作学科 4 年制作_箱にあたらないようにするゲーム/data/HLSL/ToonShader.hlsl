//=============================================================================
// トゥーンシェーダー[ToonShader.hlsl]
// Author : HUMITO KIMURA
//=============================================================================

//****************************************************************
//グローバル変数
//****************************************************************
//ワールドマトリクス
float4x4 g_World;

//ワールド、ビュー、射影座標変換マトリックス
float4x4 g_WorldViewProjection;

//照明の方向ベクトル
float4 g_LightDir;
float4 g_LightDir1 = float4(0.251633853, 0.774258018, -0.580693543, .999999940);

//オブジェクトの色(Diffuse)
float4 g_Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

//オブジェクトのテクスチャー
sampler g_TexSampler0 : register(s0);//テクスチャサンプラー
sampler g_ToonMap;

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************
//バーテックスシェーダーからピクセルシェーダーへ渡すための構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float2 tex		: TEXCOORD0;//テクスチャ座標
	float3 nor		: TEXCOORD1;//法線ベクトル
	float4 depth	: TEXCOORD2;//深度情報
	float4 diffuse	: COLOR0;	//ディフューズ
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VertexShader3D(in float4 pos	: POSITION,	//頂点の座標
					in float4 normal : NORMAL,	//法線ベクトル
					in float2 tex : TEXCOORD0,	//テクスチャ座標
					out VS_OUTPUT outVertex)	//出力用頂点構造体
{
	outVertex = (VS_OUTPUT)0;

	//頂点の座標を行列変換する(Z値としても扱う)
	outVertex.pos = mul(pos, g_WorldViewProjection);

	//テクスチャ座標
	outVertex.tex = tex;

	//法線ベクトルは今回TEXCOORD2のものを使ってるので、ワールド行列とかける
	float3 nor = mul(float4(normal.xyz, 0.0f), g_World);
	outVertex.nor = normalize(nor);

	//Z値
	outVertex.depth = outVertex.pos.z / outVertex.pos.w;

	//マテリアル
	outVertex.diffuse = g_Color;
}

//****************************************************************
//ピクセルシェーダー構造体定義
//****************************************************************
//ピクセルシェーダー出力
struct PS_OUTPUT
{
	float4 col0 : COLOR0;		//シーンの色情報
	float4 col1 : COLOR1;		//シーンのZ値情報
};

//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PixelShader3D(in VS_OUTPUT inVertex,	//入力用頂点シェーダー
					out PS_OUTPUT outPixel)	//出力用色情報
{
	//ピクセルの値を0.0f～1.0fにする
	float p = dot(inVertex.nor, -g_LightDir.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	//トゥーンシェーダーの座標の色
	float4 ToonColor = tex2D(g_ToonMap, float2(p, 0.0f));

	//色情報を格納する
	outPixel.col0 = p * tex2D(g_TexSampler0, inVertex.tex)
					* ToonColor
					* inVertex.diffuse;
	outPixel.col0.a = 1.0f;

	//深度情報を計算して法線に送る
	outPixel.col1 = inVertex.depth;
}
//EOF