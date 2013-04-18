#ifndef __AID_EA8061_H__
#define __AID_EA8061_H__

#include "smart_dimming_ea8061.h"

#define aid_300nit_260nit_B3_1st	0x00
#define aid_300nit_260nit_B3_2nd	0x0A
#define aid_250nit_190nit_B3_1st	0x00
#define aid_250nit_190nit_B3_2nd	0x0A
#define aid_188nit_B3_1st	0x00
#define aid_186nit_182nit_B3_1st	0x01
#define aid_188nit_B3_2nd		0x96
#define aid_186nit_B3_2nd		0x11
#define aid_184nit_B3_2nd		0x73
#define aid_182nit_B3_2nd		0xC2
#define aid_180nit_110nit_B3_1st	0x02
#define aid_180nit_110nit_B3_2nd	0x06
#define aid_100nit_90nit_B3_1st	0x00
#define aid_80nit_70nit_B3_1st	0x01
#define aid_60nit_50nit_B3_1st	0x02
#define aid_40nit_30nit_B3_1st	0x03
#define aid_20nit_B3_1st	0x04
#define aid_100nit_B3_2nd		0x71
#define aid_90nit_B3_2nd		0xF6
#define aid_80nit_B3_2nd		0x80
#define aid_70nit_B3_2nd		0xF2
#define aid_60nit_B3_2nd		0x66
#define aid_50nit_B3_2nd		0xE2
#define aid_40nit_B3_2nd		0x54
#define aid_30nit_B3_2nd		0xC4
#define aid_20nit_B3_2nd		0x35
#define AOR40_BASE_188		207
#define AOR40_BASE_186		224
#define AOR40_BASE_184		241
#define AOR40_BASE_182		258
#define AOR40_BASE_180		275
#define AOR40_BASE_170		260
#define AOR40_BASE_160		246
#define AOR40_BASE_150		231
#define AOR40_BASE_140		217
#define AOR40_BASE_130		202
#define AOR40_BASE_120		188
#define AOR40_BASE_110		173
#define base_20to100			110

const struct rgb_offset_info aid_rgb_fix_table[] = {
	{GAMMA_180CD, IV_11, CI_RED, -1}, {GAMMA_180CD, IV_11, CI_GREEN, -2}, {GAMMA_180CD, IV_11, CI_BLUE, 3},
	{GAMMA_170CD, IV_11, CI_RED, -1}, {GAMMA_170CD, IV_11, CI_GREEN, -2}, {GAMMA_170CD, IV_11, CI_BLUE, 3},
	{GAMMA_160CD, IV_11, CI_RED, -1}, {GAMMA_160CD, IV_11, CI_GREEN, -2}, {GAMMA_160CD, IV_11, CI_BLUE, 3},
	{GAMMA_150CD, IV_11, CI_RED, -1}, {GAMMA_150CD, IV_11, CI_GREEN, -2}, {GAMMA_150CD, IV_11, CI_BLUE, 3},
	{GAMMA_140CD, IV_11, CI_RED, -1}, {GAMMA_140CD, IV_11, CI_GREEN, -2}, {GAMMA_140CD, IV_11, CI_BLUE, 3},
	{GAMMA_130CD, IV_11, CI_RED, -1}, {GAMMA_130CD, IV_11, CI_GREEN, -2}, {GAMMA_130CD, IV_11, CI_BLUE, 3},
	{GAMMA_120CD, IV_11, CI_RED, -1}, {GAMMA_120CD, IV_11, CI_GREEN, -2}, {GAMMA_120CD, IV_11, CI_BLUE, 3},
	{GAMMA_110CD, IV_11, CI_RED, -1}, {GAMMA_110CD, IV_11, CI_GREEN, -2}, {GAMMA_110CD, IV_11, CI_BLUE, 3},
	{GAMMA_100CD, IV_11, CI_BLUE, 2},
	{GAMMA_90CD, IV_11, CI_BLUE, 5},
	{GAMMA_80CD, IV_11, CI_BLUE, 8},
	{GAMMA_70CD, IV_11, CI_BLUE, 11},
	{GAMMA_60CD, IV_11, CI_BLUE, 15},
	{GAMMA_50CD, IV_11, CI_BLUE, 19},
	{GAMMA_40CD, IV_11, CI_BLUE, 24},
	{GAMMA_30CD, IV_11, CI_BLUE, 31},
	{GAMMA_20CD, IV_11, CI_BLUE, 34},
	{GAMMA_90CD, IV_23, CI_GREEN, -2},
	{GAMMA_80CD, IV_23, CI_RED, -1}, {GAMMA_80CD, IV_23, CI_GREEN, -3},
	{GAMMA_70CD, IV_23, CI_RED, -3}, {GAMMA_70CD, IV_23, CI_GREEN, -5},
	{GAMMA_60CD, IV_23, CI_RED, -5}, {GAMMA_60CD, IV_23, CI_GREEN, -7},
	{GAMMA_50CD, IV_23, CI_RED, -8}, {GAMMA_50CD, IV_23, CI_GREEN, -11},
	{GAMMA_40CD, IV_23, CI_RED, -15}, {GAMMA_40CD, IV_23, CI_GREEN, -17},
	{GAMMA_30CD, IV_23, CI_RED, -32}, {GAMMA_30CD, IV_23, CI_GREEN, -20},
	{GAMMA_20CD, IV_23, CI_RED, -43}, {GAMMA_20CD, IV_23, CI_GREEN, -20}, {GAMMA_20CD, IV_23, CI_BLUE, 7},
	{GAMMA_30CD, IV_35, CI_GREEN, -13},
	{GAMMA_20CD, IV_35, CI_RED, -5}, {GAMMA_20CD, IV_35, CI_GREEN, -24},
};

static unsigned char aid_command_20[] = {
	aid_20nit_B3_1st,
	aid_20nit_B3_2nd,
};

static unsigned char aid_command_30[] = {
	aid_40nit_30nit_B3_1st,
	aid_30nit_B3_2nd,
};

static unsigned char aid_command_40[] = {
	aid_40nit_30nit_B3_1st,
	aid_40nit_B3_2nd,
};

static unsigned char aid_command_50[] = {
	aid_60nit_50nit_B3_1st,
	aid_50nit_B3_2nd,
};

static unsigned char aid_command_60[] = {
	aid_60nit_50nit_B3_1st,
	aid_60nit_B3_2nd,
};

static unsigned char aid_command_70[] = {
	aid_80nit_70nit_B3_1st,
	aid_70nit_B3_2nd,
};

static unsigned char aid_command_80[] = {
	aid_80nit_70nit_B3_1st,
	aid_80nit_B3_2nd,
};

static unsigned char aid_command_90[] = {
	aid_100nit_90nit_B3_1st,
	aid_90nit_B3_2nd,
};

static unsigned char aid_command_100[] = {
	aid_100nit_90nit_B3_1st,
	aid_100nit_B3_2nd,
};

static unsigned char aid_command_110[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_120[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_130[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_140[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_150[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_160[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_170[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_180[] = {
	aid_180nit_110nit_B3_1st,
	aid_180nit_110nit_B3_2nd,
};

static unsigned char aid_command_182[] = {
	aid_186nit_182nit_B3_1st,
	aid_182nit_B3_2nd,
};

static unsigned char aid_command_184[] = {
	aid_186nit_182nit_B3_1st,
	aid_184nit_B3_2nd,
};

static unsigned char aid_command_186[] = {
	aid_186nit_182nit_B3_1st,
	aid_186nit_B3_2nd,
};

static unsigned char aid_command_188[] = {
	aid_188nit_B3_1st,
	aid_188nit_B3_2nd,
};

static unsigned char aid_command_190[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_200[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_210[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_220[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_230[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_240[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_250[] = {
	aid_250nit_190nit_B3_1st,
	aid_250nit_190nit_B3_2nd,
};

static unsigned char aid_command_300[] = {
	aid_300nit_260nit_B3_1st,
	aid_300nit_260nit_B3_2nd,

};

static unsigned char *aid_command_table[GAMMA_MAX] = {
	aid_command_20,
	aid_command_30,
	aid_command_40,
	aid_command_50,
	aid_command_60,
	aid_command_70,
	aid_command_80,
	aid_command_90,
	aid_command_100,
	aid_command_110,
	aid_command_120,
	aid_command_130,
	aid_command_140,
	aid_command_150,
	aid_command_160,
	aid_command_170,
	aid_command_180,
	aid_command_182,
	aid_command_184,
	aid_command_186,
	aid_command_188,
	aid_command_190,
	aid_command_200,
	aid_command_210,
	aid_command_220,
	aid_command_230,
	aid_command_240,
	aid_command_250,
	aid_command_300
};

#endif
