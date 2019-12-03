#include "vgm.h"
#include "libsynth.h"
#include "badgetime.h"

#define BUTTON_A (1<<4)
extern volatile uint32_t MISC[];
#define MISC_REG(i) MISC[(i)/4]

#include <string.h>
#include <stdio.h>
extern FILE *f;

extern const uint8_t emeraldhill_vgm_start[] asm("_binary_emeraldhill_vgm_start");
extern const uint8_t emeraldhill_vgm_end[]   asm("_binary_emeraldhill_vgm_end");

void YM2612Write(uint8_t addr, uint8_t data)
{
	*(unsigned int *)(0x80040000 + (addr << 2)) = data;
}

void genVGMSamples(int nSamples) {
	// ~901 48 MHz cycles per sample
	// ~300 3 cycle instructions per sample
	while (nSamples--) {
		for (int i = 0; i < 48; i++) {
			__asm("nop");
			__asm("nop");
			__asm("nop");
		}
	}
}

void playVGM()
{
	const uint8_t *vgmPtr;
	const uint8_t *pcmDataPtr;
	uint32_t tmp;

	audio_regs->csr = 1;

	//vgmPtr = emeraldhill_vgm_start + *(unsigned int *)(emeraldhill_vgm_start + 0x34) + 0x34;
	vgmPtr = emeraldhill_vgm_start + 0x2f26; // HACK!
	pcmDataPtr = emeraldhill_vgm_start + 0x47;
	for (;;) {
		delay(0);
		if (MISC_REG(MISC_BTN_REG) & BUTTON_A)
			break;
		if ((*vgmPtr & 0xF0) == 0x70) {
			genVGMSamples((*vgmPtr++ & 0xF) + 1);
		} else if ((*vgmPtr & 0xF0) == 0x80) {
			YM2612Write(0, 0x2a);
			YM2612Write(1, *pcmDataPtr++);
			genVGMSamples(*vgmPtr++ & 0xF);
			vgmPtr++;
		} else {
			switch (*vgmPtr++) {
			case 0x4f: // PSG
			case 0x50: // PSG
				// Not supported
				vgmPtr++;
				break;
			case 0x52: // YM2612 port 0
				YM2612Write(0, *vgmPtr++);
				YM2612Write(1, *vgmPtr++);
				break;
			case 0x53: // YM2612 port 1
				YM2612Write(2, *vgmPtr++);
				YM2612Write(3, *vgmPtr++);
				break;
			case 0x61:
				tmp = *vgmPtr++;
				tmp |= (*vgmPtr++) << 8;
				genVGMSamples(tmp);
				break;
			case 0x62:
				genVGMSamples(735);
				break;
			case 0x63:
				genVGMSamples(882);
				break;
			case 0x66:
				//printf("LOOPING VGM\n");
				vgmPtr = emeraldhill_vgm_start + 0xb541;
				break;
			case 0xe0:
				tmp = *vgmPtr++;
				tmp |= (*vgmPtr++) << 8;
				tmp |= (*vgmPtr++) << 16;
				tmp |= (*vgmPtr++) << 24;
				pcmDataPtr = emeraldhill_vgm_start + 0x47 + tmp;
				break;
			}
		}
		if (vgmPtr >= emeraldhill_vgm_end) {
			//printf("LOOPING VGM\n");
			vgmPtr = emeraldhill_vgm_start + 0xb541;
		}
	}
}
