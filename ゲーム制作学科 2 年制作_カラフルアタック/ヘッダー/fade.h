#ifndef _FADE_H_
#define _FADE_H_

#include "main.h"

typedef enum
{
	FADE_NONE=0,
	FADE_IN,
	FADE_OUT,
	FADE_MAX,
}FADE;

HRESULT InitFade (void);
void UpdateFade (void);
void DrawFade (void);
void UninitFade (void);
void SetFade (FADE fade);
FADE GetFade (void);

#endif _FADE_H_
//EOF