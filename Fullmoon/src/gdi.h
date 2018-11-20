// gdi+ 条件
#include <comdef.h>
#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#include "../Includes/GdiPlus.h"
#include "../Includes/GdiPlusInit.h"
#pragma comment(lib, "Lib/gdiplus.lib")
using namespace Gdiplus;
#define GP Gdiplus::
#define Rect_ Gdiplus::Rect

#define RED_ GP Color::Red
#define GREEN_ GP Color::Green
#define BLUE_ GP Color::Blue
#define BLACK_ GP Color::Black
#define WHITE_ GP Color::White

#endif
#pragma comment(lib,"Msimg32.lib")
#pragma comment(lib,"Winmm.lib")//MCI API媒体控制接口
