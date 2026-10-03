#ifndef _FUEL_H_
#define _FUEL_H_

#include "main.h"

#define FUEL_MAX (20)

HRESULT InitFuPolygon (void);	//ƒ|ƒŠƒSƒ“‚Ì‰Šú‰»
void UpdateFuPolygon (void);	//ƒ|ƒŠƒSƒ“‚ÌXV
void AddSubFuel (int num);		//”R—¿‚Ì‰ÁZŒ¸Z
void DrawFuPolygon (void);		//ƒ|ƒŠƒSƒ“‚Ì•`‰æ
void UninitFuPolygon (void);	//ƒ|ƒŠƒSƒ“‚ÌI—¹


#endif _FUEL_H_