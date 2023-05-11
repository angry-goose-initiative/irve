/* config.h
 * Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * Contains build-time configuration information for IRVE
 *
*/

#ifndef CONFIG_H
#define CONFIG_H

#if __has_include("cmake_config.h")
#include "cmake_config.h"
#define IRVE_CONFIG_VERSION_MAJOR   IRVE_CMAKE_VERSION_MAJOR
#define IRVE_CONFIG_VERSION_MINOR   IRVE_CMAKE_VERSION_MINOR
#define IRVE_CONFIG_VERSION_PATCH   IRVE_CMAKE_VERSION_PATCH
#define IRVE_CONFIG_VERSION_STRING  IRVE_CMAKE_VERSION_STRING
#define IRVE_CONFIG_DISABLE_LOGGING IRVE_CMAKE_DISABLE_LOGGING
#else
#define IRVE_CONFIG_VERSION_MAJOR   0 
#define IRVE_CONFIG_VERSION_MINOR   0 
#define IRVE_CONFIG_VERSION_PATCH   0 
#define IRVE_CONFIG_VERSION_STRING  "IRVE (Version UNKNOWN)"
#define IRVE_CONFIG_DISABLE_LOGGING 0
#endif

#endif//CONFIG_H
