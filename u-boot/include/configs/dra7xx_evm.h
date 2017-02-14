/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated.
 * Lokesh Vutla	  <lokeshvutla@ti.com>
 *
 * Configuration settings for the TI DRA7XX board.
 * See ti_omap5_common.h for omap5 common settings.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_DRA7XX_EVM_H
#define __CONFIG_DRA7XX_EVM_H

#define CONFIG_DRA7XX
#define CONFIG_DRA7XX_JAMR3
#define CONFIG_BOARD_EARLY_INIT_F

#ifdef CONFIG_SPL_BUILD
#define CONFIG_IODELAY_RECALIBRATION
#endif

#define CONFIG_VERY_BIG_RAM
#define CONFIG_PHYS_64BIT
#define CONFIG_NR_DRAM_BANKS		2
#define CONFIG_MAX_MEM_MAPPED		0x80000000

/* MMC ENV related defines */
#ifdef CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SLOT2: eMMC(1) */
#define CONFIG_ENV_SIZE			(128 << 10)
#define CONFIG_ENV_OFFSET		0xE0000
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#endif
#define CONFIG_CMD_SAVEENV

#if (CONFIG_CONS_INDEX == 1)
#define CONSOLEDEV			"ttyO0"
#elif (CONFIG_CONS_INDEX == 3)
#define CONSOLEDEV			"ttyO2"
#endif
#define CONFIG_SYS_NS16550_COM1		UART1_BASE	/* Base EVM has UART0 */
#define CONFIG_SYS_NS16550_COM2		UART2_BASE	/* UART2 */
#define CONFIG_SYS_NS16550_COM3		UART3_BASE	/* UART3 */
#define CONFIG_BAUDRATE			115200

#define CONFIG_SYS_OMAP_ABE_SYSCK

/* Define the default GPT table for eMMC */
#define PARTS_DEFAULT \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs}"

#include <configs/ti_omap5_common.h>

/* Enable option to stop boot process by any key
 * even when the boot delay is set to zero.
 */
#define CONFIG_ZERO_BOOTDELAY_CHECK

/* Enhance our eMMC support / experience. */
#define CONFIG_CMD_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_PART
#define CONFIG_HSMMC2_8BIT
#define CONFIG_HSMMC3_8BIT
#define CONFIG_OMAP_MMC1_HS_52
#define CONFIG_OMAP_MMC2_DDR_52

/* CPSW Ethernet */
#define CONFIG_CMD_NET			/* 'bootp' and 'tftp' */
#define CONFIG_IP_DEFRAG 		/* Allow assembly of IP fragments into one */
#define CONFIG_CMD_DHCP
#define CONFIG_BOOTP_DNS		/* Configurable parts of CMD_DHCP */
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_NET_RETRY_COUNT		10
#define CONFIG_CMD_PING
#define CONFIG_CMD_MII
#define CONFIG_DRIVER_TI_CPSW		/* Driver for IP block */
#define CONFIG_MII			/* Required in net/eth.c */
#define CONFIG_PHY_GIGE			/* per-board part of CPSW */
#define CONFIG_PHYLIB

/* SPI */
#undef	CONFIG_OMAP3_SPI
#define CONFIG_TI_QSPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_CMD_SF
#define CONFIG_CMD_SPI
#define CONFIG_SPI_FLASH_BAR
#define CONFIG_TI_SPI_MMAP
#define CONFIG_SF_DEFAULT_SPEED                64000000
#define CONFIG_DEFAULT_SPI_MODE                SPI_MODE_0

/*
 * Default to using SPI for environment, etc.
 * 0x000000 - 0x010000 : QSPI.SPL (64KiB)
 * 0x010000 - 0x020000 : QSPI.SPL.backup1 (64KiB)
 * 0x020000 - 0x030000 : QSPI.SPL.backup2 (64KiB)
 * 0x030000 - 0x040000 : QSPI.SPL.backup3 (64KiB)
 * 0x040000 - 0x140000 : QSPI.u-boot (1MiB)
 * 0x140000 - 0x1C0000 : QSPI.u-boot-spl-os (512KiB)
 * 0x1C0000 - 0x1D0000 : QSPI.u-boot-env (64KiB)
 * 0x1D0000 - 0x1E0000 : QSPI.u-boot-env.backup1 (64KiB)
 * 0x1E0000 - 0x9E0000 : QSPI.kernel (8MiB)
 * 0x9E0000 - 0x2000000 : USERLAND
 */
#define CONFIG_SYS_SPI_KERNEL_OFFS	0x1E0000
#define CONFIG_SYS_SPI_ARGS_OFFS	0x140000
#define CONFIG_SYS_SPI_ARGS_SIZE	0x80000
#if defined(CONFIG_QSPI_BOOT)
/* In SPL, use the environment and discard MMC support for space. */
#ifdef CONFIG_SPL_BUILD
#undef CONFIG_SPL_MMC_SUPPORT
#undef CONFIG_SPL_MAX_SIZE
#define CONFIG_SPL_MAX_SIZE             (128 << 10) /* 64 KiB */
#endif
#define CONFIG_SPL_ENV_SUPPORT
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_ENV_SPI_MAX_HZ           CONFIG_SF_DEFAULT_SPEED
#define CONFIG_ENV_SIZE			(64 << 10)
#define CONFIG_ENV_SECT_SIZE		(64 << 10) /* 64 KB sectors */
#define CONFIG_ENV_OFFSET		0x1C0000
#define CONFIG_ENV_OFFSET_REDUND	0x1D0000
#endif

#define CONFIG_DRA7XX_JAMR3

/* SPI SPL */
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_DMA_SUPPORT
#define CONFIG_TI_EDMA
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_BUS             0
#define CONFIG_SPL_SPI_CS              0
#define CONFIG_SYS_SPI_U_BOOT_OFFS     0x40000

#define CONFIG_SUPPORT_EMMC_BOOT

/* USB xHCI HOST */
#define CONFIG_CMD_USB
#define CONFIG_USB_HOST
#define CONFIG_USB_XHCI
#define CONFIG_USB_XHCI_OMAP
#define CONFIG_USB_STORAGE
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS 2

#define CONFIG_OMAP_USB_PHY
#define CONFIG_OMAP_USB2PHY2_HOST

/* SATA */
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_CMD_SCSI
#define CONFIG_LIBATA
#define CONFIG_SCSI_AHCI
#define CONFIG_SCSI_AHCI_PLAT
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	1
#define CONFIG_SYS_SCSI_MAX_LUN		1
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
						CONFIG_SYS_SCSI_MAX_LUN)

/* Parallel NOR Support */
#if defined(CONFIG_NOR)
/* NOR: device related configs */
#define CONFIG_SYS_MAX_FLASH_SECT	512
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_SIZE		(64 * 1024 * 1024) /* 64 MB */
/* #define CONFIG_INIT_IGNORE_ERROR */
#undef CONFIG_SYS_NO_FLASH
#define CONFIG_CMD_FLASH
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
#define CONFIG_SYS_FLASH_PROTECTION
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_MTD
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_BASE		(0x08000000)
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE
/* Reduce SPL size by removing unlikey targets */
#ifdef CONFIG_NOR_BOOT
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE		(128 * 1024)	/* 128 KiB */
#define MTDIDS_DEFAULT			"nor0=physmap-flash.0"
#define MTDPARTS_DEFAULT		"mtdparts=physmap-flash.0:" \
					"1m(NOR.u-boot)," \
					"128k(NOR.u-boot-env)," \
					"128k(NOR.u-boot-env.backup1)," \
					"256k(NOR.u-boot-spl-os)," \
					"8m(NOR.kernel)," \
					"-(NOR.rootfs)"
#define CONFIG_ENV_OFFSET		0x00100000
#define CONFIG_ENV_OFFSET_REDUND	0x00120000
#endif
#endif  /* NOR support */

/* Enforce secure boot for high secure devices */
#ifdef CONFIG_OMAP_SECURE
#define CONFIG_SECURE_BOOT
#define CONFIG_SECURE_IMAGE_AUTH
#define CONFIG_SECURE_FW_MEMORY_SIZE            0x00680000
#define CONFIG_SECURE_NOFW_MEMORY_SIZE          0x00180000
/* Set aside protected RAM region - size in KB */
#define CONFIG_PRAM                            (CONFIG_SECURE_FW_MEMORY_SIZE + \
                                                CONFIG_SECURE_NOFW_MEMORY_SIZE) >> 10
#endif

/* pcf support */
#define CONFIG_PCF8575
#define CONFIG_SYS_I2C_PCF8575_CHIP { {0x21, 0xeaf7} }

/*
 * Enable this flag if you want to boot remotecores
 * from SPL. The specific cores loaded is controlled
 * by the array cores_to_boot[] in spl.c:board_init_r()
 */
#undef CONFIG_LATE_ATTACH

#ifdef CONFIG_LATE_ATTACH

/* Enable this flag to use a DMA pool with late attach instead of a CMA pool */
#undef CONFIG_LATE_ATTACH_DMA_POOL

/* Include ELF loader for parsing remotecore binaries */
#define CONFIG_CMD_ELF
#endif

/* Define the address to which the IPU binary is
 * loaded from persistent storage
 */
#define IPU_LOAD_ADDR		0xa0fff000
/* Using the same load address for the DSP binary as well as the IPU binary.
 * This is not an issue as we are performing the loads in serial.
 */
#define DSP_LOAD_ADDR		(IPU_LOAD_ADDR)

#define CONFIG_CREATE_BOARD_SYMLINK
/* EEPROM */
#define CONFIG_EEPROM_CHIP_ADDRESS 0x50
#define CONFIG_EEPROM_BUS_ADDRESS 0

#endif /* __CONFIG_DRA7XX_EVM_H */
