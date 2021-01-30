#pragma once
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

struct Mem
{
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM];

	Byte operator[](u32 Address) const {
		return Data[Address];
	}
	Byte& operator[](u32 Address) {
		return Data[Address];
	}

	void Initialise() {
		for (u32 i = 0; i < MAX_MEM; i++) Data[i] = 0;
	}
	void WriteWord(s32& Cycles, Word Value, u32 Address) {
		Data[Address] = Value & 0xFF;
		Data[Address + 1] = (Value >> 8);
		Cycles -= 2;
	}

};

struct CPU
{
	Word PC; // Program counter : The program counter is a 16 bit register which points to the next instruction to be executed. The value of program counter is modified automatically as instructions are executed.

	Word SP; // Stack Pointer : The processor 6502 supports a 256 byte stack located between $0100 and $01FF. The stack pointer is an 8 bit register and holds the low 8 bits of the next free location on the stack. The location of the stack is fixed and cannot be moved.

	Byte A, X, Y; // Accumulator, Index register X and Y

	Byte C : 1; // Carry flag
	Byte Z : 1; // Zero flag
	Byte I : 1; // Interrupt disable flag
	Byte D : 1; // Decimal flag
	Byte B : 1; // Break Command
	Byte V : 1; // Overflow flag
	Byte N : 1; // Negative flag

	void reset(Mem& memory) {
		PC = 0xFFFC;
		SP = 0x0100;
		C = Z = I = D = V = N = 0;
		A = X = Y = 0;
	}
	Byte FetchByte(s32& Cycles, Mem& memory) {
		Byte Data = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}
	Word FetchWord(s32& Cycles, Mem& memory) {
		// 6502 is little endian
		Word Data = memory[PC];
		PC++;
		Data |= (memory[PC] << 8);
		PC++;
		Cycles -= 2;
		// if yoy wanted to handle endianness you would have to swap bytes here if(PLATFORM_BIG_ENDIAN) SwapBytesInWordData()
		return Data;
	}

	Byte ReadByte(s32& Cycles, Byte Address, Mem& memory) {
		Byte Data = memory[Address];
		Cycles--;
		return Data;
	}

	static constexpr Byte
		INS_LDA_IM = 0xA9,
		INS_LDA_ZP = 0xA5,
		INS_LDA_ZPX = 0xB5,
		INS_LDA_AB = 0xAD,
		INS_LDA_ABX = 0xBD,
		INS_LDA_ABY = 0xB9,
		INS_LDA_INDX = 0xA1,
		INS_LDA_INDY = 0xB1,

		INS_JSR = 0x20;

	s32 execute(s32 Cycles, Mem& memory) {
		s32 CyclesRequested = Cycles;
		while (Cycles > 0)
		{
			Byte Ins = FetchByte(Cycles, memory);
			switch (Ins)
			{
			case INS_LDA_IM: {
				Byte value = FetchByte(Cycles, memory);
				A = value;
				LDASetStatus();
				break;
			}
			case INS_LDA_ZP: {
				Byte ZeroPageAddr = FetchByte(Cycles, memory);
				A = ReadByte(Cycles, ZeroPageAddr, memory);
				LDASetStatus();
				break;
			}
			case INS_LDA_ZPX: {
				Byte ZeroPageAddr = FetchByte(Cycles, memory);
				ZeroPageAddr += X;
				Cycles--;
				A = ReadByte(Cycles, ZeroPageAddr, memory);
				LDASetStatus();
				break;
			}
			case INS_JSR: {
				Word SubAddr = FetchWord(Cycles, memory);
				memory.WriteWord(Cycles, PC - 1, SP);
				PC = SubAddr;
				SP += 2;
				Cycles--;
				break;
			}
			/*case INS_LDA_AB: {
				Word AbsAddress = FetchWord(Cycles, memory);
				ReadByte(Cycles, AbsAddress, memory);
			}*/
			default:
			{std::cout << "Instruction Not handled" << Ins << std::endl; break; }
			}
		}
		return CyclesRequested - Cycles;
	}
	void LDASetStatus()
	{
		Z = (A == 0);
		N = (A & 0b10000000) > 0;
	}

};
