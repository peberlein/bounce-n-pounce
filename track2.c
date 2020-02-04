#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

static char *names[] = {
    "road",
    "dirt1",
    "dirt2",
    "lrail1",
    "lrail2",
    "rail1",
    "rail2",
    "rrail1",
    "rrail2",
    "rock1",
    "rock2",
    "ltrock",
    "rtrock",
    "lbrock",
    "rbrock",
    "lbridge",
    "water1",
    "water2",
    "rbridge",
//    "lstripe1",
//    "lstripe2",
//    "rstripe1",
//    "rstripe2",
//    "lshore1",
//    "lshore2",
//    "rshore1",
//    "rshore2",
//    "tshore1",
//    "tshore2",
//    "bshore1",
//    "bshore2",
    "season1",
    "season2",
    "season3",
    "season4",
    "fuel1",
    "fuel2",
    "fuel3",
    "fuel4",
    "fuel5",
    "blank",
};
static char *short_names[] = {
// road tiles
    "RD","D1","D2","L1","L2","I1","I2","R1","R2",
// rock tiles (mid, top, low)
    "M1","M2","TL","TR","LL","LR",
// bridge, water
    "BL","W1","W2","BR",
// seasonal item		    
    "S1","S2","S3","S4",
// fuel tank
    "F1","F2","F3","F4","F5",
    "__",
    "__",
    "__",
};
                
        
// 0-road 1-dirt1 2-dirt2 3-left rail1 
// 4-left rail2  5-rail1 6-rail2 7-right rail1 
// 8-right rail2  9-mid rock1 A-mid rock2 B-left top rock 
// C-right top rock D-left low rock E-right low rock
// F=bridgeL 10=water1 11=water2 12=bridgeR// 13=stripeL1 14=stripeL2 15=stripeR1 16=stripeR2
// 17=shoreL1 18=shoreL2 19=shoreR1 1A=shoreR2
// 1B=shoreU1 1C=shoreU2 1D=shoreD1 1E=shoreD2

enum {
    RD = 0,  // 0 road
    D1,  // 1 dirt1
    D2,  // 2 dirt2
    L1,  // 3 lrail1
    L2,  // 4 lrail2
    I1,  // 5 rail1
    I2,  // 6 rail2
    R1,  // 7 rrail1
    R2,  // 8 rrail2
    M1,  // 9 mid rock1
    M2,  // a mid rock2
    TL,  // b top left rock
    TR,  // c top right rock
    LL,  // d low left rock
    LR,  // e low right rock
    BL,  // f lbridge
    W1,  // 10 water1
    W2,  // 11 water2
    BR,  // 12 rbridge

    S1, // seasonal item
    S2,
    S3,
    S4,
    F1, // fuel tank
    F2,
    F3,
    F4,
    F5, // fuel spout
    __,
};


static char
    road[] = {RD,RD,RD,RD}, // ' '
    dirt[] = {D1,D2,D2,D1}, // '.'
    rail[] = {L1,I1,L2,I2, I1,R1,I2,R2}, // 'I'
    up[] = {TL,RD,M2,TL, RD,TR,TR,M1}, // '\' or '/'
    mid[] = {M1,M2,M2,M1}, // '+'
    low[] = {M1,LL,LL,RD, LR,M2,RD,LR,}, // '/' or '\'
    water[] = {W1,W2,W2,W1},  // '~'
    shoreU[] = {0x1B,0x1C,W2,W1}, // '^'
    shoreD[] = {W1,W2,0x1D,0x1E}, // 'v'
    shoreL[] = {W1,0x17,W2,0x18}, // '<'
    shoreR[] = {0x19,W2,0x1A,W1}, // '>'
    bridge[] = {BL,RD,BL,RD, RD,BR,RD,BR,}, // '[' or ']'
    rock[] = {TR,TL,LR,LL};
    
/*
  11[RD,RD, L1,L2, L2,L1, I1,I2, I2,I1, R1,R2, R2,R1] 23[W1,D2, W2,D1, W2,L1, W1,I1, W2,I1, W1,R1, L2,RD, I2,RD, R2,RD, W1,RD, W2,RD, RD,L1, RD,I1, RD,R1] 31[D1,D2, D2,D1] 42[D1,M2, D2,M1, L2,M1, I2,M1, I2,M2, R2,M2, RD,RD, L2,RD, I2,RD]

7-8: RD,RD, L1,L2, L2,L1, I1,I2, I2,I1, R1,R2, R2,R1     W1,D2, W2,D1, W2,L1, W1,I1,               L2,RD, I2,RD,        W1,RD, W2,RD,                         D1,D2  D2,D1     D1,M2, D2,M1, L2,M1, I2,M1, I2,M2, R2,M2,        L2,RD, I2,RD]

*/


#define ____ __,__
#define RDRD RD,RD
#define L1L2 L1,L2
#define L2L1 L2,L1
#define I1I2 I1,I2
#define I2I1 I2,I1
#define R1R2 R1,R2
#define R2R1 R2,R1
#define M1M2 M1,M2
#define M2M1 M2,M1
#define M1LL M1,LL
#define M2LR M2,LR
#define LLRD LL,RD
#define LLTL LL,TL
#define TLLL TL,LL
#define TRLR TR,LR
#define TLM2 TL,M2
#define RDTL RD,TL
#define RDTR RD,TR
#define TRM1 TR,M1
#define LRRD LR,RD
#define LRTR LR,TR
#define D1D2 D1,D2
#define D2D1 D2,D1
#define L2RD L2,RD
#define I2RD I2,RD
#define M1D2 M1,D2
#define M1I1 M1,I1
#define M1R1 M1,R1
#define M2D1 M2,D1
#define M2L1 M2,L1
#define M2I1 M2,I1
#define L2TL L2,TL
#define W1M2 W1,M2
#define W2M1 W2,M1
#define W1W2 W1,W2
#define W2W1 W2,W1
#define RDL1 RD,L1
#define RDM1 RD,M1
#define RDM2 RD,M2
#define R2M2 R2,M2
#define I2M1 I2,M1
#define RDI1 RD,I1
#define W1R1 W1,R1
#define RDW1 RD,W1
#define RDW2 RD,W2
#define W1R2 W1,R2
#define W1RD W1,RD
#define W2RD W2,RD
#define W1D2 W1,D2
#define W2D1 W2,D1
#define W2I2 W2,I2
#define RDI2 RD,I2
#define W1TR W1,TR
#define W2I1 W2,I1
#define BLBL BL,BL
#define BRBR BR,BR
#define RDBL RD,BL
#define BLL1 BL,L1
#define BLW1 BL,W1
#define BRW2 BR,W2
#define BRRD BR,RD
#define W1BR W1,BR
#define TLRD TL,RD
#define M2RD M2,RD
#define RDBR RD,BR
#define D1W2 D1,W2
#define D2W1 D2,W1
#define I2W1 I2,W1
#define R2W2 R2,W2
#define W2BL W2,BL
#define W2L1 W2,L1
#define W1I1 W1,I1
#define R2RD R2,RD
#define RDR1 RD,R1
#define RDS1 RD,S1
#define S1S3 S1,S3
#define S3RD S3,RD
#define RDS2 RD,S2
#define S2S4 S2,S4
#define S4RD S4,RD
#define RDF1 RD,F1
#define F1F3 F1,F3
#define F3RD F3,RD
#define RDF2 RD,F2
#define F2F4 F2,F4
#define F4RD F4,RD
#define RDF5 RD,F5
#define F5RD F5,RD
#define M2W1 M2,W1
#define M1W2 M1,W2
#define M1RD M1,RD
#define D1M2 D1,M2
#define D2M1 D2,M1
#define L2M1 L2,M1
#define I2M2 I2,M2
#define BLRD BL,RD
#define LRR1 LR,R1
#define I2W2 I2,W2
#define LRM2 LR,M2
#define L2W1 L2,W1
#define TLW2 TL,W2
#define LLW2 LL,W2
#define LRW1 LR,W1
#define LRW2 LR,W2
#define BLM1 BL,M1
#define L2BL L2,BL
#define RDLR RD,LR

static const unsigned char
s[][64] = {
//[0    1    2    3    4    5    6   ][7    8    9    10   11   12   13   14   15   16   17   18   19   20  ][21   22  ][23   24   25   26   27   28   29   30   31  ]
  {____,____,____,____,____,____,____, ____,____,____,____,____,____,____,____,____,____,____,____,____,____, ____,____, ____,____,____,____,____,____,____,____,____},
//              1-6: rails                          7-20: rock                                                                          dirt        water       bridge
//set1[] = {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LLTL,LRTR,D1D2,D2D1,W1W2,W2W1,BLBL,BRBR,W2BL,W1BR,BLW1,BRW2,BR,RD},
//              1-6: empty                          7-20: rock                                                                          water->rock water       road->rock              rl->rk road<->rail
//set2[] = {RDRD,____,____,____,____,____,____,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,L2TL,LRTR,W1M2,W2M1,W1W2,I2RD,W2W1,RDL1,RDM1,RDM2,R2M2,I2M1,RDI1},
//              1-6: empty                          7-20: rock                                                            road->tr->rk  empty       water       bridge                  bridge->water
//set3[] = {RDRD,____,____,____,____,____,____,M1M2,M2M1,RDI1,W1R1,RDW1,RDW2,W1RD,W2RD,W1D2,W2D1,RDTR,TRM1,W2I2,RDI2,W1TR,W2I1,W1W2,W2W1,BLBL,BRBR,RDBL,BLL1,BLW1,BRW2,BRRD},
//              1-6: empty                          7-20: rock                                                            road->tr->rk  empty       water       bridge                  bridge->water
//set4[] = {RDRD,____,____,____,____,____,____,M1M2,M2M1,RDI1,W1R1,RDW1,RDW2,W1RD,W2RD,W1D2,W2D1,RDTR,TRM1,W1TR,W2I1,____,____,W1W2,W2W1,BLBL,BRBR,RDBL,W1BR,____,____,BRRD},

// L1L2,L2L1: RDM1 RDM2 M1RD M2RD TLRD LRW2 BLM1 W1M2 W1BR W2M1
// I1I2,I2I1,R1R2,R2R1: M1RD M2RD TLRD LRW2 BLM1
// D1D2,D2D1: RDR1 RDM1 RDM2 R2RD M1RD M1W2 M2RD M2W1 TLRD LRR1 LRW2 BLM1 W1M2 W2M1
/*

*/


//[0    1    2    3    4    5    6   ][7    8    9    10   11   12   13   14   15   16   17   18   19   20  ][21   22  ][23   24   25   26   27   28   29   30   31  ]
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, D1D2,D2D1, BRBR,BLBL,RDW1,RDW2,W2RD,RDBL,BRRD,W1W2,W2W1},//4f 29:7123,7e 29:3203
#if 1
  {RDRD,W1M2,W2M1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,W1RD,RDI1,BLRD,BLL1,BLW1,BRW2,W1D2,W2D1,RDTR,TRM1,W2I1,W1R1, M1W2,M2W1, RDL1,RDI1,RDW1,RDW2,LRR1,L2RD,I2RD,W1W2,W2W1},//
  {RDRD,RDM1,RDM2,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,R2RD,TRLR,L2W1,I2W2,I2W1,R2W2,LRRD,LRTR, M1RD,M2RD, W2L1,W1I1,RDR1,RDW2,W2RD,L2RD,I2RD,W1W2,W2W1},//
  {RDRD,BLM1,W1BR,I1I2,I2I1,R1R2,RDM2, M1M2,M2M1,W1RD,M2LR,BLRD,W2BL,BLW1,BRW2,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, W1M2,W2M1, BRBR,BLBL,I2M1,R2M2,W2RD,L2RD,I2RD,W1W2,W2W1},//
  {RDRD,RDBR,W1BR,I1I2,I2I1,R1R2,R2R1, ____,____,____,____,____,____,____,____,____,____,____,____,____,____, ____,____, RDL1,RDI1,RDR1,RDW2,W2RD,L2RD,I2RD,W1W2,W2W1},//
  {RDRD,RDM1,RDM2,I1I2,TLRD,R1R2,R2R1, D1W2,D2W1,W1RD,M2LR,BLRD,W2BL,RDBR,L2BL,I2RD,RDTL,I2W1,R2W2,LRRD,LRTR, W1I1,W2L1, M2D1,M1D2,M2L1,M1I1,M2I1,M1R1,BRRD,W1W2,W2W1},//
  {RDRD,RDBR,W1BR,LRW2,I2I1,R1R2,R2R1, D1W2,D2W1,W1RD,M2LR,BLRD,W2BL,R2RD,BRW2,L2W1,I2W2,I2W1,R2W2,W2I1,W1R1, W1RD,M2RD, L2M1,I2M2,I2M1,R2M2,D2M1,D1M2,BRRD,W1W2,W2W1},//

#elif 0
  {____,____,____,____,____,____,____, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,TLLL,TRLR,W1M2,W2M1,RDTR,TRM1,LRRD,LRTR, ____,____, ____,____,____,____,____,____,____,____,____},
  {RDRD,W1BR,____,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,BLW1,BRW2,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, M1W2,M2W1, RDL1,RDI1,RDW1,RDW2,LRR1,I2RD,L2RD,W1W2,W2W1},// a 30:1132
  {RDRD,____,____,W1R2,BLRD,BLM1,RDM2, M1M2,M2M1,I2W1,M2LR,BRRD,R2W2,RDBR,BRW2,L2W1,I2W2,D1W2,D2W1,LRRD,W1RD, RDM1,RDM2, BRBR,BLBL,RDW1,RDW2,____,I2RD,____,W1W2,W2W1},
  {RDRD,____,____,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,RDI1,W1R1,BRRD,BLL1,BLW1,BRW2,W1D2,W2D1,RDTR,TRM1,W2I1,W1RD, RDR1,R2RD, I2M1,R2M2,W1I1,W2L1,W2RD,I2RD,L2RD,W1W2,W2W1},//39 28:4134
  {____,____,____,____,____,____,____, M1M2,M2M1,____,M2LR,BRRD,____,____,____,M1W2,M2W1,RDTR,TRM1,LRRD,LRTR, ____,____, I2M1,R2M2,D1M2,D2M1,____,I2M2,L2M1,____,____},//a0a2 28:1115
  {RDRD,W1BR,RDBR,M2RD,M1RD,____,____, ____,____,I2W1,W1R1,____,R2W2,____,____,L2W1,I2W2,W2L1,W1I1,W2I1,W1RD, ____,____, M1I1,M1R1,M1D2,M2D1,M2L1,M2I1,____,____,____},//
  {RDRD,W1BR,RDBR,____,BLRD,W2BL,LRW2, ____,L2BL,I2W1,BLRD,W2BL,R2W2,RDBR,____,____,____,D1W2,D2W1,____,W1RD, W1M2,W2M1, BRBR,BLBL,RDW1,RDW2,W2RD,TLRD,W1RD,W1W2,W2W1},//

#elif 0
  {RDRD,W1M2,W2M1,____,BLM1,____,RDBL, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,BLW1,BRW2,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, RDM1,RDM2, W1RD,W2RD,RDW1,RDW2,RDL1,LRR1,RDI1,W1W2,W2W1},//7e 29:2203,a 30:1132
  {RDRD,RDBR,W1BR,____,____,LRW2,RDBL, M1M2,M2M1,M1LL,M2LR,LLRD,RDBL,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, R2RD,____, W1RD,W2RD,RDW1,RDW2,BLBL,BRRD,BRBR,W1W2,W2W1},//4f 28:3133
  {RDRD,W1M2,W2M1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,W1D2,W1R1,W2D1,RDBL,BLW1,BRW2,W2I1,BLL1,RDTR,TRM1,____,RDI1, ____,____, D1M2,D2M1,L2M1,I2M1,I2M2,R2M2,____,W1W2,W2W1},//a0 28:1114
  {RDRD,M1RD,M2RD,TLRD,RDLR,LRM2,____, M1M2,M2M1,RDBR,M2LR,D1W2,L2W1,L2BL,I2RD,W2BL,BLRD,D2W1,I2W1,LRRD,R2W2, M1W2,M2W1, I2W2,W2RD,RDW1,RDW2,RDL1,LRR1,RDI1,W1W2,W2W1},//
  {RDRD,____,W1BR,I1I2,I2I1,R1R2,R2R1, ____,____,RDBR,____,D1W2,RDBL,BLW1,BRW2,W2BL,BLRD,D2W1,I2W1,____,R2W2, ____,W1BR, W1RD,W2RD,RDR1,L2RD,W1I1,W2L1,I2RD,W1W2,W2W1},//
  {____,____,____,____,____,____,____, M1M2,M2M1,____,M2LR,____,____,____,____,W2BL,BLRD,RDTR,TRM1,LRRD,LRTR, ____,____, R2RD,____,RDW1,RDW2,RDL1,LRR1,RDI1,W1W2,W2W1},//
// FIXME: not aligned: RDBR,RDBL,RDI1,R2RD
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, M1W2,M2W1, ____,____,RDW1,RDW2,RDL1,LRR1,RDI1,W1W2,W2W1,},// a 30
  {RDRD,W1M2,W2M1,____,BLM1,____,RDBL, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,BLW1,BRW2,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, ____,RDM2, W1RD,W2RD,RDW1,RDW2,BLBL,BRRD,BRBR,W1W2,W2W1,},//7e 29
  {RDRD,____,W1BR,I1I2,I2I1,R1R2,R2R1, W1D2,W2D1,RDBR,____,D1W2,RDBL,W1R1,W2I1,____,BLRD,D2W1,I2W1,____,R2W2, D1D2,D2D1, W1RD,W2RD,RDW1,RDW2,BLBL,BRRD,BRBR,W1W2,W2W1,},// b 28
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,____,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, D1D2,D2D1, D1M2,D2M1,L2M1,I2M1,I2M2,R2M2,____,____,____,},//a0 28
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,____,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, D1D2,D2D1, D1M2,D2M1,L2M1,I2M1,I2M2,R2M2,____,____,____,},//a2 28
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, ____,____,W1D2,W1R1,W2D1,RDBL,BLW1,BRW2,W2I1,BLL1,____,____,____,RDI1, D1D2,D2D1, W1RD,W2RD,RDW1,RDW2,BLBL,BRRD,BRBR,W1W2,W2W1,},//0e 27
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,____,____,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, D1D2,D2D1, D1M2,D2M1,L2M1,I2M1,I2M2,R2M2,____,____,____,},//70
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,____,____,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, D1D2,D2D1, D1M2,D2M1,L2M1,I2M1,I2M2,R2M2,____,____,____,},//72
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,____,____,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, D1D2,D2D1, D1M2,D2M1,L2M1,I2M1,I2M2,R2M2,____,____,____,},//7d
  {RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,____,____,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,____, D1D2,D2D1, M1I1,M1R1,____,____,M2D1,M2L1,____,M2I1,M1D2,},//21
  {RDRD,W1M2,W2M1,I1I2,I2I1,R1R2,R2R1, M1M2,M2M1,M1LL,M2LR,LLRD,____,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, RDM1,RDM2, ____,____,____,I2M1,____,R2M2,____,W1W2,W2W1,},//39
  {RDRD,M1RD,M2RD,TLRD,____,____,____, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,TLLL,TRLR,TLM2,RDTL,RDTR,____,LRRD,____, RDM1,RDM2, W1RD,W2RD,RDW1,RDW2,____,____,____,W1W2,W2W1,},// f
  {RDRD,M1RD,M2RD,TLRD,____,____,____, M1M2,M2M1,M1LL,____,LLRD,LLTL,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,____,____, RDM1,RDM2, W1RD,W2RD,RDW1,RDW2,____,____,____,W1W2,W2W1,},// f
#else
  {____,____,____,____,____,____,____, M1M2,M2M1,W1D2,W2D1,W2I1,BLL1,BRRD,RDBL,____,W1R1,RDTR,TRM1,W1RD,W2RD, ____,____, RDW1,RDW2,BLBL,BRBR,BLW1,BRW2,RDI1,W1W2,W2W1},//de:31 1212
  {____,____,____,____,____,____,____, M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,M2W1,M1W2,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR, LRR1,RDL1, RDW1,RDW2,____,____,BLW1,BRW2,RDI1,W1W2,W2W1},//a :30 
  {RDRD,RDM2,BLM1,____,____,____,____, ____,____,____,____,____,____,____,____,____,____,____,____,____,____, ____,____, ____,____,____,____,____,____,____,____,____},//7e:29
  {____,____,____,____,____,____,____, ____,____,____,____,____,____,____,____,____,____,____,____,____,____, ____,____, ____,____,____,____,____,____,____,____,____},//
  {____,____,____,____,____,____,____, ____,____,____,____,____,____,____,____,____,____,____,____,____,____, ____,____, ____,____,____,____,____,____,____,____,____},//
  {____,____,____,____,____,____,____, ____,____,____,____,____,____,____,____,____,____,____,____,____,____, ____,____, ____,____,____,____,____,____,____,____,____},//
#endif
  {____,____,____,____,____,____,____, RDS1,S1S3,S3RD,RDS2,S2S4,S4RD,RDF1,F1F3,F3RD,RDF2,F2F4,F4RD,RDF5,F5RD, ____,____, ____,____,____,____,____,____,____,____,____},//
},

//7e 2203 RDRD,RDM2,BLM1,W1M2,W2M1,W1RD,W2RD,M1M2,M2M1,M1LL,M2LR,LLRD,LLTL,BLW1,BRW2,TLM2,RDTL,RDTR,TRM1,LRRD,LRTR,____,____,____,RDBL,RDW1,RDW2,BLBL,BRRD,BRBR,W1W2,W2W1, 29

//4f 4443 RDRD,RDBR,W1BR,TLW2,LLW2,LRW1,LRW2,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,W2RD,LRTR,M1W2,M2W1,____,RDBL,RDW1,RDW2,BLBL,BRRD,BRBR,W1W2,W2W1, 31

// common RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LLTL,LRTR,D1D2,D2D1,W1W2,W2W1,BLBL,BRBR,W2BL,W1BR,BLW1,BRW2,__,__
//                                                          RDI1,W1R1,RDW1,RDR2,W1RD,W2RD,W1D2,W2D1,              L2Rl,RDI2,W1M2,W2M1,              RDL1,RDM1,RDM2,R2M2,I2M1,RDI1,I2,RD
//                                                                                                                                             W2I2,W2I1,W1TR,W2I1,                            RDBL,BLL1,              BR,RD
//                                                                                                                                             W1TR,                                                        

//39 1222 RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,L2TL,LRTR,W1M2,W2M1,W1W2,W2W1,RDL1,RDM1,RDM2,R2M2,I2M1,RDI1,I2RD,
//de 1413 RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,RDI1,W1R1,RDW1,RDW2,W1RD,W2RD,W1D2,W2D1,RDTR,TRM1,W1TR,W2I1,D1D2,D2D1,W1W2,W2W1,BLBL,BRBR,RDBL,BLL1,BLW1,BRW2,BRRD,




// 0 1010 RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,____,____,____,____,____,____,____,____,____,____,____,____,____,____,D1D2,D2D1,____,____,____,____,____,____,____,____,____,
// 1 1100 RDRD,____,____,____,____,____,____,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,____,TRLR,TLM2,RDTL,RDTR,TRM1,LLTL,LRTR,____,____,____,____,____,____,____,____,____,____,____,
// 2 1110 RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,____,____,TLM2,RDTL,RDTR,TRM1,LLTL,____,D1D2,D2D1,____,____,____,____,____,____,____,____,____,
// 7 1100 RDRD,____,____,____,____,____,____,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,LLTL,LRTR,____,____,____,____,____,____,____,____,____,____,____,
//2d 1110 RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,M1LL,____,LLRD,____,____,____,TLM2,RDTL,RDTR,TRM1,____,____,D1D2,D2D1,____,____,____,____,____,____,____,____,____,
//34 1102 RDRD,____,____,____,____,____,____,M1M2,M2M1,M1LL,M2LR,LLRD,LRRD,TLLL,TRLR,TLM2,RDTL,RDTR,TRM1,____,LRTR,____,____,____,____,____,RDM1,RDM2,____,____,____,____,
//dd 1110 RDRD,L1L2,L2L1,I1I2,I2I1,R1R2,R2R1,M1M2,M2M1,M1LL,____,LLRD,____,____,____,____,____,RDTR,TRM1,____,____,D1D2,D2D1,____,____,____,____,____,____,____,____,____,


#define START1 0
#define START2 7
#define START3 21
#define START4 23

// set 1 - rails or (something else)
seg_11[] = {RDRD, L1L2, L2L1, I1I2, I2I1, R1R2, R2R1},  // 1-6: rails
seg_12[] = {RDRD, TLRD, M2RD, W1M2, W2M1, RDM1, RDM2},   // 1-6: unused, water->rock, road->rock
seg_13[] = {RDRD, RDRD, RDRD, RDBL, RDBR, RDM1, RDM2},   // 1-6: unused, road->bridge, road->rock
*seg1[] = {&s[0][START1*2], &s[1][START1*2], &s[2][START1*2], &s[3][START1*2], &s[4][START1*2], &s[5][START1*2], &s[6][START1*2], &s[7][START1*2], &s[8][START1*2]},

// set 2 - rocks or (something else)
seg_21[] = {
    M2M1, M1M2, M1LL, M2LR, LLRD, LRRD,
    TLLL, TRLR, TLM2, RDTL, RDTR, TRM1, LLTL, LRTR},  // 7-20: rock
seg_22[] = {
    L2RD, I2RD,  /* 7-8: lrail->road */
    BLW1, BRW2,  /* 9-10: bridge->water */
    D1W2, D2W1,  /* 11-12: dirt->water */
    RDW1, RDW2,  /* 13-14: road->water */
    I2W1, R2W2,  /* 15-16: rrail->water */
    RDBL, RDBR,  /* 17-18: road->bridge */
    LLRD,         /* 23: lower lrock->road */
    W2BL},        /* 20: water2->bridge */
seg_23[] = {
    W1D2, W2D1,  /* 7-8: water->dirt */
    W2L1, W1I1, W2I1, W1R1,  /* 9-12: water->rails */
    L2RD, I2RD, R2RD,  /* 13-15: rail->road */
    W1RD, W2RD,  /* 16-17: water->road */
    RDL1, RDI1, RDR1},   /* 18-20: road->rails */
seg_24[] = {
    RDS1, S1S3, S3RD, RDS2, S2S4, S4RD,  /* 7-12 seasonal item */
    RDF1, F1F3, F3RD, RDF2, F2F4, F4RD,  /* 13-18 fuel tank */
    RDF5, F5RD}, /* 19-20 fuel spout */
seg_25[] = {
    M2M1, M1M2, M1LL, M2LR, LLRD, LRRD,
    TLLL, TRLR, TLM2, RDTL, RDTR, TRM1, L2TL, LRTR},  // 7-20: rock
*seg2[] = {&s[0][START2*2], &s[1][START2*2], &s[2][START2*2], &s[3][START2*2], &s[4][START2*2], &s[5][START2*2], &s[6][START2*2], &s[7][START2*2], &s[8][START2*2]},

// set 3 - misc
seg_31[] = {D1D2, D2D1}, // 21-22: dirt
seg_32[] = {RDW1, RDW2}, // 21-22: road->water
seg_33[] = {M2W1, M1W2}, // 21-22: rock->water
seg_34[] = {M2RD, M1RD}, // 21-22: rock->road
seg_35[] = {L2RD, I2RD}, // 21-22: left rail->road
seg_36[] = {W1M2, W2M1}, // 21-22: water->rock
*seg3[] = {&s[0][START3*2], &s[1][START3*2], &s[2][START3*2], &s[3][START3*2], &s[4][START3*2], &s[5][START3*2], &s[6][START3*2], &s[7][START3*2], &s[8][START3*2]},

// set 4 water stuff
seg_41[] = {
    M1D2, M2D1, M2L1, M2I1, M1I1, M1R1,  /* 23-28: rock->rails */
    RDRD,  /* 29: unused */
    L2RD, I2RD,  /* 30-31: rails->road */
},
seg_42[] = {
    D1M2, D2M1, L2M1, I2M1, I2M2, R2M2,  /* 23-28: rails->rock */
    RDRD,  /* 29: unused */
    L2RD, I2RD},  /* 30-31: rails->road */
seg_43[] = {
    W1BR,  /* 23: water1->bridge */
    BLBL, BRBR,  /* 24-25: bridge->bridge */
    BLRD, BRRD,  /* 26-27: bridge->road */
    W1W2, W2W1,  /* 28-29: water->water */
    W1RD, W2RD},   /* 30-31: water->road */
seg_44[] = {
    LRR1, /* 23: lower rrock->rrail */
    RDL1, RDI1,  /* 24-25: road->lrails */
    I2W1, I2W2,  /* 26-27: rail->water */
    W1W2, W2W1,  /* 28-29: water->water */
    RDW2, RDW1},   /* 30-31: road->water */
seg_45[] = {
    RDTR,  /* 23: road->top right */
    L2W1, R2W2,  /* 24-25: lrail->water rrail-water (will be glitchy going into seg 10) */
    I2W1, I2W2,  /* 26-27: rail->water */
    W1W2, W2W1,  /* 28-29: water->water */
    RDW2, RDW1},   /* 30-31: road->water */
seg_46[] = {
    M1D2, M2D1, M2L1, M2I1, M1I1, M1R1,  /* 23-28: rock->rails */
    LRM2, LRTR, // 25-26: lower right -> rock, top right
    RDRD, 
    W1W2, W2W1,  /* 28-29: water->water */
    M2RD, M1RD}, // 23-24: rock->road
seg_47[] = {
    W1BR,  /* 23: water1->bridge */
    TLW2, LLW2,  /* 24-25: top/lower left->water */
    LRW1, LRW2,  /* 26-27: lower right->water */
    W1W2, W2W1,  /* 28-29: water->water */
    W1RD, W2RD},   /* 30-31: water->road */
seg_48[] = {
    R2M2, /* 23: road->rock */
    RDL1, RDI1,  /* 24-25: road->lrails */
    I2M1, I2RD,  /* 26-27: rail->rock,road */
    W1W2, W2W1,  /* 28-29: water->water */
    RDM1, RDM2},   /* 30-31: road->rock */
*seg4[] = {&s[0][START4*2], &s[1][START4*2], &s[2][START4*2], &s[3][START4*2], &s[4][START4*2], &s[5][START4*2], &s[6][START4*2], &s[7][START4*2], &s[8][START4*2]};

// segment groups [0-6] [7-20] [21-22] [23-31]
char set_size[] = {7, 14, 2, 9};

static int transition_counts[32*32] = {}; // the number of patterns that use each transition

static int count_use(const char *use)
{
    int count = 0;
    if (use) 
	for (int i = 0; i < 32*32; i++)
            count += use[i];
    return count;
}

static int common(const char *use1, const char *use2)
{
    int count = 0;
    if (use1 && use2)
        for (int i = 0; i < 32*32; i++)
            count += use1[i] && use2[i];
    return count;
}

static int satisfies(const char *table, const char *use, char *unsat)
{
    int i, j, n = 0;
    for (i = 0; i < 32*32; i++) {
        if (use[i] == 0)
            continue;
        int from = i >> 5, to = i & 31;
        for (j = 0; j < 32*2; j += 2) {
            if (i == 0 && j != 0)
                continue; // ignore RD->RD not in zero position
            if (table[j] == from && table[j+1] == to) {
                unsat[i] = 0;
                //printf("found %s->%s\n", short_names[from], short_names[to]);
                n++;
                break;
            }
        }
    }
    return n;
}
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


static void print_seg(const char *use, const unsigned char *s, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        int x = s[i*2];
        int y = s[i*2+1];
        if (use[x*32+y])
            printf("%s%s,", short_names[x], short_names[y]);
        else
            printf("____,");
    }
}

static void print_tg(const char *use, int a, int b, int c, int d)
{
    print_seg(use, seg1[a], set_size[0]);
    printf(" ");
    print_seg(use, seg2[b], set_size[1]);
    printf(" ");
    print_seg(use, seg3[c], set_size[2]);
    printf(" ");
    print_seg(use, seg4[d], set_size[3]);
    printf("// %x%x%x%x ", a, b, c, d);
    printf("%d\n", count_use(use));
}

static void print_tilegroup(const char *use, int tg)
{
    print_tg(use, (tg>>12)&15, (tg>>8)&15, (tg>>4)&15, tg&15);
}

static void find_in_sets(int i)
{
    int x, y;
    for (x = 0; x < ARRAY_SIZE(seg1); x++) {
        for (y = 0; y < set_size[0]*2; y+= 2) {
            if (seg1[x][y] == (i>>5) && seg1[x][y+1] == (i&31))
                printf(" 1%d.%d", x, y/2);
        }
    }
    for (x = 0; x < ARRAY_SIZE(seg2); x++) {
        for (y = 0; y < set_size[1]*2; y+= 2) {
            if (seg2[x][y] == (i>>5) && seg2[x][y+1] == (i&31))
                printf(" 2%d.%d", x, y/2+START2);
        }
    }
    for (x = 0; x < ARRAY_SIZE(seg3); x++) {
        for (y = 0; y < set_size[2]*2; y+= 2) {
            if (seg3[x][y] == (i>>5) && seg3[x][y+1] == (i&31))
                printf(" 3%d.%d", x, y/2+START3
            );
        }
    }
    for (x = 0; x < ARRAY_SIZE(seg4); x++) {
        for (y = 0; y < set_size[3]*2; y+= 2) {
            if (seg4[x][y] == (i>>5) && seg4[x][y+1] == (i&31))
                printf(" 4%d.%d", x, y/2+START4);
        }
    }
}


static int table_conflict(char *use, char *table, char *above_use, char *above)
{
    int i;
    for (i = 0; i < 32*2; i += 2) {
        int x = table[i]*32+table[i+1];
        int y = above[i]*32+above[i+1];
        if (use[x] && above_use[y] && x != y)
            return 1;
    }
    return 0;
}
static void print_conflict(char *use, char *table, char *above_use, char *above)
{
    int i;
    for (i = 0; i < 32*2; i += 2) {
        int x = table[i]*32+table[i+1];
        int y = above[i]*32+above[i+1];
        if (i == START2*2 || i == START3*2 || i == START4*2)
            printf(" ");
        if (use[x] && above_use[y] && x != y)
            printf("!!!! ");
        else
            printf("     ");
    }
    printf("\n");
}

static void generate_tilegroup(int tg, char *table)
{   
    int a = (tg >> 12) & 15;
    int b = (tg >> 8) & 15;
    int c = (tg >> 4) & 15;
    int d = (tg >> 0) & 15;

    memcpy(table, seg1[a], set_size[0]*2);
    memcpy(table+set_size[0]*2, seg2[b], set_size[1]*2);
    memcpy(table+(set_size[0]+set_size[1])*2, seg3[c], set_size[2]*2);
    memcpy(table+(set_size[0]+set_size[1]+set_size[2])*2, seg4[d], set_size[3]*2);
}

static int get_constrained_tilegroup(char *above_use, int above_tg, char *use, char *below_use, int below_tg)
{
    int a, b, c, d, depth;
    char table[32*2], unsat[32*32], best_table[32*2], above[32*2], below[32*2];
    int count = 0, best_n = 0, best;

    count = count_use(use);
    if (above_use) generate_tilegroup(above_tg, above);
    if (below_use) generate_tilegroup(below_tg, below);

    for (depth = 1; depth <= MAX(MAX(MAX(ARRAY_SIZE(seg1),ARRAY_SIZE(seg2)),ARRAY_SIZE(seg3)),ARRAY_SIZE(seg4)); depth++) {
        for (d = 0; d < MIN(depth,ARRAY_SIZE(seg4)); d++) {
            memcpy(table+(set_size[0]+set_size[1]+set_size[2])*2, seg4[d], set_size[3]*2);
            for (c = 0; c < MIN(depth,ARRAY_SIZE(seg3)); c++) {
                memcpy(table+(set_size[0]+set_size[1])*2, seg3[c], set_size[2]*2);
                for (b = 0; b < MIN(depth,ARRAY_SIZE(seg2)); b++) {
                    memcpy(table+set_size[0]*2, seg2[b], set_size[1]*2);
                    for (a = 0; a < MIN(depth,ARRAY_SIZE(seg1)); a++) {
                        memcpy(table, seg1[a], set_size[0]*2);
                        char tmp[32*32];
                        memcpy(tmp, use, 32*32);

                        //printf("%04x\n", (a << 12)|(b<<8)|(c<<4)|d);
                        int n = satisfies(table, use, tmp);

                        if (n == count) {
                            if (above_use && table_conflict(use, table, above_use, above)) {
                                //print_conflict(use, table, above_use, above);
                                continue;
                            }
                            if (below_use && table_conflict(use, table, below_use, below)) {
                                //print_conflict(use, table, below_use, below);
                                continue;
                            }
                            print_tg(use, a, b, c, d);

                            return (a<<12)|(b<<8)|(c<<4)|d;
                        }
                        if (n > best_n) {
                            best_n = n;
                            best = (a<<12)|(b<<8)|(c<<4)|d;
                            memcpy(unsat, tmp, 32*32);
                            memcpy(best_table, table, 32*2);
                        }
                    }
                }
            }
        }
    }
    printf("\ntotal=%d/%d best=%04x\n", best_n, count, best);
    for (int i = 0; i < 32*2; i+=2) {
        int from = best_table[i], to = best_table[i+1];
        if (i==START2*2 || i==START3*2 || i==START4*2) printf(" ");
        printf("%s%s,", short_names[from], short_names[to]);
    }
    printf("\n");
    for (int i = 0; i < 32*2; i+=2) {
        int from = best_table[i], to = best_table[i+1];
        if (i==START2*2 || i==START3*2 || i==START4*2) printf(" ");
        printf("%s ", use[from*32+to] ? "====" : "    ");
    }
    printf("\n");

    if(0)for (int i = 0; i < 32*2; i+=2) {
        int from = best_table[i], to = best_table[i+1];
        printf("%02x.%d %s->%s %s\n", 
            i < set_size[0]*2 ? 0x10+(best>>12) :
            i < (set_size[0]+set_size[1])*2 ? 0x20+((best>>8)&15) :
            i < (set_size[0]+set_size[1]+set_size[2])*2 ? 0x30+((best>>4)&15) : 
                0x40+(best&15),i/2,
            short_names[from], short_names[to],
            (i > 0 && from*32+to == __*32+__) ? "----" :
            use[from*32+to] ? "used" : "");
    }
    for (int i = 0; i < 32*32; i++) {
        int from = i >> 5, to = i&31;
        if (unsat[i]) {
            printf("need %s->%s <=== (%d)", short_names[from], short_names[to], transition_counts[i]);
            find_in_sets(i);
            printf("\n");
	}
    }
    return 0;
}

static int get_tilegroup(char *use)
{
    return get_constrained_tilegroup(NULL, 0, use, NULL, 0);
}


static void load_track(void)
{
    int start = 0x16f5;
    int end = 0x18f5;
    char track[512];
    int i;
    FILE *f = fopen("roms/brubber.12c","rb");
    fseek(f, start, SEEK_SET);
    fread(track, 1, sizeof(track), f);
    fclose(f);
    for (i = 0; i < sizeof(track); i++) {
        printf("%s%d", (i&7) ? "," : "\n", track[511-i]);
    }
    printf("\n");
}

static unsigned char segments[2048]; // 16 segments of 16 rows of 8 bytes (16 nibbles)


static void decode_segment(int k, int row, unsigned char *buf1, unsigned char *buf2)
{
    int i, j = row;

    unsigned char *seg = segments + (k&15)*128 + j*8;
    for (i = 0; i < 16; i++) {
        char *map = i < 8 ?
#ifdef SHORE
               //012345678_9abcdef
                " I.+/[vv\\]^^<~* ": // with shore
                " I.+\\]vv/[^^>~* ";
#else
                " I.+/[~~\\]~~~~* ":  // without shore
                " I.+\\]~~/[~~~~* ";
#endif
        char c = (seg[i>>1] >> (4-(i&1)*4))&0xf;
        //putchar(map[c]);
        char *p =
            c == 0 ? road :
            c == 1 ? (i < 8 ? rail : rail+4) :
            //c == 1 ? rail :
            c == 2 ? dirt :
            c == 3 ? mid :
            c == 4 ? (i < 8 ? low : low+4) :
            c == 8 ? (i < 8 ? up : up+4) :
#if 1
            c == 5 ? (i < 8 ? bridge : bridge+4) :
            c == 9 ? (i < 8 ? bridge+4 : bridge) :
#else
            c == 5 ? road :
            c == 9 ? road :
#endif
#ifdef SHORE
            c == 6 ? shoreD :
            c == 7 ? shoreD :
            c == 10 ? shoreU :
            c == 11 ? shoreU :
            c == 12 ? (i < 8 ? shoreL : shoreR) :
#endif
            c == 14 ? rock :
            c == 15 ? road :
            water;
        buf1[i*2] = p[0];
        buf1[i*2+1] = p[1];
        buf2[i*2] = p[2];
        buf2[i*2+1] = p[3];
    }
    // special case for finish line
    if (k == 16 && row == 0) {
        buf2[6] = S1;
        buf2[7] = S2;
        buf2[8] = F1;
        buf2[9] = F2;
    } else if (k == 16 && row == 1) {
        buf1[6] = S3;
        buf1[7] = S4;
        buf1[8] = F3;
        buf1[9] = F4;
        buf1[10] = F5;
    }
    //putchar('\n');
}

static void calculate_transitions(unsigned char *src, unsigned char *dst, char *use)
{
    int i;
    for (i = 0; i < 32; i++) {
        //printf("%02x%02x ", src[i], dst[i]);
	    use[src[i]*32+dst[i]] = 1;
    }
    //printf("\n");
}

static void print_segment_part(unsigned char *seg)
{
    int i;
	for (i = 0; i < 16; i++) {
	    char *map = i < 8 ?
#ifdef SHORE
               //0123456789abcdef
                " I.+/[vv\\]^^<~*}":
		        " I.+\\]vv/[^^>~*{";
#else          //012345678 9abcdef
                " I.+/[~~\\]~~~~* ":  // without shore
                " I.+\\]~~/[~~~~* ";
#endif         //01234 56789abcdef   
	    putchar(map[(seg[i>>1] >> (4-(i&1)*4))&0xf]);
	}
}

static void print_segment(int k)
{
    int i, j;
    printf("\n%x:\n", k);
    for (j = 0; j < 16; j++) {
    	unsigned char *seg = segments + k*128 + j*8;
        print_segment_part(seg);
    	putchar('\n');
    }
}

static void print_segments(void)
{
    char *spacer = "     ";
    int j, k;
    for (j = 0; j < 8; j++) {
        printf("%d:%s              ", j, spacer);
    }
    printf("\n");
    for (j = 0; j < 16; j++) {
        for (k = 0; k < 8; k++) {
            unsigned char *seg = segments + k*128 + j*8;
        	print_segment_part(seg);
            printf("%s", spacer);
        }
        printf("\n");
    }
    printf("\n");
    for (j = 8; j < 16; j++) {
        printf("%x:%s              ", j, spacer);
    }
    printf("\n");
    for (j = 0; j < 16; j++) {
        for (k = 8; k < 16; k++) {
            unsigned char *seg = segments + k*128 + j*8;
        	print_segment_part(seg);
            printf("%s", spacer);
        }
        printf("\n");
    }
}

static void load_segments(void)
{
    FILE *f = fopen("segments.dat","rb");
    if (!f) return;
    fread(segments, 1, 2048, f);
    fclose(f);

    // patches to segments for simpler encoding
    segments[11*128 + 0*8 + 1] ^= (1 ^ 0) << 4; // rail to road
    segments[11*128 + 1*8 + 1] ^= (14 ^ 0) << 4; // rock to rail

    segments[10*128 + 12*8 + 1] ^= (8 ^ 0); // slant to road
    //segments[10*128 + 10*8 + 6] = 0x03;
    //segments[10*128 + 11*8 + 6] = 0x04;
    segments[10*128 + 12*8 + 6] = 0x04;

#if 0
    segments[10*128 + 11*8 + 0] = 0x44;
    segments[10*128 + 11*8 + 1] = 0x40;
    segments[10*128 + 11*8 + 7] = 0x44;
    segments[10*128 + 12*8 + 0] = 0x00;
    segments[10*128 + 12*8 + 1] = 0x00;
    segments[10*128 + 12*8 + 7] = 0x00;
#endif
    
    segments[4*128 + 8*8 + 4] = 0x00; // remove rock before water
    
    segments[5*128 + 11*8 + 2] = 0xf0; // remove rock at fork

    // move gap in bridge down 3
    segments[9*128 + 9*8 + 2] ^= 0x01;
    segments[9*128 + 10*8 + 2] ^= 0x01;
    segments[9*128 + 11*8 + 2] ^= 0x01;
    segments[9*128 + 13*8 + 2] ^= 0x01;
    segments[9*128 + 14*8 + 2] ^= 0x01;
    segments[9*128 + 15*8 + 2] ^= 0x01;

    // fix ff change / to +
    segments[15*128 + 0*8 + 6] = 0x03;
    // fix f6 change \ to +
    segments[6*128 + 15*8 + 6] = 0x03;
    // fix 4f change / to +
    segments[15*128 + 14*8 + 0] = 0x38;
    segments[15*128 + 15*8 + 0] = 0x33;

    // move rocks away from edge transitions
    segments[15*128 + 0*8 + 4] = 0x00;
    segments[15*128 + 1*8 + 4] = 0xe0;
    segments[15*128 + 14*8 + 4] = 0xe0;
    segments[15*128 + 15*8 + 4] = 0x00;

    // fix de transition edge
    segments[13*128 + 0*8 + 6] = 0x01;
    segments[13*128 + 1*8 + 5] = 0x00;
    segments[13*128 + 1*8 + 6] = 0x81;
    segments[13*128 + 2*8 + 5] = 0x08;

    segments[13*128 + 7*8 + 5] = 0x03;
    segments[13*128 + 8*8 + 5] = 0x04;
    segments[13*128 + 8*8 + 6] = 0x31;
    segments[13*128 + 9*8 + 6] = 0x41;

    segments[13*128 + 11*8 + 6] = 0x01;
    segments[13*128 + 12*8 + 6] = 0xe1;


    //segments[15*128 + 5*8] = 0x40; // smooth slant
    //segments[15*128 + 4] = 0x00; // remove rock
    //segments[15*128 + 15*8 + 4] = 0x00; // remove rock
}


// cannot allow 7->8 combo (too many transitions)
// 8 patterns of 64 segments
static char patterns[] = {
    0,0,0,0,0,7,7,7,
    7,7,0,7,0,7,0,7,
    0,8,9,9,9,9,9,9,
    12,10,7,7,7,1,1,1,
    1,1,3,4,11,2,2,2,
    0,0,0,0,0,7,7,7,
    3,4,11,13,13,13,0,7,
    //0,7,0,7,8,12,10,0,
    0,7,0,0,8,12,10,0,

    0,0,0,0,0,13,13,8,
    10,7,7,1,3,4,11,2,
    1,0,0,0,0,8,9,12,
    10,7,7,1,13,13,13,0,
    0,0,0,0,0,13,13,8,
    10,7,7,1,3,4,14,14,
    11,2,0,0,0,8,9,12,
    10,1,3,4,11,0,0,0,

    0,0,0,0,0,13,13,8,
    10,7,7,1,3,4,14,14,
    11,0,0,0,0,8,9,12,
    10,7,7,1,13,13,13,0,
    0,1,2,2,2,2,5,6,
    //7,8,9,10,7,7,13,8,
    0,8,9,10,7,7,13,8,
    12,9,9,9,10,1,1,2,
    2,2,13,3,4,11,0,0,

    0,0,0,13,13,5,15,6,
    3,4,14,11,0,0,0,0,
    13,13,5,15,6,3,4,14,
    11,0,0,0,0,7,7,7,
    0,0,0,13,13,8,9,12,
    9,12,9,9,9,9,9,9,
    10,5,15,6,3,4,11,0,
    0,8,12,12,9,12,0,0,

    0,0,0,13,13,5,15,6,
    3,4,14,11,13,13,2,2,
    13,13,5,15,6,3,4,14,
    11,0,3,4,6,5,6,5,
    15,15,6,13,13,8,9,12,
    9,12,9,9,9,9,9,9,
    10,5,15,6,3,4,11,0,
    0,8,12,12,9,12,0,0,

    7,2,2,2,2,0,11,0,
    14,0,14,14,6,1,3,4,
    15,6,13,13,8,9,9,12,
    10,2,13,2,13,0,0,7,
    // 5,6,7,7,14,14,11,8, // original 7,14
    5,6,7,0,14,14,11,8,    // changed  0,14
    12,10,5,15,15,15,15,6,
    3,4,6,3,9,9,10,1,
    //1,13,13,2,2,14,0,0, // attempt 1
    //1,13,13,2,1,14,0,0,  // attempt 2
    1,13,13,2,13,14,0,0, // original

    0,8,12,9,9,12,10,2,
    2,2,3,4,14,14,11,8,
    10,7,7,5,15,15,6,5,
    15,6,0,8,9,10,1,13,
    13,13,13,13,13,13,13,8,
    12,9,12,9,12,9,9,10,
    5,15,15,6,5,6,2,1,
    1,5,6,3,4,11,0,0,

    5,6,1,3,4,14,14,14,
    14,11,13,5,15,15,15,6,
    5,6,7,7,13,8,12,12,
    12,9,9,9,12,12,10,1,
    3,4,14,14,14,14,14,11,
    8,9,9,9,9,9,9,9,
    10,5,15,6,5,6,5,6,
    5,15,6,3,4,11,0,0,
};

struct {
//    int l, r; // left, right tile strip index
    char *use;
    int tileset, left_idx, right_idx;
} seg_tr[16*16] = {};

struct {
    char *use0, *use1, *use2, *use3, *use4;
    int tileset1, tileset2, tileset3;
    int left_idx[31], right_idx[31];
} seg_in[17] = {};

static char* get_use(int i)
{
    return  i >= 18*16 ? seg_in[i&15].use3 : 
            i >= 17*16 ? seg_in[i&15].use2 :
            i >= 16*16 ? seg_in[i&15].use1 :
            seg_tr[i].use;
}

static int left_count = 0, right_count = 0;
static unsigned char *left_seg = NULL, *right_seg = NULL;

static int get_segment(int ts, unsigned char *src, unsigned char *dst, unsigned char **seg, int *count)
{
    unsigned char buf[16];
    int i, j;
    for (i = 0; i < 16; i++) {
        int n = (ts >> 12) & 15;
        for (j = 0; j < 32; j++) {
            if (j == START2) 
                n = (ts >> 8) & 15;
            else if (j == START3) 
                n = (ts >> 4) & 15;
            else if (j == START4) 
                n = (ts >> 0) & 15;
            if (s[n][j*2] == src[i] && s[n][j*2+1] == dst[i]) {
                buf[i] = j << 3;
                break;
            }
        }
        if (j == 32) {
            printf("didn't find %s%s in %04x\n", short_names[src[i]], short_names[dst[i]], ts);
            return -1;
        }
    }

    for (i = 0; i < *count; i++) {
        for (j = 0; j < 16; j++) {
            if (*(*seg + i*16 + j) & 7)
                printf("%p %x,%x corrupt: %02x\n", *seg, i, j, *(*seg + i*16 + j));
        }
        if (memcmp(buf, *seg + i*16, 16) == 0)
            return i;
    }

    ++*count;
    *seg = realloc(*seg, (i+1) * 16);
    printf("%p %d\n", *seg, i);
    memcpy((*seg) + i*16, buf, 16);
    return i;
}

static void generate_segments(FILE *f)
{
    unsigned char prev[32] = {};
    unsigned char buf1[32] = {};
    unsigned char buf2[32] = {};
    int i, j, l, r;

    fprintf(f, "StripIndex:   ; %lu bytes\n", ARRAY_SIZE(seg_in)*31*2);
    for (i = 0; i < ARRAY_SIZE(seg_in); i++) {
        int ts = seg_in[i].tileset1;
        fprintf(f, "       DATA ");
        for (j = 0; j < 16; j++) {
            if (j == 4+1) 
                ts = seg_in[i].tileset2;
            else if (j == 8+1) 
                ts = seg_in[i].tileset3;
            decode_segment(i, j, buf1, buf2);
            if (j > 0) {
                l = get_segment(ts, prev, buf1, &left_seg, &left_count);
                if (l == -1) {
                    printf("failed seg %d line %d left\n", i, j);
                    exit(0);
                }
                r = get_segment(ts, prev+16, buf1+16, /*&left_seg, &left_count*/&right_seg, &right_count);
                if (r == -1) {
                    printf("failed seg %d line %d right\n", i, j);
                    exit(0);
                }

                fprintf(f, ">%02X%02X, ", l, r);
            }
            l = get_segment(ts, buf1, buf2, &left_seg, &left_count);
            if (l == -1) {
                printf("failed seg %d line %d left\n", i, j);
                exit(0);
            }
            r = get_segment(ts, buf1+16, buf2+16, /*&left_seg, &left_count*/&right_seg, &right_count);
            if (r == -1) {
                printf("failed seg %d line %d right\n", i, j);
                exit(0);
            }
            fprintf(f, ">%02X%02X%s", l, r, j < 15 ? "," : "\n");

            memcpy(prev, buf2, 32);
        }
    }
    fprintf(f, "TransitionStripIndex:  ; %d bytes\n", 16*16*2);
    for (i = 0; i < 16*16; i++) {
        l = 0;
        r = 0;
        if (seg_tr[i].use) {
            decode_segment(i&15, 15, buf1, buf2); // bottom of next segment
            memcpy(prev, buf2, 32);
            decode_segment(i>>4, 0, buf1, buf2); // top of current segment
            //printf("%02x: %04x ", i, seg_tr[i].tileset);
            //for (j = 0; j < 32; j++) printf(" %2s%2s", short_names[prev[j]], short_names[buf1[j]]);
            //printf("\n");
            l = get_segment(seg_tr[i].tileset, prev, buf1, &left_seg, &left_count);
            r = get_segment(seg_tr[i].tileset, prev+16, buf1+16, /*&left_seg, &left_count*/&right_seg, &right_count);
            //printf("         ");
            //for (j = 0; j < 16; j++) printf("  %2d ", left_seg[l*16+j]>>3);
            //for (j = 0; j < 16; j++) printf("  %2d ", right_seg[r*16+j]>>3);
            //printf("\n");
        }
        fprintf(f, "%s>%02X%02X%s",
            (i&15) == 0 ? "       DATA " : "",
            l, r,
            (i&15) == 15 ? "\n" : ",");

    }

    fprintf(f, "LeftStrips:   ; %d bytes\n", left_count * 16);
    for (i = 0; i < left_count; i++) {
        char tmp[20];
        sprintf(tmp, " ; %x\n", i);
        for (j = 0; j < 16; j+=2) {
            fprintf(f, "%s>%02x%02x%s", 
                j == 0 ? "       DATA " : "",
                left_seg[i*16+j], left_seg[i*16+j+1],
                j == 14 ? tmp : ",");
        }
    }

    fprintf(f, "RightStrips:   ; %d bytes\n", right_count * 16);
    for (i = 0; i < right_count; i++) {
        char tmp[20];
        sprintf(tmp, " ; %x\n", i);
        for (j = 0; j < 16; j+=2) {
            fprintf(f, "%s>%02x%02x%s", 
                j == 0 ? "       DATA " : "",
                right_seg[i*16+j], right_seg[i*16+j+1],
                j == 14 ? tmp : ",");
        }
    }

}

static void calculate_internal(int seg)
{
    int j;
    unsigned char prev[32] = {};
    unsigned char buf1[32] = {};
    unsigned char buf2[32] = {};

    seg_in[seg].use0 = calloc(32,32); // rows 0-4
    seg_in[seg].use1 = calloc(32,32); // rows 0-8
    seg_in[seg].use2 = calloc(32,32); // rows 4-12
    seg_in[seg].use3 = calloc(32,32); // rows 8-16
    seg_in[seg].use4 = calloc(32,32); // rows 12-16

    for (j = 0; j < 8; j++) {
        decode_segment(seg, j, buf1, buf2);
        if (j < 4) {
            if (j != 0)
                calculate_transitions(prev, buf1, seg_in[seg].use0);
            calculate_transitions(buf1, buf2, seg_in[seg].use0);
        }
        if (j != 0)
            calculate_transitions(prev, buf1, seg_in[seg].use1);
        calculate_transitions(buf1, buf2, seg_in[seg].use1);
        memcpy(prev, buf2, 32);
    }   

    decode_segment(seg, 3, buf1, prev);
    for (j = 4; j < 12; j++) {
        decode_segment(seg, j, buf1, buf2);
        calculate_transitions(prev, buf1, j == 4 ? seg_in[seg].use1 : seg_in[seg].use2);
        calculate_transitions(buf1, buf2, seg_in[seg].use2);
        memcpy(prev, buf2, 32);
    }
    
    decode_segment(seg, 7, buf1, prev);
    for (j = 8; j < 16; j++) {
        decode_segment(seg, j, buf1, buf2);
        calculate_transitions(prev, buf1, j == 8 ? seg_in[seg].use2 : seg_in[seg].use3);
        calculate_transitions(buf1, buf2, seg_in[seg].use3);
        if (j >= 12) {
            calculate_transitions(prev, buf1, seg_in[seg].use4);
            calculate_transitions(buf1, buf2, seg_in[seg].use4);
        }                
        memcpy(prev, buf2, 32);
    }
}


int main(int argc, char *argv[])
{
    int i, j, seg;
    load_segments();
    //print_segments();
    //print_segment(15);

    FILE *f = fopen("bnp_seg.asm", "w");
    fprintf(f, "TileGroups:  ; character transition sets  %lu bytes\n", (ARRAY_SIZE(s)-1)*32*2);
    for (j = 1; j < ARRAY_SIZE(s); j++) {
        fprintf(f, "       DATA");
        for (i = 0; i < 64; i += 2) {
            fprintf(f, "%c>%02X%02X", i==0?' ':',', s[j][i], s[j][i+1]);
        }
        fprintf(f, "\n");
    }
    fclose(f);

    f = fopen("bnp_data.asm", "w");
    fprintf(f, "Track:  ; track segment data  %lu bytes\n", (ARRAY_SIZE(patterns)));
    for (j = 0; j < ARRAY_SIZE(patterns); j++) {
        fprintf(f, "%s>%02X%s%s",
            (j&7)==0?"       BYTE ":",",
            patterns[j],
            (j&7)==7?"\n":"",
            (j&63)==63?"\n":"");
    }

    // calculate transitions between two segments in the track patterns
    int pattern_count = 0;
    for (i = 0; i < sizeof(patterns); i++) {
        if ((i & 63) == 63)
            continue; // end of pattern
        int last = patterns[i];
        int seg = patterns[i+1];

        if (seg_tr[last*16+seg].use)
            continue; // transitions already calculated

        pattern_count++;
        unsigned char prev[32] = {};
        unsigned char buf1[32] = {};
        unsigned char buf2[32] = {};
        char *use;
        use = calloc(32,32);
        seg_tr[last*16+seg].use = use;

    	// calculate internal tile groups first
        if (seg_in[seg].use1 == NULL) {
            calculate_internal(seg);
        }

    	// calculate tile groups
        decode_segment(seg, 11, buf1, prev);
    	for (j = 12; j < 16; j++) {
	    decode_segment(seg, j, buf1, buf2);
            calculate_transitions(prev, buf1, use);
            calculate_transitions(buf1, buf2, use);
            memcpy(prev, buf2, 32);
        }

    	// pattern transition strip is from bottom of current frame to top of last frame
    	for (j = 0; j < 4; j++) {
            decode_segment(last, j, buf1, buf2);
            calculate_transitions(prev, buf1, use);
    	    calculate_transitions(buf1, buf2, use);
            memcpy(prev, buf2, 32);
        }
        //decode_segment(last, 0, buf1, buf2);
        //calculate_transitions(prev, buf1, use);

        //printf("seg %d->%d  ", last, seg);
        //seg_tr[last*16+seg].tileset = get_tilegroup(use);
        //printf("\t%04x\n", seg_tr[last*16+seg].tileset);
    }

    calculate_internal(16); // finish line

    int transitions = 0;
    for (i = 0; i < 32*32; i++) {
        int count = 0;
        for (j = 0; j < 19*16; j++) {
            char *use = get_use(j);
            if (use && use[i]) {
                count++;
                //if (count++ == 0)
                //    printf("%s,%s:", short_names[i>>5], short_names[i&31]);
                //printf(" %02x", j);
                
            }
        }
        if (count) {
            transition_counts[i] = count;
            //printf("\n");
            transitions++;
        }
    }
    printf("%d transitions, %d patterns\n", transitions, pattern_count);

    for (i = 0; i < 32*32; i++) {
        //int tmp[32*32];
        if (!transition_counts[i]) continue;
        //memcpy(tmp, transition_counts, sizeof(tmp));
        char tmp[32];
        memset(tmp, 1, 32);

        printf("%s,%s:", short_names[i>>5], short_names[i&31]);
        for (j = 0; j < 19*16; j++) {
            char *use = get_use(j);
            if (use && use[i]) {
                int k;
                for (k = 0; k<32; k++) {
                    int n = (s[1][k*2] << 5) | s[1][k*2+1];
                    if (n == i) {
                        memset(tmp, 0, 32);
                        break;
                    }

                    if (use[n])
                        tmp[k] = 0;
                }
            }
        }
        for (j = 0; j < 32; j++) {
            if (tmp[j])
                printf(" %d", j);
        }
        printf("\n");

    }


#if 0
    for (i = 0; i < 32*32; i++) {
        if (sets[i] == 0) continue;
        for (j = i+1; j < 32*32; j++) {
            if (sets[j] == 0) continue;
            if (sets[i] & sets[j])
                continue;
            printf("%s,%s and %s,%s are distinct\n", short_names[i>>5], short_names[i&31], short_names[j>>5], short_names[j&31]);
        }
    }
#endif


    
#if 1
    fprintf(f, "SegmentTransitionsInternal:  ; internal segment transitions %d bytes \n", 16*3*2);
    for (j = 0; j < ARRAY_SIZE(seg_in); j++) {
        seg_in[j].tileset1 = get_tilegroup(seg_in[j].use1);
        seg_in[j].tileset2 = get_constrained_tilegroup(
            seg_in[j].use1, seg_in[j].tileset1,
            seg_in[j].use2, 
            NULL, 0);
        seg_in[j].tileset3 = get_constrained_tilegroup(
            seg_in[j].use2, seg_in[j].tileset2,
            seg_in[j].use3, 
            NULL, 0);
        printf("%x %d:%04x %d:%04x %d:%04x\n", j,
            count_use(seg_in[j].use1), seg_in[j].tileset1,
            count_use(seg_in[j].use2), seg_in[j].tileset2,
            count_use(seg_in[j].use3), seg_in[j].tileset3);
        for (i = 0; i < 16; i += 4) {
            if ((seg_in[j].tileset2 & (0xf << i)) == 0)
                seg_in[j].tileset2 |= seg_in[j].tileset3 & (0xf << i);
            if ((seg_in[j].tileset1 & (0xf << i)) == 0)
                seg_in[j].tileset1 |= seg_in[j].tileset2 & (0xf << i);
        }
        fprintf(f, "       DATA >%04X,>%04X,>%04X\n", seg_in[j].tileset1, seg_in[j].tileset2, seg_in[j].tileset3);
    }
    fprintf(f, "SegmentTransitions:  ; segment transitions  %d bytes\n", 16*16*2);
    for (j = 0; j < 16*16; j++) {
        int tr = 0;
        if (seg_tr[j].use) {
            print_tilegroup(seg_in[j&15].use4, seg_in[j&15].tileset3);
            printf("^\n");
            tr = get_constrained_tilegroup(
                seg_in[j&15].use4, seg_in[j&15].tileset3, 
                seg_tr[j].use, 
                seg_in[j>>4].use0, seg_in[j>>4].tileset1);
            printf("v\n");
            print_tilegroup(seg_in[j>>4].use0, seg_in[j>>4].tileset1);
            seg_tr[j].tileset = tr;
            printf("%02x %d:%04x %s\n", j, count_use(seg_tr[j].use), tr, count_use(seg_tr[j].use) > 32 ?"!":"");
        }
        fprintf(f, "%s>%04X%s",
            (j&15)==0 ? "       DATA " : ",",
            tr,
            (j&15)==15 ? "\n" : "");
    }
#endif

    generate_segments(f);
    printf("left: %d  right: %d    %d bytes\n", left_count, right_count, (left_count + right_count) * 16);


#if 0
    // merge overlapping segments up to 32
    while (1) {
        int best = 0, best_i, best_j;

        for (i = 0; i < 16*19; i++) {
            char *use1 = get_use(i);
            if (!use1)
                continue;
            for (j = 0; j < 16*19; j++) {
                char *use2 = get_use(j);
                if (!use2 || use1 == use2)
                    continue;
                int n = common(use1, use2);
                if ((count_use(use1) - n) + (count_use(use2) - n) + n > 32)
                    continue; // too many to merge
                if (n > best) {
                    best = n;
                    best_i = i;
                    best_j = j;
                }
            }
        }
        if (best == 0)
            break;
        char *tmp = calloc(32,32), 
            *use_i = get_use(best_i),
            *use_j = get_use(best_j);
        printf("merging %02x:%d:%p and %02x:%d:%p have %d in common\n",
            best_i, count_use(use_i), use_i,
            best_j, count_use(use_j), use_j, best);
        // merge
        for (i = 0; i < 32*32; i++)
            tmp[i] = use_i[i] || use_j[i];
        // update all existing pointers to new tmp
        for (i = 0; i < 16*19; i++) {
            if (i >= 16*18) {
                if (seg_in[i&15].use3 == use_i || seg_in[i&15].use3 == use_j)
                    seg_in[i&15].use3 = tmp;
            } else if (i >= 16*17) {
                if (seg_in[i&15].use2 == use_i || seg_in[i&15].use2 == use_j)
                    seg_in[i&15].use2 = tmp;
            } else if (i >= 16*16) {
                if (seg_in[i&15].use1 == use_i || seg_in[i&15].use1 == use_j)
                    seg_in[i&15].use1 = tmp;
            } else if (seg_tr[i].use == use_i || seg_tr[i].use == use_j)
                seg_tr[i].use = tmp;
        }
        // free old ones
        free(use_i);
        free(use_j);
    }


    char used[256] = {};
    pattern_count = 0;
    for (i = 0; i < 256; i++) {
        char *use_i = get_use(i);
        if (!use_i || used[i])
            continue;
        printf("%02x", i);
        pattern_count++;
        used[i] = 1;
        for (j = 0; j < 16*19; j++) {
            char *use_j = get_use(j);
            if (!use_j || used[j])
                continue;
            if (use_i == use_j) {
                printf(" %02x", j);
                pattern_count++;
                used[j] = 1;
            }
        }
        printf("  count=%d\n", count_use(use_i));
        for (j = 0; j < 32*32; j++) {
            if (use_i[j]) {
                printf("%s,%s ", short_names[j>>5], short_names[j&31]);
            }
        }
        printf("\n");
    }
    printf("pattern_count=%d\n", pattern_count); // sanity check to see if it's the same as before
#endif

    fclose(f);



    return 0;
}
