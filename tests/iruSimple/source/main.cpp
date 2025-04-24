#include <string>
#include <malloc.h>
#include <3ds.h>

static u8 CTR_ALIGN(4) BUF[16];

void send() {
	puts("User pressed A - Sending packets!");

	u8 packet[] = {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x89, 0x89};

	for (int i = 0; i < 10; i++) {
		BUF[i] = packet[i];
	}

	Result ret = iruSendData(&BUF[0], 10, false);
	printf("iruSendData: %08X\n", ret);

	Handle event = 0;
	ret = IRU_GetSendFinishedEvent(&event);
	printf("IRU_GetSentEvent: %08X\n", ret);

	puts("Waiting for all packets to be sent!");

	svcWaitSynchronization(event, -1LL);

	puts("All packet sent!");
}

void recieve(u8* sharedMemAddr) {
	puts("User pressed B - Recieving packets!");

	u32 transferCount = 0;

	puts("Waiting for packets to arrive!");

	Result ret = IRU_StartRecvTransfer(10, 0);
	printf("IRU_StartRecvTransfer: %08X\n", ret);

	Handle event = 0;
	ret = IRU_GetRecvFinishedEvent(&event);
	printf("IRU_GetRecvEvent: %08X\n", ret);

	svcWaitSynchronization(event, -1LL);

	ret = IRU_WaitRecvTransfer(&transferCount);
	printf("IRU_WaitRecvTransfer: %08X | transferCount: %d\n", ret, transferCount);

	u8* recvpacket = (u8*)sharedMemAddr;

	printf("Packet recieved: ");
	for (int i = 0; i < transferCount; i++) {
		printf("%d, ", recvpacket[i]);
	}
	printf("\n");

	puts("All packet recieved\n");
}

int main() {
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	puts("Console attached!");

	puts("----ir:u Init ----");
	
	u8* sharedMemAddr = 0;
	u32 sharedMemSize = 0x1000;
	sharedMemAddr = (u8*)memalign(0x1000, sharedMemSize);
	Result ret = iruInit((u32*)sharedMemAddr, sharedMemSize);
	printf("iruInit: %08X\n", ret);

	ret = IRU_SetBitRate(3);
	printf("IRU_SetBitRate: %08X\n", ret);

	u8 bitrate = 0;
	ret = IRU_GetBitRate(&bitrate);
	printf("IRU_GetBitRate: %08X | Bitrate: %d (Should be 3) \n", ret, bitrate);

	puts("------------------");

	puts("Press A to run as sender / B to run as reciever");

	while(aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		auto kDown = hidKeysDown();

		if (kDown & KEY_A) {
			send();
		}

		if (kDown & KEY_B) {
			recieve(sharedMemAddr);
		}

		if (kDown & KEY_START) {
			break;
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}