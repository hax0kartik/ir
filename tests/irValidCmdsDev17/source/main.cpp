#include <string>
#include <vector>
#include <3ds.h>

extern "C" {
	#include "i2c.h"
	#include "csvc.h"
}

#define SAVE_CURSOR_POS "\e[s"
#define RESTORE_CURSOR_POS "\e[u"
#define CLEAR_SCREEN "\e[2J\e[H"
#define CLEAR_LINE "\e[K"
#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define WHITE "\e[37m"
#define SET_CURSOR_POS(x, y) "\e[" #y ";" #x "H"

void PrintState(PrintConsole *top, circlePosition *pos, uint8_t cmdid) {
	consoleSelect(top);
	printf(SAVE_CURSOR_POS);
	printf(SET_CURSOR_POS(1, 27));
	printf(CLEAR_LINE"Press UP/DOWN to increment/decrement CMDID\n");
	printf(CLEAR_LINE"Press A to issue command\n");
	printf(CLEAR_LINE"Press B to launch extrapad\n");
	printf(SET_CURSOR_POS(1, 30));
	printf(CLEAR_LINE"CMDID: %02X | X: %02X Y: %02X", cmdid, pos->dx, pos->dy);
	printf(RESTORE_CURSOR_POS);
}

int main() {
	gfxInitDefault();

	PrintConsole top, bottom;

	consoleInit(GFX_TOP, &top);
	consoleInit(GFX_BOTTOM, &bottom);

	consoleSelect(&bottom);

	puts("Console attached!");

	Result ret = 1;

	if (ret != 0) {
		puts("i2cIrInit failed. Trying to steal handle!");
		Handle outHandle;
		ret = svcControlService(SERVICEOP_STEAL_CLIENT_SESSION, &outHandle, "i2c::IR");
		if (ret == 0) {
			puts("Handle stolen sucessfully!");
			i2cIrSetHandle(outHandle);
		}
	}

	circlePosition pos {};
	uint8_t cmdid = 0;

	uint8_t buf[0x100];

	while(aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		irrstCstickRead(&pos);

		auto kDown = hidKeysDown();

		if (kDown & KEY_START) {
			break;
		}

		if (kDown & KEY_DOWN) {
			if (cmdid == 0) {
				cmdid = 0xFF;
			} else {
				cmdid--;
			}
		}

		if (kDown & KEY_RIGHT) {
			cmdid += 0x10;
		}

		if (kDown & KEY_LEFT) {
			cmdid -= 0x10;
		}

		if (kDown & KEY_UP) {
			cmdid++;
		}

		if (kDown & KEY_A) {
			printf("Issuing command : %02X\n", cmdid);
			Result ret = I2C_WriteCommand8(17, cmdid);
			if (R_FAILED(ret)) {
				printf("I2C_WriteCommand8 failed: %08X\n", ret);
			}
		}

		if (kDown & KEY_B) {
			printf("Launching extrapad!\n");
			aptLaunchLibraryApplet(APPID_EXTRAPAD, buf, 0x100, 0);
		}

		PrintState(&top, &pos, cmdid);
		consoleSelect(&bottom);

		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}