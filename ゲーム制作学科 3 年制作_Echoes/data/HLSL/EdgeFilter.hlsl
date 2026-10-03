//==============================================================================
//
// エッジフィルター
// Author : HUMITO KIMURA
//
//==============================================================================
//*****************************************************************************
// シェーダー構造体定義
//*****************************************************************************
// 頂点シェーダ出力
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};
// 頂点シェーダ入力
struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Col1 : COLOR0;
	float4 Col2 : COLOR1;
	float2 Tex : TEXCOORD0;
};
// ピクセルシェーダ出力
struct PS_OUTPUT
{
	float4 Col1 : COLOR0;
	float4 Col2 : COLOR1;
};

//*****************************************************************************
// シェーダー用グローバル変数
//*****************************************************************************
//１テクセルのサイズ
float2 m_Tex = float2(0.0001f, 0.0001f);

//エッジ対象オブジェクトのID
int m_ID = 0;

//エッジの色
float4 m_Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

//サンプラー
sampler tex0 : register(s0);
sampler tex1 : register(s1);

//==============================================================================
// 頂点シェーダー出力
//==============================================================================
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	Out.Pos = Pos;
	Out.Tex = Tex;

	return Out;
}
//==============================================================================
// ピクセルシェーダー出力
//==============================================================================
PS_OUTPUT PS(VS_INPUT In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	Out.Col1 = In.Col1;
	Out.Col2 = In.Col2;
	Out.Col2.a = (float)m_ID;

	return Out;
}
//==============================================================================
// ピクセルシェーダー出力(フィルター用)
//==============================================================================
float4 PS1(VS_OUTPUT In) : COLOR0
{
	//出力用色情報
	float4 Col = m_Color;

	//1ピクセルのサイズ
	float texelX = m_Tex.x;
	float texelY = m_Tex.y;

	//テクスチャ1番の色(ID)取得
	float myID = tex2D(tex1, In.Tex).a;
	
	//テクスチャ座標から上下左右の座標
	float2 TexUp = float2(In.Tex.x, In.Tex.y + texelY);
	float2 TexBottom = float2(In.Tex.x, In.Tex.y - texelY);
	float2 TexLeft = float2(In.Tex.x - texelX, In.Tex.y);
	float2 TexRight = float2(In.Tex.x + texelX, In.Tex.y);
	
	//斜め４ヶ所の座標
	float2 TexUpLeft = float2(In.Tex.x - texelX, In.Tex.y + texelY);
	float2 TexUpRight = float2(In.Tex.x + texelX, In.Tex.y + texelY);
	float2 TexBottomLeft = float2(In.Tex.x - texelX, In.Tex.y - texelY);
	float2 TexBottomRight = float2(In.Tex.x + texelX, In.Tex.y - texelY);
	
	//上下左右のα値(ID)
	float ColUp = tex2D(tex1, TexUp).a;
	float ColBottom = tex2D(tex1, TexBottom).a;
	float ColLeft = tex2D(tex1, TexLeft).a;
	float ColRight = tex2D(tex1, TexRight).a;
	
	//斜め4ヶ所のα値(ID)
	float ColUpLeft = tex2D(tex1, TexUpLeft).a;
	float ColUpRigh = tex2D(tex1, TexUpRight).a;
	float ColBottomLeft = tex2D(tex1, TexBottomLeft).a;
	float ColBottomRigh = tex2D(tex1, TexBottomRight).a;
	
	//全方位のIDと比較
	if (myID != ColUp
	||  myID != ColBottom
	||  myID != ColLeft
	||  myID != ColRight
	||  myID != ColUpLeft
	||  myID != ColUpRigh
	||  myID != ColBottomLeft
	||  myID != ColBottomRigh)
	{
		return Col;
	}

	return tex2D(tex0, In.Tex);
}

//==============================================================================
//テクニックのセット
//==============================================================================
technique TShader
{
	pass P0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}

	pass P1
	{
		VertexShader = compile vs_1_1 VS();
		PixelShader = compile ps_2_0 PS1();
	}
}
//EOF