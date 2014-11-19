	LDA	0,i
	STBYTEA	0x0000,d
	LDBYTEA	0xFFFF,d
	STBYTEA	lastByte,d
	LDA	0xFFFD,d
	STA	oldWord,d
	LDA	0x8080,i
	STA	0xFFFD,d
	STA	0xFFFF,d
	LDBYTEA	0x0000,d
	ANDA	0xFF,i
	CPA	0x80,i
	BREQ	wrapped
	CPA	0x00,i
	BREQ	overRAM
	BR	noRAM
wrapped:STRO	aWrap,d
	BR	checkROM
overRAM:STRO	aOverRAM,d
	BR	checkROM
noRAM:	STRO	aNoRAM,d
checkROM:LDA	0xFFFD,d
	CPA	oldWord,d
	BREQ	noROM
	STRO	aOverROM,d
	STOP
noROM:	STRO	aNoROM,d
	STOP
lastByte:.BYTE	0
oldWord:.WORD	0
aWrap:	.ASCII	"0xFFFF write wraps\n\x00"
aOverRAM:.ASCII	"RAM overwrites\n\x00"
aNoRAM:	.ASCII	"RAM is R/O\n\x00"
aOverROM:.ASCII	"ROM overwrites\n\x00"
aNoROM:	.ASCII	"ROM is R/O\n\x00"
	.END
