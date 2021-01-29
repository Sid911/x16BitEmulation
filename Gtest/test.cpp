#include "pch.h"

#include "emu_6502.h"
#include "test.h"

class M6502Test : public testing::Test {
public:
	Mem mem;
	CPU cpu;
	virtual void SetUp() {
		cpu.reset(mem);
	}
};

void VerfiyUnmodifiedFlags(const CPU& cpuCopy, const CPU& cpu)
{
	EXPECT_EQ(cpu.C, cpuCopy.C);
	EXPECT_EQ(cpu.I, cpuCopy.I);
	EXPECT_EQ(cpu.D, cpuCopy.D);
	EXPECT_EQ(cpu.B, cpuCopy.B);
	EXPECT_EQ(cpu.V, cpuCopy.V);
}

TEST_F(M6502Test, CPUdoesNothingIfZeroCyclesRequested) {

	//given:
	constexpr s32 NUM_CYCLES = 0;

	//when:
	s32 CyclesUsed = cpu.execute(NUM_CYCLES, mem);

	//then:
	EXPECT_EQ(CyclesUsed, 0);

}

TEST_F(M6502Test, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction) {

	//given:
	constexpr s32 NUM_CYCLES = 1;
	mem[0xFFFC] = CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x84;

	//when:
	s32 CyclesUsed = cpu.execute(NUM_CYCLES, mem);

	//then:
	EXPECT_EQ(CyclesUsed, 2);

}
TEST_F(M6502Test, TestExecutingABadInstructionDoesNotGoToInfiniteLoop) {

	//given:
	constexpr s32 NUM_CYCLES = 1;
	mem[0xFFFC] = 0x0;
	mem[0xFFFD] = 0x0;

	//when:
	s32 CyclesUsed = cpu.execute(NUM_CYCLES, mem);

	//then:
	EXPECT_EQ(CyclesUsed, 1);

}

TEST_F(M6502Test, TestLDAImmediateCanLoadToRegister) {
	mem[0xFFFC] = CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x84;

	CPU cpuCopy = cpu;
	cpu.execute(2, mem);

	EXPECT_EQ(cpu.A, 0x84);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
}
TEST_F(M6502Test, TestLDAImmediateCanEffectZeroFlag) {
	mem[0xFFFC] = CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x0;
	CPU cpuCopy = cpu;

	cpu.execute(2, mem);

	EXPECT_EQ(cpu.A, 0x0);
	EXPECT_TRUE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
}

TEST_F(M6502Test, TestLDAZeroPageCanLoadToRegister) {
	mem[0xFFFC] = CPU::INS_LDA_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	CPU cpuCopy = cpu;

	cpu.execute(3, mem);

	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
}

TEST_F(M6502Test, TestLDAZeroPageXCanLoadToRegister) {
	cpu.X = 5;

	mem[0xFFFC] = CPU::INS_LDA_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0047] = 0x39;
	CPU cpuCopy = cpu;

	cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x39);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
}

TEST_F(M6502Test, TestLDAZeroPageXCanLoadToRegister_Wrap_) {
	cpu.X = 0xFF;

	mem[0xFFFC] = CPU::INS_LDA_ZPX;
	mem[0xFFFD] = 0x80;
	mem[0x007F] = 0x37;
	CPU cpuCopy = cpu;

	cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
}

TEST_F(M6502Test, TestLDAAbsoluteCanLoadToRegister) {
	mem[0xFFFC] = CPU::INS_LDA_AB;
	mem[0xFFFD] = 0x80;
	mem[0x007F] = 0x44; 
	mem[0x4480] = 0x37;

	constexpr s32 EXPECTED_CYCLES = 4;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}

TEST_F(M6502Test, TestLDAAbsoluteXCanLoadToRegisterWhenCrossesPageBoundry) {
	cpu.X = 0xFF;

	mem[0xFFFC] = CPU::INS_LDA_ABX;
	mem[0xFFFD] = 0x02;
	mem[0x007F] = 0x44; 
	mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses Page boundry for extra 1 cycle

	constexpr s32 EXPECTED_CYCLES = 5;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}

TEST_F(M6502Test, TestLDAAbsoluteCanLoadToRegister) {
	mem[0xFFFC] = CPU::INS_LDA_AB;
	mem[0xFFFD] = 0x80;
	mem[0x007F] = 0x44;
	mem[0x4480] = 0x37;

	constexpr s32 EXPECTED_CYCLES = 4;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}

TEST_F(M6502Test, TestLDAAbsoluteXCanLoadToRegister) {
	cpu.X = 1;

	mem[0xFFFC] = CPU::INS_LDA_ABX;
	mem[0xFFFD] = 0x80;
	mem[0x007F] = 0x44; 
	mem[0x4480] = 0x37;

	constexpr s32 EXPECTED_CYCLES = 4;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}

TEST_F(M6502Test, TestLDAAbsoluteXCanLoadToRegisterWhenCrossesPageBoundry) {
	cpu.X = 0xFF;

	mem[0xFFFC] = CPU::INS_LDA_ABX;
	mem[0xFFFD] = 0x02;
	mem[0x007F] = 0x44;
	mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses Page boundry for extra 1 cycle

	constexpr s32 EXPECTED_CYCLES = 5;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}

//Y
TEST_F(M6502Test, TestLDAAbsoluteYCanLoadToRegister) {
	cpu.X = 1;

	mem[0xFFFC] = CPU::INS_LDA_ABY;
	mem[0xFFFD] = 0x80;
	mem[0x007F] = 0x44;
	mem[0x4480] = 0x37;

	constexpr s32 EXPECTED_CYCLES = 4;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}

TEST_F(M6502Test, TestLDAAbsoluteYCanLoadToRegisterWhenCrossesPageBoundry) {
	cpu.X = 0xFF;

	mem[0xFFFC] = CPU::INS_LDA_ABY;
	mem[0xFFFD] = 0x02;
	mem[0x007F] = 0x44;
	mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses Page boundry for extra 1 cycle

	constexpr s32 EXPECTED_CYCLES = 5;
	CPU cpuCopy = cpu;

	s32 CyclesUsed = cpu.execute(4, mem);
	EXPECT_EQ(cpu.A, 0x37);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyUnmodifiedFlags(cpuCopy, cpu);
	EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}