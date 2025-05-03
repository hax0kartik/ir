#pragma once
#include <3ds.h>
#include <type_traits>

constexpr static void inline ExclusiveWrite(auto *addr, auto val) {
    if constexpr(std::is_same_v<decltype(val), int8_t> || std::is_same_v<decltype(val), uint8_t>) {
        do
            __ldrexb(addr);

        while (__strexb(addr, val));
    } else if constexpr(std::is_same_v<decltype(val), int16_t> || std::is_same_v<decltype(val), uint16_t>) {
        do
            __ldrexh(addr);

        while (__strexh(addr, val));
    } else if constexpr(std::is_same_v<decltype(val), int32_t> || std::is_same_v<decltype(val), uint32_t>) {
        do
            __ldrex(addr);

        while (__strex(addr, val));
    } else {
        // #error "Unknown Type"
    }
}