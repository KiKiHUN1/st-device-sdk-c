/* ***************************************************************************
 *
 * Copyright (c) 2020 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <string.h>

#include "iot_main.h"
#include "iot_debug.h"
#include "security/iot_security_helper.h"

#include "mbedtls/version.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "mbedtls/cipher.h"

static iot_error_t _iot_security_url_encode(char *buf, size_t buf_len)
{
	size_t i;

	if (!buf) {
		IOT_ERROR("buf is null");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!buf_len) {
		IOT_ERROR("length is zero");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	for (i = 0; i < buf_len; i++) {
		switch (buf[i]) {
		case '+':
			buf[i] = '-';
			break;
		case '/':
			buf[i] = '_';
			break;
		default:
			break;
		}
	}

	return IOT_ERROR_NONE;
}

static iot_error_t _iot_security_url_decode(char *buf, size_t buf_len)
{
	size_t i;

	if (!buf) {
		IOT_ERROR("buf is null");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!buf_len) {
		IOT_ERROR("length is zero");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	for (i = 0; i < buf_len; i++) {
		switch (buf[i]) {
		case '-':
			buf[i] = '+';
			break;
		case '_':
			buf[i] = '/';
			break;
		default:
			break;
		}
	}

	return IOT_ERROR_NONE;
}

iot_error_t iot_security_base64_encode(const unsigned char *src, size_t src_len,
                                       unsigned char *dst, size_t dst_len,
                                       size_t *out_len)
{
	int ret;

	if (!src || (src_len == 0)) {
		IOT_ERROR("invalid src with %d@%p", (int)src_len, src);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!dst || (dst_len == 0)) {
		IOT_ERROR("invalid dst with %d@%p", (int)dst_len, dst);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!out_len) {
		IOT_ERROR("length output buffer is null");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	IOT_DEBUG("src: %d@%p, dst: %d@%p", (int)src_len, src, (int)dst_len, dst);

	ret = mbedtls_base64_encode(dst, dst_len, out_len, src, src_len);
	if (ret) {
		IOT_ERROR("mbedtls_base64_encode = -0x%04X", -ret);
		IOT_ERROR_DUMP_AND_RETURN(BASE64_ENCODE, -ret);
	}

	IOT_DEBUG("done: %d@%p", (int)*out_len, dst);

	return IOT_ERROR_NONE;
}

iot_error_t iot_security_base64_decode(const unsigned char *src, size_t src_len,
                                       unsigned char *dst, size_t dst_len,
                                       size_t *out_len)
{
	int ret;

	if (!src || (src_len == 0)) {
		IOT_ERROR("invalid src with %d@%p", (int)src_len, src);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!dst || (dst_len == 0)) {
		IOT_ERROR("invalid dst with %d@%p", (int)dst_len, dst);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!out_len) {
		IOT_ERROR("length output buffer is null");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	IOT_DEBUG("src: %d@%p, dst: %d@%p", (int)src_len, src, (int)dst_len, dst);

	ret = mbedtls_base64_decode(dst, dst_len, out_len, src, src_len);
	if (ret) {
		IOT_ERROR("mbedtls_base64_decode = -0x%04X", -ret);
		IOT_ERROR_DUMP_AND_RETURN(BASE64_DECODE, -ret);
	}

	IOT_DEBUG("done: %d@%p", (int)*out_len, dst);

	return IOT_ERROR_NONE;
}

iot_error_t iot_security_base64_encode_urlsafe(const unsigned char *src, size_t src_len,
                                               unsigned char *dst, size_t dst_len,
                                               size_t *out_len)
{
	int ret;

	if (!src || (src_len == 0)) {
		IOT_ERROR("invalid src with %d@%p", (int)src_len, src);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!dst || (dst_len == 0)) {
		IOT_ERROR("invalid dst with %d@%p", (int)dst_len, dst);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!out_len) {
		IOT_ERROR("length output buffer is null");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	IOT_DEBUG("src: %d@%p, dst: %d@%p", (int)src_len, src, (int)dst_len, dst);

	ret = mbedtls_base64_encode(dst, dst_len, out_len, src, src_len);
	if (ret) {
		IOT_ERROR("mbedtls_base64_encode = -0x%04X", -ret);
		IOT_ERROR_DUMP_AND_RETURN(BASE64_URL_ENCODE, -ret);
	}

	ret = _iot_security_url_encode((char *)dst, *out_len);
	if (ret) {
		IOT_ERROR("_iot_security_url_encode = %d", ret);
		IOT_ERROR_DUMP_AND_RETURN(BASE64_URL_ENCODE, ret);
	}

	IOT_DEBUG("done: %d@%p", (int)*out_len, dst);

	return IOT_ERROR_NONE;
}

iot_error_t iot_security_base64_decode_urlsafe(const unsigned char *src, size_t src_len,
                                             unsigned char *dst, size_t dst_len,
                                             size_t *out_len)
{
	unsigned char *src_dup = NULL;
	size_t align_len;
	size_t i;
	int ret;

	if (!src || (src_len == 0)) {
		IOT_ERROR("invalid src with %d@%p", (int)src_len, src);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!dst || (dst_len == 0)) {
		IOT_ERROR("invalid dst with %d@%p", (int)dst_len, dst);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!out_len) {
		IOT_ERROR("length output buffer is null");
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	IOT_DEBUG("src: %d@%p, dst: %d@%p", (int)src_len, src, (int)dst_len, dst);

	align_len = IOT_SECURITY_B64_ALIGN_LEN(src_len);
	src_dup = (unsigned char *)iot_os_malloc(align_len + 1);
	if (src_dup == NULL) {
		IOT_ERROR("malloc failed for align buffer");
		IOT_ERROR_DUMP_AND_RETURN(MEM_ALLOC, 0);
	}

	memcpy(src_dup, src, src_len);
	/* consider '=' removed from tail */
	for (i = src_len; i < align_len; i++) {
		src_dup[i] = '=';
	}
	src_dup[align_len] = '\0';

	ret = _iot_security_url_decode((char *)src_dup, align_len);
	if (ret) {
		IOT_ERROR("_iot_security_url_decode = %d", ret);
		iot_os_free(src_dup);
		IOT_ERROR_DUMP_AND_RETURN(BASE64_URL_DECODE, ret);
	}

	ret = mbedtls_base64_decode(dst, dst_len, out_len, (const unsigned char *)src_dup, align_len);
	if (ret) {
		IOT_ERROR("mbedtls_base64_decode = -0x%04X", -ret);
		iot_os_free(src_dup);
		IOT_ERROR_DUMP_AND_RETURN(BASE64_URL_DECODE, -ret);
	}

	iot_os_free(src_dup);
	IOT_DEBUG("done: %d@%p", (int)*out_len, dst);

	return IOT_ERROR_NONE;
}

iot_error_t iot_security_sha256(const unsigned char *input, size_t input_len, unsigned char *output, size_t output_len)
{
	int ret;

	if (!input || (input_len == 0)) {
		IOT_ERROR("invalid input with %d@%p", (int)input_len, input);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

	if (!output || (output_len < IOT_SECURITY_SHA256_LEN)) {
		IOT_ERROR("invalid output with %d@%p", (int)output_len, output);
		IOT_ERROR_DUMP_AND_RETURN(INVALID_ARGS, 0);
	}

#if MBEDTLS_VERSION_NUMBER > 0x03000000
	ret = mbedtls_sha256(input, input_len, output, 0);
#else
	ret = mbedtls_sha256_ret(input, input_len, output, 0);
#endif
	if (ret) {
		IOT_ERROR("mbedtls_sha256_ret = -0x%04X", -ret);
		IOT_ERROR_DUMP_AND_RETURN(SHA256, -ret);
	}

	return IOT_ERROR_NONE;
}
