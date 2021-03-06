/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated, <www.ti.com>
 *
 * Lokesh Vutla <lokeshvutla@ti.com>
 *
 * Based on previous work by:
 * Aneesh V       <aneesh@ti.com>
 * Steve Sakoman  <steve@sakoman.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <palmas.h>
#include <sata.h>
#include <linux/string.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <usb.h>
#include <linux/usb/gadget.h>
#include <asm/arch/dra7xx_iodelay.h>
#include <asm/emif.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sata.h>
#include <environment.h>
#include <dwc3-uboot.h>
#include <dwc3-omap-uboot.h>
#include <ti-usb-phy-uboot.h>
#include <pcf8575.h>
#include <spl.h>
#include <remoteproc.h>
#include <libfdt.h>
#include <fdt_support.h>

#include "mux_data.h"
#include "../common/include/board_detect.h"

#define board_is_dra74x_evm()		board_ti_is("5777xCPU")
#define board_is_dra74x_revh_or_later() board_is_dra74x_evm() &&	\
				(strncmp("H", board_ti_get_rev(), 1) <= 0)
#define board_ti_get_emif_size()	board_ti_get_emif1_size() +	\
					board_ti_get_emif2_size()

#ifdef CONFIG_DRIVER_TI_CPSW
#include <cpsw.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/* GPIO 7_11 */
#define GPIO_DDR_VTT_EN 203

/* pcf chip address enet_mux_s0 */
#define PCF_ENET_MUX_ADDR	0x21
#define PCF_SEL_ENET_MUX_S0	4

#define SYSINFO_BOARD_NAME_MAX_LEN	37

const struct omap_sysinfo sysinfo = {
	"Board: UNKNOWN(DRA7 EVM) REV UNKNOWN\n"
};

const struct emif_regs emif1_ddr3_532_mhz_1cs = {
	.sdram_config_init              = 0x61851ab2,
	.sdram_config                   = 0x61851ab2,
	.sdram_config2			= 0x08000000,
	.ref_ctrl                       = 0x000040F1,
	.ref_ctrl_final			= 0x00001035,
	.sdram_tim1                     = 0xCCCF36B3,
	.sdram_tim2                     = 0x308F7FDA,
	.sdram_tim3                     = 0x027F88A8,
	.read_idle_ctrl                 = 0x00050000,
	.zq_config                      = 0x0007190B,
	.temp_alert_config              = 0x00000000,
	.emif_ddr_phy_ctlr_1_init       = 0x0E24400A,
	.emif_ddr_phy_ctlr_1            = 0x0E24400A,
	.emif_ddr_ext_phy_ctrl_1        = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2        = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3        = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4        = 0x009B009B,
	.emif_ddr_ext_phy_ctrl_5        = 0x009E009E,
	.emif_rd_wr_lvl_rmp_win         = 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl         = 0x00000000,
	.emif_rd_wr_lvl_ctl             = 0x00000000,
	.emif_rd_wr_exec_thresh         = 0x00000305
};

const struct emif_regs emif2_ddr3_532_mhz_1cs = {
	.sdram_config_init              = 0x61851B32,
	.sdram_config                   = 0x61851B32,
	.sdram_config2			= 0x08000000,
	.ref_ctrl                       = 0x000040F1,
	.ref_ctrl_final			= 0x00001035,
	.sdram_tim1                     = 0xCCCF36B3,
	.sdram_tim2                     = 0x308F7FDA,
	.sdram_tim3                     = 0x027F88A8,
	.read_idle_ctrl                 = 0x00050000,
	.zq_config                      = 0x0007190B,
	.temp_alert_config              = 0x00000000,
	.emif_ddr_phy_ctlr_1_init       = 0x0E24400A,
	.emif_ddr_phy_ctlr_1            = 0x0E24400A,
	.emif_ddr_ext_phy_ctrl_1        = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2        = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3        = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4        = 0x009B009B,
	.emif_ddr_ext_phy_ctrl_5        = 0x009E009E,
	.emif_rd_wr_lvl_rmp_win         = 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl         = 0x00000000,
	.emif_rd_wr_lvl_ctl             = 0x00000000,
	.emif_rd_wr_exec_thresh         = 0x00000305
};

const struct emif_regs emif_1_regs_ddr3_666_mhz_1cs_dra_es1 = {
	.sdram_config_init              = 0x61862B32,
	.sdram_config                   = 0x61862B32,
	.sdram_config2			= 0x08000000,
	.ref_ctrl                       = 0x0000493E,
	.ref_ctrl_final			= 0x0000144A,
	.sdram_tim1                     = 0xD113781C,
	.sdram_tim2                     = 0x308F7FE3,
	.sdram_tim3                     = 0x009F86A8,
	.read_idle_ctrl                 = 0x00050000,
	.zq_config                      = 0x0007190B,
	.temp_alert_config              = 0x00000000,
	.emif_ddr_phy_ctlr_1_init       = 0x0E24400D,
	.emif_ddr_phy_ctlr_1            = 0x0E24400D,
	.emif_ddr_ext_phy_ctrl_1        = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2        = 0x00A400A4,
	.emif_ddr_ext_phy_ctrl_3        = 0x00A900A9,
	.emif_ddr_ext_phy_ctrl_4        = 0x00B000B0,
	.emif_ddr_ext_phy_ctrl_5        = 0x00B000B0,
	.emif_rd_wr_lvl_rmp_win         = 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl         = 0x00000000,
	.emif_rd_wr_lvl_ctl             = 0x00000000,
	.emif_rd_wr_exec_thresh         = 0x00000305
};

const struct emif_regs emif1_ddr3_532_mhz_1cs_2G = {
	.sdram_config_init              = 0x61851ab2,
	.sdram_config                   = 0x61851ab2,
	.sdram_config2			= 0x08000000,
	.ref_ctrl                       = 0x000040F1,
	.ref_ctrl_final			= 0x00001035,
	.sdram_tim1                     = 0xCCCF36B3,
	.sdram_tim2                     = 0x30BF7FDA,
	.sdram_tim3                     = 0x427F8BA8,
	.read_idle_ctrl                 = 0x00050000,
	.zq_config                      = 0x0007190B,
	.temp_alert_config              = 0x00000000,
	.emif_ddr_phy_ctlr_1_init       = 0x0024400B,
	.emif_ddr_phy_ctlr_1            = 0x0E24400B,
	.emif_ddr_ext_phy_ctrl_1        = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2        = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3        = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4        = 0x009B009B,
	.emif_ddr_ext_phy_ctrl_5        = 0x009E009E,
	.emif_rd_wr_lvl_rmp_win         = 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl         = 0x00000000,
	.emif_rd_wr_lvl_ctl             = 0x00000000,
	.emif_rd_wr_exec_thresh         = 0x00000305
};

const struct emif_regs emif2_ddr3_532_mhz_1cs_2G = {
	.sdram_config_init              = 0x61851B32,
	.sdram_config                   = 0x61851B32,
	.sdram_config2			= 0x08000000,
	.ref_ctrl                       = 0x000040F1,
	.ref_ctrl_final			= 0x00001035,
	.sdram_tim1                     = 0xCCCF36B3,
	.sdram_tim2                     = 0x308F7FDA,
	.sdram_tim3                     = 0x427F88A8,
	.read_idle_ctrl                 = 0x00050000,
	.zq_config                      = 0x0007190B,
	.temp_alert_config              = 0x00000000,
	.emif_ddr_phy_ctlr_1_init       = 0x0024400B,
	.emif_ddr_phy_ctlr_1            = 0x0E24400B,
	.emif_ddr_ext_phy_ctrl_1        = 0x10040100,
	.emif_ddr_ext_phy_ctrl_2        = 0x00910091,
	.emif_ddr_ext_phy_ctrl_3        = 0x00950095,
	.emif_ddr_ext_phy_ctrl_4        = 0x009B009B,
	.emif_ddr_ext_phy_ctrl_5        = 0x009E009E,
	.emif_rd_wr_lvl_rmp_win         = 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl         = 0x00000000,
	.emif_rd_wr_lvl_ctl             = 0x00000000,
	.emif_rd_wr_exec_thresh         = 0x00000305
};

void emif_get_reg_dump(u32 emif_nr, const struct emif_regs **regs)
{
	u64 ram_size;

	ram_size = board_ti_get_emif_size();

	switch (omap_revision()) {
	case DRA752_ES1_0:
	case DRA752_ES1_1:
	case DRA752_ES2_0:
		switch (emif_nr) {
		case 1:
			if (ram_size > CONFIG_MAX_MEM_MAPPED)
				*regs = &emif1_ddr3_532_mhz_1cs_2G;
			else
				*regs = &emif1_ddr3_532_mhz_1cs;
			break;
		case 2:
			if (ram_size > CONFIG_MAX_MEM_MAPPED)
				*regs = &emif2_ddr3_532_mhz_1cs_2G;
			else
				*regs = &emif2_ddr3_532_mhz_1cs;
			break;
		}
		break;
	case DRA722_ES1_0:
		*regs = &emif_1_regs_ddr3_666_mhz_1cs_dra_es1;
		break;
	default:
		*regs = &emif1_ddr3_532_mhz_1cs;
	}
}

static const struct dmm_lisa_map_regs lisa_map_dra7_1536MB = {
	.dmm_lisa_map_0 = 0x0,
	.dmm_lisa_map_1 = 0x80640300,
	.dmm_lisa_map_2 = 0xC0500220,
	.dmm_lisa_map_3 = 0xFF020100,
	.is_ma_present	= 0x1
};

static const struct dmm_lisa_map_regs lisa_map_2G_x_2 = {
	.dmm_lisa_map_0 = 0x0,
	.dmm_lisa_map_1 = 0x0,
	.dmm_lisa_map_2 = 0x80600100,
	.dmm_lisa_map_3 = 0xFF020100,
	.is_ma_present	= 0x1
};

const struct dmm_lisa_map_regs lisa_map_dra7_2GB = {
	.dmm_lisa_map_0 = 0x0,
	.dmm_lisa_map_1 = 0x0,
	.dmm_lisa_map_2 = 0x80740300,
	.dmm_lisa_map_3 = 0xFF020100,
	.is_ma_present	= 0x1
};

void emif_get_dmm_regs(const struct dmm_lisa_map_regs **dmm_lisa_regs)
{
	u64 ram_size;

	ram_size = board_ti_get_emif_size();

	switch (omap_revision()) {
	case DRA752_ES1_0:
	case DRA752_ES1_1:
	case DRA752_ES2_0:
		if (ram_size > CONFIG_MAX_MEM_MAPPED)
			*dmm_lisa_regs = &lisa_map_dra7_2GB;
		else
			*dmm_lisa_regs = &lisa_map_dra7_1536MB;
		break;
	case DRA722_ES1_0:
	default:
		*dmm_lisa_regs = &lisa_map_2G_x_2;
	}
}

static int fdt_addprop_aliases(void *fdt, char *node, char *propstr,
	int *val, int len)
{
	const char *path;
	int offs;

	/* get path of mmc node */
	path = fdt_getpath_prop(fdt, "/aliases", node, 0);
	if (!path) {
		debug("%s not found in aliases\n", node);
		return -1;
	}

	offs = fdt_path_offset(fdt, path);
	if (offs < 0) {
		debug("error get offset for %s\n", path);
		return -1;
	}

	return fdt_appendprop(fdt, offs, propstr, val, len);
}

static int board_fixup_fdt_mmc(void *fdt, int mmc_num, int pinctl)
{
	const struct fdt_property *mmc_prop, *prop;
	const char *path;
	char propstr[24];
	u32 *src, *dest;
	int length, si_rev = omap_revision(), index;

	sprintf(propstr, "mmc%d", mmc_num);
	/* get path of mmc node */
	path = fdt_getpath_prop(fdt, "/aliases", propstr, &length);
	if (!path) {
		debug("mmc%d node not found in DT aliases\n", mmc_num);
		goto err;
	}

	sprintf(propstr, "mmc%d_pinctl%d_iodelay", mmc_num, pinctl);
	/* get property of mmc1 iodelay node */
	prop = fdt_getprop_path(fdt, "/chosen", propstr, &length);
	if (!prop) {
		debug("%s node not found in DT\n", propstr);
		goto err;
	}

	if (length != 8) {
		debug("no mmc-iodelay update is needed\n");
		goto err;
	}

	sprintf(propstr, "pinctrl-%d", pinctl);
	mmc_prop = fdt_getprop_path(fdt, (char *)path, propstr, &length);
	if (!mmc_prop) {
		debug("prop for pinctrl-4 not found\n");
		goto err;
	}

	if (length != 8) {
		debug("no mmc-iodelay update is needed\n");
		goto err;
	}

	/* check silicon-revision */
	switch (si_rev) {
	case DRA752_ES1_0:
	case DRA752_ES1_1:
		index = 0;
		break;
	case DRA752_ES2_0:
		index = 1;
		break;
	default:
		printf("invalid si-revision\n");
		goto err;
	}

	/* udpate the iodelay structure based on si-revision*/
	src = (u32 *)&prop->data[index * 4];
	dest = (u32 *)&mmc_prop->data[4];
	*dest = *src;
	do_fixup_by_path(fdt, path, propstr, &mmc_prop->data[0], length, 0);

	debug("iodelay update for mmc%d-pinctl%d-done\n", mmc_num, pinctl);

	return 0;
err:
	debug("iodelay update for mmc%d-pinctl%d-failed\n", mmc_num, pinctl);
	return -1;
}

int board_fixup_fdt(void *fdt)
{
	int mmc_num, pinctrl, max_freq;

	/* dt-update for mmc pinctl iodelays based on si-revsion */
	for (mmc_num = 1; mmc_num <= 4; ++mmc_num) {
		for (pinctrl = 0; pinctrl <= 4; ++pinctrl)
			board_fixup_fdt_mmc(fdt, mmc_num, pinctrl);
	}

	switch (omap_revision()) {
	case DRA752_ES1_0:
	case DRA752_ES1_1:
		break;

	case DRA752_ES2_0:
		max_freq = cpu_to_fdt32(192000000);
		/* dt-update: add mmc1 property*/
		fdt_addprop_aliases(fdt, "mmc1", "sd-uhs-sdr104", NULL, 0);
		fdt_addprop_aliases(fdt, "mmc1", "max-frequency", &max_freq, 4);

		/* dt-update: add mmc2 property*/
		fdt_addprop_aliases(fdt, "mmc2", "mmc-hs200-1_8v", NULL, 0);
		fdt_addprop_aliases(fdt, "mmc2", "max-frequency", &max_freq, 4);
		break;
	default:
		break;
	}

#if defined(CONFIG_OMAP_SECURE)
	/* dt-update for secure parts */
	hs_device_fixup_fdt(fdt);
#endif

	return 0;
}

/**
 * @brief board_init
 *
 * @return 0
 */
int board_init(void)
{
#if defined(CONFIG_SPL_ENV_SUPPORT) && defined(CONFIG_SPL_SPI_PROD_OS_BOOT)
	char serialno[72];
	uint32_t serialno_lo, serialno_hi;

#ifdef CONFIG_ENV_IS_IN_MMC
	struct mmc *mmc;
	spl_mmc_init(&mmc);
#endif

	env_init();
	env_relocate_spec();

	if (!getenv("serial#")) {
		printf("serial# not set, setting...\n");
		serialno_lo = readl((*ctrl)->control_std_fuse_die_id_3);
		serialno_hi = readl((*ctrl)->control_std_fuse_prod_id_0);
		sprintf(serialno, "%08x%08x", serialno_hi, serialno_lo);
		setenv("serial#", serialno);
	}
#endif

	gpmc_init();
	gd->bd->bi_boot_params = (0x80000000 + 0x100); /* boot param addr */

	return 0;
}

#ifdef CONFIG_CMD_FASTBOOT
extern int load_ptbl(void);
#endif

void dram_init_banksize(void)
{
	u64 ram_size;

	ram_size = board_ti_get_emif_size();

	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = get_effective_memsize();
	if (ram_size > CONFIG_MAX_MEM_MAPPED) {
		gd->bd->bi_dram[1].start = 0x200000000;
		gd->bd->bi_dram[1].size = ram_size - CONFIG_MAX_MEM_MAPPED;
	}
}

int board_late_init(void)
{
#ifdef CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
	char serialno[72];
	uint32_t serialno_lo, serialno_hi;
	char *name = "unknown";

	if (omap_revision() == DRA722_ES1_0)
		name = "dra72x";
	else
		name = "dra7xx";

	set_board_info_env(name);

	if (!getenv("serial#")) {
		printf("serial# not set, setting...\n");
		serialno_lo = readl((*ctrl)->control_std_fuse_die_id_3);
		serialno_hi = readl((*ctrl)->control_std_fuse_prod_id_0);
		sprintf(serialno, "%08x%08x", serialno_hi, serialno_lo);
		setenv("serial#", serialno);
	}
#endif
	init_sata(0);
#ifdef CONFIG_CMD_FASTBOOT
	load_ptbl();
#endif
	return 0;
}

#ifdef CONFIG_SPL_BUILD
void do_board_detect(void)
{
	int rc;

	rc = ti_i2c_eeprom_dra7_get(CONFIG_EEPROM_BUS_ADDRESS,
				    CONFIG_EEPROM_CHIP_ADDRESS);
	if (rc)
		printf("ti_i2c_eeprom_init failed %d\n", rc);
}

#else

void do_board_detect(void)
{
	char *bname = NULL;
	int rc;

	rc = ti_i2c_eeprom_dra7_get(CONFIG_EEPROM_BUS_ADDRESS,
				    CONFIG_EEPROM_CHIP_ADDRESS);
	if (rc)
		printf("ti_i2c_eeprom_init failed %d\n", rc);

	if (board_is_dra74x_evm()) {
		bname = "DRA74x EVM";
	/* If EEPROM is not populated */
	} else {
		if (is_dra72x())
			bname = "DRA72x EVM";
		else
			bname = "DRA74x EVM";
	}

	if (bname)
		snprintf(sysinfo.board_string, SYSINFO_BOARD_NAME_MAX_LEN,
			 "Board: %s REV %s\n", bname, board_ti_get_rev());
}
#endif	/* CONFIG_SPL_BUILD */

void set_muxconf_regs_essential(void)
{
	do_set_mux32((*ctrl)->control_padconf_core_base,
		     early_padconf, ARRAY_SIZE(early_padconf));
}

#ifdef CONFIG_IODELAY_RECALIBRATION
void recalibrate_iodelay(void)
{
	struct pad_conf_entry const *pads;
	struct iodelay_cfg_entry const *iodelay;
	int npads, niodelays;

	switch (omap_revision()) {
	case DRA722_ES1_0:
		pads = dra72x_core_padconf_array;
		npads = ARRAY_SIZE(dra72x_core_padconf_array);
		iodelay = dra722_sr1_0_iodelay_cfg_array;
		niodelays = ARRAY_SIZE(dra722_sr1_0_iodelay_cfg_array);
		break;
	case DRA752_ES1_0:
	case DRA752_ES1_1:
		pads = dra74x_core_padconf_array;
		npads = ARRAY_SIZE(dra74x_core_padconf_array);
		iodelay = dra742_es1_1_iodelay_cfg_array;
		niodelays = ARRAY_SIZE(dra742_es1_1_iodelay_cfg_array);
		break;
	default:
	case DRA752_ES2_0:
		pads = dra74x_core_padconf_array;
		npads = ARRAY_SIZE(dra74x_core_padconf_array);
		iodelay = dra742_es2_0_iodelay_cfg_array;
		niodelays = ARRAY_SIZE(dra742_es2_0_iodelay_cfg_array);
		/* Setup port1 and port2 for rgmii with 'no-id' mode */
		clrset_spare_register(1, 0, RGMII2_ID_MODE_N_MASK |
				      RGMII1_ID_MODE_N_MASK);
		break;
	}
	__recalibrate_iodelay(pads, npads, iodelay, niodelays);
}
#endif

#if !defined(CONFIG_SPL_BUILD) && defined(CONFIG_GENERIC_MMC)
int board_mmc_init(bd_t *bis)
{
	omap_mmc_init(0, 0, 0, -1, -1);
	omap_mmc_init(1, 0, 0, -1, -1);
	return 0;
}
#endif

#ifdef CONFIG_USB_DWC3
static struct dwc3_device usb_otg_ss1 = {
	.maximum_speed = USB_SPEED_SUPER,
	.base = DRA7_USB_OTG_SS1_BASE,
	.needs_fifo_resize = false,
	.index = 0,
};

static struct dwc3_omap_device usb_otg_ss1_glue = {
	.base = (void *)DRA7_USB_OTG_SS1_GLUE_BASE,
	.utmi_mode = DWC3_OMAP_UTMI_MODE_SW,
	.index = 0,
};

static struct ti_usb_phy_device usb_phy1_device = {
	.pll_ctrl_base = (void *)DRA7_USB3_PHY1_PLL_CTRL,
	.usb2_phy_power = (void *)DRA7_USB2_PHY1_POWER,
	.usb3_phy_power = (void *)DRA7_USB3_PHY1_POWER,
	.index = 0,
};

static struct dwc3_device usb_otg_ss2 = {
	.maximum_speed = USB_SPEED_SUPER,
	.base = DRA7_USB_OTG_SS2_BASE,
	.needs_fifo_resize = false,
	.index = 1,
};

static struct dwc3_omap_device usb_otg_ss2_glue = {
	.base = (void *)DRA7_USB_OTG_SS2_GLUE_BASE,
	.utmi_mode = DWC3_OMAP_UTMI_MODE_SW,
	.index = 1,
};

static struct ti_usb_phy_device usb_phy2_device = {
	.usb2_phy_power = (void *)DRA7_USB2_PHY2_POWER,
	.index = 1,
};

int board_usb_init(int index, enum usb_init_type init)
{
	enable_usb_clocks(index);
	switch (index) {
	case 0:
		if (init == USB_INIT_DEVICE) {
			usb_otg_ss1.dr_mode = USB_DR_MODE_PERIPHERAL;
			usb_otg_ss1_glue.vbus_id_status = OMAP_DWC3_VBUS_VALID;
		} else {
			usb_otg_ss1.dr_mode = USB_DR_MODE_HOST;
			usb_otg_ss1_glue.vbus_id_status = OMAP_DWC3_ID_GROUND;
		}

		ti_usb_phy_uboot_init(&usb_phy1_device);
		dwc3_omap_uboot_init(&usb_otg_ss1_glue);
		dwc3_uboot_init(&usb_otg_ss1);
		break;
	case 1:
		if (init == USB_INIT_DEVICE) {
			usb_otg_ss2.dr_mode = USB_DR_MODE_PERIPHERAL;
			usb_otg_ss2_glue.vbus_id_status = OMAP_DWC3_VBUS_VALID;
		} else {
			usb_otg_ss2.dr_mode = USB_DR_MODE_HOST;
			usb_otg_ss2_glue.vbus_id_status = OMAP_DWC3_ID_GROUND;
		}

		ti_usb_phy_uboot_init(&usb_phy2_device);
		dwc3_omap_uboot_init(&usb_otg_ss2_glue);
		dwc3_uboot_init(&usb_otg_ss2);
		break;
	default:
		printf("Invalid Controller Index\n");
	}

	return 0;
}

int board_usb_cleanup(int index, enum usb_init_type init)
{
	switch (index) {
	case 0:
	case 1:
		ti_usb_phy_uboot_exit(index);
		dwc3_uboot_exit(index);
		dwc3_omap_uboot_exit(index);
		break;
	default:
		printf("Invalid Controller Index\n");
	}
	disable_usb_clocks(index);
	return 0;
}

int usb_gadget_handle_interrupts(int index)
{
	u32 status;

	status = dwc3_omap_uboot_interrupt_status(index);
	if (status)
		dwc3_uboot_handle_interrupt(index);

	return 0;
}
#endif

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_OS_BOOT)
int spl_start_uboot(void)
{
	/* break into full u-boot on 'c' */
	if (serial_tstc() && serial_getc() == 'c')
		return 1;

#ifdef CONFIG_SPL_SPI_PROD_OS_BOOT
	/* boot the os if boot mode is qspi prod */
	if (spl_boot_mode() == SPI_MODE_PROD)
		return 0;
#else
	if ((get_sysboot_value() & SYSBOOT_TYPE_MASK) == SYSBOOT_TYPE_PROD)
		return 0;
#endif

#ifdef CONFIG_SPL_ENV_SUPPORT
#ifdef CONFIG_ENV_IS_IN_MMC
	struct mmc *mmc;
	spl_mmc_init(&mmc);
#endif

	env_init();
	env_relocate_spec();

	/*
	 * Function call returns 1 only if the environment variable is
	 * set to 1,y,Y,t or T. It returns -1 or 0 otherwise.
	 */
	if (getenv_yesno("boot_os") == 1)
		return 0;
#endif

	return 1;
}
#endif

/*
 * read mac address of the ethernet macs from efuse into
 * the input array. The input array needs to be 12 bytes
 * long to accommodate both mac addresses. No checks are
 * done on the pointer validity.
 */
void read_mac_addr_from_efuse(uint8_t *mac_addr_arr)
{
	uint8_t *mac_addr;
	uint32_t mac_hi, mac_lo;

	/* try reading mac address from efuse */
	mac_addr = &mac_addr_arr[0];
	mac_lo = readl((*ctrl)->control_core_mac_id_0_lo);
	mac_hi = readl((*ctrl)->control_core_mac_id_0_hi);
	mac_addr[0] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = mac_hi & 0xFF;
	mac_addr[3] = (mac_lo & 0xFF0000) >> 16;
	mac_addr[4] = (mac_lo & 0xFF00) >> 8;
	mac_addr[5] = mac_lo & 0xFF;

	mac_addr = &mac_addr_arr[6];
	mac_lo = readl((*ctrl)->control_core_mac_id_1_lo);
	mac_hi = readl((*ctrl)->control_core_mac_id_1_hi);
	mac_addr[0] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = mac_hi & 0xFF;
	mac_addr[3] = (mac_lo & 0xFF0000) >> 16;
	mac_addr[4] = (mac_lo & 0xFF00) >> 8;
	mac_addr[5] = mac_lo & 0xFF;

	return;
}

#ifdef CONFIG_DRIVER_TI_CPSW

/* Delay value to add to calibrated value */
#define RGMII0_TXCTL_DLY_VAL		((0x3 << 5) + 0x8)
#define RGMII0_TXD0_DLY_VAL		((0x3 << 5) + 0x8)
#define RGMII0_TXD1_DLY_VAL		((0x3 << 5) + 0x2)
#define RGMII0_TXD2_DLY_VAL		((0x4 << 5) + 0x0)
#define RGMII0_TXD3_DLY_VAL		((0x4 << 5) + 0x0)
#define VIN2A_D13_DLY_VAL		((0x3 << 5) + 0x8)
#define VIN2A_D17_DLY_VAL		((0x3 << 5) + 0x8)
#define VIN2A_D16_DLY_VAL		((0x3 << 5) + 0x2)
#define VIN2A_D15_DLY_VAL		((0x4 << 5) + 0x0)
#define VIN2A_D14_DLY_VAL		((0x4 << 5) + 0x0)

extern u32 *const omap_si_rev;

static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= 2,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_addr	= 3,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 2,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};

int board_eth_init(bd_t *bis)
{
	int ret;
	uint8_t *mac_addr;
	uint8_t mac_addr_arr[12];
	uint32_t ctrl_val;

	read_mac_addr_from_efuse(mac_addr_arr);

	mac_addr = &mac_addr_arr[0];
	if (!getenv("ethaddr")) {
		printf("<ethaddr> not set. Validating first E-fuse MAC\n");

		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
	}


	mac_addr = &mac_addr_arr[6];
	if (!getenv("eth1addr")) {
		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("eth1addr", mac_addr);
	}

	ctrl_val = readl((*ctrl)->control_core_control_io1) & (~0x33);
	ctrl_val |= 0x22;
	writel(ctrl_val, (*ctrl)->control_core_control_io1);

	if (*omap_si_rev == DRA722_ES1_0) {
		cpsw_data.active_slave = 0;
		cpsw_data.slave_data[0].phy_addr = 3;
		pcf8575_output(PCF_ENET_MUX_ADDR, PCF_SEL_ENET_MUX_S0,
			       PCF8575_OUT_LOW);
	}

	ret = cpsw_register(&cpsw_data);
	if (ret < 0)
		printf("Error %d registering CPSW switch\n", ret);

	return ret;
}
#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
/* VTT regulator enable */
static inline void vtt_regulator_enable(void)
{
	if (omap_hw_init_context() == OMAP_INIT_CONTEXT_UBOOT_AFTER_SPL)
		return;

	/* Do not enable VTT for DRA722 */
	if (omap_revision() == DRA722_ES1_0)
		return;

	/*
	 * EVM Rev G and later use gpio7_11 for DDR3 termination.
	 * This is safe enough to do on older revs.
	 */
	gpio_request(GPIO_DDR_VTT_EN, "ddr_vtt_en");
	gpio_direction_output(GPIO_DDR_VTT_EN, 1);
}

int board_early_init_f(void)
{
	vtt_regulator_enable();
	return 0;
}
#endif
