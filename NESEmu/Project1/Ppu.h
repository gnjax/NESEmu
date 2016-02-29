#pragma once
#include <cstdint>
#include <queue>
#include "Nes.h"
#include "NESToRGBA.h"

#define		PTSIZE			0x1000
#define		NTSIZE			0x400
#define		PSIZECACA		0x10
#define		VRAMSIZE		0x8000
#define		PT0INDEX		0x0
#define		PT1INDEX		0x1000
#define		NT0INDEX		0x2000
#define		NT1INDEX		0x2400
#define		NT2INDEX		0x2800
#define		NT3INDEX		0x2c00
#define		IPINDEX			0x3F00
#define		SPINDEX			0x3F10

#define		NTMASK(x)		(x & 0b00000011)
#define		INCMASK(x)		((x & 0b00000100) >> 2)
#define		SPTMASK(x)		((x & 0b00001000) >> 3)
#define		BPTMASK(x)		((x & 0b00010000) >> 4)
#define		VBIMASK(x)		((x & 0b10000000) >> 7)
#define		BGMASK(x)		((x & 0x00000010) >> 1)
#define		SPMASK(x)		((x & 0b00000100) >> 2)
#define		BGSMASK(x)		((x & 0b00001000) >> 3)
#define		SPSMASK(x)		((x & 0b00010000) >> 4)
#define		SPOMASK			0b00100000
#define		SPHMASK			0b01000000
#define		VBMASK			0b10000000

#define		COARSEXMASK		0x001F
#define		COARSEYMASK		0x03E0
#define		HTABLEMASK		0x0400
#define		VTABLEMASK		0x0800
#define		FINEYMASK		0x7000
#define		MAXCOARSEX		31
#define		MAXCOARSEY		29
#define		COARSEYOVF		31
#define		MAXFINEY		0x7000
#define		NTBYTEFETCH(x)		(0x2000 | (x & 0x0FFF))
#define		ATTRBYTEFETCH(x)	(0x23C0 | (x & 0x0C00) | ((x >> 4) & 0x38) | ((x >> 2) & 0x07))
#define		ATTROFFSET(x)		(x & 0x0020 == 0x0020) ? (4 + (x & 0x0002)) : (x & 0x0002)
//IF D1 = 1 -> right else left
//IF D5 = 1 -> bottom else top

#define		CYCLESPERSCANLINE	341
#define		HRESOLUTION			256
//NTSC
#define		SCANLINES			262
//#define	VRESOLUTION			224

//PAL
// #define	SCANLINES			312
#define		VRESOLUTION			240

typedef struct s_tile {
	uint16_t	nameTable;
	uint16_t	attributeTable;
	uint16_t	lowTile;
	uint16_t	highTile;
} t_tile;

typedef struct s_register {
	bool				writeToggle;
	uint16_t			currentAddress;
	uint16_t			temporaryAddress;
	unsigned char		fineXScroll;
	uint16_t			lowPlaneShift;
	uint16_t			highPlaneShift;
	uint16_t			lowPaletteShift;
	uint16_t			highPaletteShift;
	unsigned char		spritesX[8];
	unsigned char		spritesLowPlaneShift[8];
	unsigned char		spritesHighPlaneShift[8];
	unsigned char		spritesAttributes[8];
} t_register;

class Ppu {
	char				*vram;
	char				*ram;
	unsigned char		*oam;
	unsigned char		*secondaryOam;
	char				*screenMatrix;
	char				*output;
	unsigned char		oamAddr;
	uint16_t			vramMirrors[0x8000];
	int					actualScanline;
	int					actualPixel;
	bool				evenFrame;
	t_tile				currentTile;
	t_register			registers;
	NESToRGBA			converter;
	bool				frameRendered;
	int					spritesRegistersCounter;
	bool				initialization;
	bool				nmi;
public:
	Ppu(char*, char*, char *, bool);
	~Ppu();
	bool		nmiOccured();
	void		PpuControlWrite();
	void		PpuMaskWrite();
	void		PpuStatusRead();
	void		PpuOamAddressWrite();
	void		PpuOamDataRead();
	void		PpuOamDataWrite();
	void		PpuScrollWrite();
	void		PpuAddrWrite();
	void		PpuDataWrite();
	void		PpuDataRead();
	void		PpuOamDmaWrite();
	void		getNameTableIndex();
	char		getIncrement();
	uint16_t	getSpritePatternTableIndex();
	uint16_t	getBackgroundPatternTableIndex();
	bool		getVBlankInterrupt();
	bool		getBackgroundMask();
	bool		getSpriteMask();
	bool		getShowBackground();
	bool		getShowSprite();
	void		setSpriteOverflow(bool);
	void		setSpriteHit(bool);
	void		setVBlank(bool);
	void		getPpuScroll();
	void		getPpuAddr();
	void		render();
	void		loadIntoShiftRegisters();
	void		tileFetch();
	void		spriteFetch();
	void		addressWrap();
	bool		isFrameRendered();
	int			getCycle();
	int			getScanline();
	void		cycle(int);
};

