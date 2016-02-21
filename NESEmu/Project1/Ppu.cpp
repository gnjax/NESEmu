#include "Ppu.h"

Ppu::Ppu(char* vram, char* ram, bool mirroring) {
	for (int i = 0; i < VRAMSIZE; ++i) {
		if (i < NT1INDEX)		//No mirroring for both Pattern Table and 1st Name Table
			this->mirrors[i] = i;
		else if (i < NT2INDEX)	//Mirroring of 2nd Name Table
			this->mirrors[i] = i - 0x2400 + (mirroring ? 0x2400 : 0x2000);
		else if (i < NT3INDEX)	//Mirroring of 3rd Name Table
			this->mirrors[i] = i - 0x2800 + (mirroring ? 0x2000 : 0x2400);
		else if (i < NT3INDEX + NTSIZE)	//Mirroring of 4th Name Table
			this->mirrors[i] = i - 0x2C00 + 0x2400;
		else if (i < IPINDEX)	//Mirroring of 0x2000 to 0x2EFF (why ?)
			this->mirrors[i] = 0x2000 + i - 0x3000;
		else if (i < SPINDEX)	//Mirroring Image Palette to Sprite Palette
			this->mirrors[i] = i + 0x10;
		else if (i < SPINDEX + PSIZE)	//No mirroring here
			this->mirrors[i] = i;
		else if (i < 0x4000)	//Mirroring of both Image and Sprite Palette
			this->mirrors[i] = ((i - 0x3F20) % 0x10) + 0x3F10;
		else                    //Mirroring of 0x0000 to 0x4000
			this->mirrors[i] = i - 0x4000;
	}
	this->vram = vram;
	this->ram = ram;
	this->oam = new char[0xFF]();
	this->screenMatrix = new char[HRESOLUTION * VRESOLUTION]();
	this->actualPixel = 0;
	this->actualScanline = 0;
}

Ppu::~Ppu() {
}

//Fetch the nametable address to use. NT from #0 to #3
inline uint16_t	Ppu::getNameTableIndex() {
	return (this->mirrors[(NTMASK(this->ram[PPUCTRL]) * NTSIZE) + NT0INDEX]);
}

//Fetch the address increment per CPU read/write of PPUDATA. Either 1 or 32
inline char		Ppu::getIncrement() {
	return (INCMASK(this->ram[PPUCTRL]) ? (32) : (1));
}

//Fetch the sprite pattern tabble address for 8x8 sprites. Either 0x1000 or 0x0000
inline uint16_t	Ppu::getSpritePatternTableIndex() {
	return (SPTMASK(this->ram[PPUCTRL]) ? (PT1INDEX) : (PT0INDEX));
}

//Fetch the background pattern table address. Either 0x1000 or 0x0000
inline uint16_t Ppu::getBackgroundPatternTableIndex() {
	return (BPTMASK(this->ram[PPUCTRL]) ? (PT1INDEX) : (PT0INDEX));
}

//Fetch the Non Maskable Interrupt on VBLANK flag status
inline bool		Ppu::getVBlankInterrupt() {
	return (VBIMASK(this->ram[PPUCTRL]) ? (true) : (false));
}

//Fetch the Background leftmost 8px hide flag status
inline bool		Ppu::getBackgroundMask() {
	return (BGMASK(this->ram[PPUMASK]) ? (false) : (true));
}

//Fetch the Sprites leftmost 8px hide fkag status
inline bool		Ppu::getSpriteMask() {
	return (SPMASK(this->ram[PPUMASK]) ? (false) : (true));
}

//Fetch the Background show flag status
inline bool		Ppu::getShowBackground() {
	return (BGSMASK(this->ram[PPUMASK]) ? (true) : (false));
}

//Fetch the Sprite show flag status
inline bool		Ppu::getShowSprite() {
	return (SPSMASK(this->ram[PPUMASK]) ? (true) : (false));
}

//Set Sprite overflow flag. Is set whenever more than 8 sprites by scanline are evaluated. Cleared at dot 1 of pre-render line. Hardware bugged on original NES
inline void		Ppu::setSpriteOverflow(bool overflow) {
	this->ram[PPUSTATUS] = ((overflow) ? (this->ram[PPUSTATUS] | SPOMASK) : (this->ram[PPUSTATUS] & (SPOMASK ^ UNSETMASK)));
}

//Set Sprite Hit flag. Is set whenever a nonzero pixel of sprite 0 overlaps a nonzero background pixel. Cleared at dot 1 of pre-render line.
inline void		Ppu::setSpriteHit(bool hit) {
	this->ram[PPUSTATUS] = ((hit) ? (this->ram[PPUSTATUS] | SPHMASK) : (this->ram[PPUSTATUS] & (SPHMASK ^ UNSETMASK)));
}

//Set VBlank flag. Is set when the VBlank starts (dot 1 of line 241, line after the post-render line). Cleared after reading PPUSTATUS and dot 1 of pre-render line.
inline void		Ppu::setVBlank(bool vblank) {
	this->ram[PPUSTATUS] = ((vblank) ? (this->ram[PPUSTATUS] | VBMASK) : (this->ram[PPUSTATUS] & (VBMASK ^ UNSETMASK)));
}

//When a CPU write occurs to OAMDMA, takes 256 bytes from CPU memory from $XX00 -> $XXFF to oam memory. Takes 512 cycles
inline void		Ppu::OamDmaWrite() {
	memcpy(this->oam, this->ram + (this->ram[OAMDMA] << 8), 0xFF);
}

void			Ppu::cycle(int cpuCycle) {
	for (int i = 0; i < 3; ++i) {
		if (this->actualScanline == 0) { //Pre-render scanlines
			;//I HAVE NO IDEA WHAT TO DO HERE
		}
		else if (this->actualScanline < 241) { //Render the 240 visible scanlines
			if (this->actualPixel == 0) { // Idle cycle
				;//STILL NO IDEA WHAT TO DO HERE
			}
			else if (this->actualPixel < 257) { //Fetching data for each tile
				;
			}
			else if (this->actualPixel < 321) { //Fetching data for next scanline sprites
				;
			}
			else if (this->actualPixel < 341) { //Nametable byte fetch
				;
			}
		}
		if (this->actualScanline == 242 && this->actualPixel == 0)
			this->setVBlank(true);
		this->actualPixel++;
		if (this->actualPixel == CYCLESPERSCANLINE) {
			this->actualPixel = 0;
			this->actualScanline++;
		}
	}
}