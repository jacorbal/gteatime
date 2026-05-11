/**
 * @file defs.h
 *
 * @brief Central project constants for ID, icons, and tray protocols
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
 * @brief Application version string
 */
#define APP_VERSION "1.0.2"

/**
 * @brief Application author
 */
#define APP_AUTHOR "J. A. Corbal"

/**
 * @brief Application author email
 */
#define APP_AUTHOR_EMAIL "jacorbal@gmail.com"

/**
 * @brief Application description
 */
#define APP_DESCRIPTION "A lightweight GTK 3 system-tray tea & coffee timer"

/**
 * @brief Text shown when no timer is running
 */
#define APP_TIMER_OFF_TEXT "Timer is off"

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

/**
 * @brief D-Bus object path exported for StatusNotifierItem
 */
#define SNI_OBJECT_PATH "/StatusNotifierItem"

/**
 * @brief Well-known D-Bus name for the StatusNotifier watcher
 */
#define SNI_WATCHER_NAME "org.kde.StatusNotifierWatcher"

/**
 * @brief D-Bus object path for the StatusNotifier watcher
 */
#define SNI_WATCHER_PATH "/StatusNotifierWatcher"

/**
 * @brief D-Bus interface name for the StatusNotifier watcher
 */
#define SNI_WATCHER_INTERFACE "org.kde.StatusNotifierWatcher"

/**
 * @brief D-Bus interface name for the exported StatusNotifier item
 */
#define SNI_INTERFACE "org.kde.StatusNotifierItem"

/**
 * @brief Standard D-Bus properties interface name
 */
#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"

/**
 * @brief StatusNotifier category used for an application status icon
 */
#define SNI_CATEGORY_APPLICATION_STATUS "ApplicationStatus"

/**
 * @brief StatusNotifier status that keeps the item visible in panels
 */
#define SNI_STATUS_ACTIVE "Active"

/**
 * @brief StatusNotifier passive status, kept for protocol completeness
 */
#define SNI_STATUS_PASSIVE "Passive"

/**
 * @brief XEmbed selection name format for the current X11 screen number
 */
#define XEMBED_SYSTEM_TRAY_SELECTION_FORMAT "_NET_SYSTEM_TRAY_S%d"

/**
 * @brief XEmbed client message atom used to request docking
 */
#define XEMBED_SYSTEM_TRAY_OPCODE "_NET_SYSTEM_TRAY_OPCODE"

/**
 * @brief XEmbed opcode used to dock a tray icon window
 */
#define XEMBED_SYSTEM_TRAY_REQUEST_DOCK 0


#endif  /* ! DEFS_H */
