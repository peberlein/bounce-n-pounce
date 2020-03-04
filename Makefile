AS:=../xdt99-master/xas99.py
DM:=../xdt99-master/xdm99.py

TOOLS:=../legend/tools/$(shell uname -m)-$(shell uname -s)
DAN2:=$(TOOLS)/dan2
MAG:=$(TOOLS)/mag
CH:=$(TOOLS)/ch
CO:=$(TOOLS)/co
SP:=$(TOOLS)/sp
MP:=$(TOOLS)/mp
BIN2ASM:=$(TOOLS)/bin2asm


# for writing binary data with gawk
export LC_ALL=C


DATA := bnp_chr.dat bnp_gfx.dat bnp_spr.dat \
	sfx_1.dat sfx_2.dat sfx_3.dat sfx_4.dat sfx_5.dat sfx_6.dat \
	sfx_7.dat sfx_8.dat sfx_9.dat sfx_10.dat sfx_11.dat

all: bnp_8.bin

bnp.rpk: layout.xml bnp_8.bin
	zip -q $@ $^

bnp_8.bin: bnp.asm bnp_data.asm bnp_seg.asm $(DATA)
	$(AS) -b -R $< -L bnp.lst
	cat bnp_6000_b0.bin bnp_6000_b1.bin bnp_6000_b2.bin bnp_6000_b3.bin > $@
	ls -l bnp_*.bin

bnp_ea5.dsk: bnp_ea5.asm bnp_data.asm bnp_seg.asm $(DATA)
	$(AS) -i -R $< -o BNP -L bnp_ea5.lst
	$(DM) $@ --initialize DSSD -n BNP_EA5
	$(DM) $@ -a BNP
	$(DM) $@ -a BNQ
	$(DM) $@ -a BNR
	$(DM) $@ -a BNS

bnp_chr.dat: bnp_new.mag
	( \
	gawk -F: -e '$$1=="CH" { if (i>=0 && i<96) { for(x=1;x<=16;x+=2) printf "%c",strtonum("0x" substr($$2,x,2)) } i++ }' $^ ; \
	gawk -F: -e '$$1=="CO" { if (i>=0 && i<96) { for(x=1;x<=16;x+=2) printf "%c",strtonum("0x" substr($$2,x,2)) } i++ }' $^ ; \
	) > $@

bnp_gfx.dat: bnp_new.mag
	( \
	gawk -F: -e '$$1=="CH" { if ((i>=96 && i<124) || (i>=128 && i<156) || (i>=160 && i<188) || (i>=192 && i<220) || (i>=224 && i<252)) { for(x=1;x<=16;x+=2) printf "%c",strtonum("0x" substr($$2,x,2)) } i++ }' $^ ; \
	gawk -F: -e '$$1=="CO" { if ((i>=96 && i<124) || (i>=128 && i<156) || (i>=160 && i<188) || (i>=192 && i<220) || (i>=224 && i<252)) { for(x=1;x<=16;x+=2) printf "%c",strtonum("0x" substr($$2,x,2)) } i++ }' $^ ; \
	) > $@

bnp_spr.dat: bnp_new.mag bnp_new2.mag bnp_new3.mag
	gawk -F: -e '$$1=="SP" { for(x=1;x<=64;x+=2) printf "%c",strtonum("0x" substr($$2,x,2)) }' $^ > $@

playmame:
	mame ti99_4a -cart bnp.rpk -w -nomax -nomouse

playbnj:
	mame bnj -w -nomax -nomouse -debug

sfx_1.dat: sfx
	./sfx -d 1 > $@
sfx_2.dat: sfx
	./sfx -d 2 > $@
sfx_3.dat: sfx
	./sfx -d 3 > $@
sfx_4.dat: sfx
	./sfx -d 4 > $@
sfx_5.dat: sfx
	./sfx -d 5 > $@
sfx_6.dat: sfx
	./sfx -d 6 > $@
sfx_7.dat: sfx
	./sfx -d 7 > $@
sfx_8.dat: sfx
	./sfx -d 8 > $@
sfx_9.dat: sfx
	./sfx -d 9 > $@
sfx_10.dat: sfx
	./sfx -d 10 > $@
sfx_11.dat: sfx
	./sfx -d 11 > $@


sfx: LDLIBS:=-lm
bnp_data.asm: track2
	./track2
