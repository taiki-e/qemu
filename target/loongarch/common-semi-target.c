/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Target-specific parts of semihosting/arm-compat-semi.c.
 *
 * Copyright (c) 2005, 2007 CodeSourcery.
 * Copyright (c) 2019, 2022 Linaro
 * Copyright (c) 2024 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "semihosting/common-semi.h"

uint64_t common_semi_arg(CPUState *cs, int argno)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    return env->gpr[4 + argno];
}

void common_semi_set_ret(CPUState *cs, uint64_t ret)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    env->gpr[4] = ret;
}

bool common_semi_sys_exit_is_extended(CPUState *cs)
{
    return is_64bit_semihosting(cpu_env(cs));
}

bool is_64bit_semihosting(CPUArchState *env)
{
    return !is_va32(env);
}

uint64_t common_semi_stack_bottom(CPUState *cs)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    return env->gpr[3];
}

bool common_semi_has_synccache(CPUArchState *env)
{
    return true;
}
