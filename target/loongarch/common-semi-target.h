/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Target-specific parts of semihosting/arm-compat-semi.c.
 *
 * Copyright (c) 2005, 2007 CodeSourcery.
 * Copyright (c) 2019, 2022 Linaro
 * Copyright (c) 2024 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef TARGET_LOONGARCH_COMMON_SEMI_TARGET_H
#define TARGET_LOONGARCH_COMMON_SEMI_TARGET_H

static inline target_ulong common_semi_arg(CPUState *cs, int argno)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    return env->gpr[4 + argno];
}

static inline void common_semi_set_ret(CPUState *cs, target_ulong ret)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    env->gpr[4] = ret;
}

static inline bool common_semi_sys_exit_extended(CPUState *cs, int nr)
{
    return (nr == TARGET_SYS_EXIT_EXTENDED || sizeof(target_ulong) == 8);
}

static inline bool is_64bit_semihosting(CPUArchState *env)
{
    return !is_va32(env);
}

static inline target_ulong common_semi_stack_bottom(CPUState *cs)
{
    LoongArchCPU *cpu = LOONGARCH_CPU(cs);
    CPULoongArchState *env = &cpu->env;
    return env->gpr[3];
}

static inline bool common_semi_has_synccache(CPUArchState *env)
{
    return true;
}

#endif
