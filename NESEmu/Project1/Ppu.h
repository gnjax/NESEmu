#pragma once
#include <cstdint>
#include <queue>
#include "Nes.h"

#define		PTSIZE			0x1000
#define		NTSIZE			0x400
#define		PSIZE			0x10
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
#define		UNSETMASK		0b11111111
#define		SPOMASK			0b00100000
#define		SPHMASK			0b01000000
#define		VBMASK			0b10000000

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
	char		attributeTable;
	char		lowTile;
	char		highTile;
} t_tile;

class Ppu {
	char				*vram;
	char				*ram;
	char				*oam;
	char				*screenMatrix;
	uint16_t			mirrors[0x8000];
	int					actualScanline;
	int					actualPixel;
	bool				evenFrame;
	std::queue<t_tile>	tilesQueue;
	t_tile				currentTile;
	uint16_t			nameTableOffset;
	uint16_t			attributeTableOffset;
	bool				writeToggle; //Shared between PPUSCROLL and PPUADDR
	uint16_t			scrollX;
	uint16_t			scrollY;
	uint16_t			ppuaddr;
public:
	Ppu(char*, char*, bool);
	~Ppu();
	uint16_t	getNameTableIndex();
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
	void		OamDmaWrite();
	void		render(int x, int y);
	void		tileFetch();
	void		cycle(int);
};

