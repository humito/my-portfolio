//=============================================================================
//モーションブラー(ポストエフェクト用)[MotionBlurFilter.hlsl]
//Author:HUMITO KIMURA
//=============================================================================
//****************************************************************
//グローバル変数
//****************************************************************

//テクスチャサンプラー
sampler g_TexSampler0 : register(s0);//通常レンダリング
sampler g_TexSampler1 : register(s1);//速度マップ

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
	//ボケのなめらかさ。数値を大きくすると滑らかになる。
	int nBlur = 10;

	//速度マップから速度ベクトルとZ値を取得
	float4 velocity = tex2D(g_TexSampler1, inVertex.tex);
	velocity /= (float)nBlur;

	//カウント
	int nCnt = 1;
	//ブラー適応した色
	float4 blurColor;

	//シーンのレンダリングイメージの取得
	outColor = tex2D(g_TexSampler0, inVertex.tex);

	//ブラーカラー加算処理
	for (int i = nCnt; i < nBlur; ++i)
	{
		//速度ベクトルの方向のテクセル位置を参照し、シーンのレンダリングイメージの色情報を取得
		blurColor = tex2D(g_TexSampler0, inVertex.tex + velocity.xy * (float)i);
		
		//速度マップのZ値と速度ベクトル方向のテクセル位置のZ値を比較
		if (velocity.a < blurColor.a + 0.04f)
		{
			//速度マップのZ値が低い場合は速度ベクトルの色を加算する
			nCnt++;
			outColor += blurColor;
		}
	}

	//出力する色を0.0f～1.0fにするためカウント分除算
	outColor /= (float)nCnt;
}
//EOF