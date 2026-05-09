/**
 * @file notify.h
 *
 * @brief Desktop notification API for completed timers
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef NOTIFY_H
#define NOTIFY_H


/* Project includes */
#include <app_state.h>


/**
 * @brief Send a desktop notification for a completed timer
 *
 * @param app         Shared application state
 * @param preset_name Name of the completed timer preset
 */
void notify_timer_finished(AppState *app, const char *preset_name);


#endif  /* ! NOTIFY_H */
