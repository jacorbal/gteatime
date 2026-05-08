/**
 * @file presets.h
 *
 * @brief Timer preset declarations
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'ISC License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef PRESETS_H
#define PRESETS_H


/* Glib includes */
#include <glib.h>


/**
 * @typedef Describe one named timer preset
 */
typedef struct timer_preset {
    const char *name;   /**< User-visible preset name */
    guint minutes;      /**< Preset duration in minutes */
    const char *desc;   /**< Short user-visible preset description */
} TimerPreset;


/**
 * @brief Return the complete preset table
 *
 * @param count Optional output receiving the number of presets
 *
 * @return Pointer to the static preset table
 */
const TimerPreset *presets_get_all(gsize *count);

/**
 * @brief Return the default preset used at startup
 *
 * @return Pointer to the default preset
 */
const TimerPreset *presets_get_default(void);

/**
 * @brief Return a preset by zero-based index
 *
 * @param index Preset index
 *
 * @return Pointer to the preset, or @c NULL when the index is invalid
 */
const TimerPreset *presets_get_by_index(guint index);

/**
 * @brief Find the zero-based index of a preset pointer
 *
 * @param preset Preset pointer to find
 * @param index  Output receiving the index
 *
 * @return @c true when the preset pointer belongs to the preset table
 */
gboolean presets_get_index(const TimerPreset *preset, guint *index);


#endif  /* ! PRESETS_H */
