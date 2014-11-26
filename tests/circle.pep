	DECI	radius,d
; For X = -radius to radius
	LDA	radius,d
	NEGA
	STA	x,d
	STA	y,d
	LDA	radius,d
	LDX	radius,d
	CALL	Multiply
	STA	rad2,d
loop:	LDA	x,d
	LDX	x,d
	CPA	0,i
	BRGT	pos1
	NEGA
	NEGX
pos1:	CALL	Multiply
	STA	acc,d
	LDA	y,d
	LDX	y,d
	CPA	0,i
	BRGT	pos2
	NEGA
	NEGX
pos2:	CALL	Multiply
	ADDA	acc,d
	CPA	rad2,d
	BRGT	greater
	CHARO	'#',i
	BR	merge
greater:CHARO	'.',i
merge:	LDA	x,d
	ADDA	1,i
	STA	x,d
	CPA	radius,d
	BRLE	loop
	CHARO	'\n',i
	LDA	radius,d
	NEGA
	STA	x,d
	LDA	y,d
	ADDA	1,i
	STA	y,d
	CPA	radius,d
	BRLE	loop
	STOP

radius:	.WORD	0
rad2:	.WORD	0
x:	.WORD	0
y:	.WORD	0
acc:	.WORD	0
;;; PROCEDURE Multiply
;;; X:A <- A*X
Multiply:SUBSP	10,i
	STX	0,s	; 0:  WORD n1 = X
	LDX	0,i	; X = 0
	STX	2,s	; 2: DWORD n2 = 0:A
	STA	4,s	; ...
	STX	6,s	; 6: DWORD res = 0
	STX	8,s	; ...
Mloop:	LDX	0,s	; X = n1&1
	ANDX	1,i	; ...
	BREQ	MNoAdd	; Jump if bit is clear
	LDX	8,s	; res.L += n2,L
	ADDX	4,s	; ...
	STX	8,s	; ...
	BRC	MCar	; Jump if overflow
	LDX	6,s	; res.H += n2.H
	ADDX	2,s	; ...
	STX	6,s	; ...
	BR	MNoAdd	; Jump to merge
MCar:	LDX	6,s	; res.H += n2.H + 1
	ADDX	1,i	; ...
	ADDX	2,s	; ...
	STX	6,s	; ...
MNoAdd:	LDX	0,s	; n1 /= 2
	ASRX		; ...
	STX	0,s	; ...
	LDA	4,s	; X:A = n2
	LDX	2,s	; ...
	ASLX		; X<<=1
	ASLA		; A<<=1
	BRC	MSh1	; X++ if carry
	SUBX	1,i	; ...
MSh1:	ADDX	1,i	; ...
	STX	2,s	; n2 = X:A
	STA	4,s	; ...
	LDX	0,s	; X = n1
	BRNE	Mloop	; Loop if !=0
	LDX	6,s	; X:A = res
	LDA	8,s	; ...
	ADDSP	10,i
	RET0
;;; END PROCEDURE
	.END
