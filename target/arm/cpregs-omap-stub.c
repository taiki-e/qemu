/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "qemu/osdep.h"
#include "target/arm/cpu-qom.h"
#include "internals.h"

void define_omap_cp_regs(ARMCPU *cpu)
{
    /* OMAP CP15 registers are accessible only to privileged code. */
}
