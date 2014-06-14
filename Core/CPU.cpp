#include "stdafx.h"
#include "CPU.h"
#include "Timer.h"

CPU::CPU(MemoryManager *memoryManager) : _memoryManager(memoryManager)
{
	Reset();
	Func opTable[] = { 
	//	0						1						2						3				4						5						6						7				8						9						A						B				C						D						E						F
		&CPU::BRK,			&CPU::ORA_IndX,	nullptr,				nullptr,		nullptr,				&CPU::ORA_Zero,	&CPU::ASL_Zero,	nullptr,		&CPU::PHP,			&CPU::ORA_Imm,		&CPU::ASL_Acc,		nullptr,		nullptr,				&CPU::ORA_Abs,		&CPU::ASL_Abs,		nullptr, //0
		&CPU::BPL,			&CPU::ORA_IndY,	nullptr,				nullptr,		nullptr,				&CPU::ORA_ZeroX,	&CPU::ASL_ZeroX,	nullptr,		&CPU::CLC,			&CPU::ORA_AbsY,	nullptr,				nullptr,		nullptr,				&CPU::ORA_AbsX,	&CPU::ASL_AbsX,	nullptr, //1
		&CPU::JSR,			&CPU::AND_IndX,	nullptr,				nullptr,		&CPU::BIT_Zero,	&CPU::AND_Zero,	&CPU::ROL_Zero,	nullptr,		&CPU::PLP,			&CPU::AND_Imm,		&CPU::ROL_Acc,		nullptr,		&CPU::BIT_Abs,		&CPU::AND_Abs,		&CPU::ROL_Abs,		nullptr, //2
		&CPU::BMI,			&CPU::AND_IndY,	nullptr,				nullptr,		nullptr,				&CPU::AND_ZeroX,	&CPU::ROL_ZeroX,	nullptr,		&CPU::SEC,			&CPU::AND_AbsY,	nullptr,				nullptr,		nullptr,				&CPU::AND_AbsX,	&CPU::ROL_AbsX,	nullptr, //3
		&CPU::RTI,			&CPU::EOR_IndX,	nullptr,				nullptr,		nullptr,				&CPU::EOR_Zero,	&CPU::LSR_Zero,	nullptr,		&CPU::PHA,			&CPU::EOR_Imm,		&CPU::LSR_Acc,		nullptr,		&CPU::JMP_Abs,		&CPU::EOR_Abs,		&CPU::LSR_Abs,		nullptr, //4
		&CPU::BVC,			&CPU::EOR_IndY,	nullptr,				nullptr,		nullptr,				&CPU::EOR_ZeroX,	&CPU::LSR_ZeroX,	nullptr,		&CPU::CLI,			&CPU::EOR_AbsY,	nullptr,				nullptr,		nullptr,				&CPU::EOR_AbsX,	&CPU::LSR_AbsX,	nullptr, //5
		&CPU::RTS,			&CPU::ADC_IndX,	nullptr,				nullptr,		nullptr,				&CPU::ADC_Zero,	&CPU::ROR_Zero,	nullptr,		&CPU::PLA,			&CPU::ADC_Imm,		&CPU::ROR_Acc,		nullptr,		&CPU::JMP_Ind,		&CPU::ADC_Abs,		&CPU::ROR_Abs,		nullptr, //6
		&CPU::BVS,			&CPU::ADC_IndY,	nullptr,				nullptr,		nullptr,				&CPU::ADC_ZeroX,	&CPU::ROR_ZeroX,	nullptr,		&CPU::SEI,			&CPU::ADC_AbsY,	nullptr,				nullptr,		nullptr,				&CPU::ADC_AbsX,	&CPU::ROR_AbsX,	nullptr, //7
		nullptr,				&CPU::STA_IndX,	nullptr,				nullptr,		&CPU::STY_Zero,	&CPU::STA_Zero,	&CPU::STX_Zero,	nullptr,		&CPU::DEY,			nullptr,				&CPU::TXA,			nullptr,		&CPU::STY_Abs,		&CPU::STA_Abs,		&CPU::STX_Abs,		nullptr, //8
		&CPU::BCC,			&CPU::STA_IndY,	nullptr,				nullptr,		&CPU::STY_ZeroX,	&CPU::STA_ZeroX,	&CPU::STX_ZeroY,	nullptr,		&CPU::TYA,			&CPU::STA_AbsY,	&CPU::TXS,			nullptr,		nullptr,				&CPU::STA_AbsX,	nullptr,				nullptr, //9
		&CPU::LDY_Imm,		&CPU::LDA_IndX,	&CPU::LDX_Imm,		nullptr,		&CPU::LDY_Zero,	&CPU::LDA_Zero,	&CPU::LDX_Zero,	nullptr,		&CPU::TAY,			&CPU::LDA_Imm,		&CPU::TAX,			nullptr,		&CPU::LDY_Abs,		&CPU::LDA_Abs,		&CPU::LDX_Abs,		nullptr, //A
		&CPU::BCS,			&CPU::LDA_IndY,	nullptr,				nullptr,		&CPU::LDY_ZeroX,	&CPU::LDA_ZeroX,	&CPU::LDX_ZeroY,	nullptr,		&CPU::CLV,			&CPU::LDA_AbsY,	&CPU::TSX,			nullptr,		&CPU::LDY_AbsX,	&CPU::LDA_AbsX,	&CPU::LDX_AbsY,	nullptr, //B
		&CPU::CPY_Imm,		&CPU::CMP_IndX,	nullptr,				nullptr,		&CPU::CPY_Zero,	&CPU::CMP_Zero,	&CPU::DEC_Zero,	nullptr,		&CPU::INY,			&CPU::CMP_Imm,		&CPU::DEX,			nullptr,		&CPU::CPY_Abs,		&CPU::CMP_Abs,		&CPU::DEC_Abs,		nullptr, //C
		&CPU::BNE,			&CPU::CMP_IndY,	nullptr,				nullptr,		nullptr,				&CPU::CMP_ZeroX,	&CPU::DEC_ZeroX,	nullptr,		&CPU::CLD,			&CPU::CMP_AbsY,	nullptr,				nullptr,		nullptr,				&CPU::CMP_AbsX,	&CPU::DEC_AbsX,	nullptr, //D
		&CPU::CPX_Imm,		&CPU::SBC_IndX,	nullptr,				nullptr,		&CPU::CPX_Zero,	&CPU::SBC_Zero,	&CPU::INC_Zero,	nullptr,		&CPU::INX,			&CPU::SBC_Imm,		&CPU::NOP,			nullptr,		&CPU::CPX_Abs,		&CPU::SBC_Abs,		&CPU::INC_Abs,		nullptr, //E
		&CPU::BEQ,			&CPU::SBC_IndY,	nullptr,				nullptr,		nullptr,				&CPU::SBC_ZeroX,	&CPU::INC_ZeroX,	nullptr,		&CPU::SED,			&CPU::SBC_AbsY,	nullptr,				nullptr,		nullptr,				&CPU::SBC_AbsX,	&CPU::INC_AbsX,	nullptr  //F
	};

	uint8_t cycles[] = {
		7,6,2,8,3,3,5,5,3,2,2,2,4,4,6,6,
		2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
		6,6,2,8,3,3,5,5,4,2,2,2,4,4,6,6,
		2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
		6,6,2,8,3,3,5,5,3,2,2,2,3,4,6,6,
		2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
		6,6,2,8,3,3,5,5,4,2,2,2,5,4,6,6,
		2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
		2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
		2,6,2,6,4,4,4,4,2,5,2,5,5,5,5,5,
		2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
		2,5,2,5,4,4,4,4,2,4,2,5,4,4,4,4,
		2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
		2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
		2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
		2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7
	};

	memcpy(_opTable, opTable, sizeof(Func) * 256);
	memcpy(_cycles, cycles, sizeof(uint8_t) * 256);
}

void CPU::Reset()
{
	_state.A = 0;
	_state.PC = MemoryReadWord(0xFFFC);
	_state.SP = 0xFF;
	_state.X = 0;
	_state.Y = 0;
	_state.PS = PSFlags::Zero | PSFlags::Reserved;// | PSFlags::Interrupt;
}

void CPU::Exec()
{
	uint16_t lastPC = 65535;
	uint32_t cycleCount = 0;

	Timer timer;
	while(true) {
		_currentPC = _state.PC;
		uint8_t opCode = ReadByte();
		if(_opTable[opCode] != nullptr) {
			(this->*_opTable[opCode])();
			cycleCount += this->_cycles[opCode];
			//std::cout << "OPCode: " << std::hex << (short)opCode << " PC:" << _currentPC << std::endl;
		} else {
			//std::cout << "Invalid opcode: PC:" << _currentPC << std::endl;
			throw std::exception("Invalid opcode");
		}
		lastPC = _currentPC;

		if(cycleCount >= 200000000) {
			break;
		}
	}
	std::wstring result = L"Frequency: " + std::to_wstring((int)(cycleCount / timer.GetElapsedMS() * 1000 / 1000000)) + L"mhz\n";
	OutputDebugString(result.c_str());
}

void CPU::RunBenchmark()
{
	std::ifstream romFile("6502_functional_test.bin", std::ios::in | std::ios::binary);
	if(!romFile) {
		return;
	}

	uint8_t *romMemory = new uint8_t[65536];
	romFile.read((char *)romMemory, 65536);

	//Test a;

	MemoryManager memoryManager(MapperFactory::InitializeFromFile("mario.nes"));
	//memoryManager.OnMemoryRead()->RegisterHandler(&a, &IMemoryHandler::MemoryRead);
	CPU core(&memoryManager);
	core.Exec();
}