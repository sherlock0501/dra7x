#
# Copyright 2011 Linaro Limited
#
# Aneesh V <annesh@ti.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

ifdef CONFIG_SPL_BUILD
ifdef CONFIG_PERIPHERAL_BOOT
ALL-y	+= ULO
else
ALL-y	+= MLO
endif
else
ALL-y	+= u-boot.img
endif
