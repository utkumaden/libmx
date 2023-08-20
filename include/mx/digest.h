#ifndef _MX_DIGEST_H_
#define _MX_DIGEST_H_

#include <mx/base.h>

/**
 * @brief Common function pointer function for all mx digest functions.
 * @param[out] digest The digest result.
 * @param[in]  src    The source buffer.
 * @param[in]  length The length of the buffer.
 */
typedef void (*mx_digest_function)(void *digest, const char *src, size_t length);

typedef uint32_t adler32_t;     /**< The adler32 hash.*/
typedef uint32_t fvn0_t;        /**< The FVN-0 hash. */
typedef uint32_t fvn1_t;        /**< The FVN-1 hash. */
typedef uint32_t fvn1a_t;       /**< The FVN-1a hash. */

// typedef uint16_t crc16_t;       /**< CRC16-CCIT checksum. */
// typedef uint32_t crc32_t;       /**< CRC32-IEEE checksum. */
// typedef uint8_t  md5sum_t[8];   /**< MD5SUM checksum. */
// typedef uint8_t  sha0_t  [8];   /**< SHA0 checksum. */
// typedef uint8_t  sha128_t[8];   /**< SHA128 checksum. */
// typedef uint8_t  sha256_t[8];   /**< SHA256 checksum. */

MX_API void mx_adler32(adler32_t *digest, const char *src, size_t length);
MX_API void mx_fvn0   (fvn0_t    *digest, const char *src, size_t length);
MX_API void mx_fvn1   (fvn1_t    *digest, const char *src, size_t length);
MX_API void mx_fvn1a  (fvn1a_t   *digest, const char *src, size_t length);

// MX_API void mx_crc16  (crc16_t   *digest, const char *src, size_t length);
// MX_API void mx_crc32  (crc32_t   *digest, const char *src, size_t length);
// MX_API void mx_md5sum (md5sum_t  *digest, const char *src, size_t length);
// MX_API void mx_sha0   (sha0_t    *digest, const char *src, size_t length);
// MX_API void mx_sha128 (sha128_t  *digest, const char *src, size_t length);
// MX_API void mx_sha256 (sha256_t  *digest, const char *src, size_t length);

MX_API int mx_compare_digest(const void* a, const void *b, size_t size);

#endif