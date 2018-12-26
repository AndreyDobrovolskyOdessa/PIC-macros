;	Operation template macros

Op2Template	MACRO dst,instr,src,res

	IF ((dst)==(src))
	instr src,res
	ELSE
	instr src,W
	movwf dst
	ENDIF

	ENDM


SelectOffset	MACRO off,addr
off	=1
	IF ((addr)==INDF)
off	=0
	ENDIF
	ENDM

FSRStep1	MACRO off
	IF ((off)==0)
	incf FSR,f
	ENDIF
	ENDM

FSRStep2	MACRO off,shift

	IF (((off)==0)&&(shift!=1))
	IF (shift==0)
	decf FSR,f
	ENDIF
	IF (shift==2)
	incf FSR,f
	ENDIF
	IF (shift>2)
	ERROR Illegal FSR offset in indirect operation : shift .
	ENDIF
	ENDIF

	ENDM

FSRStep3	MACRO off
	IF ((off)==0)
	decf FSR,f
	ENDIF
	ENDM

FSRStep4	MACRO off,shift

	IF (((off)==0)&&(shift!=0))
	IF (shift==1)
	incf FSR,f
	ENDIF
	IF (shift==2)
	incf FSR,f
	incf FSR,f
	ENDIF
	IF (shift>2)
	ERROR Illegal FSR offset in indirect operation : shift .
	ENDIF
	ENDIF

	ENDM

FSRStep5	MACRO off
	IF ((off)==0)
	incfsz FSR,f
	nop
	ENDIF
	ENDM

FSRStep6	MACRO off,shift

	IF (((off)==0)&&(shift!=1))
	IF (shift==0)
	decfsz FSR,f
	nop
	ENDIF
	IF (shift==2)
	incfsz FSR,f
	nop
	ENDIF
	IF (shift>2)
	ERROR Illegal FSR offset in indirect operation : shift .
	ENDIF
	ENDIF

	ENDM


Op2wiTemplate	MACRO dst,src,shift,op1,op2

	LOCAL doff,soff

	SelectOffset doff,dst
	SelectOffset soff,src

	op1 dst,src
	FSRStep1 doff*soff
	op2 dst+doff,src+soff
	FSRStep2 doff*soff,shift

	ENDM


Op2wliTemplate	MACRO dst,lit,shift,op1,op2

	LOCAL doff

	SelectOffset doff,dst

	op1 dst,low(lit)
	FSRStep1 doff
	op2 dst+doff,high(lit)
	FSRStep2 doff,shift

	ENDM


Op2wlizTemplate	MACRO dst,lit,shift,op1,op2

	LOCAL doff

	SelectOffset doff,dst

	IF (low(lit)==0)
	FSRStep1 doff
	op1 dst+doff,high(lit)
	FSRStep2 doff,shift
	ELSE
	Op2wliTemplate dst,lit,shift,op1,op2
	ENDIF

	ENDM


Op3wiTemplate	MACRO dst,src,op,shift,op1,op2

	LOCAL doff,soff,opoff

	SelectOffset doff,dst
	SelectOffset soff,src
	SelectOffset opoff,op

	op1 dst,src,op
	FSRStep1 doff*soff*opoff
	op2 dst+doff,src+soff,op+opoff
	FSRStep2 doff*soff*opoff,shift

	ENDM

Op3wliTemplate	MACRO dst,src,lit,shift,op1,op2

	LOCAL doff,soff

	SelectOffset doff,dst
	SelectOffset soff,src

	op1 dst,src,low(lit)
	FSRStep1 doff*soff
	op2 dst+doff,src+soff,high(lit)
	FSRStep2 doff*soff,shift

	ENDM


Op3wlizTemplate	MACRO dst,src,lit,shift,op1,op2

	LOCAL doff,soff

	SelectOffset doff,dst
	SelectOffset soff,src

	IF (low(lit)==0)
	mov dst,src
	FSRStep1 doff*soff
	op1 dst+doff,src+soff,high(lit)
	FSRStep2 doff*soff,shift
	ELSE
	Op3wliTemplate dst,src,lit,shift,op1,op2
	ENDIF

	ENDM



