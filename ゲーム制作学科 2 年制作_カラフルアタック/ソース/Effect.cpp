#include "Effect.h"

/*メモ：エフェクト関数を作って敵を倒した時のエフェクトをつくる
		エフェクトの画像を繋げてＵＶ座標で指定させる
*/

LPDIRECT3DTEXTURE9 g_pEfD3DTex=NULL;	//テクスチャへのポインタ
VERTEX_2D g_efVtx[EFFECT_MAX][4];		//頂点情報格納ワーク
static EFFECT g_Effect[EFFECT_MAX];		//エフェクト構造体
D3DXVECTOR3 g_rot;	//ポリゴンの回転

//エフェクト初期化
HRESULT InitEfPolygon (void)
{
	//エフェクトの個数分初期化
	for(int i=0;i<EFFECT_MAX;i++)
	{
		g_Effect[i].fX=-10;
		g_Effect[i].fY=-10;
		g_Effect[i].fWidth=10;
		g_Effect[i].fHeight=10;
		g_Effect[i].fAlpha=255.0f;
		g_Effect[i].nColor=RED;
		g_Effect[i].nCount=0;
		g_Effect[i].bUse=false;
		g_rot.z=0;

		//テクスチャの中心座標(正方形なので縦横幅は同じでfWidthは高さとして扱う)
		g_Effect[i].pos.x=(g_Effect[i].fX+(g_Effect[i].fX+g_Effect[i].fWidth))/2;
		g_Effect[i].pos.y=(g_Effect[i].fY+(g_Effect[i].fY+g_Effect[i].fHeight))/2;

		//テクスチャの対角線の長さ
		g_Effect[i].fLength=sqrtf(g_Effect[i].pos.x*g_Effect[i].pos.x+g_Effect[i].pos.y*g_Effect[i].pos.y);

		//テクスチャの対角線の角度(Xの長さ,Yの長さ)
		g_Effect[i].fAngle=atan2f(g_Effect[i].pos.x,g_Effect[i].pos.y);

	}

	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pEfDevice;

	//ゲッターで返す
	pEfDevice=GetDevice();

	//敵テクスチャ（複数）初期化
	for(int i=0;i<EFFECT_MAX;i++)
	{
		//回転軸
		g_efVtx[i][0].vtx.x=g_Effect[i].pos.x-sinf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][0].vtx.y=g_Effect[i].pos.y-cosf(g_rot.z-g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][1].vtx.x=g_Effect[i].pos.x+sinf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][1].vtx.y=g_Effect[i].pos.y-cosf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][2].vtx.x=g_Effect[i].pos.x-sinf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][2].vtx.y=g_Effect[i].pos.y+cosf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][3].vtx.x=g_Effect[i].pos.x-sinf(g_rot.z-g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][3].vtx.x=g_Effect[i].pos.x+sinf(g_rot.z-g_Effect[i].fAngle)*g_Effect[i].fLength;

		
		//中身
		g_efVtx[i][0].rhw=1.0f;
		g_efVtx[i][1].rhw=1.0f;
		g_efVtx[i][2].rhw=1.0f;
		g_efVtx[i][3].rhw=1.0f;
		
		//反射光
		g_efVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_efVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,255);
		g_efVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,255);;
		g_efVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,255);

		//テクスチャ
		g_efVtx[i][0].tex=D3DXVECTOR2(0.3f+(g_Effect[i].nColor*0.3f),1.0f);
		g_efVtx[i][1].tex=D3DXVECTOR2(0.0f+(g_Effect[i].nColor*0.3f),1.0f);
		g_efVtx[i][2].tex=D3DXVECTOR2(0.3f+(g_Effect[i].nColor*0.3f),0.0f);
		g_efVtx[i][3].tex=D3DXVECTOR2(0.0f+(g_Effect[i].nColor*0.3f),0.0f);
	}

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pEfDevice,
								"data/TEXTURE/Effect.png",
								&g_pEfD3DTex);
	return S_OK;
}
//エフェクトの設置
void SetEffect (int x,int y,int color)
{
	//エフェクト４分割セット
	for(int i=0;i<EFFECT_MAX;i++)
	{
		if(g_Effect[i].bUse==false)
		{
			g_Effect[i].bUse=true;
			g_Effect[i].fX=x;
			g_Effect[i].fY=y;

			//テクスチャ
			g_efVtx[i][0].tex=D3DXVECTOR2(0.3f+(color*0.3f),1.0f);
			g_efVtx[i][1].tex=D3DXVECTOR2(0.0f+(color*0.3f),1.0f);
			g_efVtx[i][2].tex=D3DXVECTOR2(0.3f+(color*0.3f),0.0f);
			g_efVtx[i][3].tex=D3DXVECTOR2(0.0f+(color*0.3f),0.0f);

			//テクスチャの中心座標(正方形なので縦横幅は同じでfWidthは高さとして扱う)
			g_Effect[i].pos.x=(g_Effect[i].fX+(g_Effect[i].fX+g_Effect[i].fWidth))/2;
			g_Effect[i].pos.y=(g_Effect[i].fY+(g_Effect[i].fY+g_Effect[i].fHeight))/2;

			//テクスチャの対角線の長さ
			g_Effect[i].fLength=sqrtf(g_Effect[i].pos.x*g_Effect[i].pos.x+g_Effect[i].pos.y*g_Effect[i].pos.y);



			break;//ループから抜ける
		}
	}
}
//エフェクト更新
void UpdateEfPolygon (void)
{
	for(int i=0;i<EFFECT_MAX;i++)
	{
		//スイッチONならエフェクト更新
		if(g_Effect[i].bUse==true)
		{
			//エフェクトポリゴンを拡大とカウントアップしながら透明にする
			g_Effect[i].fLength+=7.7f;
			g_Effect[i].fAlpha-=10.5f;
			g_rot.z+=D3DX_PI*ROOL_SPEED;
			g_Effect[i].nCount++;

			//カウントが一定ならスイッチOFF
			if(g_Effect[i].nCount>=100)
			{
				g_Effect[i].fX=-10;
				g_Effect[i].fY=-10;
				g_Effect[i].fWidth=10;
				g_Effect[i].fHeight=10;

				//テクスチャの中心座標(正方形なので縦横幅は同じでfWidthは高さとして扱う)
				g_Effect[i].pos.x=(g_Effect[i].fX+(g_Effect[i].fX+g_Effect[i].fWidth))/2;
				g_Effect[i].pos.y=(g_Effect[i].fY+(g_Effect[i].fY+g_Effect[i].fHeight))/2;

				//テクスチャの対角線の長さ
				g_Effect[i].fLength=sqrtf(g_Effect[i].pos.x*g_Effect[i].pos.x+g_Effect[i].pos.y*g_Effect[i].pos.y);

				g_Effect[i].fAlpha=255.0f;
				g_Effect[i].nCount=0;
				g_Effect[i].bUse=false;
			}
		}
	}

	for(int i=0;i<EFFECT_MAX;i++)
	{
		//反射光
		g_efVtx[i][0].diffuse=D3DCOLOR_RGBA(255,255,255,(int)g_Effect[i].fAlpha);
		g_efVtx[i][1].diffuse=D3DCOLOR_RGBA(255,255,255,(int)g_Effect[i].fAlpha);
		g_efVtx[i][2].diffuse=D3DCOLOR_RGBA(255,255,255,(int)g_Effect[i].fAlpha);
		g_efVtx[i][3].diffuse=D3DCOLOR_RGBA(255,255,255,(int)g_Effect[i].fAlpha);

		//回転軸
		g_efVtx[i][0].vtx.x=g_Effect[i].pos.x-sinf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][0].vtx.y=g_Effect[i].pos.y+cosf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][1].vtx.x=g_Effect[i].pos.x-sinf(-g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][1].vtx.y=g_Effect[i].pos.y-cosf(g_rot.z-g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][2].vtx.x=g_Effect[i].pos.x-sinf(g_rot.z-g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][2].vtx.y=g_Effect[i].pos.y+cosf(g_rot.z-g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][3].vtx.x=g_Effect[i].pos.x+sinf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
		g_efVtx[i][3].vtx.x=g_Effect[i].pos.x-sinf(g_rot.z+g_Effect[i].fAngle)*g_Effect[i].fLength;
	}
}
//エフェクト描画
void DrawEfPolygon (void)
{
	//Deviceオブジェクト(描画に必要)
	LPDIRECT3DDEVICE9 pEfDevice;

	//ゲッターで返す
	pEfDevice=GetDevice();

	//頂点フォーマットのセット
	pEfDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pEfDevice->SetTexture(0,g_pEfD3DTex);



	//敵ポリゴン（複数）描画
	for(int i=0;i<EFFECT_MAX;i++)
	{
		//ポリゴンの描画
		pEfDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
										2,//ポリゴンの数
										&g_efVtx[i][0],
										sizeof(VERTEX_2D));
	}

}
//エフェクト終了
void UninitEfPolygon (void)
{
	//テクスチャの開放
	if(g_pEfD3DTex!=NULL)
	{
		g_pEfD3DTex->Release();
		g_pEfD3DTex=NULL;
	}
}