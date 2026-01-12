/*
 * Semihosting Tests - LoongArch Helper
 *
 * Copyright (c) 2024 Jiaxun Yang <jiaxun.yang@flygoat.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

uintptr_t __semi_call(uintptr_t type, uintptr_t arg0)
{
    register uintptr_t t asm("a0") = type;
    register uintptr_t a0 asm("a1") = arg0;

    asm("dbcl 0xab\n\t"
        : "=r" (t)
        : "r" (t), "r" (a0));

    return t;
}
