
#ifndef __UPM6922_HEADER__
#define __UPM6922_HEADER__

#define UPM6922_WORK_DELAY_TIME (10) /*ms*/

/* Register 00h */
#define UPM6922_REG_00             0x00
#define REG00_ENHIZ_MASK            0x80
#define REG00_ENHIZ_SHIFT           7
#define REG00_HIZ_ENABLE            1
#define REG00_HIZ_DISABLE           0

#define REG00_STAT_CTRL_MASK        0x60
#define REG00_STAT_CTRL_SHIFT       5
#define REG00_STAT_CTRL_ENABLE      0
#define REG00_STAT_CTRL_DISABLE     3

#define REG00_IINLIM_MASK           0x1F
#define REG00_IINLIM_SHIFT          0
#define REG00_IINLIM_LSB            100
#define REG00_IINLIM_BASE           100
#define REG00_IINLIM_MIN            100
#define REG00_IINLIM_MAX            3200

/* Register 01h */
#define UPM6922_REG_01             0x01
#define REG01_PFM_DIS_MASK          0x80
#define REG01_PFM_DIS_SHIFT         7
#define REG01_PFM_ENABLE            0
#define REG01_PFM_DISABLE           1

#define REG01_WDT_RESET_MASK        0x40
#define REG01_WDT_RESET_SHIFT       6
#define REG01_WDT_RESET             1

#define REG01_OTG_CONFIG_MASK       0x20
#define REG01_OTG_CONFIG_SHIFT      5
#define REG01_OTG_ENABLE            1
#define REG01_OTG_DISABLE           0

#define REG01_CHG_CONFIG_MASK       0x10
#define REG01_CHG_CONFIG_SHIFT      4
#define REG01_CHG_DISABLE           0
#define REG01_CHG_ENABLE            1

#define REG01_SYS_MINV_MASK         0x0E
#define REG01_SYS_MINV_SHIFT        1
#define REG01_SYS_MINV_2P6V         0
#define REG01_SYS_MINV_2P8V         1
#define REG01_SYS_MINV_3V           2
#define REG01_SYS_MINV_3P2V         3
#define REG01_SYS_MINV_3P4V         4
#define REG01_SYS_MINV_3P5V         5
#define REG01_SYS_MINV_3P6V         6
#define REG01_SYS_MINV_3P7V         7

#define REG01_MIN_VBAT_SEL_MASK     0x01
#define REG01_MIN_VBAT_SEL_SHIFT    0
#define REG01_MIN_VBAT_3V           0
#define REG01_MIN_VBAT_2P5V         1

/* Register 0x02*/
#define UPM6922_REG_02             0x02
#define REG02_BOOST_LIM0_MASK       0x80
#define REG02_BOOST_LIM0_SHIFT      7
#define REG02_BOOST_LIM0_0P5A       0
#define REG02_BOOST_LIM0_1P2A       1

#define REG02_Q1_FULLON_MASK        0x40
#define REG02_Q1_FULLON_SHIFT       6
#define REG02_Q1_FULLON_ENABLE      1
#define REG02_Q1_FULLON_DISABLE     0

#define REG02_ICHG_MASK             0x3F
#define REG02_ICHG_SHIFT            0
#define REG02_ICHG_BASE             0
#define REG02_ICHG_LSB              60
#define REG02_ICHG_MIN              0
#define REG02_ICHG_MAX              3780

/* Register 0x03*/
#define UPM6922_REG_03             0x03
#define REG03_IPRECHG_MASK          0xF0
#define REG03_IPRECHG_SHIFT         4
#define REG03_IPRECHG_BASE          60
#define REG03_IPRECHG_LSB           60
#define REG03_IPRECHG_MIN           60
#define REG03_IPRECHG_MAX           960

#define REG03_ITERM_MASK            0x0F
#define REG03_ITERM_SHIFT           0
#define REG03_ITERM_BASE            60
#define REG03_ITERM_LSB             60
#define REG03_ITERM_MIN             60
#define REG03_ITERM_MAX             960

/* Register 0x04*/
#define UPM6922_REG_04             0x04
#define REG04_VREG_MASK             0xF8
#define REG04_VREG_SHIFT            3
#define REG04_VREG_BASE             3848
#define REG04_VREG_LSB              32
#define REG04_VREG_MIN              3848
#define REG04_VREG_MAX              4808

#define REG04_TOPOFF_TIMER_MASK         0x06
#define REG04_TOPOFF_TIMER_SHIFT        1
#define REG04_TOPOFF_TIMER_DISABLE      0
#define REG04_TOPOFF_TIMER_15M          1
#define REG04_TOPOFF_TIMER_30M          2
#define REG04_TOPOFF_TIMER_45M          3

#define REG04_VRECHG_MASK           0x01
#define REG04_VRECHG_SHIFT          0
#define REG04_VRECHG_100MV          0
#define REG04_VRECHG_200MV          1

/* Register 0x05 */
#define UPM6922_REG_05             0x05
#define REG05_EN_TERM_MASK          0x80
#define REG05_EN_TERM_SHIFT         7
#define REG05_TERM_ENABLE           1
#define REG05_TERM_DISABLE          0

#define REG05_WDT_MASK              0x30
#define REG05_WDT_SHIFT             4
#define REG05_WDT_DISABLE           0
#define REG05_WDT_40S               1
#define REG05_WDT_80S               2
#define REG05_WDT_160S              3

#define REG05_EN_TIMER_MASK         0x08
#define REG05_EN_TIMER_SHIFT        3
#define REG05_CHG_TIMER_ENABLE      1
#define REG05_CHG_TIMER_DISABLE     0

#define REG05_CHG_TIMER_MASK        0x04
#define REG05_CHG_TIMER_SHIFT       2
#define REG05_CHG_TIMER_5HOURS      0
#define REG05_CHG_TIMER_10HOURS     1

#define REG05_TREG_MASK             0x02
#define REG05_TREG_SHIFT            1
#define REG05_TREG_90C              0
#define REG05_TREG_110C             1

#define REG05_JEITA_ISET0_MASK       0x01
#define REG05_JEITA_ISET0_SHIFT      0
#define REG05_JEITA_ISET0_50PCT      0
#define REG05_JEITA_ISET0_20PCT      1

/* Register 0x06*/
#define UPM6922_REG_06             0x06
#define REG06_OVP_MASK              0xC0
#define REG06_OVP_SHIFT             6
#define REG06_OVP_5P85V             0
#define REG06_OVP_6P5V              1
#define REG06_OVP_11V               2
#define REG06_OVP_14V               3

#define REG06_BOOSTV_MASK           0x30
#define REG06_BOOSTV_SHIFT          4
#define REG06_BOOSTV_4P85V          0
#define REG06_BOOSTV_5V             1
#define REG06_BOOSTV_5P15V          2
#define REG06_BOOSTV_5P3V           3

#define REG06_VINDPM_MASK           0x0F
#define REG06_VINDPM_SHIFT          0
#define REG06_VINDPM_BASE           3900
#define REG06_VINDPM_LSB            100

/* Register 0x07*/
#define UPM6922_REG_07             0x07
#define REG07_FORCE_DPDM_MASK       0x80
#define REG07_FORCE_DPDM_SHIFT      7
#define REG07_FORCE_DPDM            1

#define REG07_TMR2X_EN_MASK         0x40
#define REG07_TMR2X_EN_SHIFT        6
#define REG07_TMR2X_ENABLE          1
#define REG07_TMR2X_DISABLE         0

#define REG07_BATFET_DIS_MASK       0x20
#define REG07_BATFET_DIS_SHIFT      5
#define REG07_BATFET_OFF            1
#define REG07_BATFET_ON             0

#define REG07_JEITA_VSET_MASK       0x10
#define REG07_JEITA_VSET_SHIFT      4
#define REG07_JEITA_VSET_4100       0
#define REG07_JEITA_VSET_VREG       1

#define REG07_BATFET_DLY_MASK       0x08
#define REG07_BATFET_DLY_SHIFT      3
#define REG07_BATFET_DLY_0S         0
#define REG07_BATFET_DLY_10S        1

#define REG07_BATFET_RST_EN_MASK    0x04
#define REG07_BATFET_RST_EN_SHIFT   2
#define REG07_BATFET_RST_DISABLE    0
#define REG07_BATFET_RST_ENABLE     1

#define REG07_VDPM_BAT_TRACK_MASK       0x03
#define REG07_VDPM_BAT_TRACK_SHIFT      0
#define REG07_VDPM_BAT_TRACK_DISABLE    0
#define REG07_VDPM_BAT_TRACK_200MV      1
#define REG07_VDPM_BAT_TRACK_250MV      2
#define REG07_VDPM_BAT_TRACK_300MV      3

/* Register 0x08*/
#define UPM6922_REG_08             0x08
#define REG08_VBUS_STAT_MASK        0xE0
#define REG08_VBUS_STAT_SHIFT       5
#define REG08_VBUS_TYPE_NONE        0
#define REG08_VBUS_TYPE_SDP         1
#define REG08_VBUS_TYPE_CDP         2
#define REG08_VBUS_TYPE_DCP         3
#define REG08_VBUS_TYPE_UNKNOWN     5
#define REG08_VBUS_TYPE_NON_STD     6
#define REG08_VBUS_TYPE_OTG         7

#define REG08_CHRG_STAT_MASK        0x18
#define REG08_CHRG_STAT_SHIFT       3
#define REG08_CHRG_STAT_IDLE        0
#define REG08_CHRG_STAT_PRECHG      1
#define REG08_CHRG_STAT_FASTCHG     2
#define REG08_CHRG_STAT_CHGDONE     3

#define REG08_PG_STAT_MASK          0x04
#define REG08_PG_STAT_SHIFT         2
#define REG08_POWER_GOOD            1

#define REG08_THERM_STAT_MASK       0x02
#define REG08_THERM_STAT_SHIFT      1
#define REG08_NOT_IN_THERM_STAT     0

#define REG08_VSYS_STAT_MASK        0x01
#define REG08_VSYS_STAT_SHIFT       0
#define REG08_IN_VSYS_STAT          1

/* Register 0x09*/
#define UPM6922_REG_09             0x09
#define REG09_FAULT_WDT_MASK        0x80
#define REG09_FAULT_WDT_SHIFT       7
#define REG09_FAULT_WDT             1

#define REG09_FAULT_BOOST_MASK      0x40
#define REG09_FAULT_BOOST_SHIFT     6
#define REG09_FAULT_BOOST           1

#define REG09_FAULT_CHRG_MASK       0x30
#define REG09_FAULT_CHRG_SHIFT      4
#define REG09_FAULT_CHRG_NORMAL     0
#define REG09_FAULT_CHRG_INPUT      1
#define REG09_FAULT_CHRG_THERMAL    2
#define REG09_FAULT_CHRG_TIMER      3

#define REG09_FAULT_BAT_MASK        0x08
#define REG09_FAULT_BAT_SHIFT       3
#define REG09_FAULT_BAT_OVP         1

#define REG09_FAULT_NTC_MASK        0x07
#define REG09_FAULT_NTC_SHIFT       0
#define REG09_FAULT_NTC_NORMAL      0
#define REG09_FAULT_NTC_WARM        2
#define REG09_FAULT_NTC_COOL        3
#define REG09_FAULT_NTC_COLD        5
#define REG09_FAULT_NTC_HOT         6

/* Register 0x0A */
#define UPM6922_REG_0A             0x0A
#define REG0A_VBUS_GD_MASK          0x80
#define REG0A_VBUS_GD_SHIFT         7
#define REG0A_VBUS_GD               1

#define REG0A_VINDPM_STAT_MASK      0x40
#define REG0A_VINDPM_STAT_SHIFT     6
#define REG0A_VINDPM_ACTIVE         1

#define REG0A_IINDPM_STAT_MASK      0x20
#define REG0A_IINDPM_STAT_SHIFT     5
#define REG0A_IINDPM_ACTIVE         1

#define REG0A_CV_STAT_MASK          0x10
#define REG0A_CV_STAT_SHIFT         4
#define REG0A_IN_CV_LOOP            1

#define REG0A_TOPOFF_ACTIVE_MASK    0x08
#define REG0A_TOPOFF_ACTIVE_SHIFT   3
#define REG0A_TOPOFF_ACTIVE         1

#define REG0A_ACOV_STAT_MASK        0x04
#define REG0A_ACOV_STAT_SHIFT       2
#define REG0A_ACOV_ACTIVE           1

#define REG0A_VINDPM_INT_MASK       0x02
#define REG0A_VINDPM_INT_SHIFT      1
#define REG0A_VINDPM_INT_ENABLE     0
#define REG0A_VINDPM_INT_DISABLE    1

#define REG0A_IINDPM_INT_MASK       0x01
#define REG0A_IINDPM_INT_SHIFT      0
#define REG0A_IINDPM_INT_ENABLE     0
#define REG0A_IINDPM_INT_DISABLE    1

/* Register 0x0B */
#define UPM6922_REG_0B             0x0B
#define REG0B_REG_RESET_MASK        0x80
#define REG0B_REG_RESET_SHIFT       7
#define REG0B_REG_RESET             1

#define REG0B_PN_MASK               0x78
#define REG0B_PN_SHIFT              3
#define REG0B_PN_UPM6922           2

#define REG0B_DEV_REV_MASK          0x03
#define REG0B_DEV_REV_SHIFT         0

/* Register 0x0C */
#define UPM6922_REG_0C             0x0C
#define REG0C_DPDM_LOCK_MASK        0x80
#define REG0C_DPDM_LOCK_SHIFT       7
#define REG0C_DPDM_MUX_UNLOCK       1
#define REG0C_DPDM_MUX_LOCK         0

#define REG0C_DP_MUX_MASK           0x18
#define REG0C_DP_MUX_SHIFT          3

#define REG0C_DM_MUX_MASK           0x06
#define REG0C_DM_MUX_SHIFT          1

#define REG0C_DPDM_OUT_HIZ          0
#define REG0C_DPDM_OUT_0P           1
#define REG0C_DPDM_OUT_0V6V         2
#define REG0C_DPDM_OUT_3P3V         3

#define REG0C_AUTO_BC12_MASK        0x40
#define REG0C_AUTO_BC12_SHIFT       6
#define REG0C_AUTO_BC12_ENABLE      1
#define REG0C_AUTO_BC12_DISABLE     0

#define REG0C_JEITA_EN_MASK         0x40
#define REG0C_JEITA_EN_SHIFT        6
#define REG0C_JEITA_ENABLE          1
#define REG0C_JEITA_DISABLE         0

/* Register 0x0D */
#define UPM6922_REG_0D             0x0D
#define REG0D_VREG_FT_MASK          0xC0
#define REG0D_VREG_FT_SHIFT         6
#define REG0D_VREG_FT_LSB           8

#define REG0D_V_RRE2FAST_MASK       0x0C
#define REG0D_V_RRE2FAST_SHIFT      2
#define REG0D_PRE2FAST_3V           0
#define REG0D_PRE2FAST_2P9V         1
#define REG0D_PRE2FAST_2P8V         2
#define REG0D_PRE2FAST_2P7V         3

#define REG0D_VINDPM_OS_MASK        0x03
#define REG0D_VINDPM_OS_SHIFT       0
#define REG0D_VINDPM_OS_3P9V        0
#define REG0D_VINDPM_OS_5P9V        1
#define REG0D_VINDPM_OS_7P5V        2
#define REG0D_VINDPM_OS_10P5V       3

/* Register 0x0E */
#define UPM6922_REG_0E             0x0E
#define REG0E_VBAT_MASK             0x7F
#define REG0E_VBAT_SHIFT            0
#define REG0E_VBAT_LSB              20
#define REG0E_VBAT_BASE             2304

/* Register 0x10 */
#define UPM6922_REG_10             0x10
#define REG10_VSYS_MASK             0x7F
#define REG10_VSYS_SHIFT            0
#define REG10_VSYS_LSB              20
#define REG10_VSYS_BASE             2304

/* Register 0x12 */
#define UPM6922_REG_12             0x12
#define REG12_VBUS_MASK             0x7F
#define REG12_VBUS_SHIFT            0
#define REG12_VBUS_LSB              100
#define REG12_VBUS_BASE             2600

/* Register 0x15 */
#define UPM6922_REG_15             0x15
#define REG15_BC12_DONE_MASK        0x80
#define REG15_BC12_DONE_SHIFT       7
#define REG15_BC12_DONE             1

#define REG15_BOOST_LIM1_MASK        0x40
#define REG15_BOOST_LIM1_SHIFT       6
#define REG15_BOOST_LIM1_DIS         0
#define REG15_BOOST_LIM1_2A          1

#define REG15_JEITA_COOL_ISET1_MASK       0x20
#define REG15_JEITA_COOL_ISET1_SHIFT      5
#define REG15_JEITA_COOL_ISET1_DIS        0
#define REG15_JEITA_COOL_ISET1_EN         1

#define REG15_JEITA_COOL_VSET_MASK        0x10
#define REG15_JEITA_COOL_VSET_SHIFT       4
#define REG15_JEITA_COOL_VSET_VREG        0
#define REG15_JEITA_COOL_VSET_4100        1

#define REG15_JEITA_WARM_ISET_MASK        0x0C
#define REG15_JEITA_WARM_ISET_SHIFT       2
#define REG15_JEITA_WARM_NO_CHG           0
#define REG15_JEITA_WARM_20PCT_ICHG       1
#define REG15_JEITA_WARM_50PCT_ICHG       2
#define REG15_JEITA_WARM_100PCT_ICHG      3

#define REG15_CONV_START_MASK             0x02
#define REG15_CONV_START_SHIFT            1
#define REG15_CONV_START_INACTIVE         0
#define REG15_CONV_START_ACTIVE           1

#define REG15_CONV_RATE_MASK             0x01
#define REG15_CONV_RATE_SHIFT            0
#define REG15_CONV_RATE_ONE_SHOT         0
#define REG15_CONV_RATE_CONTINUOUS       1

/* Register 0x16 */
#define UPM6922_REG_16             0x16


#endif /* __UPM6922_HEADER__ */

