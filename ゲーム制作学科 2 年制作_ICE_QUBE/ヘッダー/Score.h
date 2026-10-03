//=============================================================================
//ÉXÉRÉAèàóù[Score.h]
//Author:HUMITO KIMURA
//=============================================================================
#ifndef _SCORE_H_
#define _SCORE_H_

#include "game.h"

#define DIGITS (8)

HRESULT InitScore (void);
void DrawScore (void);
void AddScore (int num);
void ResultSPolygon (void);
void UninitScore (void);

#endif _SCORE_H_