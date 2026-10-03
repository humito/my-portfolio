//=============================================================================
//ファーシェーダー[Fur.hlsl]
//Author:HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************
//ワールド、ビュー、射影座標変換マトリックス
float4x4 g_WorldViewProjection;

//照明の方向ベクトル
float4 g_LightDir;

//カメラの座標
float4 g_CameraPos;

//オブジェクトの色(Diffuse)
float4 g_Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

//法線方向への倍率
float g_fOffset;

//オブジェクトのテクスチャー
sampler g_TexSampler0 : register(s0);//モデルテクスチャ
sampler g_FurTex : register(s1);	//ファーテクスチャ

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************
//バーテックスシェーダーからピクセルシェーダーへ渡すための構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float2 tex		: TEXCOORD0;//テクスチャ座標
	float3 nor		: TEXCOORD1;//法線ベクトル
	float3 eye		: TEXCOORD2;//カメラベクトル
	float3 light	: TEXCOORD3;//ライトベクトル
	float4 diffuse	: COLOR0;	//ディフューズ
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VertexShader3D(in float4 pos		: POSITION,	//頂点の座標
					in float4 normal : NORMAL,		//法線ベクトル
					in float2 tex : TEXCOORD0,		//テクスチャ座標
					out VS_OUTPUT outVertex)		//出力用頂点構造体
{
	outVertex = (VS_OUTPUT)0;

	//ローカル座標を法線の方向へ移動
	pos.xyz += normal.xyz * g_fOffset;
	pos.w = 1.0f;

	//頂点の座標を行列変換する
	outVertex.pos = mul(pos, g_WorldViewProjection);

	//テクスチャ座標
	outVertex.tex = tex;

	float3 light = -g_LightDir.xyz;

	//法線ベクトル
	outVertex.nor = normalize(normal.xyz);

	//正規化したカメラとの距離
	outVertex.eye = normalize(g_CameraPos - pos);

	float power0 = (dot(light, outVertex.nor) * 0.5f + 0.5f) * 0.5f;
	float power1 = 1.0f - abs(dot(outVertex.eye, outVertex.nor) * 1.0f) * 0.5f;

	//頂点カラー
	outVertex.diffuse = g_Color * power0 + power1;
	outVertex.diffuse *= g_fOffset * 0.5f;
	outVertex.diffuse.a = 1.0f - g_fOffset * 0.5f;
}
//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PixelShader3D(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	outColor = tex2D(g_FurTex, inVertex.tex)
				* inVertex.diffuse;
}
//EOF