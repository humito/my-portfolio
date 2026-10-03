#ifndef _TITLE_H_
#define _TITLE_H_

#include "main.h"

#define TITLE_STR_MAX (8)//タイトルの文字数

HRESULT InittPolygon (void);//ポリゴンの初期化
void UpdatetPolygon (void);//ポリゴンの更新
void DrawtPolygon (void);//ポリゴンの描画
void UninittPolygon (void);//ポリゴンの終了

#endif _TITLE_H_