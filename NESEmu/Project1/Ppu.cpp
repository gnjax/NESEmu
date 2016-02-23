#include "Ppu.h"

Ppu::Ppu(char* vram, char* ram, bool mirroring) {
	for (int i = 0; i < VRAMSIZE; ++i) {
		if (i < NT1INDEX)		//No mirroring for both Pattern Table and 1st Name Table
			this->vramMirrors[i] = i;
		else if (i < NT2INDEX)	//Mirroring of 2nd Name Table
			this->vramMirrors[i] = i - 0x2400 + (mirroring ? 0x2400 : 0x2000);
		else if (i < NT3INDEX)	//Mirroring of 3rd Name Table
			this->vramMirrors[i] = i - 0x2800 + (mirroring ? 0x2000 : 0x2400);
		else if (i < NT3INDEX + NTSIZE)	//Mirroring of 4th Name Table
			this->vramMirrors[i] = i - 0x2C00 + 0x2400;
		else if (i < IPINDEX)	//Mirroring of 0x2000 to 0x2EFF (why ?)
			this->vramMirrors[i] = 0x2000 + i - 0x3000;
		else if (i < SPINDEX + PSIZE)	//Mirroring every 4 bytes to the universal background color (transparent) located at IPINDEX
			this->vramMirrors[i] = (((i % 0x4) == 0) ? (IPINDEX) : (i));
		else if (i < 0x4000)	//Mirroring of both Image and Sprite Palette
			this->vramMirrors[i] = ((i - 0x3F20) % 0x10) + 0x3F10;
		else                    //Mirroring of 0x0000 to 0x4000
			this->vramMirrors[i] = i - 0x4000;
	}
	this->vram = vram;
	this->ram = ram;
	this->oam = new char[0xFF]();
	this->screenMatrix = new char[HRESOLUTION * VRESOLUTION]();
	this->actualPixel = 0;
	this->actualScanline = 0;
	this->evenFrame = true;
	this->registers.currentAddress = NT0INDEX;
	this->registers.temporaryAddress = NT0INDEX;
	this->registers.writeToggle = false;
}

Ppu::~Ppu() {
}

void			Ppu::PpuControlWrite() { //write to PPUCTRL
	this->getNameTableIndex();
}

void			Ppu::PpuMaskWrite() { //write to PPUMASK

}

void			Ppu::PpuStatusRead() { //read to PPUSTATUS
	this->registers.writeToggle = false;
}

void			Ppu::PpuOamAddressWrite() { //write to OAMADDR

}

void			Ppu::PpuOamDataRead() { //read to OAMDATA

}

void			Ppu::PpuOamDataWrite() { //write to OAMDATA

}

void			Ppu::PpuScrollWrite() { //write to PPUSCROLL

}

void			Ppu::PpuDataWrite() { //write to PPUDATA

}

void			Ppu::PpuDataRead() { //read to PPUDATA

}

void			Ppu::PpuOamDmaWrite() { //write to OAMDMA

}

//Fetch the nametable address to use. NT from #0 to #3
inline void		Ppu::getNameTableIndex() {
	this->registers.temporaryAddress &= 0xF3FF; //Clear Nametable bits
	this->registers.temporaryAddress |= ((uint16_t)(this->ram[PPUCTRL] & 0b00000011) << 10);
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
	this->ram[PPUSTATUS] = ((overflow) ? (this->ram[PPUSTATUS] | SPOMASK) : (this->ram[PPUSTATUS] & ~SPOMASK));
}

//Set Sprite Hit flag. Is set whenever a nonzero pixel of sprite 0 overlaps a nonzero background pixel. Cleared at dot 1 of pre-render line.
inline void		Ppu::setSpriteHit(bool hit) {
	this->ram[PPUSTATUS] = ((hit) ? (this->ram[PPUSTATUS] | SPHMASK) : (this->ram[PPUSTATUS] & ~SPHMASK));
}

//Set VBlank flag. Is set when the VBlank starts (dot 1 of line 241, line after the post-render line). Cleared after reading PPUSTATUS and dot 1 of pre-render line.
inline void		Ppu::setVBlank(bool vblank) {
	this->ram[PPUSTATUS] = ((vblank) ? (this->ram[PPUSTATUS] | VBMASK) : (this->ram[PPUSTATUS] & ~VBMASK));
}

void			Ppu::getPpuScroll() { //Get the PPUSCROLL register value. 2 successives writes for respectively x and y.
	if (!this->registers.writeToggle) {
		this->registers.fineXScroll = this->ram[PPUSCROLL] & 0b00000111;
		this->registers.temporaryAddress &= 0xFFE0; // Clears Coarse X bits from address;
		this->registers.temporaryAddress |= (this->ram[PPUSCROLL] >> 3); // And put the new value in it
	}
	else {
		this->registers.temporaryAddress &= 0x8C1F; // Clears Coarse Y and fine Y bits from address;
		this->registers.temporaryAddress |= ((uint16_t)(this->ram[PPUSCROLL] & 0b00000111) << 13);
		this->registers.temporaryAddress |= ((uint16_t)(this->ram[PPUSCROLL] & 0b11111000) << 2);
	}
	this->registers.writeToggle = !this->registers.writeToggle;
}

void			Ppu::getPpuAddr() { //Get the PPUADDR register vlaue. 2 successives writes for higher and lower bytes of the 2 bytes address.
	if (!this->registers.writeToggle)
		this->registers.temporaryAddress = (uint16_t)(this->ram[PPUADDR]) << 8;
	else
		this->registers.temporaryAddress |= this->ram[PPUADDR];
	this->registers.writeToggle = !this->registers.writeToggle;
}

//When a CPU write occurs to OAMDMA, takes 256 bytes from CPU memory from $XX00 -> $XXFF to oam memory. Takes 512 cycles
inline void		Ppu::OamDmaWrite() {
	memcpy(this->oam, this->ram + ((uint16_t)(this->ram[OAMDMA]) << 8), 0xFF);
}

inline void		Ppu::render() {
	char color;
	color = ((this->registers.lowPlaneShift >> this->registers.fineXScroll) & 0x0001) | \
		(((this->registers.highPlaneShift >> this->registers.fineXScroll) & 0x0001) << 1) | \
		(((this->registers.lowPaletteShift >> this->registers.fineXScroll) & 0x0001) << 2) | \
		(((this->registers.highPaletteShift >> this->registers.fineXScroll) & 0x0001) << 3);
	int	screenOffset = (this->actualPixel - 1) ? (this->actualScanline * (this->actualPixel - 1)) : (this->actualScanline);
	this->screenMatrix[screenOffset] = this->vram[this->vramMirrors[IPINDEX + color]];
	this->registers.lowPlaneShift >>= 1;
	this->registers.highPlaneShift >>= 1;
	this->registers.lowPaletteShift >>= 1;
	this->registers.highPaletteShift >>= 1;
}

inline void		Ppu::loadIntoShiftRegisters() {
	this->registers.lowPlaneShift |= (this->vram[this->currentTile.lowTile] << 8);
	this->registers.highPlaneShift |= (this->vram[this->currentTile.highTile] << 8);
	//TEMPORARY - ONLY GREY SCALE
	this->registers.lowPaletteShift = 0;
	this->registers.highPaletteShift = 0;
}

inline void		Ppu::tileFetch() {
	if ((this->actualPixel % 8) == 0) {
		this->currentTile.highTile = this->currentTile.lowTile + 0x8;
		this->loadIntoShiftRegisters();
	}
	else if ((this->actualPixel % 6) == 0)
		this->currentTile.lowTile = this->getBackgroundPatternTableIndex() + (this->currentTile.nameTable << 4) + ((this->registers.currentAddress & FINEYMASK) >> 12);
	else if ((this->actualPixel % 4) == 0)
		this->currentTile.attributeTable = this->vram[this->vramMirrors[ATTRBYTEFETCH(this->registers.currentAddress)]];
	else if ((this->actualPixel % 2) == 0)
		this->currentTile.nameTable = this->vram[this->vramMirrors[NTBYTEFETCH(this->registers.currentAddress)]];
}

inline void		Ppu::addressWrap() {
	if ((this->actualPixel % 8) == 0) { //After each tile fetch
		if ((this->registers.currentAddress & COARSEXMASK) == MAXCOARSEX) {
			this->registers.currentAddress &= ~COARSEXMASK; //Setting Coarse X bits to 0
			this->registers.currentAddress ^= HTABLEMASK; //Switching the horizontal table bit
		}
		else
			this->registers.currentAddress += 1;
	}
	if (this->actualPixel == HRESOLUTION) {
		if ((this->registers.currentAddress & FINEYMASK) != MAXFINEY)
			this->registers.currentAddress += 0x1000; //Increment Fine Y by 1;
		else {
			this->registers.currentAddress &= ~FINEYMASK; //Setting Fine Y bits to 0
			uint16_t coarsey = (this->registers.currentAddress & COARSEYMASK) >> 5;
			if (coarsey == MAXCOARSEY) {
				coarsey = 0;
				this->registers.currentAddress ^= VTABLEMASK; //Switching the vertical table bit
			}
			else if (coarsey == COARSEYOVF) //Might happen when reading attribute tables
				coarsey = 0;
			else
				coarsey += 1;
			this->registers.currentAddress = (this->registers.currentAddress & COARSEYMASK) | (coarsey << 5); //Puting coarseY back into the current address
		}
	}
}

void			Ppu::cycle(int cpuCycle) {
	for (int i = 0; i < 3; ++i) {
		if (this->actualScanline == 0) { //Pre-render scanline
			if (this->actualPixel >= 321 && this->actualPixel < 337)
				this->tileFetch();
		}
		else if (this->actualScanline < 241) { //Render the 240 visible scanlines
			if (this->actualPixel == 0) { // Idle cycle
				;//NOTHING TO DO HERE
			}
			else if (this->actualPixel < 257) { //Fetching data for each tile
				this->tileFetch();
				this->render();
				this->addressWrap(); //needs to be last instruction here.
			}
			else if (this->actualPixel < 321) { //Fetching data for next scanline sprites
				;
			}
			else if (this->actualPixel < 337) { //Fetching 2 tiles for the next scanline
				this->tileFetch();
			}
			else if (this->actualPixel < 341) { //Nametable byte fetch
				;
			}
		}
	
		if (this->actualScanline == 242 && this->actualPixel == 1) //VBLANK HIT
			if (this->getVBlankInterrupt())
				this->setVBlank(true);
		this->actualPixel++;
		if (this->actualPixel == CYCLESPERSCANLINE) { //End of the scanline
			this->actualPixel = 0;
			this->actualScanline++;
			if (this->actualScanline == 1 && !this->evenFrame) {
				this->actualPixel = 1; //Skip idle cycle on scanline 1 if odd frame
			}
		}
		if (this->actualScanline == SCANLINES) { //End of the frame
			this->actualScanline = 0;
			this->evenFrame = !this->evenFrame;
			this->registers.currentAddress = this->registers.temporaryAddress;
		}
	}
}