#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <ip_addr.h>
#include <espconn.h>
#include <user_interface.h>
#include <driver/ssd1306.h>

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t    user_procTaskQueue[user_procTaskQueueLen];

uint8_t sin_table[256] = {
128,131,134,137,140,143,146,149,
152,156,159,162,165,168,171,174,
176,179,182,185,188,191,193,196,
199,201,204,206,209,211,213,216,
218,220,222,224,226,228,230,232,
234,235,237,239,240,242,243,244,
246,247,248,249,250,251,251,252,
253,253,254,254,254,255,255,255,
255,255,255,255,254,254,253,253,
252,252,251,250,249,248,247,246,
245,244,242,241,239,238,236,235,
233,231,229,227,225,223,221,219,
217,215,212,210,207,205,202,200,
197,195,192,189,186,184,181,178,
175,172,169,166,163,160,157,154,
151,148,145,142,138,135,132,129,
126,123,120,117,113,110,107,104,
101,98,95,92,89,86,83,80,
77,74,71,69,66,63,60,58,
55,53,50,48,45,43,40,38,
36,34,32,30,28,26,24,22,
20,19,17,16,14,13,11,10,
9,8,7,6,5,4,3,3,
2,2,1,1,0,0,0,0,
0,0,0,1,1,1,2,2,
3,4,4,5,6,7,8,9,
11,12,13,15,16,18,20,21,
23,25,27,29,31,33,35,37,
39,42,44,46,49,51,54,56,
59,62,64,67,70,73,76,79,
81,84,87,90,93,96,99,103,
106,109,112,115,118,121,124,128
};

uint8_t pattern8x8[8][8] = {
    { 0, 32,  8, 40,  2, 34, 10, 42},   /* 8x8 Bayer ordered dithering  */
    {48, 16, 56, 24, 50, 18, 58, 26},   /* pattern.  Each input pixel   */
    {12, 44,  4, 36, 14, 46,  6, 38},   /* is scaled to the 0..63 range */
    {60, 28, 52, 20, 62, 30, 54, 22},   /* before looking in this table */
    { 3, 35, 11, 43,  1, 33,  9, 41},   /* to determine the action.     */
    {51, 19, 59, 27, 49, 17, 57, 25},
    {15, 47,  7, 39, 13, 45,  5, 37},
    {63, 31, 55, 23, 61, 29, 53, 21} 
};

void draw()
{
    static uint8_t p1 = 0, p2 = 0, p3 = 0, p4 = 0;
    static uint32_t rand = 12345;
    uint8_t x, y, t1, t2, t3, t4, z, z0;

    display_clear();

    t1 = p1;
    t2 = p2;
    for (y = 0; y < 64; y++)
    {
        t3 = p3;
        t4 = p4;
        z0 = sin_table[t1] + sin_table[t2];
        for (x = 0; x < 128; x++)
        {
            z = z0 + sin_table[t3] + sin_table[t4];
            z = z >> 2;
            if (z > pattern8x8[x & 7][y & 7])
                display_setPixel(x, y, WHITE);
            t3 += 1;
            t4 += 2;
        }
        t1 += 1;
        t2 += 2;
    }
    display_update();

    p1 += rand % 4;
    p2 -= 2;
    p3 += 3;
    p4 -= 1;

    rand = rand*109 + 89;
}

static void ICACHE_FLASH_ATTR loop(os_event_t *events)
{
    draw();
    system_os_post(user_procTaskPrio, 0, 0);
}

void ICACHE_FLASH_ATTR user_init(void)
{
    system_update_cpu_freq(160);
    
    i2c_init();
    display_init(0x3c);

    system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
    system_os_post(user_procTaskPrio, 0, 0);
}
