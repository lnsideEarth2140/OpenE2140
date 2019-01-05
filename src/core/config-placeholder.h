//
// Created by Ion Agorria on 03/01/19
//

/**
 * This only defines the data type for configs so the IDE doesn't complain about missing defs
 */

#ifndef OPENE2140_CONFIG_PLACEHOLDER_H
#define OPENE2140_CONFIG_PLACEHOLDER_H

/* Default resolution height */
#ifndef DEFAULT_RESOLUTION_HEIGHT
#define DEFAULT_RESOLUTION_HEIGHT 0
#endif

/* Default resolution width */
#ifndef DEFAULT_RESOLUTION_WIDTH
#define DEFAULT_RESOLUTION_WIDTH 0
#endif

/* Directory name containing assets */
#ifndef GAME_ASSETS_DIR
#define GAME_ASSETS_DIR ""
#endif

/* Names of asset containers to load */
#ifndef GAME_ASSET_CONTAINER_NAMES
#define GAME_ASSET_CONTAINER_NAMES {""}
#endif

/* File to dump when crashing */
#ifndef GAME_DUMP_FILE
#define GAME_DUMP_FILE ""
#endif

/* File to store log output */
#ifndef GAME_LOG_FILE
#define GAME_LOG_FILE ""
#endif

/* Game name */
#ifndef GAME_NAME
#define GAME_NAME ""
#endif

/* Game version major digit */
#ifndef GAME_VERSION_MAJOR
#define GAME_VERSION_MAJOR 0
#endif

/* Game version minor digit */
#ifndef GAME_VERSION_MINOR
#define GAME_VERSION_MINOR 0
#endif

/* True if being building on macos */
#ifndef OPENE2140_IS_MACOS
#define OPENE2140_IS_MACOS false
#endif

/* True if being building on windows */
#ifndef OPENE2140_IS_WINDOWS
#define OPENE2140_IS_WINDOWS false
#endif

/* Enables the use of Boost library */
#ifndef OPENE2140_USE_BOOST
#define OPENE2140_USE_BOOST false
#endif

#endif //OPENE2140_CONFIG_PLACEHOLDER_H
