#pragma once
#include <3ds.h>
#include "exclusive_rw.hpp"
#include "CirclePad.hpp"

namespace ir {

struct PadEntry {
    int32_t currPadState;
    int32_t pressedPadState;
    int32_t releasedPadState;
    CirclePadEntry circlePadState;
};

class Ring {
public:
    Ring() {
        Reset();
    }

    void Reset() {
        mTickCount = -1;
        mOldTickCount = -1;
        mUpdatedIndex = -1;
    }

    void WriteToRing(PadEntry *entry, CirclePadEntry *circlePadEntry) {
        uint32_t index = mUpdatedIndex;

        if (index == 7)
            index = 0;
        else
            index++;

        ExclusiveWrite(&mEntries[index].currPadState, entry->currPadState);
        ExclusiveWrite(&mEntries[index].pressedPadState, entry->pressedPadState);
        ExclusiveWrite(&mEntries[index].releasedPadState, entry->releasedPadState);
        ExclusiveWrite((uint16_t*)&mEntries[index].circlePadState.x, circlePadEntry->x);
        ExclusiveWrite((uint16_t*)&mEntries[index].circlePadState.y, circlePadEntry->y);

        if (index == 0) { // When index is 0 we need to update tickcount
            mOldTickCount = mTickCount;
            mTickCount = svcGetSystemTick();
        }

        ExclusiveWrite(&mUpdatedIndex, index);
    }
private:
    int64_t mTickCount = 0;
    int64_t mOldTickCount = 0;
    int32_t mUpdatedIndex = 0;
    uint32_t padding;
    PadEntry mEntries[8];
};

} // End of namespace ir
