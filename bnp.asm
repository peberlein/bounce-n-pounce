* Bounce'n'Pounce
* Copyright 2020 Pete Eberlein

       SAVE >6000,>8000  ; Assembler writes full 8K banks

VDPWD  EQU  >8C00             ; VDP write data
VDPWA  EQU  >8C02             ; VDP set read/write address
VDPRD  EQU  >8800             ; VDP read data (don't forget NOP after address)
VDPSTA EQU  >8802             ; VDP status
VDPWM  EQU  >4000             ; VDP address write mask
VDPRM  EQU  >8000             ; VDP address register mask


* VDP RAM layout
CLRTAB EQU >0000  ; Char color table (6KB = >1800)
SCRTAB EQU >1800  ; Screen table (768 bytes = >300)
SPRTAB EQU >1B00  ; Sprite list table (128 bytes)
PATTAB EQU >2000  ; Char pattern table (6KB = >1800)
SPRPAT EQU >3800  ; Sprite patterns (>800 bytes)

WRKSP  EQU  >8300             ; Workspace memory in fast RAM
R0LB   EQU  WRKSP+1           ; Register low byte address
R1LB   EQU  WRKSP+3           ; Register low byte address
R2LB   EQU  WRKSP+5           ; Register low byte address
R3LB   EQU  WRKSP+7           ; Register low byte address
R4LB   EQU  WRKSP+9           ; Register low byte address
R5LB   EQU  WRKSP+11          ; Register low byte address
R6LB   EQU  WRKSP+13          ; Register low byte address
R7LB   EQU  WRKSP+15          ; Register low byte address
R8LB   EQU  WRKSP+17          ; Register low byte address
R9LB   EQU  WRKSP+19          ; Register low byte address
R10LB  EQU  WRKSP+21          ; Register low byte address
R11LB  EQU  WRKSP+23          ; Register low byte address
R12LB  EQU  WRKSP+25          ; Register low byte address
R13LB  EQU  WRKSP+27          ; Register low byte address


* Fast RAM layout
* 8300:831F register workspace
* 8320:832F 16-byte scratch buffer
* 8330:8373 DrawStrip function
* 8374:837F program variables
* 8380:83BF 32 line tile strip buffer (64 bytes)
* 83C0:83FF program variables

       DORG WRKSP+>20
       
* Address >8320
SCRBUF DATA 0,0,0,0,0,0,0,0  ; 16 byte buffer used for char transitions
* Address >8330
DRWSCR DATA 0,0,0,0,0,0,0,0  ; Draw screen function
       DATA 0,0,0,0,0,0,0,0
       DATA 0,0,0,0,0,0,0,0
       DATA 0,0,0,0,0,0,0,0
       DATA 0,0
CURPAT DATA 0                ; Current pattern table for this season
CURTRK DATA 0                ; Pointer to current segment in Track
TGNEW  DATA 0                ; Next top tile group calculated by SetNewTG
TG0    DATA 0                ; Tile group for middle third of screen
TG1    DATA 0                ; Tile group for middle third of screen
TG2    DATA 0                ; Tile group for bottom third of screen
* Address >8380
SEGBUF DATA 0,0,0,0,0,0,0,0  ; Tile Strip Buffer (32 lines x 2 bytes) (must be aligned to 128 bytes)
       DATA 0,0,0,0,0,0,0,0
       DATA 0,0,0,0,0,0,0,0
       DATA 0,0,0,0,0,0,0,0
SEGMSK EQU ~>0040     ; Tile Strip Buffer mask, for wrap-around at 64 bytes
* Address >83C0
MAXSPD EQU 148
MINSPD EQU 48
SPEED  DATA 0         ; speed  48-148
DELTA  BYTE 0         ; distance screen scrolled this frame (for moving cars relative to the road)
COUNTR BYTE 0         ; must be in the same word as DELTA
CAR    DATA 0,0       ; BBDJ 00XX  BB=bounce D=direction(2 bits) J=jumping or crashing
ECAR1  DATA 0,0       ; BBDC YYXX  C=cartype
ECAR2  DATA 0,0       ; BBDC YYXX  C=cartype
ECAR3  DATA 0,0       ; BBDC YYXX  C=cartype
ECAR4  DATA 0,0       ; BBDC YYXX  C=cartype
ROCK1  DATA 0         ; YYXX  (inactive when over >C000)
ROCK2  DATA 0         ; YYXX
ROCK3  DATA 0         ; YYXX
ROCK4  DATA 0         ; YYXX

SCOREH DATA 0         ; Score, high 16-bits
SCOREL DATA 0         ; Score, low 16-bits
HSCORE DATA 0,0       ; High score, 32-bits
LIVES  BYTE 0         ; Lives, 8-bits
BONUSP BYTE 0         ; Bonus points 300 to 500, divided by 4, increase each season
SMASHD BYTE 0         ; Number of cars smashed during this season
CURSEG BYTE 0         ; Current segment
EXLIFE DATA 0         ; Accumulate points, gives extra life and wraps at 30K (until total score exceeds 1M)
RAND16 DATA 0         ; random seed (must be nonzero)
MUSIC  DATA 0         ; Music sound list pointer
MUSICC BYTE 0         ; Music counter
SOUNDC BYTE 0         ; Sound counter
SOUND  DATA 0         ; Sound effect list pointer
KEYS   DATA 0
MOTOR  BYTE 0,0       ; Motor sound effect
MOTORA BYTE 0
MOTORB BYTE 0
MOTORV BYTE 0,0,0,0   ; Volume byte, repeat byte zero, zero address


RockRelativeToCar EQU ROCK1-(((ECAR1+2)/2)&>7FFF)


       AORG >6000         ; Cartridge header in all banks

*******************************************************************
       BANK ALL
*******************************************************************

HEADER
       BYTE >AA     ; Standard header
       BYTE >01     ; Version number 1
       BYTE >01     ; Number of programs (optional)
       BYTE >00     ; Reserved (for FG99 this can be G,R,or X)
       DATA >0000   ; Pointer to power-up list
       DATA PRGLST  ; Pointer to program list
       DATA >0000   ; Pointer to DSR list
*      DATA >0000   ; Pointer to subprogram list (overlaps PRGLST)

PRGLST DATA >0000   ; Next program list entry
       DATA START   ; Program address
*      BYTE         ; Length of name (STRI prepends the length byte)
       STRI 'BOUNCE''N''POUNCE'
       EVEN


; DONE finish line
; DONE player crash and splash animation
; DONE new cars smash animation
; DONE crashed cars score on screen
; DONE smash cars by landing on them
; DONE scoring and high score and extra lives, bonus cars at end
; DONE bonus score for landing on islands
; DONE cars random movement
; TODO skull car targets player
; DONE dump truck: slow (black) and fast (light blue) states
; DONE dump truck rocks and collisions
; DONE music (intro, driving, finish line)
; DONE sounds (ex-life, dump truck drop, engine rev)
; DONE sounds (boink, smash, crash, splash, jumping, warning beep)
; TODO cheat mode: unlimited lives, no scoring (up up down down left right left right fire)
; DONE sprite limit flickering
; TODO two player mode
; TODO start car in middle of bridge



BANK0  EQU >6000
BANK1  EQU >6002
BANK2  EQU >6004
BANK3  EQU >6006


BankSwitch1:
       CLR @BANK1
       B *R10

BankSwitch2:
       CLR @BANK2
       B *R10

BankSwitch3:
       CLR @BANK3
       B *R10

BankSwitch0:
       CLR @BANK0
       B *R10

;* Copy R2 bytes from R1 to VDP address R0
;VDPW   MOVB @R0LB,*R14      ; Send low byte of VDP RAM write address
;       ORI  R0,VDPWM        ; Set read/write bits 14 and 15 to write (01)
;       MOVB R0,*R14         ; Send high byte of VDP RAM write address
;!      MOVB *R1+,*R15       ; Write byte to VDP RAM
;       DEC  R2              ; Byte counter
;       JNE  -!              ; Check if done
;       RT

;* Read R2 bytes to R1 from VDP address R0 (R0 is preserved)
;VDPR   MOVB @R0LB,*R14      ; Send low byte of VDP RAM write address
;       MOVB R0,*R14         ; Send high byte of VDP RAM write address
;       NOP                  ; Very important for 9918A prefetch, otherwise glitches can occur
;!      MOVB @VDPRD,*R1+
;       DEC R2
;       JNE -!
;       RT

START
       CLR @BANK0            ; switch to bank 0

*******************************************************************
       BANK 0
*******************************************************************

       LWPI WRKSP             ; Load the workspace pointer to fast RAM
       LIMI 0                 ; Interrupts off

       LI R14,VDPWA
       LI R15,VDPWD


       MOV @>83C0,@RAND16     ; Copy gpl random to random seed
       ;MOV @>83D6,@RAND16     ; Copy frame counter to random seed


       ; Copy DrawStripLoop function into fast RAM
       LI R0,DRWSCR
       LI R1,DrawStripSource
       LI R2,(DrawStripEnd-DrawStripStart)/2
!      MOV *R1+,*R0+
       DEC R2
       JNE -!


       ; Initialize VDP registers
       LI R1,VDPINI
       LI R0,VDPRM     ; Register mask
!      MOVB *R1+,*R14  ; Write register data
       MOVB R0,*R14    ; Write register number
       AI R0,>100
       CI R0,>800+VDPRM
       JL -!

       ; Load sprite patterns
       LI R0,SPRPAT
       LI R1,x#Sprites
       LI R2,256   ; 256 * 8 = 64 sprite patterns
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

       CLR @HSCORE
       CLR @HSCORE+2

       CLR @MUSIC
       CLR @SOUND

       ;JMP GameRestart
       B @TitleScreen

CarInitData:
       DATA 0,>0078
       DATA 0,>C0C0
       DATA 0,>C0C0
       DATA 0,>C0C0
       DATA 0,>C0C0
       DATA >C000
       DATA >C000
       DATA >C000
       DATA >C000


GameRestart:

       ; Select pattern table for first season
       LI R0,PatternTable+(0*28*8)
       MOV R0,@CURPAT

       LI R1,x#Track   ;+(3*64)+40
       MOV R1,@CURTRK        ; Track pointer in bank 1

       LI R0,>0500
       MOVB R0,@LIVES        ; initialize number of lives

       CLR @SCOREH
       CLR @SCOREL
       CLR @EXLIFE

       LI R0,(300/4)*>100
       MOVB R0,@BONUSP     ; initialize bonus points

       B @GetReadyPlayer

SeasonRestart:

       CLR @SMASHD     ; Reset smashed cars to zero (and clear CURSEG)

       LI R0,x#IntroMusic
       MOV R0,@MUSIC
       LI R0,>0100
       MOVB R0,@MUSICC

Restart:

       LI R0,48  ; Speed = 48/32 = 1.5X
       MOV R0,@SPEED

       ; setup initial cars
       LI R4,CAR
       LI R1,CarInitData
       LI R2,14
!
       MOV *R1+,*R4+
       DEC R2
       JNE -!


       BL @PlayAudio

       ; Load initial segment table
       LI R10,x#LoadSegmentData
       BL @BankSwitch1

       BL @PlayAudio

       CLR R0
       MOVB @CURSEG,R0
       ANDI R0,>0E00
       CI R0,>0800          ; bridge segment
       JNE !
       LI R0,>0018
       A R0,@CAR+2
!

       ; Clear the screen
       LI R0,SCRTAB+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R2,32*8
!      CLR *R15
       DEC R2
       JNE -!

       BL @PlayAudio


       ; Load initial transitions
       LI R5,PATTAB
       MOV @TG0,R6       ; tilegroup
       CLR R7
       BL @LoadTileGroup

       BL @PlayAudio

       LI R5,PATTAB+>800
       MOV @TG1,R6       ; tilegroup
       CLR R7
       BL @LoadTileGroup

       BL @PlayAudio

       LI R5,PATTAB+>1000
       MOV @TG2,R6       ; tilegroup
       CLR R7
       BL @LoadTileGroup

       LI R10,x#StartMotor
       BL @BankSwitch3

       ; Draw initial screen
       LI R13,(7*8)*32

MainLoop:

       ;BL @VSYNC
       BL @PlayAudio


       MOV R13,R5
       S @SPEED,R13      ; Add speed to scroll offset

       MOV R13,R2
       MOV R5,R1
       ANDI R1,>FFE0
       ANDI R2,>FFE0
       S R2,R1
       SLA R1,3
       MOVB R1,@DELTA  ; Save Y delta for moving enemy cars

       ANDI R13,>1FFF ; Wrap at 32 lines


       ; 32 steps per pixel
       ; screen change event every 8 lines


       ; trigger every 8 rows scrolled
       XOR R13,R5
       ANDI R5,>0800
       JEQ NoChange

       LI R1,2       ; Score 2 points for 8 rows, or 8 points per segment
       BL @AddScore

       LI R10,x#LoadPartialSegment
       BL @BankSwitch1

       LI R5,PATTAB+>1000
       MOV @TG1,R6       ; new tilegroup
       MOV @TG2,R7       ; old tilegroup
       MOV R6,@TG2
       BL @LoadTileGroup

       BL @PlayAudio

       LI R5,PATTAB+>800
       MOV @TG0,R6       ; new tilegroup
       MOV @TG1,R7       ; old tilegroup
       MOV R6,@TG1
       BL @LoadTileGroup

       BL @PlayAudio

       LI R5,PATTAB
       MOV @TGNEW,R6     ; new tilegroup
       MOV @TG0,R7       ; old tilegroup
       MOV R6,@TG0
       BL @LoadTileGroup

       BL @PlayAudio

       MOV R13,R0
       ANDI R0,>1800
       CI R0,>1800
       JNE NoChange

       ; at the end of track season?
       CLR R0
       MOVB @CURSEG,R0
       CI R0,>1000
       JNE NoChange

       ; don't change until player has finished jumping
       MOV @CAR,R1
       SRC R1,1
       JOC NoChange

       B @NextSeason

NoChange:
       BL @DrawStripLoop

       BL @PlayAudio



       ; Check for loading jumping sprites
       MOV @CAR,R0
       SRC R0,1        ; move lowest bit into carry
       JNC JumpSpritesDone ; not jumping or crashing
       LI R2,LoadJumpSprites

       CI R0,>6000/2+>8000
       JNE !
       LI R1,x#Sprites+>A00
       B *R2
!
       CI R0,>5000/2+>8000
       JNE !
       LI R1,x#Sprites+>B00
       B *R2
!
       CI R0,>4000/2+>8000
       JNE !
       LI R1,x#Sprites+>C00
       B *R2
!
       CI R0,>3000/2+>8000
       JNE !
       LI R1,x#Sprites+>D00
       B *R2
!
       CI R0,>2000/2+>8000
       JNE !
       LI R1,x#Sprites+>E00
       B *R2
!
       CI R0,>1000/2+>8000
       JNE !
       LI R1,x#Sprites+>F00
       B *R2
!

CRASHT EQU >7000     ; crash timer

       ; splash sprites
       CI R0,CRASHT/2+>8003
       JNE !
       LI R1,x#Sprites+>1000
       B *R2
!
       CI R0,CRASHT/2+>7C03
       JNE !
       LI R1,x#Sprites+>1100
       B *R2
!
       CI R0,CRASHT/2+>7803
       JNE !
       LI R1,x#Sprites+>1200
       B *R2
!
       CI R0,CRASHT/2+>7403
       JNE !
       LI R1,x#Sprites+>1300
       B *R2
!

       ; crash sprites
       CI R0,CRASHT/2+>8001
       JNE !
       LI R1,x#Sprites+>1400
       B *R2
!
       CI R0,CRASHT/2+>7C01
       JNE !
       LI R1,x#Sprites+>1500
       B *R2
!
       CI R0,CRASHT/2+>7801
       JNE !
       LI R1,x#Sprites+>1600
       B *R2
!
       CI R0,CRASHT/2+>7401
       JNE !
       LI R1,x#Sprites+>1700
       B *R2
!
JumpSpritesDone:


       ; handle player movement first, then draw sprites

       BL @ReadKeys

       ; TODO (if bounced, can't steer, only jump)
       ; TODO (if jumping, can't change speed, only steer)

       MOV @CAR,R7     ; Jumping counter
       MOV R7,R1
       MOV @SPEED,R5
       JEQ PlayerCrashed

       SRC R1,1        ; move lowest bit into carry
       JOC Steer       ; Jumping goes straight to steer (no accel/decel)

       LI R6,>100
       C R7,R6
       JL !

       ; Bouncing
       S R6,@CAR

       SRC R1,5
       JOC Accel
       JMP Decel
!
       LI R0,KeyUp
       CZC @KEYS,R0
       JNE !

       MOV @DELTA,R1  ; COUNTR
       SRC R1,1       ; move lowest bit into carry
       JNC !          ; only test for up every other frame

Accel  CI R5,MAXSPD      ; 148 (220) maximum speed
       JHE !
       INC R5         ; Speed up
       JMP SaveSpeed
!

       LI R0,KeyDown
       CZC @KEYS,R0
       JNE !
Decel  CI R5,MINSPD      ; 48 (20) minimum speed
       JLE !
       DEC R5        ; Slow down
SaveSpeed:
       MOV R5,@SPEED   ; Save speed
       LI R10,x#SetMotor
       BL @BankSwitch3
!

       LI R0,KeyFire
       CZC @KEYS,R0
       JNE !

       CI R5,88       ; Speed less than 100?
       JL !

       LI R7,>7801     ; 100ish Frames of jumping
       MOV R7,@CAR
       LI R10,x#StopMotor
       BL @BankSwitch3


       LI R10,x#JumpingSound
       BL @SetSound

       LI R1,x#Sprites+>900
       B @LoadJumpSprites
!

       C R7,R6
       JL Steer

       ; Bouncing
       SLA R1,2
       JOC Left
       JMP Right

Steer:
       LI R0,KeyLeft
       CZC @KEYS,R0
       JNE !
Left   DEC @CAR+2
       JMP !!
!
       LI R0,KeyRight
       CZC @KEYS,R0
       JNE !
Right  INC @CAR+2
!


       ; Draw player car
       ; when bounced, move in bounce direction
       ; other move based on controls, and animate sprites
       ; when jumping, no collisions with other cars
       ; when jumping, can move left or right but not accel/decel
       ; at the end of the jump, speed decreases by 32
       ; check collisions, and set bounce counter


       LI R0,360
       S @SPEED,R0
       SRL R0,1
       AB @CAR+3,R0
       SWPB R0         ; R0 = YYXX player car

       LI R12,NextSprite

       MOV R7,R2
       SRC R2,1        ; move lowest bit into carry
       JNC Driving
Jumping:
       LI R2,>0100
       S R2,R7         ; Decrement jump counter
       C R2,R7         ; Until zero
       JLE !
       CLR R7

       ; TODO check for landing on island

       ; smash any cars underneath
       B @SmashLanding
SmashLandingDone:

       CLR R2
       MOVB @CURSEG,R2
       CI R2,>1000      ; check for next season
       JNE !
       B @NextSeason
!
       MOV R7,@CAR


       CI R7,>1000
       JHE !
       DEC @SPEED     ; Decrease speed by 32
       LI R10,x#SetMotor
       BL @BankSwitch3
!
       JMP DoneDriving

PlayerCrashed:

       ; decrement counter and then rewind to checkpoint
       AI R7,->100
       MOV R7,@CAR
       CI R7,>100
       JHE DoneDriving

       ; Rewind track to safety
       LI R10,x#RewindSafely
       BL @BankSwitch1

       B @GetReadyPlayer

Driving:

       LI R4,CAR+2
       MOV R0,*R4
       MOV R4,R6
       BL @Bounce

DoneDriving:


       LI R0,SPRTAB+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14

       LI R0,>0100
       AB R0,@COUNTR  ; Increment counter

       BL @DrawCars   ; Draw other cars

       ; Speed = 20 -> 1.5 pixels per frame
       ; Steps of 2
       ; Speed = 222 -> 4.5 pixels per frame
       ; 101 total steps
       ; 20 -> 48/32
       ; 222 -> 144/32
       ; Displayed speed = (spd - 38) * 2



       ; R10 is speed 48-148

       ; Draw 3 sprites for speed
       ; 20-99 in red
       ; 100-220 in white or black


       LI R0,>200D ; YYXX  starting middle digit
       LI R1,>0008 ; red zero
       LI R3,100
       LI R4,>0400 ; increment to next number

       MOV @SPEED,R2
       JEQ !!!!! ; Displayed speed is zero

       AI R0,->000D ; move to hundreds digit
       AI R2,-38
       A R2,R2 ; R2 is speed 20-220

       C R2,R3
       JL !!    ; less than 100
       LI R1,>0401 ; black '1' digit
       S R3,R2
       C R2,R3
       JL !
       S R3,R2
       A R4,R1     ; two
!
       BL @PutSprite
       ANDI R1,>00FF
!
       AI R0,>000D ; move to tens digit
       LI R3,10
!
       C R2,R3
       JL !
       S R3,R2
       A R4,R1     ; next digit
       JMP -!
!
       BL @PutSprite
!      AI R0,>000D ; move to ones digit
       ANDI R1,>00FF
       SLA R2,10
       A R2,R1
       BL @PutSprite


       MOV @DELTA,R0   ; COUNTR
       SRC R0,4
       JOC NoWarning

       ; Draw exclamation mark for segments 4,11,12,14
       LI R4,UnsafeSegments
       MOVB @CURSEG,R1
       CB R1,*R4+
       JEQ Warning
       CB R1,*R4+
       JEQ Warning
       CB R1,*R4+
       JEQ Warning
       CB R1,*R4+
       JNE NoWarning
Warning:
       ANDI R0,>F000
       JNE !
       LI R10,x#WarningBeep
       BL @SetSound
!

       LI R0,>1078   ; YYXX
       LI R1,>2C0B   ; yellow <!>
       BL @PutSprite
       LI R1,>2801   ; black <!>
       BL @PutSprite
NoWarning:


       ; Draw player car

       LI R0,360       ; calculate y based on speed
       S @SPEED,R0
       SRL R0,1
       AB @CAR+3,R0
       SWPB R0         ; R0 = YYXX player car

       LI R12,NextSprite

       MOV @CAR,R7    ; bouncing and counters
       SRL R7,1
       JOC DrawJumpingOrCrash

       ; draw normal car sprites
       MOV @DELTA,R2  ; COUNTR
       ANDI R2,>0004
       ; move bit 2 to bit
       SRC R2,3      ; R2 = animation offset

       CI R7,>100
       JL !
       CLR R2     ; no animate if bounced
!
       LI R5,PlayerSprites
       BL *R12   ; @NextSprite     ; black tires
       BL *R12   ; @NextSprite     ; red body
       BL *R12   ; @NextSprite     ; white part
       JMP DrawPlayerDone

DrawJumpingOrCrash:
       SRL R7,1
       JOC DrawCrash

       MOV @CAR,R7    ; bouncing and counters

       ; Move jumping car up then down
       MOV R7,R6
       SRL R6,1
       ANDI R6,>FF00
       CI R7,>3800
       JHE !
       S R6,R0
       JMP !!
!      A R6,R0
       AI R0,->3800
!

       CLR R2
       LI R5,JumpSprites
       AI R0,->0808
       BL *R12   ; @NextSprite    ; left tire
       AI R0,16
       BL *R12   ; @NextSprite    ; right tire
       AI R0,-8
       BL *R12   ; @NextSprite    ; red body
       BL *R12   ; @NextSprite    ; white part

       AI R0,>1000-8
       BL *R12   ; @NextSprite   ; left tire
       AI R0,16
       BL *R12   ; @NextSprite   ; right tire
       AI R0,-8
       BL *R12   ; @NextSprite   ; red body
       BL *R12   ; @NextSprite   ; white part
       JMP DrawPlayerDone
DrawCrash:
       CI R7,(CRASHT->2000)/4      ; animation is only 32 frames
       JLE DrawPlayerDone

       MOV @CAR+2,R0

       LI R12,NextSpritePlusOffset
       CLR R2   ; no animate

       LI R5,CrashSprites
       SRL R7,1
       JNC !
       LI R5,SplashSprites
!
       NEG R7
       AI R7,(CRASHT/8)    ; shifted 4 times

       SRL R7,4
       ANDI R7,>0030      ; Add N*16 bytes to pick right table
       A R7,R5

       AI R0,->0808
       MOV R0,R3
       BL *R12   ; @NextSprite    ; upper left
       BL *R12   ; @NextSprite    ; upper right
       AI R0,-16
       BL *R12   ; @NextSprite    ; red body
       BL *R12   ; @NextSprite    ; white part

       MOV R3,R0
       AI R0,>1000
       BL *R12   ; @NextSprite   ; lower left
       BL *R12   ; @NextSprite   ; lower right
       AI R0,-16
       BL *R12   ; @NextSprite   ; red body
       BL *R12   ; @NextSprite   ; white part


DrawPlayerDone:


       ; Draw cars last every other frame to flicker

       MOV @DELTA,R2  ; COUNTR
       SRC R2,1
       JOC !
       BL @DrawCars   ; Draw other cars

       ; Terminate the sprite list
       MOVB @Terminator,*R15

       BL @DrawPlaceHolders
       JMP !!
!
       ; Terminate the sprite list
       MOVB @Terminator,*R15
!


       ; enemies
       ; check collisions,
       ;   with player, set bounce counter
       ;   with bouncing car, set bounce counter and direction
       ;   with car, move away from it
       ;   with boulder, crash and no points  (not in 4k)
       ; check tiles and colortable under front
       ;  bounced?  crash and score points
       ;  left clear, move left
       ;  right clear, move right
       ;  otherwise stop
       ; top or bottom of screen
       ;  respawn at opposite edge
       ; move or change direction (when?)
       ;  drive straight, left or right  (use bounce direction bits)
       ; crashed car?  cartype with zero speed





       ; now check collisions (after all sprites are drawn)

       ; check tiles and colortable under front
       ; tile index < 8 indicates road, ok
       ; colortable gray - ok
       ; colortable blue - splash
       ; colortable other - crash

       LI R5,>0E00   ; Grey color bg



       ; check player car collisions
       LI R4,CAR
       MOV *R4+,R7
       MOV *R4+,R0   ; R0=YYXX
       SRC R7,1
       JOC NoCrash   ; Can't crash if jumping

       MOV R0,R6
       AI R0,15      ; Check right side
       BL @CollisionCheck
       MOV R2,R3
       MOV R6,R0     ; Check left side
       BL @CollisionCheck

       ANDI R2,>FE00  ; Make light blue into dark blue
       ANDI R3,>FE00
       CI R2,>0400  ; Dark blue bg
       JEQ CrashWater
       CI R3,>0400  ; Dark blue bg
       JEQ CrashWater

       S R5,R2      ; R2 -= >0E00
       JNE Crash    ; Road clear
       S R5,R3      ; R3 -= >0E00
       JEQ NoCrash  ; Road clear
Crash:
       LI R7,CRASHT+3  ; Crash for 192 frames (land)
       LI R8,x#CrashSound
       JMP CrashLand
CrashWater:
       LI R7,CRASHT+7  ; Crash for 192 frames (water)
       LI R8,x#SplashSound
CrashLand:
       MOV R7,@CAR     ; Store crash counter and crash bits
       MOV R8,@SOUND
       LI R8,>0100
       MOVB R8,@SOUNDC
       CLR @SPEED      ; set speed to zero
       LI R10,x#StopMotor
       BL @BankSwitch3
NoCrash:


       ;B @MainLoop   ; no enemy car action


       ; check enemy cars

       ;LI R4,ECAR1+2
       ;INCT R4
CrashLoop:
       MOV *R4+,R1  ; Car type and stuff
       MOV *R4,R0    ; R0 = YYXX

       MOV R1,R2
       ANDI R2,>000F
       CI R2,SmashType
       JHE SmashedCar

       AI R0,15
       BL @CollisionCheck  ; Returns R2=x000 R3=x000 x=bgcolor
       MOV R2,R3
       MOV *R4,R0    ; R0 = YYXX
       BL @CollisionCheck  ; Returns R2=x000 R3=x000 x=bgcolor

       MOV *R4,R0    ; R0 = YYXX

       CI R0,>E800     ; Car off top of screen?
       JHE CarOk
       CI R0,>C800     ; Car off bottom of screen?
       JL CarOk

       ; need to respawn car on top or bottom of screen

       MOV R0,R6       ; Save for comparison later
!
       BL @Random      ; R0 = random
       MOV R0,R7
       MOV R0,R1
       ANDI R7,>000F   ; Car type
       CI R7,9        ; try again if too high
       JH -!
       SRL R0,8   ; 00XX

       ANDI R1,>0070  ; get random movement direction
       SOC R1,R7


       CI R6,>D800     ; Top or bottom? (halfway between C000 and F000)
       JHE !
       ; Respawn at top
       AI R0,>F000
       JMP !!

!      ; Respawn at bottom
       AI R0,>C100
!
       MOV R0,R6
       BL @CollisionCheck  ; Returns R2=x000 R3=x000 x=bgcolor
       S R5,R2
       JNE NextCar    ; Don't save if not clear
       MOV R6,R0
       AI R0,15
       BL @CollisionCheck  ; Returns R2=x000 R3=x000 x=bgcolor
       S R5,R2
       JNE NextCar    ; Don't save if not clear

       MOV R6,*R4      ; Save new location
       MOV R7,@-2(R4)  ; Save new car type

       JMP NextCar

CarOk:
       DECT R4       ; R4 points to bounce counter and cartype
       BL @GetCarType ; R1=car type  R7=speed
       INCT R4

       S R5,R2      ; R2 -= >0E00
       JEQ LeftOK

       S R5,R3      ; R3 -= >0E00
       JNE EitherNotOK
       INCT R0      ; Shift car right
       ANDI R1,>FFEF
       MOV R1,@-2(R4)
       JMP EitherNotOK

LeftOK:
       S R5,R3      ; R3 -= >0E00
       JEQ BothOK

       DECT R0      ; Shift car left
       ORI R1,>0010
       MOV R1,@-2(R4)

EitherNotOK:

       CI R1,>100     ; Bouncing?
       JL Stuck
       BL @SmashCarFunc
       JMP NextCar

BothOK:
       MOV R1,R3
       CI R1,>100     ; Bouncing?
       JHE Bouncing

       SLA R7,4
       S R7,R0       ; Subtract speed

       MOV R1,R8
       ANDI R8,>000F
       JEQ AfterSpeed   ; 0=tank, no random move
       CI R8,>0009      ; 9+ dump truck
       JHE DumpTruck

       ; only move every other frame
       MOV @DELTA,R8  ; COUNTR
       SRC R8,1
       JOC AfterSpeed

       ; add random movement
       ANDI R3,>0070
       SRL R3,3
       A @DriveDir(R3),R0

AfterSpeed:
       AB @DELTA,R0  ; Add scrolling offset
AfterDelta:
       MOV R0,*R4    ; Save car coordinates

       MOV R4,R6
       BL @Bounce

NextCar:
       INCT R4
       CI R4,ECAR4+4
       JNE CrashLoop

       LI R2,4
       JMP MoveRocksLoop

SmashedCar:  ; or High Score
       CI R2,ScoreType
       JEQ FloatingScore

       AB @DELTA,R0  ; Add scrolling offset
       MOV R0,*R4    ; Save car coordinates

       AI R1,>0100
       MOV R1,@-2(R4)
       MOV R1,R0
       ANDI R0,>0F00
       JNE NextCar

       MOV *R4,R0
       CI R0,>B000
       JL !
       LI R0,>B000
       MOVB R0,*R4     ; set YY coordinate to stay on screen
!
       ANDI R1,>00F0     ; keep score number from smashedtype
       JEQ ScoreOff
       ORI R1,>4000+ScoreType
       JMP !

FloatingScore:
       AI R1,->0100
       CI R1,>0100
       JHE !
ScoreOff:
       ; turn off score, spawn car
       LI R0,>C0C0
       MOV R0,*R4    ; Save car coordinates
       CLR R1
!
       MOV R1,@-2(R4)
       JMP NextCar

Bouncing:
       ANDI R3,>0030
       SRL R3,3
       A @BounceDir(R3),R0

       AI R1,->100    ; Decrement bounce counter
       CI R1,>100
       JHE !
       ; finished bouncing, randomize direction
       MOV @RAND16,R8
       ANDI R8,>0070
       XOR R8,R1
!
       MOV R1,@-2(R4)
       JMP AfterDelta


Stuck:
       MOV R1,R8
       ANDI R8,>000F
       CI R8,>0009      ; 9+ dump truck
       JL AfterSpeed

DumpTruck:
       CI R8,>000B
       JHE AfterSpeed

       MOV @DELTA,R9      ; COUNTR
       ANDI R9,>007F

       CI R8,>0009
       JNE !
       ; slow dump truck
       CI R9,>0010
       JNE AfterSpeed
       INC @-2(R4)    ; change to dumping truck
       JMP AfterSpeed
!      ; dumping truck
       CI R9,>0020
       JNE AfterSpeed
       INC @-2(R4)    ; change to dumped truck

       ; don't drop on track 0, or season summer(arcade) or fall(intellivision)
       MOV @CURPAT,R10
       CI R10,PatternTable    ; track 0
       JEQ AfterSpeed
       CI R10,Fall            ; I like fall better
       JEQ AfterSpeed

       ; spawn rock
       MOV R4,R10
       SRL R10,1
       MOV R0,@RockRelativeToCar(R10)

       ; make sound
       LI R10,x#DumpRockSound
       BL @SetSound

       JMP AfterSpeed

       ; Move Rocks
* R2 must be 4
MoveRocksLoop:
       MOV *R4+,R1
       CI R1,>C000
       JL !
       CI R1,>F000
       JL !!
!
       ; add delta and store it
       AB @DELTA,R1
       MOVB R1,@-2(R4)
!
       DEC R2
       JNE MoveRocksLoop

       B @MainLoop


; R1=car type, R4=pointer to Car YYXX word
; Modifies R1,R2,R10
SmashCarFunc:
       ANDI R1,>000F    ; Get car type
       MOVB @CarScores(R1),R2  ; Get score index
       SRL R2,7
       MOV @ScoreTable(R2),R1

       LI R10,>0100
       AB R10,@SMASHD  ; Increment smashed car count

SmashCarFuncNoScore:      ; call this with R1=0, R2=0
       SLA R2,3
       ORI R2,SmashType    ; Smashed car type
       MOV R2,@-2(R4)

       MOV R11,R2
       BL @AddScore       ; Add R1 to score
       MOV R2,R11

       LI R10,x#SmashSound
       B @SetSound            ; return thru SetSound


CarTypes:  ; SSsC  SS=sprite index s=speed C=color
       DATA >601C  ; 0 Tank, darkgreen, 300pts
       DATA >6424  ; 1 Thresher, yellow/blue, 200pts
       DATA >503D  ; 2 Car, purple, 300pts
       DATA >5C3F  ; 3 Car, white, 500pts
       DATA >6833  ; 4 Car, light green, 200pts
       DATA >5424  ; 5 Car, dark blue, 200pts
       DATA >5822  ; 6 Car, green, 300pts
       DATA >4C31  ; 7 Car, black, 300pts
       DATA >6C41  ; 8 Jolly Roger Car, black, 500pts
       DATA >7031  ; 9 Dump truck slow, 300pts
       DATA >7441  ; A Dump truck dumping
       DATA >7855  ; B Dump truck fast
       DATA >8006  ; C crashed speed=0
       DATA >A00F  ; D score?
SmashType:
       EQU >000C
ScoreType:
       EQU >000D

PT_500 EQU 1
PT_300 EQU 2
PT_200 EQU 3
CarScores:
       BYTE PT_300  ; 0 Tank
       BYTE PT_200  ; 1 Thresher
       BYTE PT_300  ; 2 Car, purple
       BYTE PT_500  ; 3 Car, white
       BYTE PT_200  ; 4
       BYTE PT_200  ; 5
       BYTE PT_300  ; 6
       BYTE PT_300  ; 7
       BYTE PT_500  ; 8
       BYTE PT_300  ; 9
       BYTE PT_300  ; A
       BYTE PT_300  ; B
       EVEN
ScoreTable:
       DATA 0,500,300,200


VDPINI
       BYTE >02          ; VDP Register 0: 02 (Bitmap Mode)
       BYTE >E2          ; VDP Register 1: 16x16 Sprites
       BYTE (SCRTAB/>400); VDP Register 2: Screen Image Table
       BYTE (CLRTAB/>40)+>7F ; VDP Register 3: Color Table
       BYTE (PATTAB/>800)+>3; VDP Register 4: Pattern Table
       BYTE (SPRTAB/>80) ; VDP Register 5: Sprite List Table
       BYTE (SPRPAT/>800); VDP Register 6: Sprite Pattern Table
       BYTE >F1          ; VDP Register 7: White on Black

UnsafeSegments:
       BYTE >04
       BYTE >0B
       BYTE >0C
       BYTE >0E

Terminator:
       BYTE >D0
       EVEN

; Sprites described as >SS0C  SS=sprite index C=color
PlayerSprites:
       DATA >3801  ; wheels, black
       DATA >3006  ; body, red
       DATA >340F  ; motor & axles, white

JumpSprites:
       DATA >4401     ; top left tire, black
       DATA >4801     ; top right tire, black
       DATA >3C06     ; top body, red
       DATA >400F     ; top engine, white
       DATA >C401     ; bottom left tire, black
       DATA >C801     ; bottom right tire, black
       DATA >BC06     ; bottom body, red
       DATA >C00F     ; bottom engine, white

; Crash and splash sprites: >SSOC SS=sprite index O=Xoffset/8 C=color
CrashSprites:
       DATA >3C06,>4026,>441F,>480B,>BC06,>C026,>C41F,>C80B
       DATA >3C06,>4026,>440B,>482F,>BC06,>C026,>C40F,>C82B
       DATA >3C06,>4026,>440F,>482B,>BC06,>C026,>C40B,>C82F
       DATA >3C06,>4026,>440F,>482B,>BC06,>C026,>C40B,>C82F

SplashSprites:
       DATA >3C07,>4027,>441F,>4805,>BC07,>C027,>C41F,>C805
       DATA >3C07,>4027,>440F,>482F,>BC07,>C027,>C40F,>C82F
       DATA >3C07,>4027,>440F,>482F,>BC07,>C027,>C40F,>C82F
       DATA >3C07,>4027,>440F,>482F,>BC07,>C027,>C40F,>C82F


* Get car type in R1, putting speed in R7
GetCarType:
       MOV *R4,R1      ; R1=car type and bouncing
       MOV R1,R7
       ANDI R7,>000F
       A R7,R7
       MOV @CarTypes(R7),R7
       ANDI R7,>0070   ; extract speed
       RT


DriveDir:   ; down up    right left
       DATA >0100,>FF00,>0001,>FFFF
BounceDir:  ; DR    DL    UR    UL
       DATA >0101,>01FF,>FF01,>FEFF

* Draw all non-player cars (offscreen to keep sprites from glitching too much)
* R3 = number of sprites from DrawCars
* Modifies R0,R5
DrawPlaceHolders:
       LI R5,>C100   ; this will be offscreen
       LI R0,VDPWM+SPRTAB
!      MOVB @R0LB,*R14
       MOVB R0,*R14
       MOVB R5,*R15  ; update only the Y offset
       AI R0,4
       DEC R3
       JNE -!
       
       RT


* Draw all non-player cars
* R2 = sprite animate offset
* Modifies R1,R2,R4,R5, returns number of sprites in R3
DrawCars:
       MOV @DELTA,R2  ; COUNTR
       SRC R2,1
       ANDI R2,>0002
       SRC R2,2      ; R2 = animation offset
       LI R4,ECAR1
       LI R3,8       ; 4 cars and 4 rocks
DrawCarsLoop:
       MOV *R4+,R1
       MOVB *R4+,*R15  ; YY
       MOVB *R4+,*R15  ; XX
       MOV R1,R5
       ANDI R5,>000F   ; R5=car type
       CI R5,SmashType
       JHE DrawSmash
       SLA R5,1
       MOV @CarTypes(R5),R5
       CI R1,>100      ; bouncing?
       JHE AfterOffset
       A R2,R5         ; add animation offset
AfterOffset:
       MOVB R5,*R15    ; Sprite index
       SRC R5,4
       SRL R5,4        ; shift out speed
       MOVB R5,*R15    ; Color

       CI R4,ECAR4+4
       JNE DrawCarsLoop

       ; draw rocks in road,
       ; R4 is pointing to ROCK1
       LI R2,4
       LI R5,>FC01    ; rock sprite and color
RockLoop:
       MOV *R4+,R1
       MOVB R1,*R15  ; YY
       MOVB @R1LB,*R15  ; XX
       MOVB R5,*R15    ; sprite
       MOVB @R5LB,*R15 ; color
       DEC R2
       JNE RockLoop

       RT

DrawSmash:
       CI R5,ScoreType
       JEQ DrawScore

       MOV R1,R5
       ANDI R5,>0C00
       ORI R5,>8008
       MOVB R5,*R15
       MOVB @R5LB,*R15

       DECT R4
       MOVB *R4+,*R15  ; YY
       MOVB *R4+,*R15  ; XX

       AI R5,>1007
       INC R3

       JMP AfterOffset

DrawScore:
       MOV R1,R5
       ANDI R5,>0030
       SLA R5,6
       ORI R5,>A00F
       JMP AfterOffset

NextSpritePlusOffset:
       MOV *R5,R1
       ANDI R1,>0030
       SRL R1,1
       A R1,R0
       MOV *R5+,R1
       ANDI R1,>FF0F
       JMP PutSprite

* Put R0 and *R5+ in VDP, adding R2 to R1
NextSprite:
       MOV *R5+,R1
       A R2,R1
       ; fall thru to PutSprite


* Put R0 and R1 in VDP
PutSprite:
       MOVB R0,*R15
       MOVB @R0LB,*R15
       MOVB R1,*R15
       MOVB @R1LB,*R15
       RT


* Check for collisions at location R0=YYXX  R5=>0E00
* Modifies R0,R1,R2,R3
CollisionCheck:
       CI R0,>C000
       JL !
       MOV R0,R2
       ANDI R0,>00FF   ; Read top line if car is above top of screen
       CI R2,>E000
       JHE !
       AI R0,>BF00     ; Read bottom line if car is below bottom of screen
!
       MOV R0,R1       ; R1=YYYY YYYY XXXX XXXX
       SRL R1,3         ; R1=YYYY YYYY XXXX XXXX
       MOVB @R0LB,@R1LB
       SRL R1,3         ; R1=0000 00YY YYYX XXXX

       AI R1,SCRTAB
       MOVB @R1LB,*R14  ; Set VDP read address
       MOVB R1,*R14
       CLR R2
       MOVB @VDPRD,R2     ; R2 = character under top left

       ; Check Left side
       CI R2,>0800
       JHE !
       ; Empty road
       MOV R5,R2       ; return road color
       RT
!
       MOV R0,R1
       SWPB R1
       ANDI R1,7     ; R1=y offset

       SRL R0,3
       ANDI R0,>1800     ; R0=which 8-line section
       A R0,R1
       ; 000S SCCC CCCC CYYY
       ;AI R1,CLRTAB  ; CLRTAB is now 0


       ; now lookup characters in color table to get bg color
       ; (grey=road, blue=water, etc)

       SRL R2,5      ; R2 = character code * 8
       A R1,R2       ; R2 += y offset

       ; Get from color table
       MOVB @R2LB,*R14
       MOVB R2,*R14
       CLR R2
       MOVB @VDPRD,R2
       ANDI R2,>0F00

CollisionCheckDone:
       RT

* Smash any cars colliding with the player
* R0 = player car
* R7 = jump counter (preserve)
SmashLanding:
       MOV R0,R6
       LI R5,>0E00
       BL @CollisionCheck
       C R5,R2
       JNE SmashLandingExit

       MOV R6,R0
       AI R0,15
       BL @CollisionCheck
       C R5,R2
       JNE SmashLandingExit
       
       MOV R6,R0

       LI R4,ECAR1+2
!
       MOV R0,R5
       S *R4,R5
       ABS R5
       CI R5,>0F00
       JHE !
       SWPB R5     ; R5=XXYY difference
       ABS R5
       CI R5,>0F00
       JHE !

       MOV @-2(R4),R1
       ANDI R1,>000F
       CI R1,SmashType
       JHE !
       BL @SmashCarFunc
!
       AI R4,4
       CI R4,ECAR4+6
       JL -!!

       ; TODO do island landing check
       LI R10,x#IslandCheck
       BL @BankSwitch1
       BL @AddScore

SmashLandingExit:
       LI R10,x#StartMotor
       BL @BankSwitch3

       B @SmashLandingDone



* Check for bounce between 2 cars
* R4 = car pointer to position
* R6 = other car pointer to position
* Preserve R4,R5
* Modifies R1-R2,R7, R8, R10
Bounce:
       C R4,R6     ; Can't bounce self
       JEQ BounceRet

       MOV *R4,R1  ; R1=car YYXX
       S *R6,R1    ; R1=YYXX difference
       ABS R1
       CI R1,>0F00
       JHE BounceRet
       SWPB R1     ; R1=XXYY difference
       ABS R1
       CI R1,>0F00
       JHE BounceRet

       CI R6,Rock1
       JHE BounceRock

       ; Cars are touching, get types
       MOV @-2(R4),R1
       MOV @-2(R6),R7

       MOV R7,R8
       ANDI R8,>000F   ; get other car type
       CI R8,SmashType     ; smashed type?
       JHE BounceRet

       ; if car is player car, bounce
       CI R4,CAR+2
       JEQ BounceCars

       ; if either car is bouncing, set both to bouncing
       CI R1,>0100  ; Bouncing?
       JHE BounceCars
       CI R7,>0100  ; Bouncing?
       JHE BounceCars

       ;MOV R11,R10     ; save return address

       ; Scramble directions
       ;BL @RANDOM
       ;ANDI R0,>00F0
       ;XOR R0,R1
       ;MOV R1,@-2(R4)

       ;BL @RANDOM
       ;ANDI R0,>00F0
       ;XOR R0,R7
       ;MOV R7,@-2(R6)

       ;MOV R10,R11   ; restore return address

       ; Not bouncing, just shift left or right
       CB @1(R4),@1(R6)
       JH !
       DEC *R4      ; Shift one way
       INC *R6
       JMP BounceRet
!      
       INC *R4      ; Shift the other way
       DEC *R6
       JMP BounceRet
       
BounceCars:
       ANDI R1,>000F ; get car type only
       JEQ !    ; tank?
       CI R1,>0009   ; dump truck?
       JHE BounceDumpTruckR1
       JMP !!
!      ; maybe tank
       CI R4,CAR+2
       JEQ !    ; player
       AI R1,->1800    ; shorten bounce counter for tank
!

       ANDI R7,>000F
       JEQ !    ; tank?
       CI R7,>0009   ; dump truck?
       JHE BounceDumpTruckR7
       JMP !!
!
       AI R7,->1800    ; shorten bounce counter for tank
!
       AI R1,>1F00  ; set bounce counters
       AI R7,>1F00

       ; Calculate directions
       MOV *R4,R8
       S *R6,R8
       JOC !
       ORI R1,>0020   ; moving up/down
       JMP !!
!      ORI R7,>0020   ; moving up/down
!

       CB @1(R4),@1(R6)
       JHE !
       ORI R1,>0010   ; moving left/right
       JMP !!
!      ORI R7,>0010   ; moving left/right
!

       MOV R1,@-2(R4)  ; store new flags
       MOV R7,@-2(R6)  ; store new flags

       MOV R11,R7
       LI R10,x#BounceSound
       BL @SetSound
       MOV R7,R11

BounceRet:
       AI R6,4
       CI R6,ECAR4+6
       JL Bounce
       DECT R6
       CI R6,ROCK4+2
       JNE Bounce

       RT

BounceRock:
       CI R4,CAR+2    ; if player, crash
       JNE !
       LI R7,CRASHT+3  ; Crash for 192 frames (land)
       LI R8,x#CrashSound
       MOV R7,@CAR     ; Store crash counter and crash bits
       MOV R8,@SOUND
       LI R8,>0100
       MOVB R8,@SOUNDC
       CLR @SPEED      ; set speed to zero
       LI R10,x#StopMotor
       B @BankSwitch3  ; return thru stop motor

!
       ; a rock cannot hit its own dump truck
       MOV R4,R1
       SRL R1,1
       AI R1,RockRelativeToCar
       C R1,R6
       JEQ BounceRet

       ; don't crash offscreen rocks
       MOV *R6,R1
       CI R1,>C000
       JHE BounceRet

       ; bouncing enemies get smashed with score
       MOV @-2(R4),R1
       CI R1,>0100
       JHE BounceDumpTruckR1

       ; otherwise just crash
       CLR R1
       CLR R2
       MOV R11,R8
       BL @SmashCarFuncNoScore
       MOV R8,R11
       JMP BounceRet


BounceDumpTruckR7:
       MOV R7,R1     ; set R1=dump truck type
       MOV R4,R7     ; save R4
       MOV R6,R4     ; use R6
       JMP !

BounceDumpTruckR1:
       MOV R4,R7     ; save R4
!
       MOV R11,R8
       BL @SmashCarFunc   ;
       MOV R8,R11
       MOV R7,R4     ; Restore R4

       JMP BounceRet


KeyFire:
       EQU >0100
KeyLeft:
       EQU >0200
KeyRight:
       EQU >0400
KeyDown:
       EQU >0800
KeyUp:
       EQU >1000

DebounceP:
       LI R1,100   ; need at least 100 loops of non-P to return
!
       TB 2+3    ; P
       JNE -!
       DEC R1
       JNE -!
       RT

* Modifies R0,R1,R10,R12
ReadKeys:
       MOV R11,R9
       LI R1,>0500     ; Select Keyboard column 5 lines
       LI R12,>0024    ; Select address lines starting at line 18
       LDCR R1,3       ; Send 3 bits to set one 8 of output lines enabled
       CLR R12  ;LI R12,>0006    ; Select address lines to read starting at line 3

       TB 2+3    ; P
       JEQ NoPause

       ; Turn off all sound attenuators
       LWPI SNDREG
       LI R0,>9F00
       LI R0,>BF00
       LI R0,>DF00
       LI R0,>FF00
       LWPI WRKSP

       BL @DebounceP
       ; Pause on
!
       TB 2+3    ; P
       JEQ -!
       BL @DebounceP


NoPause:


       LI R1,>0600     ; Select Joystick 1 lines
       LI R12,>0024    ; Select address lines starting at line 18
       LDCR R1,3       ; Send 3 bits to set one 8 of output lines enabled
       LI R12,>0006    ; Select address lines to read starting at line 3
       STCR R0,5
       
       CLR R1          ; Select Keyboard column 0 lines
       LI R12,>0024    ; Select address lines starting at line 18
       LDCR R1,3       ; Send 3 bits to set one 8 of output lines enabled
       CLR R12  ;LI R12,>0006    ; Select address lines to read starting at line 3
       
       TB 1+3            ; Space
       JEQ !
       ANDI R0,~KeyFire
!
       TB 2+3            ; Enter
       JEQ !
       ANDI R0,~KeyFire
!

       LI R1,>0100     ; Select Keyboard column 1 lines
       LI R12,>0024    ; Select address lines starting at line 18
       LDCR R1,3       ; Send 3 bits to set one 8 of output lines enabled
       CLR R12  ;LI R12,>0006    ; Select address lines to read starting at line 3
       
       TB 5+3            ; S
       JEQ !
       ANDI R0,~KeyLeft
!
       TB 7+3            ; X
       JEQ !
       ANDI R0,~KeyDown
!

       LI R1,>0200     ; Select Keyboard column 2 lines
       LI R12,>0024    ; Select address lines starting at line 18
       LDCR R1,3       ; Send 3 bits to set one 8 of output lines enabled
       CLR R12  ;LI R12,>0006    ; Select address lines to read starting at line 3
       
       TB 5+3            ; D
       JEQ !
       ANDI R0,~KeyRight
!
       TB 6+3            ; E
       JEQ !
       ANDI R0,~KeyUp
!
       MOV R0,@KEYS

       BL *R9
       


PlayAudio:
       ; Check to see if VDP interrupt was triggered
       MOVB @VDPSTA,R0
       SLA R0,1
       JOC !
       RT
!      ; and if so, then play sounds
       LI R10,x#PlaySoundLists
       B @BankSwitch3

* Add R1 to score, giving extra life at every 30K
* Modifes R1,R10
AddScore:
       A R1,@SCOREL
       JNC !
       INC @SCOREH  ; add carry
!
       A @EXLIFE,R1
       JOC !
       CI R1,30000
       JHE !
       MOV R1,@EXLIFE
       RT
!      ; get extra life
       AI R1,-30000
       MOV R1,@EXLIFE

       LI R1,>100
       AB R1,@LIVES

       ; play extra life sound
       LI R10,x#ExLifeSound
       ; JMP SetSound

SetSound:
       MOV @SOUND,@SOUND  ; test sound for NULL
       JEQ !
       C R10,@SOUND
       JH !!
!      MOV R10,@SOUND
       LI R10,>0100
       MOVB R10,@SOUNDC
!
       RT

SetMotorSound:
       LI R10,MOTOR
       JMP SetSound

MASK   DATA >B6DF
* Return Random number in R0
Random:
       MOV @RAND16,R0  ; Seed -> r0, seed must not be 0
       SRL R0,1        ; Shift r0 1 bit to the right, carry goes to C bit in status register
       JNC !           ; If carry was not set, jump to label no-carry
       XOR @MASK,R0    ; calculate new random number with 0xb400 as mask
!      MOV R0,@RAND16
       RT





* R3=PATTAB address
LoadCharData:
       MOV R11,R12      ; Save return address

       MOV R3,R0
       LI R1,x#CharData
       LI R2,96
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

       MOV R3,R0
       AI R0,CLRTAB-PATTAB
       LI R1,x#CharData+(96*8)
       LI R2,96
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

       B *R12        ; Return to saved address

* returns to Restart
GetReadyPlayer:

       LI R0,VDPWM+SPRTAB
       MOVB @R0LB,*R14
       MOVB R0,*R14
       
       MOVB @Terminator,*R15

       ; Clear the screen
       LI R0,SCRTAB+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R1,>2000
       LI R2,8*32
!      MOVB R1,*R15
       DEC R2
       JNE -!

       BL @PlayAudio

       LI R3,PATTAB
       BL @LoadCharData

       BL @PlayAudio

       MOVB @LIVES,R0
       JEQ GameOver
       LI R0,>0100
       SB R0,@LIVES

       ; print score and lives
       BL @DrawStatus

       LI R0,SCRTAB+VDPWM+(5*32+7)
       LI R1,GetReadyText
       LI R2,s#GetReadyText
       BL @DrawTextAt


       LI R2,120
       BL @Delay

       MOV @MUSIC,R0
       JEQ !
       B @Restart
!      ; no music playing yet
       B @SeasonRestart

GameOver:
       LI R0,x#SilenceMusic
       MOV R0,@MUSIC
       LI R0,>0100
       MOVB R0,@MUSICC

       BL @PlayAudio

       ; print score and lives
       BL @DrawStatus

       LI R0,SCRTAB+VDPWM+(5*32+11)
       LI R1,GameOverText
       LI R2,s#GameOverText
       BL @DrawTextAt

       LI R2,60
       BL @Delay

!      ; wait until fire button is pressed
       LI R2,1
       BL @Delay

       BL @ReadKeys
       ANDI R0,KeyFire
       JNE -!


TitleScreen:
       BL @ClearScreen

       ; Load pattern and color data in all 3 tables
       LI R3,PATTAB
       BL @LoadCharData
       LI R3,PATTAB+>800
       BL @LoadCharData
       LI R3,PATTAB+>1000
       BL @LoadCharData

       LI R0,SCRTAB+(7*32)
       LI R1,x#TitleText
       LI R2,(10*32)/8
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

!      ; wait until fire button isn't pressed
       LI R2,1
       BL @Delay

       BL @ReadKeys
       ANDI R0,KeyFire
       JEQ -!

!      ; wait until fire button is pressed
       LI R2,1
       BL @Delay

       BL @ReadKeys
       ANDI R0,KeyFire
       JNE -!

       BL @ClearScreen

       B @GameRestart


ClearScreen:
       ; Clear screen with spaces
       LI R0,SCRTAB+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R1,>2000
       LI R2,24*32
!      MOVB R1,*R15
       DEC R2
       JNE -!
       RT


* Multiply bonus multiplier by the number of smashed cars, or 50K if zero cars smashed
* Modifies R0-R1, result in R1
CalculateBonus:
       CLR R0
       MOVB @BONUSP,R0
       SRL R0,6
       MOVB @SMASHD,R1
       SRL R1,8
       MPY R1,R0       ; Result low bits in R1
       JNE !
       LI R1,50000     ; Zero cars is 50K bonus
!
       RT

NextSeason:
       AI R13,>4000
       LI R12,>2800

       ; Load special sprite for finish line
       LI R0,SPRPAT+(>40*8)
       LI R1,x#Sprites+(>3C*8)   ; Spring
       LI R9,>400B             ; bright yellow
       MOV @CURPAT,R2
       CI R2,Spring
       JNE !
       LI R1,x#Sprites+(>40*8)   ; Summer
       LI R9,>4001             ; black
!      CI R2,Summer
       JNE !
       LI R1,x#Sprites+(>44*8)   ; Fall
       LI R9,>4006             ; dark red
!      CI R2,Fall
       JNE !
       LI R1,x#Sprites+(>48*8)   ; Winter
       LI R9,>4001             ; black
!      LI R2,4           ; Copy one sprite (4*8 bytes)
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

       LI R0,VDPWM+SPRTAB+(3*4)+1
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R0,>3000
       MOVB R0,*R15     ; X-coordinate
       MOVB R9,*R15     ; sprite index
       MOVB @R9LB,*R15  ; sprite color
       MOVB @Terminator,*R15


FinishLoop:
       BL @PlayAudio

       MOV R13,R5
       S @SPEED,R13      ; Add speed to scroll offset

       C R13,R12
       JH !
FINISH EQU >1580
       CI R12,FINISH
       JEQ FinishLoopDone

       LI R12,FINISH

       LI R10,x#LoadFinishData
       BL @BankSwitch1

       ;LI R5,PATTAB+>1000 ; bottom
       ;MOV @TGNEW,R6     ; new tilegroup
       ;MOV @TG2,R7       ; old tilegroup
       ;MOV R6,@TG2
       ;BL @LoadTileGroup

       BL @PlayAudio

       LI R5,PATTAB+>800 ; middle
       MOV @TGNEW,R6     ; new tilegroup
       MOV @TG1,R7       ; old tilegroup
       MOV R6,@TG1
       BL @LoadTileGroup

       BL @PlayAudio

       LI R5,PATTAB      ; top
       MOV @TGNEW,R6     ; new tilegroup
       MOV @TG0,R7       ; old tilegroup
       MOV R6,@TG0
       BL @LoadTileGroup

       BL @PlayAudio
!
       CI R12,FINISH
       JNE !
       BL @DrawFinishSprite
!
       BL @DrawStripLoop

       BL @PlayAudio

       LI R0,>0100
       AB R0,@COUNTR  ; Increment counter

       ; Set other sprite list address
       LI R0,VDPWM+SPRTAB
       MOVB @R0LB,*R14
       MOVB R0,*R14

       MOV @SPEED,R0
       CI R0,MAXSPD+30
       JEQ !
       INC R0
       MOV R0,@SPEED
!
       NEG R0

       AI R0,360
       SRL R0,1
       AB @CAR+3,R0
       CI R0,>5600
       JEQ !!
       JL !
       AI R0,->0200
!
       AI R0,>0100
       MOVB R0,@CAR+3
!
       SWPB R0         ; R0 = YYXX player car


       MOV @DELTA,R2  ; COUNTR
       ANDI R2,>0004
       ; move bit 2 to bit
       SRC R2,3      ; R2 = animation offset

       LI R10,NextSprite
       LI R5,PlayerSprites

       BL *R10   ; @NextSprite     ; black tires
       BL *R10   ; @NextSprite     ; red body
       BL *R10   ; @NextSprite     ; white part
       CI R12,FINISH
       JEQ FinishLoop

       ; Don't add terminator if extra sprite is being drawn
       MOVB @Terminator,*R15

       JMP FinishLoop

DrawFinishSprite:
       LI R0,VDPWM+SPRTAB+(3*4)
       MOVB @R0LB,*R14
       MOVB R0,*R14

       MOVB R13,R0
       NEG R0
       SLA R0,3
       AI R0,>0400
       MOVB R0,*R15
       RT

FinishLoopDone:
       LI R13,FINISH
       BL @DrawFinishSprite
       BL @DrawStripLoop
       LI R10,x#StopMotor
       BL @BankSwitch3
       BL @PlayAudio

       LI R0,x#FinishMusic
       LI R1,>0100
       MOV R0,@MUSIC
       MOVB R1,@MUSICC


       ; next season pattern table
       MOV @CURPAT,R0
       AI R0,8*28
       CI R0,PatternTable+(5*8*28)
       JNE !
       LI R0,PatternTable+(1*8*28)
!
       MOV R0,@CURPAT

       ; next track (wrap at 8 to 3)
       MOV @CURTRK,R1
       CI R1,x#Track+(8*64)-1
       JL !
       LI R1,x#Track+(3*64)
!
       MOV R1,@CURTRK



       ; Clear top and bottom screen with spaces
       LI R0,SCRTAB+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R1,>2000
       LI R2,8*32
!      MOVB R1,*R15
       DEC R2
       JNE -!

       ; Load pattern and color data in top and bottom tables
       LI R3,PATTAB
       BL @LoadCharData

       LI R0,SCRTAB+(32*16)+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R1,>2000
       LI R2,8*32
!      MOVB R1,*R15
       DEC R2
       JNE -!

       LI R3,PATTAB+>1000
       BL @LoadCharData

       BL @CalculateBonus    ; Returns bonus in R1
       BL @AddScore          ; Get extra lives if any

       BL @DrawStatus


       LI R0,SCRTAB+VDPWM+(4*32+8)
       LI R1,CongratulationsText
       LI R2,s#CongratulationsText
       BL @DrawTextAt

       LI R2,60
       BL @Delay

       LI R0,SCRTAB+VDPWM+(6*32+6)
       LI R1,YouSmashedText
       LI R2,s#YouSmashedText
       BL @DrawTextAt

       MOVB @SMASHD,R1
       SRL R1,8
       CLR R2
       BL @DrawNumber

       MOVB @SMASHD,R0
       SRL R0,8
       LI R1,CarsText
       LI R2,s#CarsText
       CI R0,1
       JNE !
       LI R1,CarText
       DEC R2
!      BL @DrawText

       LI R2,60
       BL @Delay

       LI R0,SCRTAB+VDPWM+(17*32+8)
       LI R1,BonusPointsAreText
       LI R2,s#BonusPointsAreText
       BL @DrawTextAt

       LI R2,60
       BL @Delay

       MOVB @SMASHD,R1
       JNE !

       LI R0,SCRTAB+VDPWM+(19*32+10)
       LI R1,Extra50000Text
       LI R2,s#Extra50000Text
       BL @DrawTextAt
       JMP !!
!
       LI R0,SCRTAB+VDPWM+(19*32+8)
       CLR R1
       MOVB @BONUSP,R1
       SRL R1,6
       CLR R2
       BL @DrawNumberAt

       LI R1,TimesText
       LI R2,s#TimesText
       BL @DrawText

       MOVB @SMASHD,R1
       SRL R1,8
       CLR R2
       BL @DrawNumber

       LI R1,EqualsText
       LI R2,s#EqualsText
       BL @DrawText

       BL @CalculateBonus
       CLR R2
       BL @DrawNumber

!
       LI R2,60
       BL @Delay

       LI R0,SCRTAB+VDPWM+(21*32+7)
       MOV @CURPAT,R3

       LI R1,NextWaySpringText
       LI R2,s#NextWaySpringText
       CI R3,Spring
       JEQ !

       LI R1,NextWaySummerText
       LI R2,s#NextWaySummerText
       CI R3,Summer
       JEQ !

       LI R1,NextWayFallText
       LI R2,s#NextWayFallText
       CI R3,Fall
       JEQ !

       LI R1,NextWayWinterText
       LI R2,s#NextWayWinterText
!
       BL @DrawTextAt

       LI R2,60
       BL @Delay

       LI R0,SCRTAB+VDPWM+(23*32+11)
       LI R1,GoodLuckText
       LI R2,s#GoodLuckText
       BL @DrawTextAt

       LI R2,120
       BL @Delay


       ; increase bonus points for next season, up to 500
       CLR R0
       MOVB @BONUSP,R0
       CI R0,(500/4)*>100
       JEQ !
       AI R0,(100/4)*>100
       MOVB R0,@BONUSP
!


       B @SeasonRestart

DrawTextAt:
       MOVB @R0LB,*R14
       MOVB R0,*R14
DrawText:
       MOVB *R1+,*R15
       DEC R2
       JNE DrawText
       RT

* Draw scores and lives
* Modifies R0-R5,R12
DrawStatus:
       MOV R11,R12       ; Save return address

       LI R0,SCRTAB+VDPWM+(0*32+2)
       LI R1,ScoreText
       LI R2,s#ScoreText
       BL @DrawTextAt

       LI R0,SCRTAB+VDPWM+(1*32+2)
       MOV @SCOREH,R2
       MOV @SCOREL,R1
       ; check if current score is high score
       C R2,@HSCORE
       JL !!
       JH !
       C R1,@HSCORE+2
       JL !!
!      ; Set it
       MOV R2,@HSCORE
       MOV R1,@HSCORE+2
!
       BL @DrawNumberAt

       LI R0,SCRTAB+VDPWM+(1*32+13)
       MOV @HSCORE,R2
       MOV @HSCORE+2,R1
       BL @DrawNumberAt

       LI R0,SCRTAB+VDPWM+(2*32+2)
       MOVB @R0LB,*R14
       MOVB R0,*R14
       LI R1,>2300     ; '#' car
       MOVB @LIVES,R2
       JEQ !!
       SRL R2,8
!      MOVB R1,*R15
       DEC R2
       JNE -!
!


       B *R12            ; Return to saved address

* Wait for R2 vsyncs
* Modifies R0-R2,R10,R12,R13
Delay
       MOV R11,R13           ; Save return address
!
       MOVB @VDPSTA,R12      ; Clear interrupt first so we catch the edge
       CLR R12
!      TB 2                  ; CRU Address bit 0002 - VDP INT
       JEQ -!                ; Loop until set

       LI R10,x#PlaySoundLists
       BL @BankSwitch3

       DEC R2
       JNE -!!             ; Loop until R2 is zero

       B *R13                ; Return to saved address


* Draw the 32-bit number in R2:R1
* Modifies R0-R5
DrawNumberAt:
       MOVB @R0LB,*R14
       MOVB R0,*R14
DrawNumber:
       LI R3,Digits
!      MOV *R3+,R5
       MOV *R3+,R4
       JEQ DrawNumberZero
       C R2,R5
       JL -!
       JNE !
       C R1,R4
       JL -!
!      ; Draw digits
       LI R0,'0'*>100
DrawCount:     ; Count the digit in this place
       C R2,R5
       JL DrawDigit
       JNE !
       C R1,R4
       JL DrawDigit
!      ; Increment digit
       AI R0,>100
       S R5,R2
       S R4,R1
       JOC DrawCount
       DEC R2
       JMP DrawCount
DrawDigit:      ; Draw digit and get next place value
       MOVB R0,*R15
       MOV *R3+,R5
       MOV *R3+,R4
       JNE -!!
       RT
DrawNumberZero:
       LI R0,'0'*>100
       MOVB R0,*R15
       RT


Digits:
       DATA >3b9a,>ca00  ; 1000000000
       DATA >05f5,>e100  ; 100000000
       DATA >0098,>9680  ; 10000000
       DATA >000f,>4240  ; 1000000
       DATA >0001,>86a0  ; 100000
       DATA 0,10000
       DATA 0,1000
       DATA 0,100
       DATA 0,10
       DATA 0,1
       DATA 0,0

* Reading the VDP INT bit from the CRU doesn't clear the status register, so it should be safe to poll.
* The CRU bit appears to get updated even with interrupts disabled (LIMI 0)
* Modifies R12
;VSYNC
;       MOVB @VDPSTA,R12      ; Clear interrupt first so we catch the edge
;       CLR R12
;!      TB 2                  ; CRU Address bit 0002 - VDP INT
;       JEQ -!                ; Loop until set
;       RT


ScoreText:
       TEXT "1UP       HI SCORE       2UP"
GameOverText:
       TEXT "GAME OVER"
GetReadyText:
       TEXT "GET READY PLAYER 1"
CongratulationsText:
       TEXT "CONGRATULATIONS!"
YouSmashedText:
       TEXT "YOU SMASHED "
CarsText:
       TEXT " CARS."
CarText:
       TEXT " CAR."
BonusPointsAreText:
       TEXT "BONUS POINTS ARE-"
TimesText:
       TEXT " * "
EqualsText:
       TEXT " = "
Extra50000Text:
       TEXT "EXTRA  50000"
NextWaySummerText:
       TEXT "NEXT WAY IS SUMMER"
NextWayFallText:
       TEXT " NEXT WAY IS FALL"
NextWayWinterText:
       TEXT "NEXT WAY IS WINTER"
NextWaySpringText:
       TEXT "NEXT WAY IS SPRING"
GoodLuckText:
       TEXT "GOOD LUCK!"

       EVEN

LoadJumpSprites:
       LI R0,SPRPAT+(>3C*8)
       LI R2,4*32/8
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

       LI R0,SPRPAT+(>BC*8)
       LI R2,4*32/8
       LI R10,x#LoadSpriteData
       BL @BankSwitch2

       B @JumpSpritesDone



* Load tile group
* R5=PATTAB address
* R6=NNNN  tile group nibbles
* R7=NNNN  previous tile group
LoadTileGroup:
       C R6,R7
       JNE !
LoadTileGroupDone:
       RT
!
       LI R12,TileGroups-(32*2)  ; tilegroup base, adjusted since row 0 doesn't exist
       LI R10,>92E7    ; size of each group in nibble reverse order
       AI R5,VDPWM
!
       MOV R6,R8
       SLA R6,4
       ANDI R8,>F000     ; R8=x000 x=new tilegroup
       MOV R7,R9
       SLA R7,4
       ANDI R9,>F000     ; R9=y000 y=old tilegroup

       SRL R8,6     ; multiply by 64 (stride of TileGroups table)
       SRL R9,6     ; multiply by 64 (stride of TileGroups table)
       A R12,R8     ; add base
       A R12,R9     ; add base
       MOV R10,R4
       SRL R10,4      ; next group size
       ANDI R4,>000F  ; R4=size of group
       JEQ LoadTileGroupDone
       A R4,R12    ; add group size to base
       A R4,R12
!
       MOV *R8+,R0
       C *R9+,R0
       JNE LoadTransition
LoadTransitionDone:
       AI R5,8*8
       DEC R4
       JNE -!
       JMP -!!


* Load tile group pair that is in R0
* Modifies R0-R3
LoadTransition:
       MOV R0,R1
       ANDI R0,>FF00
       XOR R0,R1
       SRL R0,5
       SLA R1,3

       ; Copy patterns
       A @CURPAT,R0
       A @CURPAT,R1

* Copy transition tiles from R0 and R1 to VDP
* Modifies R0-R3
CopyTransition:
       MOVB @R5LB,*R14   ; Set PATTAB or CLRTAB address
       MOVB R5,*R14
       LI R3,SCRBUF

       MOV *R0+,*R3+
       MOV *R0+,*R3+
       MOV *R0+,*R3+
       MOV *R0+,*R3+

       MOV *R1+,*R3+
       MOV *R1+,*R3+
       MOV *R1+,*R3+
       MOV *R1+,*R3+

       LI R3,SCRBUF

       ; Copy Transition Tiles
       LI R2,8
!      MOVB *R3+,*R15
       MOVB *R3+,*R15
       MOVB *R3+,*R15
       MOVB *R3+,*R15
       MOVB *R3+,*R15
       MOVB *R3+,*R15
       MOVB *R3+,*R15
       MOVB *R3+,*R15
       AI R3,-7
       DEC R2
       JNE -!

       CI R5,PATTAB+VDPWM       ; This requires pattern table be above color table address
       JL !

       ; Copy Colors
       AI R0,ColorTable-PatternTable-8
       AI R1,ColorTable-PatternTable-8
       AI R5,CLRTAB-PATTAB
       JMP CopyTransition

!
       AI R5,PATTAB-CLRTAB
       ;RT
       JMP LoadTransitionDone

* Draw Strips Loop
* R13=yyyyyyy pppfffff  y=row p=pixel f=fractional 1/32 pixel
* Modifies R0-R8
DrawStripLoop:
       ; Set screen address
       LI R0,SCRTAB+VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14

       ; R3 = pixel scroll offset
       MOV R13,R3
       SLA R3,3
       ANDI R3,>0700
       MOVB R3,@R3LB

       ; R5 = screen scroll offset
       MOV R13,R5
       ANDI R5,>1F00
       SRL R5,7
       AI R5,SEGBUF

       LI R6,48      ; 24 rows of left and right strips
       LI R1,R0LB    ; Used by DrawStrip
       LI R7,x#LeftStrips
       LI R8,x#LeftStrips ^ x#RightStrips
       LI R2,>AAAA   ; Bits toggle on-off for each iteration of DrawStrip

       B @DRWSCR     ; jump to DrawStripStart fast RAM

PatternTable:
       BCOPY 'bnp_gfx.dat'
Spring EQU PatternTable+(8*28*1)
Summer EQU PatternTable+(8*28*2)
Fall   EQU PatternTable+(8*28*3)
Winter EQU PatternTable+(8*28*4)
ColorTable EQU PatternTable+(8*28*5)

       COPY 'bnp_seg.asm'

DrawStripSource:
       XORG DRWSCR
* This will be copied into fast RAM
DrawStripStart:
       CLR @BANK1    ; Strip data in bank 1
DrawStripOuter:
       CLR R4
       MOVB *R5+,R4
       ANDI R5,SEGMSK  ; Mask pointer for wrap-around
       SRL R4,4
       A R7,R4        ; Current strips offset
       XOR R8,R7      ; Toggle left-right strips

* Draw strip - 16 characters to screen table (address already set)
* R1 = @R0LB
* R3 = y offsets in high and low bytes
* R4 = strip pointer
* Modifies: R0,R2
DrawStrip: ; loop is unrolled 4 times
       ;LI R2,2
!
       MOV R3,R0      ; copy y offsets to R0
       SOC *R4+,R0    ; set upper bits to strip bytes
       MOVB R0,*R15   ; copy first byte to screen table
       MOVB *R1,*R15  ; copy second byte to screen table

       MOV R3,R0      ; copy y offsets to R0
       SOC *R4+,R0    ; set upper bits to strip bytes
       MOVB R0,*R15   ; copy first byte to screen table
       MOVB *R1,*R15  ; copy second byte to screen table

       MOV R3,R0      ; copy y offsets to R0
       SOC *R4+,R0    ; set upper bits to strip bytes
       MOVB R0,*R15   ; copy first byte to screen table
       MOVB *R1,*R15  ; copy second byte to screen table

       MOV R3,R0      ; copy y offsets to R0
       SOC *R4+,R0    ; set upper bits to strip bytes
       MOVB R0,*R15   ; copy first byte to screen table
       MOVB *R1,*R15  ; copy second byte to screen table

       SRC R2,1
       JNC DrawStrip
       ;DEC R2
       ;JNE -!
       DEC R6
       JNE DrawStripOuter

       CLR @BANK0     ; Back to bank 0
       RT

DrawStripEnd:
       EVEN
DSTPSZ EQU DrawStripEnd-DrawStripStart



*******************************************************************
       BANK 1       ; track segments transitions data
*******************************************************************
LoadSegmentData:
       MOV R11,R10
*      R1 = segment index to load from
       MOV @CURTRK,R0
       MOV R0,R1
       AI R1,-Track
       ANDI R1,>003F
       CI R1,>003F
       JNE !
       ; end of previous track
       INC R0
       MOV R0,@CURTRK
!
       MOVB *R0+,R1
       MOVB R1,@CURSEG
       SRL R1,8

       ; Load initial tilegroup transitions
       LI R0,6
       MOV R1,R0      ; R0=R1
       A R1,R0        ; R0=R1*2
       A R1,R0        ; R0=R1*3
       A R0,R0        ; R0=R1*6
       AI R0,SegmentTransitionsInternal
       LI R2,TG0      ; Destination
       MOV *R0+,*R2+
       MOV *R0+,*R2+
       MOV *R0+,*R2+

       ; Load initial strip pointers
       LI R0,31*2
       MPY R1,R0
       AI R1,StripIndex

       LI R0,SEGBUF
       LI R2,31
!      MOV *R1+,*R0+
       DEC R2
       JNE -!

       JMP BankSwitch0


EndOfTrack:
       LI R0,>1000          ; This will trigger next season
       MOVB R0,@CURSEG
       MOV @SEGBUF+>3A,@SEGBUF+>3E

       JMP BankSwitch0

LoadPartialSegment:
       MOV R11,R10
       MOV @CURTRK,R9      ; R9=Track pointer
       MOVB *R9+,R8
       SRL R8,8            ; R8=current segment type

       MOV R13,R0
       ANDI R0,>1800
       CI R0,>1800
       JNE !
       ; load bottom transition 3

       MOV R9,R1
       AI R1,-Track
       ANDI R1,>003F       ; check for end of track
       JEQ EndOfTrack

       MOV R9,@CURTRK
       SLA R8,4
       SOCB *R9,@R8LB   ; R8=>00xy   x=current y=next
       MOVB *R9,@CURSEG
       MOV R8,R4
       SLA R8,1
       MOV @SegmentTransitions(R8),R1
       MOV @TransitionStripIndex(R8),@SEGBUF+62

       ANDI R4,>000F
       LI R5,SEGBUF+(24*2)
       LI R2,7
       SRL R0,7
       JMP LoadStrips
!

       ; load other transition 0-2
       ;SWPB R0    ; R0=>00 >08 >10
       ;SLA R0,1   ; R0=>00 >10 >20
       SRL R0,7

       ; R5=transition*16+SEGBUF
       MOV R0,R5
       AI R5,SEGBUF

       MOV R0,R2
       SRL R2,3

       ; calculate next internal tilegroup
       MOV R8,R1
       A R8,R1
       A R8,R1
       A R1,R1
       A R2,R1       ; R1 = R8*6 + R2
       MOV @SegmentTransitionsInternal(R1),R1

       MOV R8,R4
       LI R2,8
LoadStrips:
       ; R4 = Segment
       LI R3,31*2
       MPY R4,R3
       A R0,R4
       AI R4,StripIndex

!      ; R4=Source  R5=Destination  R2=Count
       MOV *R4+,*R5+
       DEC R2
       JNE -!

SetNewTG:
       ; R1 = new tilegroup
       MOV @TG0,R0    ; R0 = old tilegroup
       LI R3,>0FFF

       MOV R1,R2
       SZC R3,R2  ; ANDI R2,>F000
       JNE !
       MOV R0,R2
       SZC R3,R2  ; ANDI R2,>F000
       SOC R2,R1
!
       LI R3,>F0FF
       MOV R1,R2
       SZC R3,R2  ; ANDI R2,>0F00
       JNE !
       MOV R0,R2
       SZC R3,R2  ; ANDI R2,>0F00
       SOC R2,R1
!
       SRA R3,4
       MOV R1,R2
       SZC R3,R2  ; ANDI R2,>00F0
       JNE !
       MOV R0,R2
       SZC R3,R2  ; ANDI R2,>00F0
       SOC R2,R1
!
       SRA R3,4
       MOV R1,R2
       SZC R3,R2  ; ANDI R2,>000F
       JNE !
       MOV R0,R2
       SZC R3,R2  ; ANDI R2,>000F
       SOC R2,R1
!
       MOV R1,@TGNEW

       B @BankSwitch0

LoadFinishData:
       MOV R11,R10

       ; Set new tilegroup
       MOV @SegmentTransitionsInternal+>60,R1

       ; Load initial strip pointers
       CLR R0
       LI R2,31     ; copy 31 rows
       LI R4,>0010  ; segment 16
       LI R5,SEGBUF

       JMP LoadStrips

RewindSafely:
       MOV R11,R10
       MOV @CURTRK,R0
!
       ; need two safe segments in a row
       BL @CheckTrack
       BL @CheckTrack

       MOV R0,@CURTRK
       B @BankSwitch0

CheckTrack:
       MOV R0,R1
       AI R1,-Track
       ANDI R1,>003f
       JEQ !

       DEC R0      ; move back one

       MOVB *R0,R1
       MOVB R1,@CURSEG
       LI R2,RewindSegments
       CB *R2+,R1
       JEQ -!
       CB *R2+,R1
       JEQ -!
       CB *R2+,R1
       JEQ -!
       CB *R2+,R1
       JEQ -!
       CB *R2+,R1
       JEQ -!
       CB *R2+,R1
       JEQ -!
*       CB *R2+,R1
*       JEQ -!
!
       RT

RewindSegments:
*       BYTE 4,5,6,11,12,14,15     ; includes split road
       BYTE 4,6,11,12,14,15      ; doesn't include starting split road
       EVEN

* R0 = player car YYXX
* R13 = scroll offset
IslandCheck:
       MOV R11,R10

       MOV @CURTRK,R9      ; R9=Track pointer
       CLR R2
       MOVB *R9,R2

       ; R13 greater than >1800, R9 is above
       ; R13 less than >1800 , R9 is current

       ; when R2=>0B00, island is from >9070 to >F0A0
       ; when R2=>0E00, island is from >7060 to >E0F0
       ; when R2=>0C00, island is from >C060 to >10D0 (wrapped)

       MOV R13,R1
       SLA R1,3
       ANDI R1,>FF00
       A R0,R1

       CI R2,>0B00    ; small island
       JNE !
       CI R1,>9000
       JL IslandCheckDone
       CI R1,>F000
       JHE IslandCheckDone
       SWPB R1
       CI R1,>7000
       JL IslandCheckDone
       CI R1,>A000
       JHE IslandCheckDone
       JMP IslandBonus
!
       CI R2,>0E00    ; big island
       JNE !
       CI R1,>7000
       JL IslandCheckDone
       CI R1,>E000
       JHE IslandCheckDone
       SWPB R1
       CI R1,>6000
       JL IslandCheckDone
       CI R1,>F000
       JHE IslandCheckDone
       JMP IslandBonus
!
       CI R2,>0C00
       JNE IslandCheckDone
       MOVB @-1(R9),R2
       CI R2,>0C00        ; short bridge
       JNE IslandCheckDone
       CI R1,>C000
       JHE !
       CI R1,>1000
       JHE IslandCheckDone
!      SWPB R1
       CI R1,>6000
       JL IslandCheckDone
       CI R1,>D000
       JHE IslandCheckDone
       JMP IslandBonus

IslandCheckDone:
       CLR R1     ; Set R1 to 1000 if landing on island
       B @BankSwitch0
IslandBonus:
       LI R1,>4000+ScoreType  ; Score 1000 type
       MOV R1,@ECAR1      ; Overwrite enemy 1 too bad
       MOV R0,R1
       AI R1,>0020
       MOV R1,@ECAR1+2    ; Set score position
       LI R1,1000   ; Set R1 to 1000 if landing on island
       B @BankSwitch0



       COPY 'bnp_data.asm'


*******************************************************************
       BANK 2       ; sprites
*******************************************************************
* Copy R2 * 8 bytes from bank 2 address R1 to VDP address R0
LoadSpriteData:
       MOV R11,R10
       ORI R0,VDPWM
       MOVB @R0LB,*R14
       MOVB R0,*R14
!
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       MOVB *R1+,*R15
       DEC R2
       JNE -!
       JMP BankSwitch0

TitleText:
       BYTE 32,0,1,18,19,2,3,16,17,18,22,4,23,32,6,16,17,6,32,0,1,18,19,2,3,16,17,18,22,4,23,32
       BYTE 32,8,9,26,27,10,11,24,25,26,30,8,31,32,32,24,25,32,32,14,15,26,27,10,11,24,25,26,30,8,31,32
       TEXT '                                '
       TEXT '                                '
       TEXT '           @2020 $%&            '
       TEXT '                                '
       TEXT '                                '
       TEXT '     PRESS JOYSTICK FIRE OR     '
       TEXT '                                '
       TEXT '       SPACE BAR TO START       '

CharData:
       BCOPY 'bnp_chr.dat'

Sprites:
       BCOPY 'bnp_spr.dat'



*******************************************************************
       BANK 3       ; sfx
*******************************************************************
SNDREG EQU >8400

* Modifies R0-R1,R10
PlaySoundLists:
       MOV R11,R10  ; set bank return address

       MOV @MUSIC,R0
       JEQ !
       CLR R1
       MOVB @MUSICC,R1
       BL @PlaySoundList
       MOV R0,@MUSIC
       MOVB R1,@MUSICC
!
       MOV @SOUND,R0
       JEQ !
       CLR R1
       MOVB @SOUNDC,R1
       BL @PlaySoundList
       MOV R0,@SOUND
       MOVB R1,@SOUNDC
!
       JMP BankSwitch0
       
PlaySoundList:
       AI R1,->100   ; decrement count until zero
       JNE !!
       MOVB *R0+,R1  ; number of sound bytes to copy
       SWPB R1
!
       MOVB *R0+,@SNDREG    ; copy sound byte
       DEC R1
       JNE -!
       MOVB *R0+,R1  ; number of frames to delay
       JNE !
       ; reload music pointer
       MOVB *R0+,R1
       SWPB R1
       MOVB *R0+,R1
       SWPB R1
       MOV R1,R0
       LI R1,>0100
!
       RT


MotorSound: ; Copied to MOTOR
       BYTE 4,>E3,>C0,>0E,>F3,0,0,0

MotorFreqs:
       DATA >C00E
       DATA >CB0C
       DATA >CA0B
       DATA >CC0A
       DATA >C00A
       DATA >CA0B
       DATA >CF0A
       DATA >C50A
       DATA >CB09
       DATA >C50A
       DATA >C309
       DATA >CC08
       DATA >C00A
       DATA >C909
       DATA >C209
       DATA >CC08
       DATA >C608
       DATA >C108
       DATA >CC07
       DATA >CC08
       DATA >C808
       DATA >C508
       DATA >C108
       DATA >CE07
       DATA >CB07
       DATA >C807

StartMotor:
       LI R0,MotorSound
       LI R1,MOTOR
       MOV *R0+,*R1+
       MOV *R0+,*R1+
       MOV *R0+,*R1+
       MOV *R0+,*R1+

SetMotor:
       MOV @SPEED,R10
       ;JEQ StopMotor
       SRL R10,2
       SLA R10,1
       AI R10,MotorFreqs-((MINSPD/4)*2)
       C *R10,@MOTORA
       JNE !
       MOV R11,R10
       B @BankSwitch0  ; return if no change
!
       MOV *R10,@MOTORA
       JMP !
StopMotor:
       LI R10,>FF00
       MOVB R10,@MOTORV
!      LI R10,x#SetMotorSound
       B @BankSwitch0



IntroMusic:
       BCOPY 'sfx_3.dat'
MainMusic:
       BCOPY 'sfx_4.dat'
       BYTE (MainMusic&>FF00)/>100,MainMusic&>FF
FinishMusic:
       BCOPY 'sfx_5.dat'
       BYTE 0,0
SilenceMusic:
       BYTE 2,>9F,>BF,0,0,0

* Sounds in order from high to low priority
CrashSound:
       BCOPY 'sfx_2.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
SplashSound:
       BCOPY 'sfx_1.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
JumpingSound:
       BCOPY 'sfx_6.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
DumpRockSound:
       BCOPY 'sfx_10.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
WarningBeep:
       BCOPY 'sfx_7.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
SmashSound:
       BCOPY 'sfx_8.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
ExLifeSound:
       BCOPY 'sfx_11.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
BounceSound:
       BCOPY 'sfx_9.dat'
       BYTE (MOTOR&>FF00)/>100,MOTOR&>FF
