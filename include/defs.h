/**
 * @file defs.h
 *
 * @brief Central project constants for ID and icons
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef DEFS_H
#define DEFS_H


/**
 * @brief Application identifier used by GtkApplication
 */
#define APP_ID "io.github.jacorbal.gteatime"

/**
 * @brief User-visible application title
 */
#define APP_TITLE "Tea & Coffee Timer"

/**
 * @brief Desktop/icon identifier without file extension
 */
#define APP_DESKTOP_ID "gteatime"

/**
 * @brief Generic application icon name
 */
#define APP_ICON_NAME "gteatime"

/**
 * @brief Tray icon name used while the timer is idle
 */
#define TRAY_ICON_IDLE_NAME "gteatime-off"

/**
 * @brief Preferred tray icon name used while the timer is running
 */
#define TRAY_ICON_RUNNING_NAME "gteatime-on"

/**
 * @brief Fallback running tray icon when the preferred icon is
 *        unavailable
 */
#define TRAY_ICON_RUNNING_FALLBACK_NAME "preferences-system-time"


#endif  /* ! DEFS_H */
