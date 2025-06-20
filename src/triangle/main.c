#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define OT_SIZE 1

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 256
#define TRIANGLE_WIDTH 120
#define TRIANGLE_HEIGHT 100

u_long _ramsize   = 0x00200000; // 2 Mb
u_long _stacksize = 0x00004000; // 16 Kb

DISPENV disp;
DRAWENV draw;
u_long ot[OT_SIZE];

int main() {
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;

    ResetGraph(0);
    SetGraphDebug(0);

    SetDefDispEnv(&disp, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&draw, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    PutDispEnv(&disp);
    PutDrawEnv(&draw);
    SetDispMask(1);
    
    while(1) {
        POLY_F3 triangle;

        ClearOTagR(ot, OT_SIZE);

        SetPolyF3(&triangle);
        setRGB0(&triangle, 255, 0, 0);

        // draw triangle
        setXY3(
            &triangle,
            cx,                  cy-TRIANGLE_HEIGHT/2, // (x0, y0) top
            cx-TRIANGLE_WIDTH/2, cy+TRIANGLE_HEIGHT/2, // (x1, y1) bottom left
            cx+TRIANGLE_WIDTH/2, cy+TRIANGLE_HEIGHT/2  // (x2, y2) bottom right
        );

        addPrim(ot, &triangle);

        DrawOTag(ot + OT_SIZE - 1);
        DrawSync(0);

        VSync(0);
    }

    return 0;
}
