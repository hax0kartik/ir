#include <3ds.h>
#include <cstdio>

#include "ipc/ipcHandler.hpp"

extern "C" {
#include "csvc.h"
#include "services.h"
}

#define ONERRSVCBREAK(ret) if(R_FAILED(ret)) svcBreak(USERBREAK_ASSERT);
#define OS_REMOTE_SESSION_CLOSED MAKERESULT(RL_STATUS,    RS_CANCELED, RM_OS, 26)
#define OS_INVALID_HEADER        MAKERESULT(RL_PERMANENT, RS_WRONGARG, RM_OS, 47)
#define OS_INVALID_IPC_PARAMATER MAKERESULT(RL_PERMANENT, RS_WRONGARG, RM_OS, 48)

static Result HandleNotifications(int *exit) {
    uint32_t notid = 0;
    const Result ret = srvReceiveNotification(&notid);

    if (R_FAILED(ret))
        return ret;

    switch (notid) {
        case 0x100: { // Exit
            *exit = 1;
            break;
        }

        case 0x104: // Entering sleep mode
        case 0x105: // Exiting sleep mode
            break;
    }

    return 0;
}

extern "C"
{
    extern u32 __ctru_heap, __ctru_heap_size, __ctru_linear_heap, __ctru_linear_heap_size;
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    // this is called before main
    void __system_allocateHeaps(void) {
        u32 tmp = 0;
        __ctru_heap_size = 0x8000;
        // Allocate the application heap
        __ctru_heap = 0x08000000;
        svcControlMemoryEx(&tmp, __ctru_heap, 0x0, __ctru_heap_size, MEMOP_ALLOC, (MemPerm)(MEMPERM_READWRITE | MEMREGION_BASE), false);
        // Set up newlib heap
        fake_heap_start = (char*)__ctru_heap;
        fake_heap_end = fake_heap_start + __ctru_heap_size;
    }

    void __appInit() {
        srvSysInit();
        //gdbHioDevInit();
        //gdbHioDevRedirectStdStreams(false, true, false);
        //  logInit();
    }

    // this is called after main exits
    void __appExit() {
        srvSysExit();
    }

    // stubs for non-needed pre-main functions
    void __sync_init();
    void __sync_fini();
    void __system_initSyscalls();
    void __libc_init_array(void);
    void __libc_fini_array(void);

    void initSystem(void (*retAddr)(void)) {
        __libc_init_array();
        __sync_init();
        __system_initSyscalls();
        __system_allocateHeaps();
        __appInit();
    }

    void __ctru_exit(int rc) {
        __appExit();
        __sync_fini();
        __libc_fini_array();
        svcExitProcess();
    }
}

int main() {
    const char *srvnames[] = {"", "ir:u", "ir:USER", "ir:rst"};

    const s32 SERVICECOUNT = 3;
    const s32 INDEXMAX = SERVICECOUNT * 2 + 1;
    const s32 REMOTESESSIONINDEX = SERVICECOUNT + 1;

    Handle sessionhandles[INDEXMAX];

    u32 serviceindexes[2 * SERVICECOUNT];

    s32 handlecount = SERVICECOUNT + 1;

    for (int i = 1; i <= SERVICECOUNT; i++)
        ONERRSVCBREAK(srvRegisterService(&sessionhandles[i], srvnames[i], 1));

    ONERRSVCBREAK(srvEnableNotification(&sessionhandles[0]));

    ONERRSVCBREAK(srvSubscribe(0x104));
    ONERRSVCBREAK(srvSubscribe(0x105));

    Handle target = 0;
    s32 targetindex = -1;
    int terminationflag = 0;

    ir::Initialize();

    for (;;) {
        s32 index;

        if (!target) {
            if (terminationflag && handlecount == REMOTESESSIONINDEX)
                break;
            else
                *getThreadCommandBuffer() = 0xFFFF0000;
        }

        Result res = svcReplyAndReceive(&index, sessionhandles, handlecount, target);
        s32 lasttargetindex = targetindex;
        target = 0;
        targetindex = -1;

        if (R_FAILED(res)) {

            if (res != OS_REMOTE_SESSION_CLOSED) {
                ONERRSVCBREAK(0xd9875);
            } else if (index == -1) {
                if (lasttargetindex == -1) {
                    ONERRSVCBREAK(0xd9874);
                } else
                    index = lasttargetindex;
            }

            else if (index >= handlecount)
                ONERRSVCBREAK(-1);

            svcCloseHandle(sessionhandles[index]);
            handlecount--;

            for (s32 i = index - REMOTESESSIONINDEX; i < handlecount - REMOTESESSIONINDEX; i++) {
                sessionhandles[REMOTESESSIONINDEX + i] = sessionhandles[REMOTESESSIONINDEX + i + 1];
                serviceindexes[i] = serviceindexes[i + 1];
            }

            continue;
        }

        if (index == 0)
            HandleNotifications(&terminationflag);

        else if (index >= 1 && index < REMOTESESSIONINDEX) {
            Handle newsession = 0;
            ONERRSVCBREAK(svcAcceptSession(&newsession, sessionhandles[index]));

            if (handlecount >= INDEXMAX) {
                svcCloseHandle(newsession);
                continue;
            }

            sessionhandles[handlecount] = newsession;
            serviceindexes[handlecount - REMOTESESSIONINDEX] = index - 1;
            handlecount++;

        } else if (index >= REMOTESESSIONINDEX && index < INDEXMAX) {
            const uint8_t sindex = serviceindexes[index - REMOTESESSIONINDEX];

            switch (sindex) {
                case 0:
                    ir::Handle<ir::Services::IRU>();
                    break;
                case 1:
                    ir::Handle<ir::Services::IRUSER>();
                    break;
                case 2:
                    ir::Handle<ir::Services::IRRST>();
                    break;
                default:
                    ONERRSVCBREAK(-5);
            }

            target = sessionhandles[index];
            targetindex = index;
        } else {
            ONERRSVCBREAK(-4);
        }
    }

    for (int i = 1; i <= SERVICECOUNT; i++) {
        srvUnregisterService(srvnames[i]);
        svcCloseHandle(sessionhandles[i + 1]);
    }

    svcCloseHandle(sessionhandles[0]);
}