//=============================================================================
// キューブマップ [CubeMap.hlsl]
// Author : 木村 文登
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

//カメラ座標
float4 g_CameraPos;

samplerCUBE g_CubeMap;
sampler g_Sampler;

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************

//頂点シェーダー構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float2 tex		: TEXCOORD0;//テクスチャ座標
	float3 nor		: TEXCOORD1;//法線ベクトル
	float3 cubetex	: TEXCOORD2;//キューブテクスチャ座標
	float4 posw		: TEXCOORD3;//頂点のワールド座標
};

//=============================================================================
//頂点シェーダー
//=============================================================================
void VertexShader3D(in float4 pos : POSITION,
					in float2 tex : TEXCOORD0,
					in float3 nor : NORMAL,
					out VS_OUTPUT outVertex)
{
	outVertex.pos	= mul(pos, g_WorldViewProjection);
	outVertex.posw	= mul(pos, g_World);
	outVertex.tex	= tex;
	outVertex.nor	= normalize(nor);

	//カメラから頂点へのベクトル
	float3 toEye = pos.xyz - g_CameraPos.xyz;
	//キューブマップのテクスチャ座標計算
	outVertex.cubetex = normalize(reflect(toEye, outVertex.nor));
}

//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PixelShader3D(	in VS_OUTPUT inVertex,			//入力用頂点シェーダー
					out float4 outPixel : COLOR0)	//出力用色情報
{
	//ハーフランバート
	float p = dot(inVertex.nor, -g_LightDir.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	//カメラベクトル
	float3 toEye = g_CameraPos.xyz - inVertex.posw.xyz;
	float distToEye = length(toEye);
	toEye /= distToEye;

	//フレネル反射率計算
	float A = 0.5f;
	float B = dot(-toEye, inVertex.nor);
	float C = sqrt(1.0f - A * A * (1 - B * B));
	float Rs = (A * B - C) * (A * B - C)
			/ (A * B + C) * (A * B + C);
	float Rp = (A * C - B) * (A * C - B)
			/ (A * C + B) * (A * C + B);

	float a = (Rs / Rp) / 2.0f;

	//テクスチャ色
	float4 diffuse = tex2D(g_Sampler, inVertex.tex);
	//キューブテクスチャ色
	float4 cubeColor = texCUBE(g_CubeMap, inVertex.cubetex);

	//スペキュラの強度
	float s = pow(max(dot(reflect(g_LightDir.xyz, inVertex.nor), toEye),
					0.0f), 1.3f);

	//出力色計算
	outPixel = (diffuse * 0.3f + cubeColor * 0.7f) * p + s;
	outPixel.a = a;
}
//EOF