#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/packages;/home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/tidrivers_cc13xx_cc26xx_2_20_00_08/packages;/home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/bios_6_46_00_23/packages;/home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/uia_2_00_06_52/packages;/home/tsei/ti/ccsv6/ccs_base
override XDCROOT = /home/tsei/ti/xdctools_3_32_00_06_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/packages;/home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/tidrivers_cc13xx_cc26xx_2_20_00_08/packages;/home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/bios_6_46_00_23/packages;/home/tsei/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/uia_2_00_06_52/packages;/home/tsei/ti/ccsv6/ccs_base;/home/tsei/ti/xdctools_3_32_00_06_core/packages;..
HOSTOS = Linux
endif
