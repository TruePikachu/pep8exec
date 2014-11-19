	; Test file for reading input
	STRO	str0,d
	LDX	0,i
	LDA	0,i
rdLoop:	CHARI	inBuf,x
	LDBYTEA	inBuf,x
	ADDX	1,i
	CPX	256,i
	BREQ	tooLong
	CPA	'\x0D',i
	BREQ	numLoop
	CPA	'\x0A',i
	BRNE	rdLoop

numLoop:STRO	str2,d
	STX	numChar,d
	DECO	numChar,d
	STRO	str3,d
	DECI	numChar,d
	LDA	numChar,d
	STX	numChar,d
	CPA	numChar,d
	BRNE	wrong
	STRO	str5,d
	SUBX	1,i
	BR	outLoop
wrong:	STRO	str4,d
	BR	numLoop

outLoop:SUBX	1,i
	CHARO	inBuf,x
	BRNE	outLoop
	CHARO	'\n',i
	STOP

tooLong:STRO	str1,d
	STOP

str0:	.ASCII	"Enter text, terminate with newline: \x00"
str1:	.ASCII	"Ummm...that is too long...\x00"
str2:	.ASCII	"You entered \x00"
str3:	.ASCII	" characters...how many? \x00"
str4:	.ASCII	"IN"
str5:	.ASCII	"CORRECT!\n\x00"
numChar:.WORD	0
inBuf:	.BLOCK	255
	.BLOCK	1
	.END
