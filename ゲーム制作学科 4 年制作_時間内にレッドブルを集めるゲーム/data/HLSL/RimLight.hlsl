//=============================================================================
// リムライト [RimLight.hlsl]
// Author : 木村　文登
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

//カメラの座標
float4 g_CameraPos;

//オブジェクトの色(Diffuse)
float4 g_Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

//リムライトの範囲
float g_Power = 1.2f;

//オブジェクトのテクスチャー
sampler g_TexSampler0 : register(s0);//テクスチャサンプラー

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
	float  power	: TEXCOORD3;//リムライトの強度
	float4 depth	: TEXCOORD4;//深度情報
	float4 diffuse	: COLOR0;	//ディフューズ
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VertexShader3D(in float4 pos		: POSITION,	//頂点の座標
					in float4 normal	: NORMAL,	//法線ベクトル
					in float2 tex		: TEXCOORD0,//テクスチャ座標
					out VS_OUTPUT outVertex)		//出力用頂点構造体
{
	outVertex = (VS_OUTPUT)0;

	//頂点の座標を行列変換する
	outVertex.pos = mul(pos, g_WorldViewProjection);

	//テクスチャ座標
	outVertex.tex = tex;

	//法線ベクトル
	outVertex.nor = normalize(normal.xyz);

	//ライト座標
	float3 lightVec = -g_LightDir.xyz;

	//正規化したカメラとの距離
	outVertex.eye = normalize(g_CameraPos - pos);

	//ライトがカメラの正面近くになるほど強くなる
	outVertex.power = max(0.0f, dot(outVertex.eye, -lightVec.xyz));

	//Z値
	outVertex.depth = outVertex.pos.z / outVertex.pos.w;

	//頂点カラー
	outVertex.diffuse = dot(lightVec, outVertex.nor) * g_Color;
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
void PixelShader3D(	in VS_OUTPUT inVertex,
					out PS_OUTPUT outPixel)
{
	//リムライトの強度を計算
	float power = 1.0f - max(0.0f, abs(dot(inVertex.nor, inVertex.eye)));

	//強度からさらに設定した強度を乗算
	power = power * g_Power;
	power = power * power;

	//頂点の強度と乗算
	float lightPower = power * inVertex.power;

	//ピクセル値にライトの強度を加算
	outPixel.col0 = (tex2D(g_TexSampler0, inVertex.tex)
				* inVertex.diffuse)
				+ lightPower;
	outPixel.col0.a = 1.0f;

	//深度情報を計算して法線に送る
	outPixel.col1 = inVertex.depth;
}
//EOF