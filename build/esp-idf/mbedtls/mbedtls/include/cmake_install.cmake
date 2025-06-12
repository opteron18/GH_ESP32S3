# Install script for directory: G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/blink")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "G:/Espressif/tools/tools/xtensa-esp32s3-elf/esp-12.2.0_20230208/xtensa-esp32s3-elf/bin/xtensa-esp32s3-elf-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_crypto.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_from_psa.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_from_legacy.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_superset_legacy.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_adjust_ssl.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_adjust_x509.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/sha3.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/build_info.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_adjust_auto_enabled.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_key_pair_types.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_synonyms.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_key_derivation.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_key_derivation.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_legacy.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "G:/Espressif/frameworks/esp-idf-v5.1.2/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

