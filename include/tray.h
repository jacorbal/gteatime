/**
 * @file tray.h
 *
 * @brief System tray integration API
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef TRAY_H
#define TRAY_H


/* Project includes */
#include <app_state.h>


/**
 * @brief Initialize tray integration
 *
 * @param app Shared application state
 *
 * @return @c true when either @a StatusNotifierItem or @a XEmbed tray
 *         setup succeeds
 */
gboolean tray_init(AppState *app);

/**
 * @brief Shut down tray integration and release related resources
 *
 * @param app Shared application state
 */
void tray_shutdown(AppState *app);

/**
 * @brief Refresh tray properties, tooltip, and icon state
 *
 * @param app Shared application state
 */
void tray_update(AppState *app);


#endif  /* ! TRAY_H */
