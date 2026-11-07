/**
 * @file timer.h
 *
 * @brief Countdown timer API
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef TIMER_H
#define TIMER_H


/* Project includes */
#include <app_state.h>
#include <presets.h>


/**
 * @brief Start or resets the countdown timer
 *
 * @param app    Shared application state
 * @param preset Preset to run
 */
void timer_start(AppState *app, const TimerPreset *preset);

/**
 * @brief Stop the countdown timer
 *
 * @param app Shared application state
 */
void timer_stop(AppState *app);


#endif  /* ! TIMER_H */
