//=============================================================================
//モーションブラー(速度マップ作成用)[MotionBlur.hlsl]
//Author:HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************
float4x4 g_WorldViewProjectionNew;	//現在の変換行列(ワールド * ビュー * プロジェクション)
float4x4 g_WorldViewProjectionOld;	//前回の変換行列
float4x4 g_RotationOnlyWVP;			//法線ベクトルをワールド空間上で変換するための行列

float4 g_Velocity;					//速度ベクトル

//****************************************************************
//バーテックスシェーダー構造体定義
//****************************************************************
//バーテックスシェーダーからピクセルシェーダーへ渡すための構造体
struct VS_OUTPUT
{
	float4 pos		: POSITION;	//頂点の座標
	float4 dir		: TEXCOORD0;//速度ベクトル
};

//=============================================================================
//バーテックスシェーダー
//=============================================================================
void VertexShader3D(in float4 pos : POSITION,	//頂点の座標
					in float4 nor : NORMAL,		//法線ベクトル
					in float2 tex : TEXCOORD0,	//テクスチャ座標
					out VS_OUTPUT outVertex)	//出力用頂点構造体
{
	//法線ベクトルと回転成分のみの変換行列を変換する
	float3 normal = mul(nor.xyz, g_RotationOnlyWVP);

	//現在の頂点座標
	float4 posNew = mul(pos, g_WorldViewProjectionNew);

	//前回の頂点座標
	float4 posOld = mul(pos, g_WorldViewProjectionOld);

	//頂点の移動方向ベクトル
	float3 direction = posNew.xyz - posOld.xyz;

	//頂点の移動ベクトルと頂点の法線の内積を計算
	bool flag =(dot(normalize(direction), normalize(normal)) <= 0.0f);

	//移動ベクトルが法線ベクトルと逆向きなら前座標へモデルを引き伸ばす
	outVertex.pos = flag ? posOld : posNew;
	float4 old = flag ? posNew : posOld;

	//速度ベクトルの計算
	//テクスチャ座標系の位置に合わせるため、計算した範囲を半分にする

	float2 leftDir = (outVertex.pos.xy / outVertex.pos.w - old.xy / old.w) * 0.5f;
	float2 rightDir = (old.xy / old.w - outVertex.pos.xy / outVertex.pos.w) * 0.5f;

	outVertex.dir.xy = (g_Velocity.x > 0.0f) ? rightDir : leftDir;

	//outVertex.dir.xy = (posNew.xy / posNew.w - posOld.xy / posOld.w) * 0.5f;
	//outVertex.dir.xy = (outVertex.pos.xy / outVertex.pos.w - old.xy / old.w) * 0.5f;
	//outVertex.dir.xy = (old.xy / old.w - outVertex.pos.xy / outVertex.pos.w) * 0.5f;

	//テクセルのオフセット値にするため逆にする
	outVertex.dir.y = -outVertex.dir.y;

	//深度値(Z値)を計算するためのパラメータ
	outVertex.dir.z = outVertex.pos.z;
	outVertex.dir.w = outVertex.pos.w;
}
//=============================================================================
//ピクセルシェーダー
//=============================================================================
void PixelShader3D(in VS_OUTPUT inVertex,
					out float4 outColor : COLOR0)
{
	//速度ベクトル
	outColor.xy = inVertex.dir.xy;
	outColor.z = 1.0f;

	//深度値(Z値)を計算
	outColor.w = inVertex.dir.z / inVertex.dir.w;
}
//EOF