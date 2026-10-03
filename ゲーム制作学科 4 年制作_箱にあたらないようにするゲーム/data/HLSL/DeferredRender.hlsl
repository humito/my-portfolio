//=============================================================================
// ディファードランバートシェーダー[DeferredRender.hlsl]
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

//オブジェクトの色(Diffuse)
float4 g_Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

//カメラ座標
float4 g_CameraPos;

//ポイントライトのワールド座標
float3 g_PointLightPosW[3];

//ライトカラー
float3 g_LightColor[3];

//減衰率
float g_Attenuation[3] = { 0.0f, 0.1f, 0.3f };

//ポイントライトの数
int g_PointLightNum = 0;

sampler g_Sampler;
sampler g_ColorSampler;
sampler g_NormalSampler;
sampler g_PositionSampler;

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************

//頂点シェーダー構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float2 tex		: TEXCOORD0;//テクスチャ座標
	float3 nor		: TEXCOORD1;//法線ベクトル
	float4 depth	: TEXCOORD2;//深度情報
	float4 posw		: TEXCOORD3;//ワールド座標
	float4 diffuse	: COLOR0;	//ディフューズ
};

//頂点シェーダー構造体(ポストエフェクト用)
struct VS_OUTPUT_POST
{
	float4 pos : POSITION;	//頂点の座標
	float2 tex : TEXCOORD0;	//テクスチャ座標
};

//****************************************************************
//ピクセルシェーダー構造体定義
//****************************************************************

//ピクセルシェーダー出力
struct PS_OUTPUT
{
	float4 col0 : COLOR0;//色情報
	float4 col1 : COLOR1;//法線情報
	float4 col2 : COLOR2;//深度情報
	float4 col3 : COLOR3;//座標情報
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VS_3D(	in float4 pos : POSITION,
			in float4 nor : NORMAL,
			in float2 tex : TEXCOORD0,
			out VS_OUTPUT outVertex)
{
	outVertex = (VS_OUTPUT)0;

	//ワールド座標
	outVertex.posw = mul(float4(pos.xyz, 1.0f), g_World);

	//頂点の座標を行列変換する(Z値としても扱う)
	outVertex.pos = mul(pos, g_WorldViewProjection);

	//テクスチャ座標
	outVertex.tex = tex;

	//法線ベクトルは今回TEXCOORD2のものを使ってるので、ワールド行列とかける
	float3 normal = mul(float4(nor.xyz, 0.0f), g_World).xyz;
	outVertex.nor = normalize(normal);

	//Z値
	outVertex.depth = outVertex.pos;

	//マテリアル
	outVertex.diffuse = g_Color;
}

//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PS_3D(	in VS_OUTPUT inVertex,
			out PS_OUTPUT outPixel)
{
	//色情報を格納する
	outPixel.col0 = inVertex.diffuse * tex2D(g_Sampler, inVertex.tex);
	outPixel.col0.a = 1.0f;

	//法線情報
	outPixel.col1 = float4(inVertex.nor.xyz, 1.0f);

	//深度情報を計算
	outPixel.col2 = inVertex.depth.z / inVertex.depth.w;

	//座標情報(省く)
	outPixel.col3 = inVertex.posw;
	outPixel.col3.a = 1.0f;
}

//=============================================================================
//バーテックスシェーダー(ポストエフェクト)
//=============================================================================
void VS_2D(	in float4 pos : POSITION, in float2 tex : TEXCOORD0,
			out VS_OUTPUT_POST outVertex)
{
	outVertex.pos = pos;
	outVertex.tex = tex;
}

//=============================================================================
//ピクセルシェーダー(ポストエフェクト)
//=============================================================================
void PS_2D(in VS_OUTPUT_POST inVertex,
	out float4 outColor : COLOR0)
{
	outColor = (float4)0;

	//レンダリングしたテクスチャから各情報取得
	float4 diffuse = tex2D(g_ColorSampler, inVertex.tex);
	float4 normal = tex2D(g_NormalSampler, inVertex.tex);
	float4 worldPos = tex2D(g_PositionSampler, inVertex.tex);

	//カメラベクトル
	float3 toEye = g_CameraPos.xyz - worldPos.xyz;
	float distToEye = length(toEye);
	toEye /= distToEye;

	//正規化したワールド座標
	float3 posW = normalize(worldPos.xyz);

	float4 Color = diffuse;

	//各ポイントライトとの計算g_PointLightNum
	for (int i = 0; i < g_PointLightNum; ++i)
	{
		float3 pointLightPosW = normalize(g_PointLightPosW[i]);
		float d = distance(pointLightPosW, posW);

		float A = 1.0f - (g_Attenuation[0] + g_Attenuation[1] * d
						+ g_Attenuation[2] * d * d);

		float3 pointLightDir = normalize(worldPos.xyz - g_PointLightPosW[i].xyz);

		//ハーフランバート計算
		float l = dot(normal.xyz, -pointLightDir) * 0.5f + 0.5f;
		l *= l;

		//スペキュラの強度
		float s = pow(max(dot(reflect(pointLightDir, normal.xyz), toEye),
						0.0f), 2.0f);

		//色情報を格納する
		Color.rgb += (l + s) * A;
	}

	outColor = float4(Color.rgb, 1.0f);
}
//EOF