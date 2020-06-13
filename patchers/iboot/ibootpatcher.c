/*
 * Copyright 2013-2016, iH8sn0w. <iH8sn0w@iH8sn0w.com>
 *
 * This file is part of iBoot32Patcher.
 *
 * iBoot32Patcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * iBoot32Patcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with iBoot32Patcher.  If not, see <http://www.gnu.org/licenses/>.
 */

/* iBoot32Patcher
 *
 * Universal 32-bit iBoot patcher for iPhone OS 2.0 --> iOS 10
 *
 * Build:
 * clang iBoot32Patcher.c finders.c functions.c patchers.c -Wno-multichar -I. -o iBoot32Patcher
 *
 * Usage:
 * ./iBoot32Patcher iBoot.n49.RELEASE.dfu.decrypted iBoot.n49.RELEASE.dfu.patched
 * ./iBoot32Patcher iBoot.n49.RELEASE.dfu.decrypted iBoot.n49.RELEASE.dfu.patched -b "cs_enforcement_disable=1 -v"
 * ./iBoot32Patcher iBoot.n49.RELEASE.dfu.decrypted iBoot.n49.RELEASE.dfu.patched -b "cs_enforcement_disable=1" -c "ticket" 0x80000000
 * ./iBoot32Patcher iBoot.n49.RELEASE.dfu.decrypted iBoot.n49.RELEASE.dfu.patched -c "ticket" 0x80000000
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <includes/arm32_defs.h>
#include <includes/finders.h>
#include <includes/functions.h>
#include <includes/iboot_const.h>
#include <includes/patchers.h>

int patch_iboot(char* address) {
	int ret = 0;
	struct iboot_img iboot_in;

	memset(&iboot_in, 0, sizeof(iboot_in));

	iboot_in.buf = address;
	iboot_in.len = 0x60000;

	const char* iboot_vers_str = (char*)((uintptr_t)iboot_in.buf + IBOOT_VERS_STR_OFFSET);

	iboot_in.VERS = atoi(iboot_vers_str);
	if(!iboot_in.VERS) {
		return -1;
	}

	/* Check to see if the loader has a kernel load routine before trying to apply custom boot args + debug-enabled override. */
	if(has_kernel_load(&iboot_in)) {
		ret = patch_debug_enabled(&iboot_in);
		if(ret != 0) {
			return -1;
		}
		if(has_ticket_check(&iboot_in)) {
			ret = patch_boot_args(&iboot_in, "-v rd=md0 amfi=0xff cs_enforcement_disable=1");
			if(ret != 0) {
				return -1;
			}
			patch_ticket_check(&iboot_in); // No return check as loader may not have a ticket check (if old enough.)
		}
		else {
			ret = patch_boot_args(&iboot_in, "-v amfi=0xff cs_enforcement_disable=1");
			if(ret != 0) {
				return -1;
			}
		}
	}
	ret = patch_rsa_check(&iboot_in);
	if(ret != 0) {
		return -1;
	}
	ret = patch_next_stage(&iboot_in);
	if(ret != 0) {
		return -1;
	}
	ret = patch_go(&iboot_in);
	if(ret != 0) {
		return -1;
	}
	return 0;
}
