
	NOLIST


	IFDEF GPIO
PORTA	equ GPIO
	ENDIF

_PortTotal	= 0

	IFDEF PORTA
_PortTotal	= 1
	ENDIF

	IFDEF PORTB
_PortTotal	= 2
	ENDIF

	IFDEF PORTC
_PortTotal	= 3
	ENDIF

	IFDEF PORTD
_PortTotal	= 4
	ENDIF

	IFDEF PORTE
_PortTotal	= 5
	ENDIF

	IFDEF PORTF
_PortTotal	= 6
	ENDIF


	OALLOC PORTABuf,_PortTotal


_PinBufOffset	equ ((PORTABuf-PORTA)<<8)

_PortMask	= 0


	;	Pin handling macros

; op is {clrb,setb,cplb,movb,movnotb,xorb,andb,iorb,andnotb,iornotb,xornotb}

WrPinBuf	MACRO op,pin,bool
	IF ((bool)==(-1))
	op pin+_PinBufOffset
	ELSE
	op pin+_PinBufOffset,bool
	ENDIF
_PortMask	= _PortMask | ( 1 << ( high(pin) - PORTA  ) )
	ENDM

ClrPinBuf	MACRO pin
	WrPinBuf clrb,pin,-1
	ENDM

SetPinBuf	MACRO pin
	WrPinBuf setb,pin,-1
	ENDM

CplPinBuf	MACRO pin
	WrPinBuf movnotb,pin,pin+_PinBufOffset
	ENDM

AndPinBuf	MACRO pin,bool
	WrPinBuf andb,pin,bool
	ENDM

AndnotPinBuf	MACRO pin,bool
	WrPinBuf andnotb,pin,bool
	ENDM

IorPinBuf	MACRO pin,bool
	WrPinBuf iorb,pin,bool
	ENDM

IornotPinBuf	MACRO pin,bool
	WrPinBuf iornotb,pin,bool
	ENDM

XorPinBuf	MACRO pin,bool
	WrPinBuf xorb,pin,bool
	ENDM

XornotPinBuf	MACRO pin,bool
	WrPinBuf xornotb,pin,bool
	ENDM

MovPinBuf	MACRO pin,bool
	WrPinBuf movb,pin,bool
	ENDM

MovnotPinBuf	MACRO pin,bool
	WrPinBuf movnotb,pin,bool
	ENDM



WritePinBuffer	MACRO pin
	mov high(pin),high((pin)+_PinBufOffset)
	ENDM


WritePortBuffers	MACRO
	LOCAL _PortN
_PortN	= PORTA
	WHILE ( _PortMask )
	IF ( _PortMask & 1 )
	WritePinBuffer _PortN << BITSPERBYTE
	ENDIF
_PortMask	= _PortMask >> 1
_PortN		= _PortN + 1
	ENDW
	ENDM


WrPin	MACRO op,pin,bool
	WrPinBuf op,pin,bool
	WritePortBuffers
	ENDM


ClrPin	MACRO pin
	WrPin clrb,pin,-1
	ENDM

SetPin	MACRO pin
	WrPin setb,pin,-1
	ENDM


CplPin	MACRO pin
	WrPin movnotb,pin,pin+_PinBufOffset
	ENDM


;	Pins direction handling

SelectTRISBank	MACRO
	bsf STATUS,RP0
	ENDM


UnselectTRISBank	MACRO
	bcf STATUS,RP0
	ENDM


_TurnIn		MACRO pin
	setb pin
	ENDM


_TurnOut	MACRO pin
	clrb pin
	ENDM


WrPinTRIS	MACRO op,pin
	SelectTRISBank
	op pin
	UnselectTRISBank
	ENDM


TurnIn	MACRO pin
	WrPinTRIS _TurnIn,pin
	ENDM


TurnOut	MACRO pin
	WrPinTRIS _TurnOut,pin
	ENDM


	LIST

