#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "../legend/music/player.h"

// quarter-beat is 6 frames (24 frames per beat = 150 BPM)
#define QBEAT 6

// returns divider in range of 1-1024 (or higher)
static int getnote(float freq)
{
        int f = roundf(111860.8 / freq);
        return f;
}


enum {
        GEN1 =  0x80,
        VOL1 =  0x90,
        GEN2 =  0xA0,
        VOL2 =  0xB0,
        GEN3 =  0xC0,
        VOL3 =  0xD0,
        NOISE = 0xE0,
        VOL4 =  0xF0,
};

enum {
        Periodic_6991,  // divider = 512
        Periodic_3496,  // divider = 1024
        Periodic_1748,  // divider = 2048
        Periodic_GEN3,
        White_6991, // A8  divider = 16
        White_3496, // A7  divider = 32
        White_1748, // A6  divider = 64
        White_GEN3,
};

enum {
	B7 = 0x039,
	A7 = 0x040,
	G7 = 0x047,
	F7 = 0x050,
        E7 = 0x055,
        D7 = 0x05f,
        C7 = 0x06b,
        B6 = 0x071,
        A6 = 0x07f,
        G6 = 0x08f,
        F6 = 0x0a0,

        E6 = 0x0aa,
        D6 = 0x0be,
        C6 = 0x0d6,
        B5 = 0x0e2,
        A5 = 0x0fe,
        G5 = 0x11d,
        F5 = 0x140,
        E5 = 0x153,        
        D5 = 0x17d,
        C5 = 0x1ac,
        B4 = 0x1c5,
        A4 = 0x1fc,
        G4 = 0x23b,
        F4 = 0x281,
        E4 = 0x2a7,
        D4 = 0x2fa,
        C4 = 0x357,
};

static void vsyncs(int n)
{
        int i;
        for (i = 0; i < n; i++)
                vsync();
}

static void note(int gen, int n)
{
	static int last[4] = {-1,-1,-1,-1};
	
	if (last[(gen>>5)&3] == n)
		return;
	last[(gen>>5)&3] = n;
	
	
    snd(gen | (n & 0x0f));
    snd(n >> 4);
}

static void vol(int gen, int n)
{
	static int last[4] = {-1,-1,-1,-1};

	if (last[(gen>>5)&3] == n)
		return;
	last[(gen>>5)&3] = n;

    snd(gen | (n & 0x0f));
}


int chord_vol = 4;

static void dur(int n)
{
        int i, v;
        for (i = 1; i <= n; i++) {
                v = (int)(chord_vol+log(i)*(15-chord_vol)/log(200));
                vol(VOL1, v);
                vol(VOL2, v);
                vsync();
        }
}

static void dur3(int n)
{
        int i, v;
        for (i = 1; i <= n; i++) {
                v = (int)(chord_vol+log(i)*(15-chord_vol)/log(200));
                vol(VOL3, v);
                vsync();
        }
}

static void dur4(int n)
{
        int i, v;
        for (i = 1; i <= n; i++) {
                v = (int)(chord_vol+log(i)*(15-chord_vol)/log(200));
                vol(VOL4, v);
                vsync();
        }
}


static void chord(int x, int y, int n)
{
        note(GEN1, x);
        note(GEN2, y);
        dur(n*QBEAT);

}
static void rest(int n)
{
        vol(VOL1, 15);
        vol(VOL2, 15);
        vsyncs(n*QBEAT);
}

static void chord3(int x, int y, int n)
{
        note(GEN3, x/15.0);
        //note(GEN2, y);
        dur4(n*QBEAT);
}

int main(int argc, char *argv[])
{
        int i, v, v_ = 15;

        if (strcmp(argv[1], "-d") == 0) {
                dump_soundlist = 1;
                argv++;
                argc--;
        }

        switch (atoi(argv[1]))
        {
        default:
                break;
        case 1: // splash
		vol(VOL3, 15);
                snd(NOISE | White_GEN3);
                //note(GEN3, 0x5);
                for (i = 0; i < 64; i++) {
                        if ((i & 3) == 0) {
                                note(GEN3, i < 16 ? 2 + (i>>1) : 16 - (i>>2));  
                                
                        }
			vol(VOL4, (int)(2+log(i+2*sin(i*.5))*13/log(512)));
                        vsync();
                }
		vol(VOL4, 15);
		vsync();
                break;

        case 2: // crash
		    vol(VOL3, 15);
#if 0
                snd(NOISE | 0x07);
                for (i = 0; i < 64; i++) {
                        note(GEN3, lrint(196+log(i)*100/log(15)));
                        //snd(VOL4 | lrint();
                        //vol(VOL4, (log(i+2*(i&7))*4/log(32)));
			            vol(VOL4, 5+2*sin(i));
                        vsync();
                }
                for (i = 0; i < 15; i++) {
                        vol(VOL4, (int)(3+log(i+1)*12/log(32)));
                        vsync();
                }
#else
            snd(NOISE | 0x07);
            note(GEN3, 0x10);
            for (i = 0; i < 128; i++) {
		    	if ((i & 3) == 0 && i <= 64)
			    	note(GEN3, 0x30+log(i+1)*0x10/log(128));
			    //note(GEN3, 0x10+i);
                double d = (log(i*.025+1)*10);
                //fprintf(stderr, "%f\n", d);
                vol(VOL4, i/8+
                    //(i < 64 ? 2 : -2)+
                    0*sin(d));
                vsync();
            }
            //snd(NOISE | 0x05);
		
		
            if (0)for (i = 0; i < 63; i++) {
                int chord_vol = 0;
                //int v = (int)(chord_vol+log(63-i)*(15-chord_vol)/log(200));
                int v = i * 15 / 64;
                note(GEN3, 250+i);

                vol(VOL4, v);
                vsync();
            }
		
#endif
    		vol(VOL4, 15);
	    	vsync();
            break;
		
		
        case 3: // intro
		chord_vol = 2;
                chord(G5,E5,2); chord(G5,E5,1); chord(G5,E5,1); chord(G5,E5,5); rest(1); chord(A5,F5,2); chord(G5,E5,2); chord(A5,F5,2); 
                chord(G5,E5,2); chord(G5,E5,1); chord(G5,E5,1); chord(G5,E5,3); rest(1); chord(G5,E5,2); rest(2);
                rest(4);
		
        		// don't terminate
        		if (duration) putchar(duration);
		
                return 0;
		
        case 4: // driving
		chord_vol = 4; // quieter while driving
                chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(F5,D5,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(G4,E4,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(E5,C5,2); chord(F5,D5,2);
                chord(G5,E5,2); chord(G5,E5,2); chord(C6,A5,2); chord(G5,E5,2); chord(E5,C5,2); chord(G5,E5,2); chord(C6,A5,3); rest(1);

                chord(C6,A5,2); chord(A5,F5,3); rest(1); chord(F5,D5,2); chord(B5,G5,2); chord(G5,E5,3); rest(1); chord(E5,C5,2);
                chord(A5,F5,2); chord(F5,D5,3); rest(1); chord(D5,B4,2); chord(G5,E5,2); chord(E5,C5,3); rest(1); chord(C5,A4,2);
                chord(C5,A4,2); chord(E5,C5,2); chord(G5,E5,2); chord(G5,E5,2); chord(D5,B4,2); chord(F5,D5,2); chord(A5,F5,2); chord(A5,F5,2);
                chord(G5,E5,2); chord(B5,G5,2); chord(D6,B5,2); chord(E6,C6,2); chord(C6,A5,6); rest(2);

                chord(C6,A5,2); rest(1); chord(A5,F5,1); chord(F5,D5,3); rest(1); chord(B5,G5,2); rest(1); chord(G5,E5,1); chord(E5,C5,3); rest(1);
                chord(E5,C5,2); rest(1); chord(F5,D5,1); chord(G5,E5,3); rest(1); chord(G4,E4,1); chord(A4,F4,1); chord(B4,G4,1); chord(C5,A4,1); chord(D5,B4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1);
                chord(F5,D5,2); rest(1); chord(G5,E5,1); chord(A5,F5,3); rest(1); chord(A4,F4,1); chord(B4,G4,1); chord(C5,A4,1); chord(D5,B4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1); chord(A5,F5,1);
                chord(E5,C5,2); chord(E5,C5,2); chord(F5,D5,2); chord(F5,D5,2); chord(G5,E5,2); chord(G5,E5,2); chord(C5,A4,3); rest(1);

                chord(F5,D5,2); chord(D5,B4,2); chord(E5,C5,2); chord(F5,D5,2); chord(E5,C5,2); chord(D5,B4,2); chord(C5,A4,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(F5,D5,2); chord(E5,C5,2); chord(D5,B4,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(E5,C5,2); chord(D5,B4,2); chord(C5,A4,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1); chord(A5,F5,1); chord(B5,G5,3); rest(1);

                chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(F5,D5,2); chord(E5,C5,2); chord(D5,B4,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(B4,G4,1); chord(A4,F4,1); chord(B4,G4,1); chord(C5,A4,1); chord(D5,B4,3); rest(1);
                chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(D5,B4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1); chord(A5,F5,3); rest(1);
                chord(B5,G5,2); chord(B5,G5,1); chord(B5,G5,1); chord(G5,E5,2); chord(G5,E5,1); chord(G5,E5,1); chord(C6,A5,1); chord(B5,G5,1); chord(A5,F5,1); chord(B5,G5,1); chord(C6,A5,3); rest(1);

                chord(C6,A5,2); chord(A5,F5,2); chord(F5,D5,2); chord(G5,E5,2); chord(B5,G5,2); chord(G5,E5,2); chord(E5,C5,2); chord(F5,D5,2);
                chord(A5,F5,2); chord(F5,D5,2); chord(D5,B4,2); chord(E5,C5,2); chord(G5,E5,2); chord(E5,C5,2); chord(C5,A4,3); rest(1);
                chord(C6,A5,3); rest(1); chord(A5,F5,2); chord(F5,D5,2); chord(B5,G5,2); chord(B5,G5,2); chord(G5,E5,2); chord(E5,C5,2);
                chord(A5,F5,3); rest(1); chord(F5,D5,2); chord(D5,B4,2); chord(G5,E5,2); chord(G5,E5,2); chord(E5,C5,2); chord(C5,A4,2);

                // repeat?
                break;
		
        case 5: // finish
		chord_vol = 1;
                chord(G5,E5,10); chord(A5,F5,2); chord(G5,E5,2); chord(F5,D5,2);
                chord(G5,E5,6); chord(F5,C5,2); chord(G5,D5,2); chord(A5,F5,2); chord(G5,E5,2); chord(F5,D5,2);
                chord(G5,E5,16);

                vol(VOL1,15);
                vol(VOL2,15);
                vsync();

                break;
	case 6: // jumping
		vol(VOL4, 15);
		int i = C4;
		note(GEN3, A5); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, B5); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, C6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, D6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, E6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, F6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, G6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, A6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, B6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, C7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, D7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, E7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, F7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, G7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, A7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, B7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);

		vol(VOL3, 15); vsyncs(4);
		note(GEN3, B6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, G6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, E6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, C6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);

		vol(VOL3, 15); vsyncs(4);
		note(GEN3, B6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, G6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, E6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, C6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);

		vol(VOL3, 15); vsyncs(4);
		note(GEN3, B6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, G6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, E6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);
		note(GEN3, C6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 15); vsyncs(2);

		vol(VOL3, 15); vsyncs(60);


		break;
	case 7: // beep
		vol(VOL4, 15);
		note(GEN3, F7); vol(VOL3, 7);
		for (i = 0; i < 16; i++)
			vsync();
		vol(VOL3, 15); 	vsync();
		break;

	case 8: // smash
		vol(VOL3, 15);
		snd(NOISE | White_3496);
		vol(VOL4, 0); vsyncs(4);
		vol(VOL4, 1); vsyncs(4);
		vol(VOL4, 2); vsyncs(4);
		vol(VOL4, 3); vsyncs(4);
		vol(VOL4, 4); vsyncs(4);
		vol(VOL4, 5); vsyncs(4);

		snd(NOISE | White_1748);
		vol(VOL4, 6); vsyncs(4);
		vol(VOL4, 7); vsyncs(4);
		vol(VOL4, 8); vsyncs(4);
		vol(VOL4, 9); vsyncs(4);
		vol(VOL4, 10); vsyncs(4);
		vol(VOL4, 15); vsync();

                break;
 	case 9: // bounce
		vol(VOL4, 15);
		note(GEN3, E4); vol(VOL3, 2); vsyncs(2);
		note(GEN3, F4); vol(VOL3, 3); vsyncs(2);
		note(GEN3, G4); vol(VOL3, 4); vsyncs(2);
		note(GEN3, A4); vol(VOL3, 5); vsyncs(2);
		note(GEN3, G4); vol(VOL3, 6); vsyncs(2);
		note(GEN3, F4); vol(VOL3, 7); vsyncs(2);
		note(GEN3, E4); vol(VOL3, 8); vsyncs(2);
		note(GEN3, F4); vol(VOL3, 9); vsyncs(2);
		vol(VOL3, 15); vsync();
 		break;

	case 10: // dump truck
		vol(VOL4, 15);
		note(GEN3, D7); vol(VOL3, 5); vsyncs(2); vol(VOL3, 10); vsyncs(2);
		note(GEN3, B6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 10); vsyncs(2);
		note(GEN3, G6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 10); vsyncs(2);
		note(GEN3, E6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 10); vsyncs(2);
		note(GEN3, C6); vol(VOL3, 5); vsyncs(2); vol(VOL3, 10); vsyncs(2);
		vol(VOL3, 15); vsync();
		break;
		
	case 11: // extra life
		vol(VOL4, 15);
		note(GEN3, C7); dur3(8);
		note(GEN3, G7); dur3(4);
		note(GEN3, E7); dur3(4);
		note(GEN3, G7); dur3(4);
		note(GEN3, E7); dur3(4);
		note(GEN3, G7); dur3(8);
		vol(VOL3, 15); vsync();
		break;

	case 12: // play around with notes
		chord_vol = 4; // quieter while driving
                chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(F5,D5,3); rest(1);

		chord_vol = 2;
		snd(NOISE | Periodic_GEN3);
		vol(VOL3, 15);
		{	void (*chord)(int,int,int) = chord3;
		
			chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(F5,D5,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(G4,E4,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,2); chord(C5,A4,2); chord(C5,A4,2); chord(D5,B4,2); chord(D5,B4,2); chord(E5,C5,2); chord(F5,D5,2);
                	chord(G5,E5,2); chord(G5,E5,2); chord(C6,A5,2); chord(G5,E5,2); chord(E5,C5,2); chord(G5,E5,2); chord(C6,A5,3); rest(1);

                	chord(C6,A5,2); chord(A5,F5,3); rest(1); chord(F5,D5,2); chord(B5,G5,2); chord(G5,E5,3); rest(1); chord(E5,C5,2);
                	chord(A5,F5,2); chord(F5,D5,3); rest(1); chord(D5,B4,2); chord(G5,E5,2); chord(E5,C5,3); rest(1); chord(C5,A4,2);
                	chord(C5,A4,2); chord(E5,C5,2); chord(G5,E5,2); chord(G5,E5,2); chord(D5,B4,2); chord(F5,D5,2); chord(A5,F5,2); chord(A5,F5,2);
                	chord(G5,E5,2); chord(B5,G5,2); chord(D6,B5,2); chord(E6,C6,2); chord(C6,A5,6); rest(2);

                	chord(C6,A5,2); rest(1); chord(A5,F5,1); chord(F5,D5,3); rest(1); chord(B5,G5,2); rest(1); chord(G5,E5,1); chord(E5,C5,3); rest(1);
                	chord(E5,C5,2); rest(1); chord(F5,D5,1); chord(G5,E5,3); rest(1); chord(G4,E4,1); chord(A4,F4,1); chord(B4,G4,1); chord(C5,A4,1); chord(D5,B4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1);
                	chord(F5,D5,2); rest(1); chord(G5,E5,1); chord(A5,F5,3); rest(1); chord(A4,F4,1); chord(B4,G4,1); chord(C5,A4,1); chord(D5,B4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1); chord(A5,F5,1);
                	chord(E5,C5,2); chord(E5,C5,2); chord(F5,D5,2); chord(F5,D5,2); chord(G5,E5,2); chord(G5,E5,2); chord(C5,A4,3); rest(1);

                	chord(F5,D5,2); chord(D5,B4,2); chord(E5,C5,2); chord(F5,D5,2); chord(E5,C5,2); chord(D5,B4,2); chord(C5,A4,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(F5,D5,2); chord(E5,C5,2); chord(D5,B4,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(E5,C5,2); chord(D5,B4,2); chord(C5,A4,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1); chord(A5,F5,1); chord(B5,G5,3); rest(1);

                	chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(F5,D5,2); chord(E5,C5,2); chord(D5,B4,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(B4,G4,1); chord(A4,F4,1); chord(B4,G4,1); chord(C5,A4,1); chord(D5,B4,3); rest(1);
                	chord(E5,C5,2); chord(E5,C5,1); chord(E5,C5,1); chord(C5,A4,2); chord(C5,A4,1); chord(C5,A4,1); chord(D5,B4,1); chord(E5,C5,1); chord(F5,D5,1); chord(G5,E5,1); chord(A5,F5,3); rest(1);
                	chord(B5,G5,2); chord(B5,G5,1); chord(B5,G5,1); chord(G5,E5,2); chord(G5,E5,1); chord(G5,E5,1); chord(C6,A5,1); chord(B5,G5,1); chord(A5,F5,1); chord(B5,G5,1); chord(C6,A5,3); rest(1);

                	chord(C6,A5,2); chord(A5,F5,2); chord(F5,D5,2); chord(G5,E5,2); chord(B5,G5,2); chord(G5,E5,2); chord(E5,C5,2); chord(F5,D5,2);
                	chord(A5,F5,2); chord(F5,D5,2); chord(D5,B4,2); chord(E5,C5,2); chord(G5,E5,2); chord(E5,C5,2); chord(C5,A4,3); rest(1);
                	chord(C6,A5,3); rest(1); chord(A5,F5,2); chord(F5,D5,2); chord(B5,G5,2); chord(B5,G5,2); chord(G5,E5,2); chord(E5,C5,2);
                	chord(A5,F5,3); rest(1); chord(F5,D5,2); chord(D5,B4,2); chord(G5,E5,2); chord(G5,E5,2); chord(E5,C5,2); chord(C5,A4,2);
		}
		
		break;
        case 13:
		snd(NOISE | Periodic_GEN3);
		vol(VOL3, 15);
		vol(VOL4, 8);

        if (0) for (i = 0; i <= 25; i++) {
        	float f = (500.0*(25-i) + 1000.0*(i))/25;
        	fprintf(stderr, "%f\n", f);
		    note(GEN3, getnote(f));
		    vsyncs(10);
		}
        for (i = 0; i <= 4; i++) {
        	float f = (500.0*(4-i) + 700.0*(i))/4;
        	fprintf(stderr, "%f\n", f);
		    note(GEN3, getnote(f));
		    vsyncs(4);
		}
        for (i = 0; i <= 5; i++) {
        	float f = (600.0*(5-i) + 800.0*(i))/5;
        	fprintf(stderr, "%f\n", f);
		    note(GEN3, getnote(f));
		    vsyncs(4);
		}
        for (i = 0; i <= 6; i++) {
        	float f = (700.0*(6-i) + 900.0*(i))/6;
        	fprintf(stderr, "%f\n", f);
		    note(GEN3, getnote(f));
		    vsyncs(4);
		}
        for (i = 0; i <= 7; i++) {
        	float f = (800.0*(7-i) + 950.0*(i))/7;
        	fprintf(stderr, "%f\n", f);
		    note(GEN3, getnote(f));
		    vsyncs(4);
		}


        break;
        }


        terminate_soundlist();
        return 0;
}
