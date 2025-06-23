#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libspu.h>
#include <string.h>

// Bach Goldberg Variation 5 performed by Paul Barton (short version)
#include "bach_gv5.h"

#define OT_SIZE 1
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 256

#define SPU_HEAP_SIZE 0x10000 // 64 KB heap
#define SPU_VOLUME_MAX 0x3fff

#define VAG_HEADER_SIZE         0x30 // VAG header size in bytes
#define VAG_HEADER_NAME_OFF     0x20 // Offset for name in header
#define VAG_HEADER_NAME_LEN     16   // Name length (bytes)
#define VAG_HEADER_SIZE_OFF     0x0C // Offset for data size in header
#define VAG_HEADER_RATE_OFF     0x10 // Offset for sample rate in header
#define VAG_ADPCM_BLOCK_SIZE    16   // ADPCM block size in bytes
#define VAG_ADPCM_SAMPLES       28   // Number of samples per ADPCM block

u_long _ramsize   = 0x00200000;
u_long _stacksize = 0x00004000;

DISPENV disp;
DRAWENV draw;
u_long ot[OT_SIZE];

typedef struct {
    char name[VAG_HEADER_NAME_LEN+1];
    int sample_rate;
    int data_size;
    float duration;
    int duration_sec;
    int duration_cent;
	int pitch;
} VagInfo;

void parse_vag_info(const unsigned char *vag, int total_size, VagInfo *info)
{
    int i;
    int nb_samples;

    // read sample name (ASCII, zero-terminated)
    for (i = 0; i < VAG_HEADER_NAME_LEN; i++) {
        char c = (char)vag[VAG_HEADER_NAME_OFF + i];
        info->name[i] = c;
        if (c == 0) break;
    }
    info->name[i] = 0;

    // read sample rate (big-endian, offset 0x10)
    info->sample_rate =
        ((int)vag[VAG_HEADER_RATE_OFF]     << 24) |
        ((int)vag[VAG_HEADER_RATE_OFF + 1] << 16) |
        ((int)vag[VAG_HEADER_RATE_OFF + 2] <<  8) |
        ((int)vag[VAG_HEADER_RATE_OFF + 3]);
    if (info->sample_rate == 0) {
        info->sample_rate = 44100;
	}

    // read data size (big-endian, offset 0x0C)
    info->data_size =
        ((int)vag[VAG_HEADER_SIZE_OFF]     << 24) |
        ((int)vag[VAG_HEADER_SIZE_OFF + 1] << 16) |
        ((int)vag[VAG_HEADER_SIZE_OFF + 2] <<  8) |
        ((int)vag[VAG_HEADER_SIZE_OFF + 3]);

    nb_samples = (info->data_size / VAG_ADPCM_BLOCK_SIZE) * VAG_ADPCM_SAMPLES;
    info->duration = (float)nb_samples / (float)info->sample_rate;
    info->duration_sec = (int)(info->duration);
    info->duration_cent = (int)((info->duration - (float)info->duration_sec) * 100);
	info->pitch = (int)(4096.0f * (float)info->sample_rate / 44100.0f);
}

void play_vag(const unsigned char *data, int size, int pitch)
{
    SpuCommonAttr c_attr;
    SpuVoiceAttr v_attr;
    unsigned char *vag_data;
    int vag_data_size;
    int vag_addr;

    SpuInit();
    SpuInitMalloc(SPU_HEAP_SIZE, NULL); 
    SpuSetKey(SpuOff, SPU_0CH);

    memset(&c_attr, 0, sizeof(c_attr));
    c_attr.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    c_attr.mvol.left = SPU_VOLUME_MAX;
    c_attr.mvol.right = SPU_VOLUME_MAX;
    SpuSetCommonAttr(&c_attr);

    vag_data = (unsigned char *)(data + VAG_HEADER_SIZE);
    vag_data_size = size - VAG_HEADER_SIZE;
    vag_addr = SpuMalloc(vag_data_size);
    if (vag_addr < 0) {
        return;
    }

    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
    SpuSetTransferStartAddr(vag_addr);
    SpuWrite(vag_data, vag_data_size);
    SpuIsTransferCompleted(SPU_TRANSFER_BY_DMA);

    memset(&v_attr, 0, sizeof(v_attr));
    v_attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_PITCH | SPU_VOICE_WDSA;
    v_attr.voice = SPU_0CH;
    v_attr.volume.left = SPU_VOLUME_MAX;
    v_attr.volume.right = SPU_VOLUME_MAX;
    v_attr.pitch = pitch;
    v_attr.addr = vag_addr;

    SpuSetVoiceAttr(&v_attr);
    SpuSetKey(SpuOn, SPU_0CH);
}

int main() {
    int fnt_id;
    VagInfo vi;

    ResetGraph(0);
    SetGraphDebug(0);

    SetDefDispEnv(&disp, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&draw, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    PutDispEnv(&disp);
    PutDrawEnv(&draw);
    SetDispMask(1);

    FntLoad(960, 0);
    fnt_id = FntOpen(0, 8, SCREEN_WIDTH, 16 * 8 - 8, 0, 512);

    parse_vag_info(bach_gv5_vag, sizeof(bach_gv5_vag), &vi);
    play_vag(bach_gv5_vag, sizeof(bach_gv5_vag), vi.pitch);

    while(1) {
        ClearOTagR(ot, OT_SIZE);

        FntPrint(fnt_id, "Name: %s\n", vi.name);
		FntPrint(fnt_id, "Rate: %d Hz\n", vi.sample_rate);
		FntPrint(fnt_id, "Duration: %d.%02d sec\n", vi.duration_sec, vi.duration_cent);
        FntFlush(fnt_id);

        DrawOTag(ot + OT_SIZE - 1);
        DrawSync(0);
        VSync(0);
    }
    return 0;
}
