CROSS_HOST = i686-w64-mingw32-
CROSS_BUILD =
CC = cc
X =

all: acalls.txt webagr.res

webagr.s: webagr.exe
	$(CROSS_HOST)objdump -d webagr.exe >webagr.s

imports.txt: webagr.exe
	$(CROSS_HOST)objdump -x webagr.exe >imports.txt

idata.txt: webagr.exe
	$(CROSS_HOST)objdump -j .idata -s >idata.txt

idata.bin: webagr.exe
	$(CROSS_HOST)objcopy -O binary -j .idata webagr.exe idata.bin

rdata.bin: webagr.exe
	$(CROSS_HOST)objcopy -O binary -j .rdata webagr.exe rdata.bin

c_str.txt: rdata.bin
	strings -n 1 rdata.bin >c_str.txt

webagr.res32: webagr.exe
	$(CROSS_HOST)objcopy -O binary -j .rsrc webagr.exe webagr.res32

gthunk$(X): gthunk.c
	$(CROSS_BUILD)$(CC) gthunk.c -o gthunk$(X)

res32cnv$(X): res32cnv.c
	$(CROSS_BUILD)$(CC) res32cnv.c -o res32cnv$(X)

imap.txt: idata.bin gthunk$(X)
	./gthunk$(X) >imap.txt

webagr.res: webagr.res32 res32cnv$(X)
	./res32cnv$(X) 0x2d000 webagr.res32 >webagr.res

cnvfuncs.sh: imap.txt
	echo '#! /bin/sh' >cnvfuncs.sh
	echo sed '\\' >>cnvfuncs.sh
	cat imap.txt | \
	  awk 'BEGIN { FS = " -> "; OFS = "/"; } { print $$1, $$3; }' | \
	  sed -e 's/0x000/0x4/g' -e 's/^/  -e '\''s\//g' \
	    -e 's/$$/\/g'\'' \\/g' \
	  >>cnvfuncs.sh
	echo '  "$$1" >"$$2"' >>cnvfuncs.sh
	chmod +x cnvfuncs.sh

fnwebagr.s: webagr.s cnvfuncs.sh
	./cnvfuncs.sh webagr.s fnwebagr.s

calls.txt: fnwebagr.s
	grep -e 'call  *\*' fnwebagr.s >calls.txt

acalls.txt: calls.txt
	cut '-d:' -f2 calls.txt | sort | uniq >acalls.txt

clean:
	rm -f webagr.s idata.txt idata.bin imports.txt \
	  rdata.bin c_str.txt webagr.res32 \
	  gthunk$(X) res32cnv$(X) imap.txt webagr.res \
	  cnvfuncs.sh fnwebagr.s calls.txt acalls.txt
