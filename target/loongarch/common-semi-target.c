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

static uint64_t semi_gpr(CPULoongArchState *env, unsigned reg)
{
    uint64_t val = env->gpr[reg];

    /* VA32 memory accesses use the low 32 bits of a GPR. */
    return is_va32(env) ? (uint32_t)val : val;
}

uint64_t common_semi_arg(CPUState *cs, int argno)
{
    return semi_gpr(cpu_env(cs), 4 + argno);
}

void common_semi_set_ret(CPUState *cs, uint64_t ret)
{
    CPULoongArchState *env = cpu_env(cs);

    /* Match the sign extension of a guest word result in TCG. */
    env->gpr[4] = is_va32(env) ? (uint64_t)(int32_t)ret : ret;
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
    return semi_gpr(cpu_env(cs), 3);
}

bool common_semi_has_synccache(CPUArchState *env)
{
    return true;
}
