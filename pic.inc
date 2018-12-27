	NOLIST

;	Collection of macros for Microchip PIC low-end & miidle-range
;	microcontrollers.
;
;	Copyright (C) 2018 Andrey Dobrovolsky
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.

	#include "../ram.inc"

;##############################################
;
;	1. RAM allocation macros.
;
;##############################################

;	Define variable <Name> and allocate for it <Size> bytes in bank <Bank> 

BALLOC	MACRO Name,Size,Bank

	IF (_BANK#v(Bank)FREE<(Size))
	ERROR BALLOC Name[Size]
	ELSE
Name	equ _RAMBOTTOM#v(Bank)
_RAMBOTTOM#v(Bank)	set _RAMBOTTOM#v(Bank)+(Size)
_BANK#v(Bank)FREE		set _BANK#v(Bank)FREE-(Size)
	ENDIF

	ENDM


;	Define variable <Name> and allocate for it <Size> bytes in bank 0

ALLOC	MACRO Name,Size

	BALLOC Name,Size,0

	ENDM


;	Define variable <Name> and allocate for it <Size> bytes in bank >0.
;	Selecting bank with minimal sufficient free space in order to
;	preserve maximal continuos free space available.

IALLOC	MACRO Name,Size

	LOCAL i,_SELECTEDBANK,_SELECTEDBANKFREE

_SELECTEDBANK		set 0
_SELECTEDBANKFREE	set 0xffff

i	= 1

	WHILE (i<_RAMBANKTOTAL)
	IF (((Size)<=_BANK#v(i)FREE) && (_BANK#v(i)FREE<_SELECTEDBANKFREE))
_SELECTEDBANK		set i
_SELECTEDBANKFREE	set _BANK#v(i)FREE
	ENDIF
i	= i+1
	ENDW

	BALLOC Name,Size,_SELECTEDBANK

	ENDM


;	Define variable <Name> and allocate for it <Size> bytes in overlayed RAM

OALLOC	MACRO Name,Size

	IF ((_OVRRAMBOTTOM+(Size)) > _OVRRAMTOP)
	Interrupt entry codeERROR OALLOC Name[Size]
	ELSE
Name		equ _OVRRAMBOTTOM
_OVRRAMBOTTOM	set _OVRRAMBOTTOM+(Size)
	ENDIF

	ENDM


SelectIBank	MACRO Addr
	IF (_RAMBANKTOTAL>2)
	bankisel Addr
	ENDIF
	ENDM


SetRAMPtr	MACRO Base,Index,SizeOf

	LOCAL i

	SelectIBank Base
	movlw low(Base)
i	= 0
	WHILE (i<SizeOf)
	addwf Index,W
i	= i+1
	ENDW
	movwf FSR

	ENDM



;	to be executed immediately after interrupt enrty

SaveContext	MACRO

	OALLOC W_BUF,1
	OALLOC STATUS_BUF,1

	movwf W_BUF
	swapf STATUS,W
	clrf STATUS
	movwf STATUS_BUF

	ENDM

;	to be executed before leave interrupt routine

RestoreContext	MACRO

	swapf STATUS_BUF,W
	movwf STATUS
	swapf W_BUF,f
	swapf W_BUF,W

	ENDM


;###################################################
;
;	2. Boolean (bit) operations
;
;###################################################

BITSPERBYTE	equ 8

;	Define boolean variable <name> as <bitn>'s bit of byte <addr>

boolean	MACRO name,addr,bitn

	IF ((addr)>=_OVRRAMTOP)
	ERROR boolean name,addr,bitn : illegal adress
	ELSE
	IF ((bitn)>=BITSPERBYTE)
	ERROR boolean name,addr,bitn : illegal bit number
	ELSE
	CONSTANT name=((addr)<<BITSPERBYTE)|(bitn)
	ENDIF
	ENDIF

	ENDM


clrb	MACRO bool	; bool = 0 (false)
	bcf high(bool),low(bool)
	ENDM	


setb	MACRO bool	; bool = 1 (true)
	bsf high(bool),low(bool)
	ENDM


sc	MACRO bool	; skip next command if bool == 0
	btfsc high(bool),low(bool)
	ENDM


ss	MACRO bool	; skip next command if bool == 1
	btfss high(bool),low(bool)
	ENDM


andb	MACRO dst,op	; dst &= op

	IF ((dst)!=(op))
	ss op
	clrb dst
	ENDIF

	ENDM


andnotb	MACRO dst,op	; dst &= (not op)

	IF ((dst)!=(op))
	sc op
	ENDIF
	clrb dst

	ENDM


iorb	MACRO dst,op	; dst |= op

	IF ((dst)!=(op))
	sc op
	setb dst
	ENDIF

	ENDM


iornotb	MACRO dst,op	; dst |= (not op)

	IF ((dst)!=(op))
	ss op
	ENDIF
	setb dst

	ENDM


xorb	MACRO dst,op	; dst ^= op , USES W!!!
	
	movlw (1 <<  low(dst) )
	sc op
	xorwf high(dst),f

	ENDM


xornotb	MACRO dst,op	; dst ^= (not op) , USES W!!!
	
	movlw (1 <<  low(dst) )
	ss op
	xorwf high(dst),f

	ENDM


movb	MACRO dst,src	; dst = src

	IF ((dst)!=(src))
	clrb dst
	iorb dst,src
	ENDIF

	ENDM


movnotb	MACRO dst,src	; dst = (not src) , MAY USE W!!!

	IF ((dst)!=(src))
	clrb dst
	iornotb dst,src
	ELSE
	movlw 1 << low(dst)
	xorwf high(dst),F
	ENDIF

	ENDM


;	Let's pay tribute to MCS-51!

jnb	MACRO bool,label	; jump <label> if bool == 0

	ss bool
	goto label

	ENDM


jb	MACRO bool,label	; jump <label> if bool == 1

	sc bool
	goto label

	ENDM


djnz	MACRO cnt,label		; decrement <cnt>, jump <label> if <cnt> != 0

	decfsz cnt,F
	goto label

	ENDM


LDIV	MACRO Res,Op1,Op2

Res	= ((((Op1)*2)+(Op2))/((Op2)*2))

	ENDM


	#include "../optempl.inc"

	; Arythmetic macros

;###################################################
;
;	3. Byte-oriented macros
;
;###################################################

mov	MACRO dst,src		; dst = src
	IF ((dst)!=(src))
	movf src,W
	movwf dst
	ENDIF
	ENDM


movl	MACRO dst,lit		; dst = lit(eral)
	IF (low(lit)==0)
	clrf dst
	ELSE
	movlw low(lit)
	movwf dst
	ENDIF
	ENDM

;---------------------------------------------------------------------
;	Two-operand forms for some simple assembler instructions.
;	Evaluate to single instructions in case dst == src
;---------------------------------------------------------------------

com2	MACRO dst,src	; dst = ~src
	Op2Template dst,comf,src,f
	ENDM

swap2	MACRO dst,src	; dst = swap(src)
	Op2Template dst,swapf,src,f
	ENDM

inc2	MACRO dst,src	; dst = src + 1
	Op2Template dst,incf,src,f
	ENDM

dec2	MACRO dst,src	; dst = src - 1
	Op2Template dst,decf,src,f
	ENDM

rlc2	MACRO dst,src	; dst = src << 1 (through CARRY)
	Op2Template dst,rlf,src,f
	ENDM

rrc2	MACRO dst,src	; dst = src >> 1 (through CARRY)
	Op2Template dst,rrf,src,f
	ENDM


;--------------------------------------------------------------------------
;	Byte logic (and,andnot,ior,iornot,xor,xornot).
;
;	Each operation has two forms: 3-operand (macro name ends with '3')
;	and 2-operand (with one more argument - res, wich can be 'f' or 'W').
;
;	For example:
;
;	ALLOC Var0,1
;	ALLOC Var1,1
;	ALLOC Var2,1
;
;	and3 Var2,Var0,Var1	; Var2 = Var0 & Var1
;	and  Var2,Var0,f	; Var2 &= Var0
;	and  Var2,Var0,W	; W = Var2 & Var0
;
;	For literals:
;
;	andl3 Var2,Var0,Lit1	; Var2 = Var0 & #v(Lit1)
;	andl  Var2,Lit1,f	; Var2 &= #v(Lit1)
;	andl  Var2,Lit0,W	; W = Var2 & #v(Lit0)
;	
;--------------------------------------------------------------------------

and3	MACRO dst,src,op	; dst = src & op
	movf op,W
	Op2Template dst,andwf,src,f
	ENDM

and	MACRO dst,op,res
	movf op,W
	Op2Template dst,andwf,dst,res
	ENDM

andnot3	MACRO dst,src,op	; dst = src & (~op)
	comf op,W
	Op2Template dst,andwf,src,f
	ENDM

andnot	MACRO dst,op,res
	comf op,W
	Op2Template dst,andwf,dst,res
	ENDM

andl3	MACRO dst,src,lit	; dst = src & #v(lit)
	movlw low(lit)
	Op2Template dst,andwf,src,f
	ENDM

andl	MACRO dst,lit,res
	movlw low(lit)
	Op2Template dst,andwf,dst,res
	ENDM


ior3	MACRO dst,src,op	; dst = src | op
	movf op,W
	Op2Template dst,iorwf,src,f
	ENDM

ior	MACRO dst,op,res
	movf op,W
	Op2Template dst,iorwf,dst,res
	ENDM

iornot3	MACRO dst,src,op	; dst = src | (~op)
	comf op,W
	Op2Template dst,iorwf,src,f
	ENDM

iornot	MACRO dst,op,res
	comf op,W
	Op2Template dst,iorwf,dst,res
	ENDM

iorl3	MACRO dst,src,lit	; dst = src | #v(lit)
	movlw low(lit)
	Op2Template dst,iorwf,src,f
	ENDM

iorl	MACRO dst,lit,res
	movlw low(lit)
	Op2Template dst,iorwf,dst,res
	ENDM


xor3	MACRO dst,src,op	; dst = src ^ op
	movf op,W
	Op2Template dst,xorwf,src,f
	ENDM

xor	MACRO dst,op,res
	movf op,W
	Op2Template dst,xorwf,dst,res
	ENDM

xornot3	MACRO dst,src,op	; dst = src ^ (~op)
	comf op,W
	Op2Template dst,xorwf,src,f
	ENDM

xornot	MACRO dst,op,res
	comf op,W
	Op2Template dst,xorwf,dst,res
	ENDM

xorl3	MACRO dst,src,lit	; dst = src ^ #v(lit)
	movlw low(lit)
	Op2Template dst,xorwf,src,f
	ENDM

xorl	MACRO dst,lit,res
	movlw low(lit)
	Op2Template dst,xorwf,dst,res
	ENDM

;----------------------
;	Negate
;----------------------

neg2	MACRO dst,src		; dst = - src
	com2 dst,src
	incf dst,f
	ENDM

neg	MACRO dst
	neg2 dst,dst
	ENDM

;-------------------------------------------------------------------------
;	Addition
;
;	add - add not using carry bit
;	addc - add using carry bit
;	addl - add literal to variable not using carry bit
;	addcl - add literal to variable using carry bit
;
;	Each operation has 3-operand form, wich make no overhead in case
;	dst == src. Thus "add3 A,A,B" produces the same code as "add A,B".
;
;	Trailing '_nc' in macro name means, that macro doesn't produce
;	carry bit for ongoing commands. May be useful to save few cycles.
;-------------------------------------------------------------------------

add3	MACRO dst,src,op	; dst = src + op
	movf op,W
	Op2Template dst,addwf,src,f
	ENDM

add	MACRO dst,op		; dst += op
	movf op,W
	addwf dst,f
	ENDM

addc3	MACRO dst,src,op	; dst = src + op + CARRY
	mov dst,src
	addc dst,op
	ENDM

addc	MACRO dst,op		; dst += (op + CARRY)
	movf op,W
	skpnc
	incfsz op,W
	addwf dst,f
	ENDM


;	Addition with literals

addl3	MACRO dst,src,lit	; dst = op + #v(lit)
	IF (low(lit)==0)
	mov dst,src
	clrc
	ELSE
	movlw low(lit)
	Op2Template dst,addwf,src,f
	ENDIF
	ENDM

addl	MACRO dst,lit		; dst += #v(lit)
	addl3 dst,dst,lit
	ENDM

addl3_nc	MACRO dst,src,lit
	IF (low(lit)==0)
	mov dst,src
	ENDIF
	IF (low(lit)==1)
	inc2 dst,src
	ENDIF
	IF (low(lit)==0xff)
	dec2 dst,src
	ENDIF
	IF ((low(lit)>1)&&(low(lit)<0xff))
	addl3 dst,src,lit
	ENDIF
	ENDM

addl_nc	MACRO dst,lit
	addl3_nc dst,dst,lit
	ENDM

addcl3	MACRO dst,src,lit	; dst = src + #v(lit) + CARRY
	mov dst,src
	addcl dst,lit
	ENDM

addcl	MACRO dst,lit		; dst += (#v(lit) + CARRY)
	movlw low(lit)
	IF low(lit)==0xff
	skpc
	ELSE
	skpnc
	movlw low(lit)+1
	ENDIF
	addwf dst,f
	ENDM

addcl3_nc	MACRO dst,src,lit
	mov dst,src
	addcl_nc dst,lit
	ENDM

addcl_nc	MACRO dst,lit
	IF (low(lit)==0)
	skpnc
	incf dst,f
	ENDIF
	IF (low(lit)==0xff)
	skpc
	decf dst,f
	ENDIF
	IF ((low(lit)>0)&&(low(lit)<0xff))
	addcl dst,lit
	ENDIF
	ENDM



;----------------------------------------------------------------------
;	Subtraction
;
;	sub - subtract
;	cmp - compare
;	subb - subtract using BORROW
;	cmpb - compare using BORROW
;
;	And so on with literals : subl,cmpl,subbl,cmpbl.
;
;	BORROW = 1 - CARRY
;
;	Trailing '_nc' in macro name means, that macro doesn't produce
;	carry bit for ongoing commands. May be useful to save few cycles.
;----------------------------------------------------------------------

sub3	MACRO dst,src,op	; dst = src - op
	movf op,W
	Op2Template dst,subwf,src,f
	ENDM

_sub	MACRO dst,op,res
	movf op,W
	subwf dst,res
	ENDM

sub	MACRO dst,op		; dst -= op
	_sub dst,op,f
	ENDM

cmp	MACRO what,with		; W = dst - op
	_sub what,with,W
	ENDM

subb3	MACRO dst,src,op	; dst = src - op - BORROW
	mov dst,src
	subb dst,op
	ENDM

_subb	MACRO dst,op,res
	movf op,W
	skpc
	incfsz op,W
	subwf dst,res
	ENDM

subb	MACRO dst,op		; dst -= (op + BORROW)
	_subb dst,op,f
	ENDM

cmpb	MACRO what,with		; W = dst - op - BORROW
	_subb what,with,W
	ENDM


;	Subtraction with literals

_subl3	MACRO dst,src,lit,res
	IF (low(lit)==0)
	mov dst,src
	setc
	ELSE
	movlw low(lit)
	Op2Template dst,subwf,src,res
	ENDIF
	ENDM

subl3	MACRO dst,src,lit	; dst = src - #v(lit)
	_subl3 dst,src,lit,f
	ENDM

subl	MACRO dst,lit		; dst -= #v(lit)
	_subl3 dst,dst,lit,f
	ENDM

cmpl	MACRO what,lit		; W = dst - #v(lit)
	_subl3 what,what,lit,W
	ENDM


subl3_nc	MACRO dst,src,lit
	IF (low(lit)==0)
	mov dst,src
	ENDIF
	IF (low(lit)==1)
	dec2 dst,src
	ENDIF
	IF (low(lit)==0xff)
	inc2 dst,src
	ENDIF
	IF ((low(lit)>1)&&(low(lit)<0xff))
	subl3 dst,src,lit
	ENDIF
	ENDM

subl_nc	MACRO dst,lit
	subl3_nc dst,dst,lit
	ENDM


subbl3	MACRO dst,src,lit	; dst = src - #v(lit) - BORROW
	mov dst,src
	subbl dst,lit
	ENDM

_subbl	MACRO dst,lit,res
	movlw low(lit)
	IF low(lit)==0xff
	skpnc
	ELSE
	skpc
	movlw low(lit)+1
	ENDIF
	subwf dst,res
	ENDM

subbl	MACRO dst,lit		; dst -= (#v(lit) + BORROW)
	_subbl dst,lit,f
	ENDM

cmpbl	MACRO what,lit		; W = dst - #v(lit) - BORROW
	_subbl what,lit,W
	ENDM

subbl3_nc	MACRO dst,src,lit
	mov dst,src
	subbl_nc dst,lit
	ENDM


subbl_nc	MACRO dst,lit
	IF (low(lit)==0)
	skpc
	decf dst,f
	ENDIF
	IF (low(lit)==0xff)
	skpnc
	incf dst,f
	ENDIF
	IF ((low(lit)>0)&&(low(lit)<0xff))
	subbl dst,lit
	ENDIF
	ENDM



;#######################################################################
;
;	4. Word-oriented macros.
;
;	Include 'w' in name immediately after operation name.
;
;	Words are 2-byte variables with lower byte having lower address.
;	So word address is address of its lower byte.
;
;#######################################################################
	
movw	MACRO dst,src		; dst = src
	mov dst,src
	mov dst+1,src+1 
	ENDM


movwl	MACRO dst,lit		; dst = #v(lit)
	movl dst,low(lit)
	movl dst+1,high(lit)
	ENDM


;----------------------------------------------------------------------------
;	Macros, which name ends with 'i' can accept 'INDF' operand as <dst>, 
;	<src> or <op>, assuming that FSR was set previously. Operand <shift>
;	may have values 0,1 or 2, wich means FSR shift after macro completion.
;	If FSR value doesn't matters, then use '1', it's 1 cycle faster :)
;----------------------------------------------------------------------------

movwi	MACRO dst,src,shift
	IF ((dst)!=(src))
	Op2wiTemplate dst,src,shift,mov,mov
	ENDIF
	ENDM


movwli	MACRO dst,lit,shift
	Op2wliTemplate dst,lit,shift,movl,movl
	ENDM


rlcw2	MACRO dst,src		; dst = src << 1 (trough CARRY)
	rlc2 dst,src
	rlc2 dst+1,src+1
	ENDM


rlcw	MACRO dst		; dst <<= 1 (through CARRY)
	rlcw2 dst,dst
	ENDM


rlcw2i	MACRO dst,src,shift
	Op2wiTemplate dst,src,shift,rlc2,rlc2
	ENDM


rlcwi	MACRO dst,shift
	Op2wiTemplate dst,dst,shift,rlc2,rlc2
	ENDM


rrcw2	MACRO dst,src		; dst = src >> 1 (through CARRY)
	rrc2 dst+1,src+1
	rrc2 dst,src
	ENDM


rrcw	MACRO dst		; dst >>= 1 (through CARRY)
	rrcw2 dst,dst
	ENDM


rrcw2i	MACRO dst,src,shift

	LOCAL doff,soff

	SelectOffset doff,dst
	SelectOffset soff,src

	FSRStep1 doff*soff
	rrc2 dst+doff,src+soff
	FSRStep3 doff*soff
	rrc2 dst,src
	FSRStep4 doff*soff,shift

	ENDM


rrcwi	MACRO dst,shift
	rrcw2i dst,dst,shift
	ENDM


incw2	MACRO dst,src	; dst = src + 1
	movw dst,src
	incw dst
	ENDM

incw	MACRO dst	; dst ++
	incf dst,f
	skpnz
	incf dst+1,f
	ENDM

;	incwz macro can be used for multibyte increments
;
;	Example of 8-byte variable increment:
;
;	ALLOC VeryLong,8
;
;	incw VeryLong
;	incwz VeryLong+2
;	incwz VeryLong+4
;	incwz VeryLong+6

incwz	MACRO dst
	skpnz
	incw dst
	ENDM

incw2i	MACRO dst,src,shift
	movwi dst,src,0
	incwi dst,shift
	ENDM


incwi	MACRO dst,shift

	LOCAL doff

	SelectOffset doff,dst

	incf dst,f
	FSRStep5 doff
	skpnz
	incf dst+doff,f
	FSRStep6 doff,shift

	ENDM

incwzi	MACRO dst,shift
	skpnz
	incwi dst,shift
	ENDM


decw2	MACRO dst,src	; dst = src - 1
	movw dst,src
	decw dst
	ENDM


decw	MACRO dst	; dst --
	decf dst,f
	comf dst,W
	skpnz
	decf dst+1,f
	ENDM


comw2	MACRO dst,src	; complement
	com2 dst,src
	com2 dst+1,src+1
	ENDM

comw	MACRO dst
	comw2 dst,dst
	ENDM

comw2i	MACRO dst,src,shift
	Op2wiTemplate dst,src,shift,com2,com2
	ENDM

comwi	MACRO dst,shift
	comw2i dst,dst,shift
	ENDM


negw2	MACRO dst,src	; dst = - src
	comw2 dst,src
	incw dst
	ENDM

negw	MACRO dst	; dst = - dst
	negw2 dst,dst
	ENDM

negw2i	MACRO dst,src,shift
	comw2i dst,src,0
	incwi dst,shift
	ENDM

negwi	MACRO dst,shift
	negw2 dst,dst,shift
	ENDM

;---------------------------------------------------
;	Addition (word-oriented).
;---------------------------------------------------

addw3	MACRO dst,src,op	; 9
	add3 dst,src,op
	addc3 dst+1,src+1,op+1
	ENDM


addw	MACRO dst,op		; 6
	addw3 dst,dst,op
	ENDM


addw3i	MACRO dst,src,op,shift
	Op3wiTemplate dst,src,op,shift,add3,addc3
	ENDM


addwi	MACRO dst,op,shift
	Op2wiTemplate dst,op,shift,add,addc
	ENDM


addw3_nc	MACRO dst,src,op	; 8
	add3 dst+1,src+1,op+1
	add3 dst,src,op
	skpnc
	incf dst+1,f
	ENDM


addcw3	MACRO dst,src,op
	addc3 dst,src,op
	addc3 dst+1,src+1,op+1
	ENDM

addcw	MACRO dst,op
	addcw3 dst,dst,op
	ENDM


addcw3i	MACRO dst,src,op,shift
	Op3wiTemplate dst,src,op,shift,addc3,addc3
	ENDM


addcwi	MACRO dst,op,shift
	Op2wiTemplate dst,op,shift,addc,addc
	ENDM

;---------------------------------------------------
;	Addition with literals (word-oriented).
;---------------------------------------------------

addwl3	MACRO dst,src,lit	; 9,8,5

	IF (low(lit)==0)
	mov dst,src
	addl3 dst+1,src+1,high(lit)
	ELSE
	addl3 dst,src,low(lit)
	addcl3 dst+1,src+1,high(lit)
	ENDIF

	ENDM


addwl	MACRO dst,lit		; 6,5,2,1
	addwl3 dst,dst,lit
	ENDM


addwl3i	MACRO dst,src,lit,shift
	Op3wlizTemplate dst,src,lit,shift,addl3,addcl3
	ENDM

addwli	MACRO dst,lit,shift
	Op2wlizTemplate dst,lit,shift,addl,addcl
	ENDM


addwl3_nc	MACRO dst,src,lit	; 8,7,4

	IF (((lit)&0xffff)==0)
	movw dst,src
	ENDIF
	IF (((lit)&0xffff)==1)
	incw2 dst,src
	ENDIF
	IF (((lit)&0xffff)==0xffff)
	decw2 dst,src
	ENDIF
	IF ((((lit)&0xffff)>1)&&(((lit)&0xffff)<0xffff))
	addl3_nc dst+1,src+1,high(lit)
	IF (low(lit)==0)
	mov dst,src
	ELSE
	addl3 dst,src,low(lit)
	skpnc
	incf dst+1,f
	ENDIF
	ENDIF

	ENDM


addwl_nc	MACRO dst,lit		; 6-0
	addwl3_nc dst,dst,lit
	ENDM


addcwl3	MACRO dst,src,lit
	addcl3 dst,src,low(lit)
	addcl3 dst+1,src+1,high(lit)
	ENDM

addcwl	MACRO dst,lit
	addcwl3 dst,dst,lit
	ENDM


addcwl3i	MACRO dst,src,lit,shift
	Op3wliTemplate dst,src,lit,shift,addcl3,addcl3
	ENDM

addcwli	MACRO dst,lit,shift
	Op2wliTemplate dst,lit,shift,addcl,addcl
	ENDM

;-------------------------------------------------
;	Subtraction (word-oriented).
;-------------------------------------------------

subw3	MACRO dst,src,op	; 9
	sub3 dst,src,op
	subb3 dst+1,src+1,op+1
	ENDM

subw	MACRO dst,op		; 6
	subw3 dst,dst,op
	ENDM


subw3i	MACRO dst,src,op,shift
	Op3wiTemplate dst,src,op,shift,sub3,subb3
	ENDM


subwi	MACRO dst,op,shift
	Op2wiTemplate dst,op,shift,sub,subb
	ENDM


subw3_nc	MACRO dst,src,op	; 8
	sub3 dst+1,src+1,op+1
	sub3 dst,src,op
	skpc
	decf dst+1,f
	ENDM


subbw3	MACRO dst,src,op
	subb3 dst,src,op
	subb3 dst+1,src+1,op+1
	ENDM

subbw	MACRO dst,op
	subbw3 dst,dst,op
	ENDM


subbw3i	MACRO dst,src,op,shift
	Op3wiTemplate dst,src,op,shift,subb3,subb3
	ENDM


subbwi	MACRO dst,op,shift
	Op2wiTemplate dst,op,shift,subb,subb
	ENDM

;---------------------------------------------------------
;	Subtract of literals (word-oriented).
;---------------------------------------------------------

subwl3	MACRO dst,src,lit	; 9,8,5

	IF (low(lit)==0)
	mov dst,src
	subl3 dst+1,src+1,high(lit)
	ELSE
	subl3 dst,src,low(lit)
	subbl3 dst+1,src+1,high(lit)
	ENDIF

	ENDM


subwl	MACRO dst,lit		; 6,5
	subwl3 dst,dst,lit
	ENDM


subwl3i	MACRO dst,src,lit,shift
	Op3wlizTemplate dst,src,lit,shift,subl3,subbl3
	ENDM

subwli	MACRO dst,lit,shift
	Op2wlizTemplate dst,lit,shift,subl,subbl
	ENDM


subwl3_nc	MACRO dst,src,lit	; 8,7,4

	IF (((lit)&0xffff)==0)
	movw dst,src
	ENDIF
	IF (((lit)&0xffff)==1)
	decw2 dst,src
	ENDIF
	IF (((lit)&0xffff)==0xffff)
	incw2 dst,src
	ENDIF
	IF ((((lit)&0xffff)>1)&&(((lit)&0xffff)<0xffff))
	subl3_nc dst+1,src+1,high(lit)
	IF (low(lit)==0)
	mov dst,src
	ELSE
	subl3 dst,src,low(lit)
	skpc
	decf dst+1,f
	ENDIF
	ENDIF

	ENDM


subwl_nc	MACRO dst,lit		; 6-0
	subwl3_nc dst,dst,lit
	ENDM


subbwl3	MACRO dst,src,lit
	subbl3 dst,src,low(lit)
	subbl3 dst+1,src+1,high(lit)
	ENDM

subbwl	MACRO dst,lit
	subbwl3 dst,dst,lit
	ENDM


subbwl3i	MACRO dst,src,lit,shift
	Op3wliTemplate dst,src,lit,shift,subbl3,subbl3
	ENDM

subbwli	MACRO dst,lit,shift
	Op2wliTemplate dst,lit,shift,subbl,subbl
	ENDM


;-----------------------------------------------
;	Subtract special cases.
;-----------------------------------------------

subwb3_nc	MACRO dst,src,op3	; dst, src - words, op - byte
	sub3 dst,src,op3
	subbl3_nc dst+1,src+1,0
	ENDM

;--------------------------------------------------
;	Compare (word-oriented).
;--------------------------------------------------

cmpw	MACRO what,with		; 6
	cmp what,with
	cmpb what+1,with+1
	ENDM


cmpwl	MACRO what,lit		; 6,5
	IF (low(lit)==0)
	cmpl what+1,high(lit)
	ELSE
	cmpl what,low(lit)
	cmpbl what+1,high(lit)
	ENDIF
	ENDM


cmpwi	MACRO what,with,shift
	Op2wiTemplate what,with,shift,cmp,cmpb
	ENDM


cmpwli	MACRO what,lit,shift
	Op2wlizTemplate what,lit,shift,cmpl,cmpbl
	ENDM


;----------------------------------------------------
;	Compare using BORROW (word-oriented)
;----------------------------------------------------

cmpbw	MACRO what,with
	cmpb what,with
	cmpb what+1,with+1
	ENDM


cmpbwl	MACRO what,lit
	cmpbl what,low(lit)
	cmpbl what+1,high(lit)
	ENDM


cmpbwi	MACRO what,with,shift
	Op2wiTemplate what,with,shift,cmpb,cmpb
	ENDM


cmpbwli	MACRO what,lit,shift
	Op2wliTemplate what,lit,shift,cmpbl,cmpbl
	ENDM



;###################################################
;
;	5. Multiply
;
;###################################################

;	MultiplyByte - multiply W with one bit - <Op.BitN>, add to ResH and
;	rotate right through CARRY. if ResH == ResL then result is byte,
;	otherwise rotate ResL right too, so result is word.

MultiplyByte	MACRO Op,BitN,ResH,ResL

	btfsc Op,BitN	;#v(BitN)
	addwf ResH,f
	rrf ResH,f
	IF ((ResH)!=(ResL))
	rrf ResL,f
	ENDIF

	ENDM


;	FastMultBBW - multiply W and bit-field <BitH:BitL> of Op.
;	ResH must be initialized before macro invoking and may contain
;	some initial value other than 0.
;
;	For example:
;
;	ALLOC A,1
;	ALLOC B,2
;
;	movl B+1,d'31'
;	movl A,b'00011010'
;	movlw d'10'
;	FastMultBBW A,4,1,B+1,B
;
;	will multiply d'10' with b'1101'=d'13' and add to d'31'

FastMultBBW	MACRO Op,BitH,BitL,ResH,ResL	; ResH MUST BE != ResL

	LOCAL i

	clrf ResL

i	= BitL

	WHILE (i<=(BitH))
	MultiplyByte Op,i,ResH,ResL
i	= i + 1
	ENDW

	ENDM


;	MultiplyBBW - ResH:ResL = Op * W ( word = byte * byte)
;	Bit counter Cnt is initialized externally. If ResH == ResL then
;	lower byte is dropped off (it is a little faster).
;	Be carefull - Op is being destroyed!

MultiplyBBW	MACRO Op,ResH,ResL,Cnt	; if ResH==ResL result is byte

	LOCAL Loop

Loop:

	rrf Op,f

	MultiplyByte STATUS,C,ResH,ResL

	djnz Cnt,Loop			; (8,7) * CntVal

	ENDM


;	MultiplyWord is used by MultiplyWWD, seems to be useless alone.

MultiplyWord	MACRO Op1,BitN,Op2,Res	; 10,5

	LOCAL Skip

	btfss Op1,BitN		;#v(BitN)
	goto Skip

	addw Res,Op2

Skip:	rrcw Res

	ENDM


;	MultiplyWWD - ResH:ResL = Op1H:Op1L * Op2
;
;	ResH - word, ResL - word, Op1H - byte, Op1L - byte, Op2 - word.
;	Cnt initialized externally
;	ResL may be equal to ResH, lower word will be lost.
;	Op1 may be a byte (Op1H == Op1L), it is a little faster.

MultiplyWWD	MACRO Op1H,Op1L,Op2,ResH,ResL,Cnt

	LOCAL Loop

Loop:

	IF ((Op1H)!=(Op1L))
	rrf Op1H,f
	ENDIF
	rrf Op1L,f

	MultiplyWord STATUS,C,Op2,ResH

	IF ((ResH)!=(ResL))
	IF ((Op1H)!=(Op1L))
	rrf ResL+1,f
	ENDIF
	rrf ResL,f
	ENDIF

	djnz Cnt,Loop

	ENDM



;###################################################
;
;	6. Divide
;
;###################################################


;	DivB interpretation.
;
;	DividedH - integer (byte), DividedL - fraction (byte)
;	Divider - integer (byte)
;	Divider > DividedH
;	Result (byte)- fraction
;	Remainder is in DividedH
;
;	Cnt is loaded externally. Remember, that if Cnt < 8, than only
;	<(Cnt-1):0> bits of Result are meaningfull, the rest upper bits store
;	<(7-Cnt):0> bits of initial Result value, rotated Cnt bits left. Use
;	this, if you know how, otherwise clear this upper bits after macro
;	envoking.
;	DividedH == DividedL means that DividedL = 0.
;	Argument Bit7 can be set to 0, if you are absolutely sure, that 
;	Divider's higher bit is 0. Otherwise set this argument to 1.

DivB	MACRO DividedH,DividedL,Divider,Result,Cnt,Bit7

	LOCAL Loop

Loop:

	IF ((DividedH)==(DividedL))
	clrc
	ELSE
	rlf DividedL,f
	ENDIF
	rlf DividedH,f

	IF ((Bit7)==0)

	movf Divider,W
	subwf DividedH,W
	skpnc
	movwf DividedH

	rlf Result,f

	ELSE

	movf Divider,W
	skpc
	subwf DividedH,W

	rlf Result,f

	movf Divider,W
	btfsc Result,0
	subwf DividedH,f

	ENDIF

	djnz Cnt,Loop		; (12,10) * CntVal

	ENDM




;	DivW interpretation.
;
;	DividedH - integer (word), DividedL - fraction (word)
;	Divider - integer (word)
;	Divider > DividedH
;	ResultH:ResultL (word) - fraction
;	Remainder is in DividedH
;
;	Cnt is loaded externally.
;	If Cnt <= 8 you can use ResultH == ResultL, it is slightly faster,
;	but in this case DividedL is byte (if not equal to DividedH),
;	be carefull!
;	DividedH == DividedL means that DividedL = 0.
;	Argument Bit15 can be set to 0, if you are absolutely sure, that 
;	Divider's higher bit is 0. Otherwise set this argument to 1.

;	t=(21,16)
;	if(Bit15==1) t+=2


DivW	MACRO DividedH,DividedL,Divider,ResultH,ResultL,Cnt,Bit15

	LOCAL Loop,Skip,Skip2

Loop:

	IF ((DividedH)==(DividedL))
	clrc
	ELSE
	rlf DividedL,f
	IF ((ResultH)!=(ResultL))
	rlf DividedL+1,f
	ENDIF
	ENDIF

	rlcw DividedH

	IF ((Bit15)!=0)
	bc Skip
	ENDIF

	cmpw DividedH,Divider

Skip:

	rlf ResultL,f
	IF ((ResultH)!=(ResultL))
	rlf ResultH,f
	ENDIF

	btfss ResultL,0
	goto Skip2

	subw DividedH,Divider

Skip2:

	djnz Cnt,Loop

	ENDM



DivWRounding	MACRO Divided,Divider

	LOCAL Skip

	clrc
	rlcw Divided

	bc Skip

	cmpw Divided,Divider

Skip:

	ENDM


	LIST

