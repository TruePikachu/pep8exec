	; Do all 8-bit additions and subtractions
	LDA	0,i
	STA	n1,d
	STA	n2,d
loop:	LDA	n1,d
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	STA	a1,d
	LDA	n2,d
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	STA	a2,d
	LDX	a1,d
	ADDX	a2,d
	MOVFLGA
	STA	flg,d
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	LDA	n1,d
	CALL	DByteA
	CHARO	'+',i
	LDA	n2,d
	CALL	DByteA
	CHARO	'=',i
	STX	tmp,d
	LDA	tmp,d
	CALL	DByteA
	CHARO	' ',i
	LDA	flg,d
	CALL	DispFlgA
	CHARO	'\n',i
	LDX	a1,d
	SUBX	a2,d
	MOVFLGA
	STA	flg,d
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	ASRX
	LDA	n1,d
	CALL	DByteA
	CHARO	'-',i
	LDA	n2,d
	CALL	DByteA
	CHARO	'=',i
	STX	tmp,d
	LDA	tmp,d
	CALL	DByteA
	CHARO	' ',i
	LDA	flg,d
	CALL	DispFlgA
	CHARO	'\n',i
	LDA	n1,d
	ADDA	0x10,i
	ANDA	0xFF,i
	STA	n1,d
	BRNE	loop
	LDA	n2,d
	ADDA	0x10,i
	ANDA	0xFF,i
	STA	n2,d
	BRNE	loop
	STOP
flg:	.WORD	0
tmp:	.WORD	0
n1:	.WORD	0
n2:	.WORD	0
a1:	.WORD	0
a2:	.WORD	0

;;; PROCEDURE DispFlgA
DispFlgA:SUBSP	4,i
	STA	0,s
	STX	2,s
	LDX	0,i
	RORA
	RORA
	RORA
	RORA
DFAloop:ROLA
	BRC	DFAbit
	CHARO	' ',i
	BR	DFAmrge
DFAbit:	CHARO	DFAflgs,x
DFAmrge:ADDX	1,i
	CPX	4,i
	BRNE	DFAloop
	LDX	2,s
	LDA	0,s
	RET4
DFAflgs:.ASCII	"NZVC"
;;; END PROCEDURE

;;; PROCEDURE DByteA
DByteA:	SUBSP	4,i
	STA	0,s
	STX	2,s
	LDX	0,s
	ASRX
	ASRX
	ASRX
	ASRX
	ANDX	0x0F,i
	CHARO	DBAhex,x
	LDX	0,s
	ANDX	0x0F,i
	CHARO	DBAhex,x
	LDX	2,s
	RET4
DBAhex:	.ASCII	"0123456789ABCDEF"
;;; END PROCEDURE

;;; PROCEDURE SgnExtA
SgnExtA:ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASLA
	ASRA
	ASRA
	ASRA
	ASRA
	ASRA
	ASRA
	ASRA
	ASRA
	RET0
;;; END PROCEDURE
	.END
