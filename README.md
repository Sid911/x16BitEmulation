# x16BitEmulation
This repository is basically me trying to emulate 6502 processor which is 16 bit fully on software level.
> This was built on Visual Studio 2019

#### Currently supported intructions

##### Load/Store Operations

Done |Code | Full Form | Flags Changed
----- | ---- | ------------- | -------------
Yes   | LDA |	Load Accumulator | N,Z
Yes   | LDX	| Load X Register |	N,Z
No    | LDY |	Load Y Register |	N,Z
No    | STA |	Store Accumulator | None	 
No    | STX |	Store X Register | None
No    | STY |	Store Y Register | None 

##### Jumps & Calls

Done  | Code | Second Header | Flags Changed
----- | ---- | ------------- | -------------
No    | JMP	| Jump to another location	| None
Yes   | JSR	| Jump to a subroutine	| None
No    | RTS	| Return from subroutine	| none
