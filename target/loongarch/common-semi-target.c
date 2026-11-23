/*
 * Target-specific parts of semihosting/arm-compat-semi.c.
 *
 * Copyright (c) 2024 Jiaxun Yang <jiaxun.yang@flygoat.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "semihosting/common-semi.h"

uint64_t common_semi_arg(CPUState *cs, int argno)
{
    CPULoongArchState *env = cpu_env(cs);
    return env->gpr[4 + argno];
}

void common_semi_set_ret(CPUState *cs, uint64_t ret)
{
    CPULoongArchState *env = cpu_env(cs);
    env->gpr[4] = ret;
}

bool is_64bit_semihosting(CPUArchState *env)
{
    return !is_va32(env);
}

bool common_semi_sys_exit_is_extended(CPUState *cs)
{
    return is_64bit_semihosting(cpu_env(cs));
}

uint64_t common_semi_stack_bottom(CPUState *cs)
{
    CPULoongArchState *env = cpu_env(cs);
    return env->gpr[3];
}

bool common_semi_has_synccache(CPUArchState *env)
{
    return true;
}
