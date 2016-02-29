#include		"Cpu.h"
#include "Windows.h"

Cpu::Cpu(Nes* nes, Ppu* ppu, Joypad* joypad) {
	this->nes = nes;
	this->ppu = ppu;
	this->joypad = joypad;
	this->A = 0;
	this->X = 0;
	this->Y = 0;
	this->PS = 0x24;
	this->SP = 0xFD;
}

Cpu::~Cpu() {
}

void			Cpu::setProgramCounter(uint16_t address) {
	this->PC = address;
}

inline uint16_t	Cpu::getValue(uint16_t addr, bool wrapZeroPage) {
	return ((readRAM(addr + 1, wrapZeroPage) << 8) | readRAM(addr, wrapZeroPage));
}

inline uint16_t	Cpu::getValuePageWrapped(uint16_t addr) {
	return ((readRAM(((addr & 0x00FF) == 0x00FF) ? (addr & 0xFF00) : (addr + 1)) << 8) | readRAM(addr));
}



inline void		Cpu::VC_FlagHandler(unsigned char ans, unsigned char val) {
	int	tmp = (char)ans + (char)val + (char)(C_FLAG);
	unsigned int tmpU = (unsigned int)ans + (unsigned int)val + (unsigned int)(C_FLAG);
	PS = (tmp > 127 || tmp < -128) ? (PS | 0b01000000) : (PS & 0b10111111);
	PS = (tmpU > 0xFF) ? (PS | 0b00000001) : (PS & 0b11111110);
}

inline void		Cpu::ZNV_FlagHandler(char ans, char val) {
	PS = (val == 0) ? (PS | 0b00000010) : (PS & 0b11111101);									// set Z_FLAG
	PS = (val < 0) ? (PS | 0b10000000) : (PS & 0b01111111);										// set N_FLAG
	PS = ((val & 0b10000000) != (ans & 0b10000000)) ? (PS | 0b01000000) : (PS & 0b10111111);	// set V_FLAG
//	if (val == 0)
//		PS |= 0b00000010;	// set Z_FLAG
//	if (val < 0)
//		PS |= 0b10000000;	// set N_FLAG
//	if ((val & 0b10000000) != (ans & 0b10000000))
//		PS |= 0b01000000;	// set V_FLAG
}

inline void		Cpu::ZN_FlagHandler(unsigned char val) {
	PS = (val == 0) ? (PS | 0b00000010) : (PS & 0b11111101);	// set Z_FLAG
	PS = (val >> 7) ? (PS | 0b10000000) : (PS & 0b01111111);		// set N_FLAG
	//if (val == 0)
	//	PS |= 0b00000010;	// set Z_FLAG
	//if (val < 0)
	//	PS |= 0b10000000;	// set N_FLAG
}

inline void		Cpu::CMP(unsigned char regVal, unsigned char opVal) {
	if (regVal < opVal) {
		UNSET_Z_FLAG;
		UNSET_C_FLAG;
	}
	else if (regVal > opVal) {
		UNSET_Z_FLAG;
		SET_C_FLAG;
	}
	else {
		SET_Z_FLAG;
		SET_C_FLAG;
	}
	((regVal - opVal) >> 7) ? (SET_N_FLAG) : (UNSET_N_FLAG);
}

unsigned char	Cpu::readRAM(uint16_t addr, bool wrapZeroPage) {
	unsigned char	ret = this->nes->getCpuMemory()[(wrapZeroPage) ? (addr % 0x100) : (addr)];
	switch (addr) {
	case CONTROLLERS_REGONE:
	case CONTROLLERS_REGTWO:
		this->joypad->readReg(addr);
		break;
	case OAMDATA:
		this->ppu->PpuOamDataRead();
		break;
	case PPUDATA:
		this->ppu->PpuDataRead();
		break;
	case PPUSTATUS:
		this->ppu->PpuStatusRead();
		break;
	default:
		break;
	};
	return ret;
}

void			Cpu::writeRAM(uint16_t addr, unsigned char val, bool wrapZeroPage) {
	this->nes->getCpuMemory()[(wrapZeroPage) ? (addr % 0x100) : (addr)] = val;
	switch (addr)
	{
	case PPUCTRL:
		this->ppu->PpuControlWrite();
		break;
	case PPUMASK:
		this->ppu->PpuMaskWrite();
		break;
	case OAMADDR:
		this->ppu->PpuOamAddressWrite();
		break;
	case OAMDATA:
		this->ppu->PpuOamDataWrite();
		break;
	case PPUSCROLL:
		this->ppu->PpuScrollWrite();
		break;
	case PPUDATA:
		this->ppu->PpuDataWrite();
		break;
	case PPUADDR:
		this->ppu->PpuAddrWrite();
		break;
	case OAMDMA:
		this->ppu->PpuOamDmaWrite();
		this->ppu->cycle(513);
		break;
	default:
		break;
	}
}

void			Cpu::nmiRoutine() {
	writeRAM(SP_OFFSET + SP--, (PC) >> 8);
	writeRAM(SP_OFFSET + SP--, (PC) & 0xFF);
	writeRAM(SP_OFFSET + SP--, PS);
	PC = nes->getNMIRoutine();
}

void			Cpu::loop(char *strLog) {
	char	error[64];
	int		cycles = 0;
	char	ans = A;
	char	tmp;
	uint16_t	tmp16;
	bool cFlag;

	if (PC == 0x8706) //A SUPPRIMER
	{
		A = 0xEA;
		X = 0x38;
		PS = 0xA5;
		SP = 0xF6;
	}
	++PC;
	/*char strExec[1024];
	char useless[42];
	static HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	sprintf_s(strExec, "%04X %02X\t\tA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d SL:%d", this->PC - 1, (unsigned char)this->nes->getCpuMemory()[this->PC - 1], (unsigned char)A, (unsigned char)X, (unsigned char)Y, (unsigned char)PS, (unsigned char)SP, ppu->getCycle(), ppu->getScanline());
	if (strcmp(strLog, strExec) != 0) {
		SetConsoleTextAttribute(hConsole, 12);
		printf("%s\n", strExec, ppu->getCycle());
		SetConsoleTextAttribute(hConsole, 10);
		printf("%s\n", strLog);
		SetConsoleTextAttribute(hConsole, 7);
		std::cin >> useless;
	}
	else
		printf("%s\n", strExec);*/
		//printf("%X	%X is executed  A:%X X:%X Y:%X P:%X SP:%X\n", this->PC - 1, (unsigned char)this->nes->getCpuMemory()[this->PC - 1], (unsigned char)A, (unsigned char)X, (unsigned char)Y, (unsigned char)PS, (unsigned char)SP);
		// Sleep(1000);
	
	switch (readRAM(this->PC - 1)) {

		/* ============================ Load and Store Instructions =============================== */

		// -------------- [LDA] Load Accumulator (affected flags: N, Z)
	case 0xA9:	// immediate
		A = readRAM(PC);
		++PC;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0xA5:	// absolute zero page
		A = readRAM(readRAM(PC), true);
		++PC;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0xB5:	// indexed zero page X
		tmp16 = readRAM(PC);
		A = readRAM(tmp16 + (unsigned char)X, true);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xAD:	// absolute
		A = readRAM(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0xBD:	// absolute indexed X
		tmp16 = getValue(PC);
		A = readRAM(tmp16 + (unsigned char)X);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xB9:	// absolute indexed Y
		tmp16 = getValue(PC);
		A = readRAM(tmp16 + (unsigned char)Y);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0xA1:	// indexed indirect X
		tmp16 = readRAM(PC);
		A = readRAM(getValue(tmp16 + (unsigned char)X, true));
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0xB1:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		A = readRAM(tmp16 + (unsigned char)Y);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		// -------------- [LDX] Load Register X (affected flags: N, Z)
	case 0xA2:	// immediate
		X = readRAM(PC);
		++PC;
		ZN_FlagHandler(X);
		cycles = 2;
		break;
	case 0xA6:	// absolute zero page
		X = readRAM(readRAM(PC), true);
		++PC;
		ZN_FlagHandler(X);
		cycles = 3;
		break;
	case 0xB6:	// indexed zero page Y
		tmp16 = readRAM(PC);
		X = readRAM(tmp16 + (unsigned char)Y, true);
		++PC;
		ZN_FlagHandler(X);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0xAE:	// absolute
		X = readRAM(getValue(PC));
		PC += 2;
		ZN_FlagHandler(X);
		cycles = 4;
		break;
	case 0xBE:	// absolute indexed Y
		tmp16 = getValue(PC);
		X = readRAM(tmp16 + (unsigned char)Y);
		PC += 2;
		ZN_FlagHandler(X);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;

		// -------------- [LDY] Load Register Y (affected flags: N, Z)
	case 0xA0:	// immediate
		Y = readRAM(PC);
		++PC;
		ZN_FlagHandler(Y);
		cycles = 2;
		break;
	case 0xA4:	// absolute zero page
		Y = readRAM(readRAM(PC), true);
		++PC;
		ZN_FlagHandler(Y);
		cycles = 3;
		break;
	case 0xB4:	// indexed zero page X
		tmp16 = readRAM(PC);
		Y = readRAM(tmp16 + (unsigned char)X, true);
		++PC;
		ZN_FlagHandler(Y);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xAC:	// absolute
		Y = readRAM(getValue(PC));
		PC += 2;
		ZN_FlagHandler(Y);
		cycles = 4;
		break;
	case 0xBC:	// absolute indexed X
		tmp16 = getValue(PC);
		Y = readRAM(tmp16 + (unsigned char)X);
		PC += 2;
		ZN_FlagHandler(Y);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;


		// -------------- [STA] Store Accumulator
	case 0x85:	// absolute zero page
		writeRAM(readRAM(PC), A, true);
		++PC;
		cycles = 3;
		break;
	case 0x95:	// indexed zero page X
		tmp16 = readRAM(PC);
		writeRAM(tmp16 + (unsigned char)X, A, true);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x8D:	// absolute
		writeRAM(getValue(PC), A);
		PC += 2;
		cycles = 4;
		break;
	case 0x9D:	// absolute indexed X
		tmp16 = getValue(PC);
		writeRAM(tmp16 + (unsigned char)X, A);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (6) : (5);
		break;
	case 0x99:	// absolute indexed Y
		tmp16 = getValue(PC);
		writeRAM(tmp16 + (unsigned char)Y, A);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;
	case 0x81:	// indexed indirect X
		tmp16 = readRAM(PC);
		writeRAM(getValue(tmp16 + (unsigned char)X, true), A);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x91:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		writeRAM(tmp16 + (unsigned char)Y, A);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (7) : (6);
		break;

		// -------------- [STX] Store X Register
	case 0x86:	// absolute zero page
		writeRAM(readRAM(PC), X, true);
		++PC;
		cycles = 3;
		break;
	case 0x96:	// indexed zero page Y
		tmp16 = readRAM(PC);
		writeRAM(tmp16 + (unsigned char)Y, X, true);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0x8E:	// absolute
		writeRAM(getValue(PC), X);
		PC += 2;
		cycles = 4;
		break;

		// -------------- [STY] Store Y Register
	case 0x84:	// absolute zero page
		writeRAM(readRAM(PC), Y, true);
		++PC;
		cycles = 3;
		break;
	case 0x94:	// indexed zero page X
		tmp16 = readRAM(PC);
		writeRAM(tmp16 + (unsigned char)X, Y, true);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x8C:	// absolute
		writeRAM(getValue(PC), Y);
		PC += 2;
		cycles = 4;
		break;



		/* ============================ Arithmetic Instructions =================================== */
		
		// -------------- [ADC] Add with Carry (affected flags : N, Z, C, V)
	case 0x69:	// immediate
		tmp = readRAM(PC);
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x65:	// absolute zero page
		tmp = readRAM(readRAM(PC), true);
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0x75:	// indexed zero page X
		tmp16 = readRAM(PC);
		tmp = readRAM(tmp16 + (unsigned char)X, true);
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x6D:	// absolute
		tmp = readRAM(getValue(PC));
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0x7D:	// absolute indexed X
		tmp16 = getValue(PC);
		tmp = readRAM(tmp16 + (unsigned char)X);
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x79:	// absolute indexed Y
		tmp16 = getValue(PC);
		tmp = readRAM(tmp16 + (unsigned char)Y);
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0x61:	// indexed indirect X
		tmp16 = readRAM(PC);
		tmp = readRAM(getValue(tmp16 + (unsigned char)X, true));
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x71:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		tmp = readRAM(tmp16 + (unsigned char)Y);
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		// -------------- [SBC] Sub with Carry (affected flags : N, Z, C, V)
	case 0xE9:	// immediate
	case 0xEB:
		tmp = readRAM(PC) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0xE5:	// absolute zero page
		tmp = readRAM(readRAM(PC), true) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0xF5:	// indexed zero page X
		tmp16 = readRAM(PC);
		tmp = readRAM(tmp16 + (unsigned char)X, true) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xED:	// absolute
		tmp = readRAM(getValue(PC)) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0xFD:	// absolute indexed X
		tmp16 = getValue(PC);
		tmp = readRAM(tmp16 + (unsigned char)X) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xF9:	// absolute indexed Y
		tmp16 = getValue(PC);
		tmp = readRAM(tmp16 + (unsigned char)Y) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0xE1:	// indexed indirect X
		tmp16 = readRAM(PC);
		tmp = readRAM(getValue(tmp16 + (unsigned char)X, true)) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0xF1:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		tmp = readRAM(tmp16 + (unsigned char)Y) ^ 0xFF;
		A += tmp + C_FLAG;
		VC_FlagHandler(ans, tmp);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		
		
		/* ============================ Increment and Decrement Instructions ====================== */
		
		// -------------- [INC] Increment Memory (affected flags: N, Z)
	case 0xE6:	// absolute zero page
		tmp = readRAM(readRAM(PC), true);
		writeRAM(readRAM(PC), tmp + 1, true);
		ZN_FlagHandler(tmp + 1);
		++PC;
		cycles = 5;
		break;
	case 0xF6:	// indexed zero page X
		tmp = readRAM(readRAM(PC) + (unsigned char)X, true);
		tmp16 = readRAM(PC);
		writeRAM(tmp16 + (unsigned char)X, tmp + 1, true);
		ZN_FlagHandler(tmp + 1);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0xEE:	// absolute
		tmp = readRAM(getValue(PC));
		writeRAM(getValue(PC), tmp + 1);
		ZN_FlagHandler(tmp + 1);
		PC += 2;
		cycles = 6;
		break;
	case 0xFE:	// absolute indexed X
		tmp16 = getValue(PC);
		tmp = readRAM(getValue(PC) + (unsigned char)X);
		writeRAM(tmp16 + (unsigned char)X, tmp + 1);
		ZN_FlagHandler(tmp + 1);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (8) : (7);
		break;

		// -------------- [INX] Increment X (affected flags: N, Z)
	case 0xE8:
		++X;
		ZN_FlagHandler(X);
		cycles = 2;
		break;

		// -------------- [INY] Increment Y (affected flags: N, Z)
	case 0xC8:
		++Y;
		ZN_FlagHandler(Y);
		cycles = 2;
		break;


		// -------------- [DEC] Decrement Source (affected flags: N, Z)
	case 0xC6:	// absolute zero page
		tmp = readRAM(readRAM(PC), true);
		writeRAM(readRAM(PC), tmp - 1, true);
		ZN_FlagHandler(tmp - 1);
		++PC;
		cycles = 5;
		break;
	case 0xD6:	// indexed zero page X
		tmp16 = readRAM(PC);
		tmp = readRAM(readRAM(PC) + (unsigned char)X, true);
		writeRAM(tmp16 + (unsigned char)X, tmp - 1, true);
		ZN_FlagHandler(tmp - 1);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0xCE:	// absolute
		tmp = readRAM(getValue(PC));
		writeRAM(getValue(PC), tmp - 1);
		ZN_FlagHandler(tmp - 1);
		PC += 2;
		cycles = 6;
		break;
	case 0xDE:	// absolute indexed X
		tmp16 = getValue(PC);
		tmp = readRAM(getValue(PC) + (unsigned char)X);
		writeRAM(tmp16 + (unsigned char)X, tmp - 1);
		ZN_FlagHandler(tmp - 1);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (8) : (7);
		break;

		// -------------- [DEX] Decrement X (affected flags: N, Z)
	case 0xCA:
		--X;
		ZN_FlagHandler(X);
		cycles = 2;
		break;

		// -------------- [DEY] Decrement Y (affected flags: N, Z)
	case 0x88:
		--Y;
		ZN_FlagHandler(Y);
		cycles = 2;
		break;

		
		
		/* ============================ Logical Instructions ====================================== */
		
		// -------------- [AND] Logical AND (affected flags : N, Z)
	case 0x29:	// immediate
		A &= readRAM(PC);
		++PC;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x25:	// absolute zero page
		A &= readRAM(readRAM(PC), true);
		++PC;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0x35:	// indexed zero page X
		tmp16 = readRAM(PC);
		A &= readRAM(tmp16 + (unsigned char)X, true);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x2D:	// absolute
		A &= readRAM(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0x3D:	// absolute indexed X
		tmp16 = getValue(PC);
		A &= readRAM(tmp16 + (unsigned char)X);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x39:	// absolute indexed Y
		tmp16 = getValue(PC);
		A &= readRAM(tmp16 + (unsigned char)Y);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0x21:	// indexed indirect X
		tmp16 = readRAM(PC);
		A &= readRAM(getValue(tmp16 + (unsigned char)X, true));
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x31:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		A &= readRAM(tmp16 + (unsigned char)Y);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		// -------------- [ORA] Logical Inclusive OR (affected flags : N, Z)
	case 0x09:	// immediate
		A |= readRAM(PC);
		++PC;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x05:	// absolute zero page
		A |= readRAM(readRAM(PC), true);
		++PC;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0x15:	// indexed zero page X
		tmp16 = readRAM(PC);
		A |= readRAM(tmp16 + (unsigned char)X, true);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x0D:	// absolute
		A |= readRAM(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0x1D:	// absolute indexed X
		tmp16 = getValue(PC);
		A |= readRAM(tmp16 + (unsigned char)X);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x19:	// absolute indexed Y
		tmp16 = getValue(PC);
		A |= readRAM(tmp16 + (unsigned char)Y);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0x01:	// indexed indirect X
		tmp16 = readRAM(PC);
		A |= readRAM(getValue(tmp16 + (unsigned char)X, true));
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x11:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		A |= readRAM(tmp16 + (unsigned char)Y);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		// -------------- [EOR] Exclusive-OR (affected flags : N, Z)
	case 0x49:	// immediate
		A ^= readRAM(PC);
		++PC;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x45:	// absolute zero page
		A ^= readRAM(readRAM(PC), true);
		++PC;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0x55:	// indexed zero page X
		tmp16 = readRAM(PC);
		A ^= readRAM(tmp16 + (unsigned char)X, true);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x4D:	// absolute
		A ^= readRAM(getValue(PC));
		PC += 2;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0x5D:	// absolute indexed X
		tmp16 = getValue(PC);
		A ^= readRAM(tmp16 + (unsigned char)X);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0x59:	// absolute indexed Y
		tmp16 = getValue(PC);
		A ^= readRAM(tmp16 + (unsigned char)Y);
		PC += 2;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0x41:	// indexed indirect X
		tmp16 = readRAM(PC);
		A ^= readRAM(getValue(tmp16 + (unsigned char)X, true));
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x51:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		A ^= readRAM(tmp16 + (unsigned char)Y);
		++PC;
		ZN_FlagHandler(A);
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		
		
		/* ============================ Jump, Branch, Compare, and Test Bits ====================== */
		
		// -------------- [JMP] Jump
	case 0x4c:	// absolute
		PC = getValue(PC);
		cycles = 3;
		break;
	case 0x6c:	// indirect absolute
		PC = getValuePageWrapped(getValue(PC));
		cycles = 5;
		break;


		// -------------- [BCC] Branch if Carry Clear
	case 0x90:	// relativ
		cycles = 2;
		if (C_FLAG == 0) {
			PC += (char)readRAM(PC);
			cycles = 3;
		}
		++PC;
		break;

		// -------------- [BCS] Branch if Carry Set
	case 0xB0:	// relativ		
		cycles = 1;
		if (C_FLAG != 0) {
			PC += (char)readRAM(PC);
			cycles = 2;
		}
		++PC;
		break;

		// -------------- [BEQ] Branch if Equal
	case 0xF0:	// relativ
		cycles = 1;
		if (Z_FLAG != 0) {
			PC += (char)readRAM(PC);
			cycles = 2;
		}
		++PC;
		break;

		// -------------- [BNE] Branch if not Equal
	case 0xD0:	// relativ
		cycles = 2;
		if (Z_FLAG == 0) {
			cycles = 3;
			PC += (char)readRAM(PC);
		}
		++PC;
		break;

		// -------------- [BMI] Branch if Minus
	case 0x30:	// relativ
		cycles = 1;
		if (N_FLAG != 0) {
			PC += (char)readRAM(PC);
			cycles = 2;
		}
		++PC;
		break;

		// -------------- [BPL] Branch on Plus
	case 0x10:	// relativ
		cycles = 2;
		if (N_FLAG == 0) {
			PC += (char)readRAM(PC);
			cycles = 3;
		}
		++PC;
		break;
		
		// -------------- [BVS] Branch if Overflow Set
	case 0x70: // relativ
		cycles = 1;
		if (V_FLAG != 0) {
			PC += (char)readRAM(PC);
			cycles = 2;
		}
		++PC;
		break;

		// -------------- [BVC] Branch if Overflow Clear
	case 0x50: // relativ
		cycles = 2;
		if (V_FLAG == 0) {
			PC += (char)readRAM(PC);
			cycles = 3;
		}
		++PC;
		break;


		// -------------- [CMP] Compare Memory And Accumulator (affected flags: N, Z, C)
	case 0xC9:	// immediate
		CMP(A, readRAM(PC));
		++PC;
		cycles = 2;
		break;
	case 0xC5:	// absolute zero page
		CMP(A, readRAM(readRAM(PC), true));
		++PC;
		cycles = 3;
		break;
	case 0xD5:	// indexed zero page X
		tmp16 = readRAM(PC);
		CMP(A, readRAM(tmp16 + (unsigned char)X, true));
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xCD:	// absolute
		CMP(A, readRAM(getValue(PC)));
		PC += 2;
		cycles = 4;
		break;
	case 0xDD:	// absolute indexed X
		tmp16 = getValue(PC);
		CMP(A, readRAM(tmp16 + (unsigned char)X));
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (5) : (4);
		break;
	case 0xD9:	// absolute indexed Y
		tmp16 = getValue(PC);
		CMP(A, readRAM(tmp16 + (unsigned char)Y));
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (5) : (4);
		break;
	case 0xC1:	// indexed indirect X
		tmp16 = readRAM(PC);
		CMP(A, readRAM(getValue(tmp16 + (unsigned char)X, true)));
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0xD1:	// indirect indexed Y
		tmp16 = getValue(readRAM(PC), true);
		CMP(A, readRAM(tmp16 + (unsigned char)Y));
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)Y)) ? (6) : (5);
		break;

		// -------------- [CPX] Compare Memory And X (affected flags: N, Z, C)
	case 0xE0:	// immediate
		CMP(X, readRAM(PC));
		++PC;
		cycles = 2;
		break;
	case 0xE4:	// absolute zero page
		CMP(X, readRAM(readRAM(PC), true));
		++PC;
		cycles = 3;
		break;
	case 0xEC:	// absolute
		CMP(X, readRAM(getValue(PC)));
		PC += 2;
		cycles = 4;
		break;

		// -------------- [CPY] Compare Memory And Y (affected flags: N, Z, C)
	case 0xC0:	// immediate
		CMP(Y, readRAM(PC));
		++PC;
		cycles = 2;
		break;
	case 0xC4:	// absolute zero page
		CMP(Y, readRAM(readRAM(PC), true));
		++PC;
		cycles = 3;
		break;
	case 0xCC:	// absolute
		CMP(Y, readRAM(getValue(PC)));
		PC += 2;
		cycles = 4;
		break;


		// -------------- [BIT] Bit Test (affected flag: N, V, Z)
	case 0x24:	// absolute zero page
		tmp = readRAM(readRAM(PC), true);
		++PC;
		ZNV_FlagHandler(ans, A & tmp);
		PS &= 0b00111111;
		PS |= (tmp & 0b11000000);
		cycles = 3;
		break;
	case 0x2C:	// absolute
		tmp = readRAM(getValue(PC));
		PC += 2;
		ZNV_FlagHandler(ans, A & tmp);
		PS &= 0b00111111;
		PS |= (tmp & 0b11000000);
		cycles = 4;
		break;

		

		/* ============================ Shift and Rotate Instructions ============================= */
		
		// -------------- [ASL] Arithmetic Shift Left (affected flags : N, C, Z)
	case 0x0A:	// A
		PS &= 0b11111110;
		PS |= (unsigned char)A >> 7; // set C_FLAG
		A = (unsigned char)A << 1;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x06:	// absolute zero page
		tmp = readRAM(readRAM(PC), true);
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(readRAM(PC), (unsigned char)tmp << 1);
		ZN_FlagHandler((unsigned char)tmp << 1);
		++PC;
		cycles = 5;
		break;
	case 0x16:	// indexed zero page X
		tmp16 = readRAM(PC);
		tmp = readRAM(readRAM(PC) + (unsigned char)X, true);
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(tmp16 + (unsigned char)X, (unsigned char)tmp << 1);
		ZN_FlagHandler((unsigned char)tmp << 1);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x0E:	// absolute
		tmp = readRAM(getValue(PC));
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(getValue(PC), (unsigned char)tmp << 1);	
		ZN_FlagHandler((unsigned char)tmp << 1);
		PC += 2;
		cycles = 6;
		break;
	case 0x1E:	// absolute indexed X
		tmp16 = getValue(PC);
		tmp = readRAM(getValue(PC) + (unsigned char)X);
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(tmp16 + (unsigned char)X, (unsigned char)tmp << 1);
		ZN_FlagHandler((unsigned char)tmp << 1);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (8) : (7);
		break;

		// -------------- [LSR] Logical Shift Right (affected flags : N, C, Z)
	case 0x4A:	// A
		PS &= 0b11111110;	// set C_FLAG
		PS |= (A & 0x1);
		A = (unsigned char)A >> 1;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x46:	// absolute zero page
		tmp = readRAM(readRAM(PC), true);
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(readRAM(PC), (unsigned char)tmp >> 1);
		ZN_FlagHandler((unsigned char)tmp >> 1);
		++PC;
		cycles = 5;
		break;
	case 0x56:	// indexed zero page X
		tmp16 = readRAM(PC);
		tmp = readRAM(readRAM(PC) + (unsigned char)X, true);
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(tmp16 + (unsigned char)X, (unsigned char)tmp >> 1);
		ZN_FlagHandler((unsigned char)tmp >> 1);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x4E:	// absolute
		tmp = readRAM(getValue(PC));
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(getValue(PC), (unsigned char)tmp >> 1);
		ZN_FlagHandler((unsigned char)tmp >> 1);
		PC += 2;
		cycles = 6;
		break;
	case 0x5E:	// absolute indexed X
		tmp16 = getValue(PC);
		tmp = readRAM(getValue(PC) + (unsigned char)X);
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(tmp16 + (unsigned char)X, (unsigned char)tmp >> 1);
		ZN_FlagHandler((unsigned char)tmp >> 1);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (8) : (7);
		break;

		// -------------- [ROL] Rotate Left (affected flags : N, C, Z)
	case 0x2A:	// A
		ans = PS & 0x1;
		PS &= 0b11111110;
		PS |= (unsigned char)A >> 7; // set C_FLAG
		A = ((unsigned char)A << 1) | ans;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x26:	// absolute zero page
		ans = PS & 0x1;
		tmp = readRAM(readRAM(PC), true);
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(readRAM(PC), ((unsigned char)tmp << 1) | ans);
		ZN_FlagHandler(((unsigned char)tmp << 1) | ans);
		++PC;
		cycles = 5;
		break;
	case 0x36:	// indexed zero page X
		tmp16 = readRAM(PC);
		ans = PS & 0x1;
		tmp = readRAM(readRAM(PC) + (unsigned char)X, true);
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(tmp16 + (unsigned char)X, ((unsigned char)tmp << 1) | ans);
		ZN_FlagHandler(((unsigned char)tmp << 1) | ans);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x2E:	// absolute
		ans = PS & 0x1;
		tmp = readRAM(getValue(PC));
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(getValue(PC), ((unsigned char)tmp << 1) | ans);
		ZN_FlagHandler(((unsigned char)tmp << 1) | ans);
		PC += 2;
		cycles = 6;
		break;
	case 0x3E:	// absolute indexed X
		tmp16 = getValue(PC);
		ans = PS & 0x1;
		tmp = readRAM(getValue(PC) + (unsigned char)X);
		PS &= 0b11111110;
		PS |= (unsigned char)tmp >> 7;
		writeRAM(tmp16 + (unsigned char)X, ((unsigned char)tmp << 1) | ans);
		ZN_FlagHandler(((unsigned char)tmp << 1) | ans);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (8) : (7);
		break;

		// -------------- [ROR] Rotate Right (affected flags : N, C, Z)
	case 0x6A:	// A
		ans = (PS & 0x1) << 7;
		PS &= 0b11111110;	// set C_FLAG
		PS |= (A & 0x1);
		A = ((unsigned char)A >> 1) | (unsigned char)ans;
		ZN_FlagHandler(A);
		cycles = 2;
		break;
	case 0x66:	// absolute zero page
		ans = (PS & 0x1) << 7;
		tmp = readRAM(readRAM(PC), true);
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(readRAM(PC), ((unsigned char)tmp >> 1) | (unsigned char)ans);
		ZN_FlagHandler(((unsigned char)tmp >> 1) | (unsigned char)ans);
		++PC;
		cycles = 5;
		break;
	case 0x76:	// indexed zero page X
		tmp16 = readRAM(PC);
		ans = (PS & 0x1) << 7;
		tmp = readRAM(readRAM(PC) + (unsigned char)X, true);
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(tmp16 + (unsigned char)X, ((unsigned char)tmp >> 1) | (unsigned char)ans);
		ZN_FlagHandler(((unsigned char)tmp >> 1) | (unsigned char)ans);
		++PC;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (7) : (6);
		break;
	case 0x6E:	// absolute
		ans = (PS & 0x1) << 7;
		tmp = readRAM(getValue(PC));
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(getValue(PC), ((unsigned char)tmp >> 1) | (unsigned char)ans);
		ZN_FlagHandler(((unsigned char)tmp >> 1) | (unsigned char)ans);
		PC += 2;
		cycles = 6;
		break;
	case 0x7E:	// absolute indexed X
		tmp16 = getValue(PC);
		ans = (PS & 0x1) << 7;
		tmp = readRAM(getValue(PC) + (unsigned char)X);
		PS &= 0b11111110;	// set C_FLAG
		PS |= (tmp & 0x1);
		writeRAM(tmp16 + (unsigned char)X, ((unsigned char)tmp >> 1) | (unsigned char)ans);
		ZN_FlagHandler(((unsigned char)tmp >> 1) | (unsigned char)ans);
		PC += 2;
		cycles = (BND_CHK(tmp16, (unsigned char)X)) ? (8) : (7);
		break;

		
		
		/* ============================ Transfer Instructions ===================================== */
		
		// -------------- [TAX] Transfer Accumulator to X(affected flags: N, Z)
	case 0xAA:
		X = A;
		ZN_FlagHandler(A);
		cycles = 2;
		break;

		// -------------- [TAY] Transfer Accumulator to Y (affected flags: N, Z)
	case 0xA8:
		Y = A;
		ZN_FlagHandler(A);
		cycles = 2;
		break;

		// -------------- [TXA] Transfer X to Accumulator (affected flags: N, Z)
	case 0x8A:
		A = X;
		ZN_FlagHandler(X);
		cycles = 2;
		break;

		// -------------- [TYA] Transfer Y to Accumulator (affected flags: N, Z)
	case 0x98:
		A = Y;
		ZN_FlagHandler(Y);
		cycles = 2;
		break;

		
		
		/* ============================ Stack Instructions ======================================== */
		
		// -------------- [TSX] Transfer Stack pointer to X (affected flags: N, Z)
	case 0xBA:
		X = SP;
		ZN_FlagHandler(X);
		cycles = 2;
		break;

		// -------------- [TXS] Transfer X to Stack pointer (affected flags: N, Z)
	case 0x9A:
		SP = X;
		cycles = 2;
		break;


		// -------------- [PHA] Push Accumulator on stack
	case 0x48:
		writeRAM(SP_OFFSET + SP--, A);
		cycles = 3;
		break;

		// -------------- [PHP] Push Processor Status on stack
	case 0x08:
		writeRAM(SP_OFFSET + SP--, (PS | 0b00110000));
		cycles = 3;
		break;

		// -------------- [PLA] Pull Accumulator from stack (affected flags : N, Z)
	case 0x68:
		A = readRAM(SP_OFFSET + ++SP);
		ZN_FlagHandler(A);
		cycles = 4;
		break;

		// -------------- [PLP] Pull Processor Status from stack (affected flags : ALL)
	case 0x28:
		PS &= 0b00110000;
		PS |= (readRAM(SP_OFFSET + ++SP) & 0b11001111);
		cycles = 4;
		break;

		
		
		/* ============================ Subroutine Instructions =================================== */
		
		// -------------- [JSR] Jump to Subroutine
	case 0x20:
		writeRAM(SP_OFFSET + SP--, (PC + 1) >> 8);
		writeRAM(SP_OFFSET + SP--, (PC + 1) & 0xFF);
		PC = getValue(PC);
		cycles = 6;
		break;


		// -------------- [RTS]
	case 0x60:
		PC = 0;
		PC |= readRAM(SP_OFFSET + ++SP);
		PC |= ((uint16_t)(readRAM(SP_OFFSET + ++SP))) << 8; 
		PC += 1;
		cycles = 6;
		break;


		// -------------- [RTI]
	case 0x40:
		PC = 0;
		PS &= 0b00110000;
		PS |= (readRAM(SP_OFFSET + ++SP) & 0b11001111);
		PC |= readRAM(SP_OFFSET + ++SP);
		PC |= ((uint16_t)(readRAM(SP_OFFSET + ++SP))) << 8;
		cycles = 6;
		break;

		

		/* ============================ Set and Reset (Clear) Instructions ======================== */
		
		// -------------- [CLC] Clear Carry (affected flags: C)
	case 0x18:
		UNSET_C_FLAG;
		cycles = 2;
		break;

		// -------------- [CLD] Clear Decimal Mode (affected flags: D)
	case 0xD8:
		UNSET_D_FLAG;
		cycles = 2;
		break;

		// -------------- [CLI] Clear Interrupt Disable (affected flags: I)
	case 0x58:
		UNSET_I_FLAG;
		cycles = 2;
		break;

		// -------------- [CLV] Clear Overflow Flag (affected flags: V)
	case 0xB8:
		UNSET_V_FLAG;
		cycles = 2;
		break;


		// -------------- [SEC] Set Carry Flag (affected flags: C)
	case 0x38:
		SET_C_FLAG;
		cycles = 2;
		break;

		// -------------- [SED] Set Decimal Mode (affected flags: D)
	case 0xF8:
		SET_D_FLAG;
		cycles = 2;
		break;

		// -------------- [SEI] Set Interrupt Disable (affected flags: I)
	case 0x78:
		SET_I_FLAG;
		cycles = 2;
		break;

		
		
		/* ============================ Other Instructions ======================================== */

		// -------------- [NOP] No Opération
	case 0x1A:
	case 0x3A:
	case 0x5A:
	case 0x7A:
	case 0xDA:
	case 0xFA: // ^ Unofficial NOP
	case 0xEA:
		cycles = 2;
		break;

		// -------------- [BRK] Break (affected flags: B, I)
	case 0x00:
		writeRAM(SP_OFFSET + SP--, PC >> 8);
		writeRAM(SP_OFFSET + SP--, PC & 0x0F);
		writeRAM(SP_OFFSET + SP--, (PS | 0b00110000));
		SET_B_FLAG;
		SET_I_FLAG;
		break;



		/* ============================ Unofficial Instructions ======================================== */

		// -------------- [DOP] Double Nop (unofficial)
	case 0x04:
	case 0x44:
	case 0x64: // ^ 3 cycles, Zero Page
		readRAM(readRAM(PC));
		++PC;
		cycles = 3;
		break;
	case 0x14:
	case 0x34:
	case 0x54:
	case 0x74:
	case 0xD4:
	case 0xF4: // ^ 4 cylces, Zero Page X
		readRAM(readRAM(PC));
		readRAM(readRAM(PC) + (unsigned char)X, true);
	case 0x80:
	case 0x82:
	case 0x89:
	case 0xC2:
	case 0xE2:
		++PC;
		cycles = 4;
		break;

		// -------------- [TOP] Triple Nop (unofficial)
	case 0x0C: // 4 cycles, Absolute
		readRAM(getValue(PC));
		PC += 2;
		cycles = 4;
		break;
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC: // ^ 4 or 5 cycles, Absolute Indexed X
		readRAM(getValue(PC) + X);
		PC += 2;
		cycles = 4;
		break;

		// -------------- [LAX] Shortcut for LDA then TAX (unofficial)
	case 0xA3: // 6 cycles, Indexed Indirect X
		A = readRAM(getValue(readRAM(PC) + (unsigned char)X, true));
		++PC;
		X = A;
		ZN_FlagHandler(A);
		cycles = 6;
		break;
	case 0xA7: // 3 cycles, Absolute Zero Page
		A = readRAM(readRAM(PC), true);
		++PC;
		X = A;
		ZN_FlagHandler(A);
		cycles = 3;
		break;
	case 0xAF: // 4 cycles, Absolute
		A = readRAM(getValue(PC));
		PC += 2;
		X = A;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0xB3: // 5 cycles, Indirect Indexed Y
		A = readRAM(getValue(readRAM(PC), true) + (unsigned char)Y);
		++PC;
		X = A;
		ZN_FlagHandler(A);
		cycles = 5;
		break;
	case 0xB7: // 4 cycles, Indexed Zero Page Y
		A = readRAM(readRAM(PC) + (unsigned char)Y, true);
		++PC;
		X = A;
		ZN_FlagHandler(A);
		cycles = 4;
		break;
	case 0xBF: // 4 cycles, Absolute Indexed Y
		A = readRAM(getValue(PC) + (unsigned char)Y);
		PC += 2;
		X = A;
		ZN_FlagHandler(A);
		cycles = 4;
		break;

		// -------------- [SAX] Store A AND X (unofficial)
	case 0x83: // 6 cycles, Indexed Indirect X
		writeRAM(getValue(readRAM(PC) + (unsigned char)X, true), A & X);
		++PC;
		cycles = 6;
		break;
	case 0x87: // 3 cycles, Absolute Zero Page
		writeRAM(readRAM(PC), A & X, true);
		++PC;
		cycles = 3;
		break;
	case 0x8F: // 4 cycles, Absolute
		writeRAM(getValue(PC), A & X);
		PC += 2;
		cycles = 4;
		break;
	case 0x97: // 4 cycles, Indexed Zero Page Y
		writeRAM(readRAM(PC) + (unsigned char)Y, A & X, true);
		++PC;
		cycles = 4;
		break;

		// -------------- [DCP] DEC then CMP (unofficial)
	case 0xC3: // 8 cycles, Indexed Indirect X
		tmp16 = getValue(readRAM(PC) + (unsigned char)X, true);
		printf("cacac : %X\n", readRAM(tmp16));
		writeRAM(tmp16, readRAM(tmp16) - 1);
		//ZN_FlagHandler(readRAM(tmp16));
		CMP(A, readRAM(tmp16));
		++PC;
		cycles = 8;
		break;
	case 0xC7: // 3 cycles, Absolute Zero Page
		tmp = readRAM(PC);
		writeRAM(tmp, readRAM(tmp) - 1, true);
		ZN_FlagHandler(readRAM(tmp));
		CMP(A, readRAM(tmp));
		++PC;
		cycles = 3;
		break;
	case 0xCF: // 6 cycles, Absolute
		tmp16 = getValue(PC);
		writeRAM(tmp16, readRAM(tmp16) - 1);
		ZN_FlagHandler(readRAM(tmp16));
		CMP(A, readRAM(tmp16));
		PC += 2;
		cycles = 6;
		break;
	case 0xD3: // 8 cycles, Indirect Indexed Y
		tmp16 = getValue(readRAM(PC), true) + (unsigned char)Y;
		writeRAM(tmp16, readRAM(tmp16) - 1);
		ZN_FlagHandler(readRAM(tmp16));
		CMP(A, readRAM(tmp16));
		++PC;
		cycles = 8;
		break;
	case 0xD7: // 6 cycles, Indexed Zero Page X
		tmp = readRAM(PC) + (unsigned char)X;
		writeRAM(tmp, readRAM(tmp) - 1, true);
		ZN_FlagHandler(readRAM(tmp));
		CMP(A, readRAM(tmp));
		++PC;
		cycles = 6;
		break;
	case 0xDB: // 7 cycles, Absolute Indexed Y
		tmp16 = getValue(PC) + (unsigned char)Y;
		writeRAM(tmp16, readRAM(tmp16) - 1);
		ZN_FlagHandler(readRAM(tmp16));
		CMP(A, readRAM(tmp16));
		PC += 2;
		cycles = 7;
		break;
	case 0xDF: // 7 cycles, Absolute Indexed X
		tmp16 = getValue(PC) + (unsigned char)X;
		writeRAM(tmp16, readRAM(tmp16) - 1);
		ZN_FlagHandler(readRAM(tmp16));
		CMP(A, readRAM(tmp16));
		PC += 2;
		cycles = 7;
		break;
		/* ======================================================================================== */

	default:
		sprintf_s(error, "The opcode %X is not recognised", this->PC - 1);
		Error::getInstance()->queue(error);
		break;
	}
	this->ppu->cycle(cycles);
	if (ppu->nmiOccured()) {
		this->nmiRoutine();
		this->ppu->cycle(7);
	}
}