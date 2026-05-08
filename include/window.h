/**
 * @file window.h
 *
 * @brief Main GTK window API
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'ISC License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef WINDOW_H
#define WINDOW_H


/* Project includes */
#include <app_state.h>


/**
 * @brief Create the main configuration window
 *
 * @param app Shared application state
 */
void window_create(AppState *app);

/**
 * @brief Show and present the main configuration window
 *
 * @param app Shared application state
 */
void window_show(AppState *app);

/**
 * @brief Toggle main window visibility
 *
 * @param app Shared application state
 */
void window_toggle_visible(AppState *app);

/**
 * @brief Update widgets from the current application state
 *
 * @param app Shared application state
 */
void window_update(AppState *app);


#endif  /* ! WINDOW_H */
