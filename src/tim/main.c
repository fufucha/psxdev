#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>

#include "logo.h"

#define SCREEN_W         320
#define SCREEN_H         256

// The destination area must be located within a drawable area (0, 0) - (1023, 511).
#define VRAM_IMG_X       0
#define VRAM_IMG_Y       0

// bmp2tim: -plt x y: Set palette position (default: 0 480)
// The clut data can be arranged in the frame buffer at X multiples of 16 (X=0,16,32,48,etc) and anywhere in the Y  range of 0-511.
#define VRAM_CLUT_X      0
#define VRAM_CLUT_Y      480

#define SPRITE_COLOR_R   128
#define SPRITE_COLOR_G   128
#define SPRITE_COLOR_B   128

#define SPRITE_CODE      0

#define TPAGE_MODE_4     0
#define TPAGE_MODE_8     1
#define TPAGE_MODE_16    2
#define TPAGE_MODE_24    3

u_long _ramsize   = 0x00200000;
u_long _stacksize = 0x00004000;

DISPENV disp;
DRAWENV draw;
u_long ot[1];

typedef struct {
    SPRT spr;
    DR_TPAGE tpage_prim;
} SpriteHandle;

// NOTE: bmp2tim is evil.
void LoadTIMSprite(const unsigned char* tim_buf, int x, int y, SpriteHandle* handle) {
    TIM_IMAGE tim;
    int color_mode, has_clut, tpage, w, h;

    OpenTIM((u_long*)tim_buf);
    ReadTIM(&tim);
    
    color_mode = tim.mode & 0x3;
    has_clut   = tim.mode & 0x8;

    // copy palette (CLUT) to VRAM at specified coordinates
    if (has_clut && tim.crect && tim.caddr) {
        RECT clut_rect = *tim.crect;
        clut_rect.x = VRAM_CLUT_X;
        clut_rect.y = VRAM_CLUT_Y;
        LoadImage(&clut_rect, tim.caddr);
        DrawSync(0);
    }

    // copy image pixel data to VRAM at specified coordinates
    if (tim.prect && tim.paddr) {
        RECT img_rect = *tim.prect;
        img_rect.x = VRAM_IMG_X;
        img_rect.y = VRAM_IMG_Y;
        LoadImage(&img_rect, tim.paddr);
        DrawSync(0);
    }

    // set texture page pointer for drawing primitives
    tpage = GetTPage(color_mode, 0, VRAM_IMG_X, VRAM_IMG_Y);
    SetDrawTPage(&handle->tpage_prim, 0, 0, tpage);

    // initialize sprite primitive (screen position, size, palette, color, etc.)
    SetSprt(&handle->spr);
    handle->spr.x0 = x;
    handle->spr.y0 = y;
    w = tim.prect ? tim.prect->w : 0;
    h = tim.prect ? tim.prect->h : 0;
    
    switch (color_mode) {
        case TPAGE_MODE_4:  // 4bpp
            handle->spr.w = w * 4;
            break;
        case TPAGE_MODE_8:  // 8bpp
            handle->spr.w = w * 2;
            break;
        case TPAGE_MODE_16: // 16bpp
        case TPAGE_MODE_24: // 24bpp
        default:
            handle->spr.w = w;
            break;
    }
    
    handle->spr.h = h;
    handle->spr.u0 = 0;
    handle->spr.v0 = 0;
    handle->spr.clut = has_clut ? GetClut(VRAM_CLUT_X, VRAM_CLUT_Y) : 0;
    handle->spr.r0 = SPRITE_COLOR_R;
    handle->spr.g0 = SPRITE_COLOR_G;
    handle->spr.b0 = SPRITE_COLOR_B;
    handle->spr.code = SPRITE_CODE;
}

int main() {
    SpriteHandle logo;

    ResetGraph(0);
    SetGraphDebug(0);

    SetDefDispEnv(&disp, 0, 0, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&draw, 0, 0, SCREEN_W, SCREEN_H);
    PutDispEnv(&disp);
    PutDrawEnv(&draw);
    SetDispMask(1);

    LoadTIMSprite(image, 0, 0, &logo);

    while (1) {
        ClearOTagR(ot, 1);
        AddPrim(ot, &logo.tpage_prim);
        AddPrim(ot, &logo.spr);
        DrawOTag(ot);
        DrawSync(0);
        VSync(0);
    }

    return 0;
}
