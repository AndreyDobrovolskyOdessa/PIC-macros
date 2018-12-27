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
        


