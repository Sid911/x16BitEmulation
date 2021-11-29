# x16BitEmulation
This repository is basically me trying to emulate 6502 processor which is 16 bit fully on software level at a logical standpoint. This doesn't support any kind of graphical interface.
## Disclaimer
This main goal of this project is to understand how CPUs works by directly emulating one and to debug it by single stepping instructions. The code is meant to be readable and understandable without many comments but more for writing very cohesive testings using Gtest, a lot of things could be done better.
The emulator only shows you what's going on under the hood of a 6502 CPU, without displaying stuff graphically.

> This was built on Visual Studio 2019 but has been upgraded to Visual Studio 2022. Retargeting the solution shouldn't cause any issues because of very less dependencies (only nuget package for Google test is required which should be automatically installed).

![image](https://user-images.githubusercontent.com/27860105/143931641-c1876b47-4092-4b67-9ad7-5b44031f107b.png)

___________

### Currently supported intructions

#### Load/Store Operations
These instructions transfer a single byte between memory and one of the registers. Load operations set the negative (N) and zero (Z) flags depending on the value of transferred. Store operations do not affect the flag settings.

Done |Code | Full Form | Flags Changed
----- | ---- | ------------- | -------------
Yes   | LDA |	Load Accumulator | N,Z
Yes   | LDX	| Load X Register |	N,Z
Yes    | LDY |	Load Y Register |	N,Z
Yes    | STA |	Store Accumulator | None	 
Yes    | STX |	Store X Register | None
Yes    | STY |	Store Y Register | None 

#### Jumps & Calls
The following instructions modify the program counter causing a break to normal sequential execution. The JSR instruction pushes the old PC onto the stack before changing it to the new location allowing a subsequent RTS to return execution to the instruction after the call.

Done  | Code | Second Header | Flags Changed
----- | ---- | ------------- | -------------
Yes    | JMP	| Jump to another location	| None
Yes   | JSR	| Jump to a subroutine	| None
Yes    | RTS	| Return from subroutine	| none

#### Register Transfers
The contents of the X and Y registers can be moved to or from the accumulator, setting the negative (N) and zero (Z) flags as appropriate.

Done  | Code | Second Header              | Flags Changed
----- | ---- | -------------------------- | -------------
Yes   | TAX  |	Transfer accumulator to X |	N,Z
Yes   | TAY	 |  Transfer accumulator to Y |	N,Z
Yes   | TXA  |	Transfer X to accumulator |	N,Z
Yes   | TYA  |	Transfer Y to accumulator	| N,Z


#### Stack Operator
The 6502 microprocessor supports a 256 byte stack fixed between memory locations $0100 and $01FF. A special 8-bit register, S, is used to keep track of the next free byte of stack space. Pushing a byte on to the stack causes the value to be stored at the current free location (e.g. $0100,S) and then the stack pointer is post decremented. Pull operations reverse this procedure.

The stack register can only be accessed by transferring its value to or from the X register. Its value is automatically modified by push/pull instructions, subroutine calls and returns, interrupts and returns from interrupts.
Done  | Code | Second Header                  | Flags Changed
----- | ---- | ------------------------------ | -------------
Yes   | TSX  | Transfer stack pointer to X	  |   N,Z
Yes   | TXS	 |Transfer X to stack pointer	    |   None
Yes   | PHA	 |Push accumulator on stack	      |   None
Yes   | PHP	 |Push processor status on stack  |   None
Yes   | PLA	 |Pull accumulator from stack	    |   N,Z
Yes   | PLP	 |Pull processor status from stack|	All


#### Logical 
The following instructions perform logical operations on the contents of the accumulator and another value held in memory. The BIT instruction performs a logical AND to test the presence of bits in the memory value to set the flags but does not keep the result.

Done  | Code | Second Header                | Flags Changed
----- | ---- | ---------------------------- | -------------
Yes   | AND	 |Logical AND	                  |   N,Z
Yes   | EOR	 |Exclusive OR	                |   N,Z
Yes   | ORA	 |Logical Inclusive OR	        |   N,Z
Yes   | BIT	 |Bit Test	                    |   N,V,Z

#### Arithmetic
The arithmetic operations perform addition and subtraction on the contents of the accumulator. The compare operations allow the comparison of the accumulator and X or Y with memory values.
Done  | Code | Second Header                | Flags Changed
----- | ---- | ---------------------------- | -------------
Yes   | ADC	 |  Add with Carry	            | N,V,Z,C
Yes   | SBC	 |  Subtract with Carry	        | N,V,Z,C
Yes   | CMP	 |  Compare accumulator	        | N,Z,C
Yes   | CPX	 |  Compare X register	        | N,Z,C
Yes   | CPY	 |  Compare Y register	        | N,Z,C

#### Increments & Decrements
Increment or decrement a memory location or one of the X or Y registers by one setting the negative (N) and zero (Z) flags as appropriate,

Done  | Code | Second Header                | Flags Changed
----- | ---- | ---------------------------- | -------------
Yes   | INC	 |Increment a memory location	  | N,Z
Yes   | INX	 |Increment the X register	    | N,Z
Yes   | INY	 |Increment the Y register	    | N,Z
Yes   | DEC	 |Decrement a memory location	  | N,Z
Yes   | DEX	 |Decrement the X register	    | N,Z
Yes   | DEY	 |Decrement the Y register	    | N,Z


#### Shifts

Shift instructions cause the bits within either a memory location or the accumulator to be shifted by one bit position. The rotate instructions use the contents if the carry flag (C) to fill the vacant position generated by the shift and to catch the overflowing bit. The arithmetic and logical shifts shift in an appropriate 0 or 1 bit as appropriate but catch the overflow bit in the carry flag (C).

Done  | Code | Second Header                | Flags Changed
----- | ---- | ---------------------------- | -------------
Yes   | ASL	 |  Arithmetic Shift Left	      | N,Z,C
Yes   | LSR	 |  Logical Shift Right	        | N,Z,C
Yes   | ROL	 |  Rotate Left	                | N,Z,C
Yes   | ROR	 |  Rotate Right	              | N,Z,C


#### Branches

Branch instructions break the normal sequential flow of execution by changing the program counter if a specified condition is met. All the conditions are based on examining a single bit within the processor status.

Done  | Code | Second Header                 | Flags Changed
----- | ---- | ----------------------------- | -------------
Yes   | BCC	 |  Branch if carry flag clear	 | None
Yes   | BCS	 |  Branch if carry flag set	   | None
Yes   | BEQ	 |  Branch if zero flag set	     | None
Yes   | BMI	 |  Branch if negative flag set	 | None
Yes   | BNE	 |  Branch if zero flag clear	   | None
Yes   | BPL	 |  Branch if negative flag clear| None 
Yes   | BVC	 |  Branch if overflow flag clear| None	 

> Branch instructions use relative address to identify the target instruction if they are executed. As relative addresses are stored using a signed 8 bit byte the target instruction must be within 126 bytes before the branch or 128 bytes after the branch.

#### Status Flag Changes

The following instructions change the values of specific status flags.

Done  | Code | Second Header                | Flags Changed
----- | ---- | ---------------------------- | -------------
Yes   | CLC	 |  Clear carry flag	          | C
Yes   | CLD	 |  Clear decimal mode flag	    | D
Yes   | CLI	 |  Clear interrupt disable flag| I
Yes   | CLV	 |  Clear overflow flag	        | V       
Yes   | SEC	 |  Set carry flag	            | C
Yes   | SED	 |  Set decimal mode flag	      | D
Yes   | SEI	 |  Set interrupt disable flag	| I
__________
Currently the architecture and spec sheet can be found here https://cx16.dk/6502/
