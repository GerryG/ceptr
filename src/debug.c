/**
 * @file debug.c
 * @copyright Copyright (C) 2013-2016, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */
#include "debug.h"
char G_dtype[1000]; // type debug buffer
char *dtypestr(int debug_flags) {
    char *s = &G_dtype[0];
    if (debug_flags & (D_REDUCE+D_REDUCEV)) {sprintf(s,"_REDUCE");s--;};
    if (debug_flags & D_SIGNALS) {sprintf(s,"_SIGNALS");s--;};
    if (debug_flags & D_STX_MATCH) {sprintf(s,"_STX_MATCH");s--;};
    if (debug_flags & D_STX_BUILD) {sprintf(s,"_STX_BUILD");s--;};
    if (debug_flags & D_CLOCK) {sprintf(s,"_CLOCK");s--;};
    if (debug_flags & D_STREAM) {sprintf(s,"_STREAM");s--;};
    if (debug_flags & D_LOCK) {sprintf(s,"_LOCK");s--;};
    if (debug_flags & D_LISTEN) {sprintf(s,"_LISTEN");s--;};
    if (debug_flags & D_TREE) {sprintf(s,"_TREE");s--;};
    if (debug_flags & D_PROTOCOL) {sprintf(s,"_PROTOCOL");s--;};
    if (debug_flags & D_TRANSCODE) {sprintf(s,"_TRANSCODE");s--;};
    if (debug_flags & D_STEP) {sprintf(s,"_STEP");s--;};
    if (debug_flags & D_SOCKET) {sprintf(s,"_SOCKET");s--;};
    if (debug_flags & D_BOOT) {sprintf(s,"_BOOT");s--;};
    *s=0;
    return G_dtype;
}

// set flags in input that you want to turn on (set)
// returns new debug mask
int G_debug_mask = DEBUG_DEFAULT_MASK;
int debug_enable(int debug_flags){
    return G_debug_mask |= debug_flags;
}
// set flags in input tht you want to turn off (clear)
// returns new debug mask
int debug_disable(int disable_flags){
    return G_debug_mask &= ~disable_flags;
}
