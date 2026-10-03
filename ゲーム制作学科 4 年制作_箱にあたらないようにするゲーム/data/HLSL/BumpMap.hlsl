//=============================================================================
//バンプマップシェーダー[BumpMap.hlsl]
//Author:HUMITO KIMURA
//=============================================================================

//ワールドマトリクス
float4x4 g_World;

//ワールド×ビュー×プロジェクションマトリクス
float4x4 g_WorldViewProjection;

//ワールドインバーストランスポーズ
float4x4 g_WorldInverseTranspose;

//カメラの座標
float4 g_CameraPos;

//ポイントライトのワールド座標
float3 g_PointLightPosW[3];

//オブジェクトのマテリアル色(Diffuse)
float4 g_Color;

//マテリアルスペキュラ
float3 g_SpecColor;

//ライトカラー
float3 g_LightColor[3];

//ライトスペキュラ
float3 g_LightSpecColor[3];

//アテニュエーション
float g_Attenuation[3] = { 0.0f, 0.1f, 0.3f };

//ポイントライトの個数
int g_PointLightNum = 0;

//テクスチャサンプラー
sampler g_TexSampler : register(s0);
sampler g_NormalSampler : register(s1);

//*****************************************************************************
//構造体定義
//*****************************************************************************
//頂点シェーダー出力
struct VS_OUTPUT
{
	float4 pos : POSITION;	//頂点座標(スクリーン座標)
	float2 tex : TEXCOORD0;	//テクスチャ座標
	float3 wpos: TEXCOORD1;	//ワールド座標
	float3 nor : TEXCOORD2;	//法線ベクトル
	float3 utan : TEXCOORD3;//タンジェント
};

//=============================================================================
//バーテックスシェーダー出力
//=============================================================================
void VertexShader3D(in float3 inPos : POSITION,
					in float3 inNormal : NORMAL,
					in float2 inTex : TEXCOORD0,
					in float4 inTan : TANGENT0,
					out VS_OUTPUT outVertex)
{
	//ワールドビュープロジェクション変換
	outVertex.pos = mul(float4(inPos, 1.0f), g_WorldViewProjection);

	//ワールド座標
	outVertex.wpos = mul(float4(inPos, 1.0f), g_World).xyz;
	
	//ワールド法線
	outVertex.nor = mul(inNormal, (float3x3)g_WorldInverseTranspose).xyz;
	outVertex.nor = normalize(outVertex.nor);

	//ワールドタンジェント
	outVertex.utan = mul(inTan.xyz, (float3x3)g_World).xyz;

	//テクスチャ座標
	outVertex.tex = inTex;
}

//=============================================================================
//バンプマップの法線計算
//=============================================================================
float3 NormalSamplerToWorldSpace(float3 normalMapSample,
								float3 normalW,
								float3 tangentW)
{
	//法線マップの色をベクトルに変換(-1～1)
	float3 normalT = normalMapSample * 2.0f - 1.0f;

	//Normal, Tangent, BiNormal それぞれ計算
	float3 N = normalize(normalW);
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);
	//計算した3つの要素を行列にする
	float3x3 TBN = float3x3(T, B, N);

	//bumpNormalのベクトルを返す
	return mul(normalT, TBN);
}

//=============================================================================
//ピクセルシェーダー出力
//=============================================================================
void PixelShader3D(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	//法線ベクトル正規化
	inVertex.nor = normalize(inVertex.nor);

	//カメラベクトル計算
	float3 toEye = g_CameraPos.xyz - inVertex.wpos;
	float distToEye = length(toEye);
	toEye /= distToEye;

	//テクスチャ色
	float3 texColor = tex2D(g_TexSampler, inVertex.tex).rgb;

	//バンプマップ法線取得
	float3 bumpNormalW = NormalSamplerToWorldSpace(
							tex2D(g_NormalSampler,inVertex.tex).rgb,
							inVertex.nor, inVertex.utan);

	//計算用色
	float4 Color =(float4)0;

	//正規化したワールド座標
	float3 PosW = normalize(inVertex.wpos);

	//ライトの個数分ループ
	int i = 0;
	for (i = 0; i < g_PointLightNum; ++i)
	{
		//ワールド座標と各ポイントライトとの距離
		float3 PLPosW = normalize(g_PointLightPosW[i]);
		float d = distance(PLPosW, PosW);

		//減衰率
		float A = 1.0f - (g_Attenuation[0] + g_Attenuation[1] * d
						+ g_Attenuation[2] * d * d);

		//ポイントライトから頂点へのベクトル
		float3 pointLightDir = normalize(inVertex.wpos - g_PointLightPosW[i].xyz);

		//バンプマップ法線とライトベクトルの内積を使ったハーフランバート
		float l = dot(-pointLightDir, bumpNormalW) * 0.5f + 0.5f;

		//スペキュラの強度
		float s = pow(max(dot(reflect(pointLightDir, bumpNormalW), toEye),
					0.0f), 2.0f);

		//スペキュラ色
		float3 spec = s;

		//計算用色の加算
		Color.rgb += (l + spec) * A;
	}

	//出力色
	outColor = float4(Color.rgb, 1.0f) * tex2D(g_TexSampler, inVertex.tex);
}
//EOF