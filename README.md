# PIC-macros
Macros for Microchip low-end and mid-range processor families. RAM allocation, arythmetics, port pins.

## What for?
Makes PIC .asm programs more readable and writable.

PIC assembler is simple and easy to learn and to start coding, due to compact instruction set.
But the dark side of this simplicity is large amount of code (lines), needed to express frequently used logical or arythmetical
constructions. The use of C compiler for processor lacking stack adressation abilities seems to be unefficient. Using macros
may be the right way to achieve assembler flexibility and efficiency along with readable code.
## Whom for?
If you don't want to use relocatable RAM variables, and don't want to place them manually.

If you want to work with bits as with variables.

If you need multibyte arythmetics maybe with indirectly addressed variables.

If you need to multiply/divide bytes/words/double-words.

If you want to work with port pins as with variables, don't worrying of their interfering each other.

## How?
Adjust "ram.inc" for your processor. Then,

        #include "../pic.inc"
    
and if you need port pins manipulation,

        #include "../pins.inc"
    
## Let's code!
        #define ARRAY_SIZE d'10'
        
        IALLOC Array,ARRAY_SIZE*2
        
        ; Some code changing Array values
        ; Now we will sum all Array values with result in Sum
        
        ALLOC Sum,4
        ALLOC Cnt,1

        movwl Sum,0             ; Sum = 0
        movwl Sum+2,0
        
        SetRAMPtr Array,0,0     ; FSR = #Array
        
        movl Cnt,ARRAY_SIZE     ; Cnt = #ARRAY_SIZE
        
    Loop:
    
        addwi Sum,INDF,2        ; Sum += *(FSR += 2)
        addcwl Sum+2,0
        
        djnz Cnt,Loop           ; if(( --Cnt) != 0 ) goto Loop
        
Now let's try boolean variables.

        ALLOC Bits,1
        
        boolean BitA,Bits,0
        boolean BitB,Bits,1
        boolean BitC,Bits,2
        
        clrb BitA
        setb BitB
        
        movnotb BitC,BitA
        andb BitC,BitB          ; BitC = (~BitA) & BitB

Now let's manipulate port pins.

        #include "../pins.inc"
        
        boolean PinA,PORTA,2
        boolean PinB,PORTB,1
        boolean PinC,PORTB,5
        boolean PinD,PORTC,2
        
        TurnIn PinA
        TurnOut PinB
        TurnOut PinC
        Turnin PinD
        
    Loop:
    
        CplPin PinB
        
        MovPinBuf PinC,PinA
        AndPinBuf PinC,PinD
    
        WritePortBuffers
        
        goto Loop
        
If a lot of pins are initialized, overhead may be eliminated.

        #include "../pins.inc"
        
        boolean PinA,PORTA,2
        boolean PinB,PORTB,1
        boolean PinC,PORTB,5
        boolean PinD,PORTC,2
        
        SelectTRISBank
        _TurnIn PinA
        _TurnOut PinB
        _TurnOut PinC
        _Turnin PinD
        UnselectTRISBank
        

## List of macros:

        ;	1. RAM allocation macros.
        ALLOC	MACRO Name,Size
        IALLOC	MACRO Name,Size
        OALLOC	MACRO Name,Size

        SetRAMPtr	MACRO Base,Index,SizeOf
        SaveContext	MACRO
        RestoreContext	MACRO

        ;	2. Boolean (bit) operations
        boolean	MACRO name,addr,bitn
        clrb	MACRO bool	; bool = 0 (false)
        setb	MACRO bool	; bool = 1 (true)
        sc	MACRO bool	; skip next command if bool == 0
        ss	MACRO bool	; skip next command if bool == 1
        andb	MACRO dst,op	; dst &= op
        andnotb	MACRO dst,op	; dst &= (not op)
        iorb	MACRO dst,op	; dst |= op
        iornotb	MACRO dst,op	; dst |= (not op)
        xorb	MACRO dst,op	; dst ^= op , USES W!!!
        xornotb	MACRO dst,op	; dst ^= (not op) , USES W!!!
        movb	MACRO dst,src	; dst = src
        movnotb	MACRO dst,src	; dst = (not src) , MAY USE W!!!
        jnb	MACRO bool,label	; jump <label> if bool == 0
        jb	MACRO bool,label	; jump <label> if bool == 1

        djnz	MACRO cnt,label		; decrement <cnt>, jump <label> if <cnt> != 0

        ;	3. Byte-oriented macros
        mov	MACRO dst,src		; dst = src
        movl	MACRO dst,lit		; dst = lit(eral)
        
        ;	Two-operand forms for some simple assembler instructions.
        com2	MACRO dst,src	; dst = ~src
        swap2	MACRO dst,src	; dst = swap(src)
        inc2	MACRO dst,src	; dst = src + 1
        dec2	MACRO dst,src	; dst = src - 1
        rlc2	MACRO dst,src	; dst = src << 1 (through CARRY)
        rrc2	MACRO dst,src	; dst = src >> 1 (through CARRY)

        ;	Byte logic (and,andnot,ior,iornot,xor,xornot).
        and3	MACRO dst,src,op	; dst = src & op
        and	MACRO dst,op,res
        andnot3	MACRO dst,src,op	; dst = src & (~op)
        andnot	MACRO dst,op,res
        andl3	MACRO dst,src,lit	; dst = src & #v(lit)
        andl	MACRO dst,lit,res
        ior3	MACRO dst,src,op	; dst = src | op
        ior	MACRO dst,op,res
        iornot3	MACRO dst,src,op	; dst = src | (~op)
        iornot	MACRO dst,op,res
        iorl3	MACRO dst,src,lit	; dst = src | #v(lit)
        iorl	MACRO dst,lit,res
        xor3	MACRO dst,src,op	; dst = src ^ op
        xor	MACRO dst,op,res
        xornot3	MACRO dst,src,op	; dst = src ^ (~op)
        xornot	MACRO dst,op,res
        xorl3	MACRO dst,src,lit	; dst = src ^ #v(lit)
        xorl	MACRO dst,lit,res

        ;	Negate
        neg2	MACRO dst,src		; dst = - src
        neg	MACRO dst
        
        ;	Addition
        add3	MACRO dst,src,op	; dst = src + op
        add	MACRO dst,op		; dst += op
        addc3	MACRO dst,src,op	; dst = src + op + CARRY
        addc	MACRO dst,op		; dst += (op + CARRY)
        
        ;	Addition with literals
        addl3	MACRO dst,src,lit	; dst = op + #v(lit)
        addl	MACRO dst,lit		; dst += #v(lit)
        addl3_nc	MACRO dst,src,lit
        addl_nc	MACRO dst,lit
        addcl3	MACRO dst,src,lit	; dst = src + #v(lit) + CARRY
        addcl	MACRO dst,lit		; dst += (#v(lit) + CARRY)
        addcl3_nc	MACRO dst,src,lit
        addcl_nc	MACRO dst,lit
        
        ;	Subtraction
        sub3	MACRO dst,src,op	; dst = src - op
        sub	MACRO dst,op		; dst -= op
        cmp	MACRO what,with		; W = dst - op
        subb3	MACRO dst,src,op	; dst = src - op - BORROW
        subb	MACRO dst,op		; dst -= (op + BORROW)
        cmpb	MACRO what,with		; W = dst - op - BORROW
        
        ;	Subtraction with literals
        subl3	MACRO dst,src,lit	; dst = src - #v(lit)
        subl	MACRO dst,lit		; dst -= #v(lit)
        cmpl	MACRO what,lit		; W = dst - #v(lit)
        subl3_nc	MACRO dst,src,lit
        subl_nc	MACRO dst,lit
        subbl3	MACRO dst,src,lit	; dst = src - #v(lit) - BORROW
        subbl	MACRO dst,lit		; dst -= (#v(lit) + BORROW)
        cmpbl	MACRO what,lit		; W = dst - #v(lit) - BORROW
        subbl3_nc	MACRO dst,src,lit
        subbl_nc	MACRO dst,lit
        
        ;	4. Word-oriented macros.
        movw	MACRO dst,src		; dst = src
        movwl	MACRO dst,lit		; dst = #v(lit)
        movwi	MACRO dst,src,shift
        movwli	MACRO dst,lit,shift
        rlcw2	MACRO dst,src		; dst = src << 1 (trough CARRY)
        rlcw	MACRO dst		; dst <<= 1 (through CARRY)
        rlcw2i	MACRO dst,src,shift
        rlcwi	MACRO dst,shift
        rrcw2	MACRO dst,src		; dst = src >> 1 (through CARRY)
        rrcw	MACRO dst		; dst >>= 1 (through CARRY)
        rrcw2i	MACRO dst,src,shift
        rrcwi	MACRO dst,shift
        incw2	MACRO dst,src	; dst = src + 1
        incw	MACRO dst	; dst ++
        incwz	MACRO dst
        incw2i	MACRO dst,src,shift
        incwi	MACRO dst,shift
        incwzi	MACRO dst,shift
        decw2	MACRO dst,src	; dst = src - 1
        decw	MACRO dst	; dst --
        comw2	MACRO dst,src	; complement
        comw	MACRO dst
        comw2i	MACRO dst,src,shift
        comwi	MACRO dst,shift
        negw2	MACRO dst,src	; dst = - src
        negw	MACRO dst	; dst = - dst
        negw2i	MACRO dst,src,shift
        negwi	MACRO dst,shift
        
        ;	Addition (word-oriented).
        addw3	MACRO dst,src,op	; 9
        addw	MACRO dst,op		; 6
        addw3i	MACRO dst,src,op,shift
        addwi	MACRO dst,op,shift
        addw3_nc	MACRO dst,src,op	; 8
        addcw3	MACRO dst,src,op
        addcw	MACRO dst,op
        addcw3i	MACRO dst,src,op,shift
        addcwi	MACRO dst,op,shift
        
        ;	Addition with literals (word-oriented).
        addwl3	MACRO dst,src,lit	; 9,8,5
        addwl	MACRO dst,lit		; 6,5,2,1
        addwl3i	MACRO dst,src,lit,shift
        addwli	MACRO dst,lit,shift
        addwl3_nc	MACRO dst,src,lit	; 8,7,4
        addwl_nc	MACRO dst,lit		; 6-0
        addcwl3	MACRO dst,src,lit
        addcwl	MACRO dst,lit
        addcwl3i	MACRO dst,src,lit,shift
        addcwli	MACRO dst,lit,shift
        
        ;	Subtraction (word-oriented).
        subw3	MACRO dst,src,op	; 9
        subw	MACRO dst,op		; 6
        subw3i	MACRO dst,src,op,shift
        subwi	MACRO dst,op,shift
        subw3_nc	MACRO dst,src,op	; 8
        subbw3	MACRO dst,src,op
        subbw	MACRO dst,op
        subbw3i	MACRO dst,src,op,shift
        subbwi	MACRO dst,op,shift
        
        ;	Subtract of literals (word-oriented).
        subwl3	MACRO dst,src,lit	; 9,8,5
        subwl	MACRO dst,lit		; 6,5
        subwl3i	MACRO dst,src,lit,shift
        subwli	MACRO dst,lit,shift
        subwl3_nc	MACRO dst,src,lit	; 8,7,4
        subwl_nc	MACRO dst,lit		; 6-0
        subbwl3	MACRO dst,src,lit
        subbwl	MACRO dst,lit
        subbwl3i	MACRO dst,src,lit,shift
        subbwli	MACRO dst,lit,shift
        
        ;	Subtract special cases.
        subwb3_nc	MACRO dst,src,op3	; dst, src - words, op - byte
        
        ;	Compare (word-oriented).
        cmpw	MACRO what,with		; 6
        cmpwl	MACRO what,lit		; 6,5
        cmpwi	MACRO what,with,shift
        cmpwli	MACRO what,lit,shift
        
        ;	Compare using BORROW (word-oriented)
        cmpbw	MACRO what,with
        cmpbwl	MACRO what,lit
        cmpbwi	MACRO what,with,shift
        cmpbwli	MACRO what,lit,shift
        
        ;	5. Multiply
        FastMultBBW	MACRO Op,BitH,BitL,ResH,ResL	; ResH MUST BE != ResL
        MultiplyBBW	MACRO Op,ResH,ResL,Cnt	; if ResH==ResL result is byte
        MultiplyWWD	MACRO Op1H,Op1L,Op2,ResH,ResL,Cnt
        
        ;	6. Divide
        DivB	MACRO DividedH,DividedL,Divider,Result,Cnt,Bit7
        DivW	MACRO DividedH,DividedL,Divider,ResultH,ResultL,Cnt,Bit15
        DivWRounding	MACRO Divided,Divider

        ;	Pin handling macros
        ClrPinBuf	MACRO pin
        SetPinBuf	MACRO pin
        CplPinBuf	MACRO pin
        AndPinBuf	MACRO pin,bool
        AndnotPinBuf	MACRO pin,bool
        IorPinBuf	MACRO pin,bool
        IornotPinBuf	MACRO pin,bool
        XorPinBuf	MACRO pin,bool
        XornotPinBuf	MACRO pin,bool
        MovPinBuf	MACRO pin,bool
        MovnotPinBuf	MACRO pin,bool
        
        WritePortBuffers	MACRO

        ClrPin	MACRO pin
        SetPin	MACRO pin
        CplPin	MACRO pin
        
        SelectTRISBank	MACRO
        UnselectTRISBank	MACRO
        _TurnIn		MACRO pin
        _TurnOut	MACRO pin
        TurnIn	MACRO pin
        TurnOut	MACRO pin
        

