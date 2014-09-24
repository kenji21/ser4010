/**
 * ser4010.c - SER4010 API interface
 *
 * Copyright (c) 2014, David Imhoff <dimhoff_devel@xs4all.nl>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors may
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ser4010.h"
#include <endian.h>

/**
 * Byte swap IEEE-754 'single' floating point number
 */
///@{
inline float befloattoh(float x)
{
	// Need some magic to byteswap floats since typecasting doesn't work.
	union {
		uint32_t i;
		float	 f;
	} accessor;

	accessor.f = x;
	accessor.i = be32toh(accessor.i);
	return accessor.f;
}

inline float htobefloat(float x)
{
	// Need some magic to byteswap floats since typecasting doesn't work.
	union {
		uint32_t i;
		float	 f;
	} accessor;

	accessor.f = x;
	accessor.i = htobe32(accessor.i);

	return accessor.f;
}
///@}

int ser4010_set_ods(struct serco *sdev, const tOds_Setup *ods_config)
{
	tOds_Setup l_ods_config;

	l_ods_config.bModulationType = ods_config->bModulationType;
	l_ods_config.bClkDiv = ods_config->bClkDiv;
	l_ods_config.bEdgeRate = ods_config->bEdgeRate;
	l_ods_config.bGroupWidth = ods_config->bGroupWidth;
	l_ods_config.wBitRate = htobe16(ods_config->wBitRate);
	l_ods_config.bLcWarmInt = ods_config->bLcWarmInt;
	l_ods_config.bDivWarmInt = ods_config->bDivWarmInt;
	l_ods_config.bPaWarmInt = ods_config->bPaWarmInt;

	return serco_send_command(sdev, CMD_SET_ODS, &l_ods_config, sizeof(tOds_Setup), NULL, 0);
}

int ser4010_get_ods(struct serco *sdev, tOds_Setup *ods_config)
{
	int ret;
	size_t res_len;

	res_len = sizeof(tOds_Setup);
	ret = serco_send_command(sdev, CMD_GET_ODS, NULL, 0, ods_config, &res_len);
	if (ret != STATUS_OK) {
		return ret;
	}
	if (res_len != sizeof(tOds_Setup)) {
		return -1000;
	}

	// Fix endianness
	ods_config->wBitRate = be16toh(ods_config->wBitRate);

	return 0;
}

int ser4010_set_pa(struct serco *sdev, const tPa_Setup *pa_config)
{
	tPa_Setup l_pa_config;

	// Fix endianness
	l_pa_config.fAlpha = htobefloat(pa_config->fAlpha);
	l_pa_config.fBeta = htobefloat(pa_config->fBeta);
	l_pa_config.bLevel = pa_config->bLevel;
	l_pa_config.bMaxDrv = pa_config->bMaxDrv;
	l_pa_config.wNominalCap = htobe16(pa_config->wNominalCap);

	return serco_send_command(sdev, CMD_SET_PA, &l_pa_config, sizeof(tPa_Setup), NULL, 0);
}

int ser4010_get_pa(struct serco *sdev, tPa_Setup *pa_config)
{
	int ret;
	size_t res_len;

	res_len = sizeof(tPa_Setup);
	ret = serco_send_command(sdev, CMD_GET_PA, NULL, 0, pa_config, &res_len);
	if (ret != STATUS_OK) {
		return ret;
	}
	if (res_len != sizeof(tPa_Setup)) {
		return -1000;
	}

	// Fix endianness
	pa_config->fAlpha = befloattoh(pa_config->fAlpha);
	pa_config->fBeta = befloattoh(pa_config->fBeta);
	pa_config->wNominalCap = be16toh(pa_config->wNominalCap);

	return 0;
}

int ser4010_set_freq(struct serco *sdev, float freq)
{
	// Fix endianness
	freq = htobefloat(freq);

	return serco_send_command(sdev, CMD_SET_FREQ, &freq, sizeof(float), NULL, 0);
}

int ser4010_get_freq(struct serco *sdev, float *freq)
{
	int ret;
	size_t res_len;

	res_len = sizeof(float);
	ret = serco_send_command(sdev, CMD_GET_FREQ, NULL, 0, freq, &res_len);
	if (ret != STATUS_OK) {
		return ret;
	}
	if (res_len != sizeof(float)) {
		return -1000;
	}

	// Fix endianness
	*freq = befloattoh(*freq);

	return 0;
}

int ser4010_set_fdiv(struct serco *sdev, float fdiv)
{
	// Fix endianness
	fdiv = htobefloat(fdiv);

	return serco_send_command(sdev, CMD_SET_FDIV, &fdiv, sizeof(float), NULL, 0);
}

int ser4010_get_fdiv(struct serco *sdev, float *fdiv)
{
	int ret;
	size_t res_len;

	res_len = sizeof(float);
	ret = serco_send_command(sdev, CMD_GET_FDIV, NULL, 0, fdiv, &res_len);
	if (ret != STATUS_OK) {
		return ret;
	}
	if (res_len != sizeof(float)) {
		return -1000;
	}

	// Fix endianness
	*fdiv = befloattoh(*fdiv);

	return 0;
}

int ser4010_reconfigure(struct serco *sdev)
{
	return serco_send_command(sdev, CMD_RF_SETUP, NULL, 0, NULL, 0);
}

int ser4010_load_frame(struct serco *sdev, uint8_t *data, size_t len)
{
	return serco_send_command(sdev, CMD_LOAD_FRAME, data, len, NULL, 0);
}

int ser4010_send(struct serco *sdev, unsigned int cnt)
{
	uint8_t buf[5];

	buf[0] = SEND_COOKIE_0;
	buf[1] = SEND_COOKIE_1;
	buf[2] = SEND_COOKIE_2;
	buf[3] = SEND_COOKIE_3;
	buf[4] = cnt;

	return serco_send_command(sdev, CMD_RF_SEND, buf, 5, NULL, 0);
}