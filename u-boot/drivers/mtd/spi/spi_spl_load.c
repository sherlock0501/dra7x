/*
 * Copyright (C) 2011 OMICRON electronics GmbH
 *
 * based on drivers/mtd/nand/nand_spl_load.c
 *
 * Copyright (C) 2011
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <spi_flash.h>
#include <spl.h>

#if defined(CONFIG_SPL_OS_BOOT) && !defined(CONFIG_SPL_QSPI_OS_IN_MMC)
/*
 * Load the kernel, check for a valid header we can parse, and if found load
 * the kernel and then device tree.
 */
static int spi_load_image_os(struct spi_flash *flash,
			     struct image_header *header)
{
	/* Read for a header, parse or error out. */
	spi_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS, 0x40,
		       (void *)header);

	if (image_get_magic(header) != IH_MAGIC)
		return -1;

	spl_parse_image_header(header);

	spi_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS,
		       spl_image.size, (void *)spl_image.load_addr);

	/* Read device tree. */
	spi_flash_read(flash, CONFIG_SYS_SPI_ARGS_OFFS,
		       CONFIG_SYS_SPI_ARGS_SIZE,
		       (void *)CONFIG_SYS_SPL_ARGS_ADDR);

	return 0;
}
#endif

/*
 * The main entry for SPI booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from SPI into SDRAM and starts it from there.
 */
void spl_spi_load_image(void)
{
	struct spi_flash *flash;
	struct image_header *header;

	/*
	 * Load U-Boot image from SPI flash into RAM
	 */

	flash = spi_flash_probe(CONFIG_SPL_SPI_BUS, CONFIG_SPL_SPI_CS,
				CONFIG_SF_DEFAULT_SPEED, CONFIG_DEFAULT_SPI_MODE);
	if (!flash) {
		puts("SPI probe failed.\n");
		hang();
	}

	/* use CONFIG_SYS_TEXT_BASE as temporary storage area */
	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE);

#ifdef CONFIG_SPL_OS_BOOT
	if (spl_start_uboot() ||
#ifdef CONFIG_SPL_QSPI_OS_IN_MMC
	    spl_mmc_load_image_raw_os()
#else
	    spi_load_image_os(flash, header)
#endif
	)
#endif
	{
		/* Load u-boot, mkimage header is 64 bytes. */
		spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS, 0x40,
			       (void *)header);
		spl_parse_image_header(header);
		spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS,
			       spl_image.size, (void *)spl_image.load_addr);
	}
}
