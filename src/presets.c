/**
 * @file presets.c
 *
 * @brief Static tea and coffee preset table
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'ISC License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

/* Glib includes */
#include <glib.h>

/* Local includes */
#include <presets.h>


/* Options */
static const TimerPreset timer_presets[] = {
    { "Green tea",                2, "Low-temperature infusion" },
    { "White tea",                3, "Delicate low-temperature infusion" },
    { "French press",             4, "Full immersion coffee" },
    { "Pu-erh tea",               5, "Earthy fermented tea infusion" },
    { "Rooibos tea",              6, "Naturally caffeine-free infusion" },
    { "Chamomile tea",            7, "Gentle floral herbal infusion" },
    { "Ginger tea",               8, "Warming spicy herbal infusion" },
    { "Hibiscus tea",             9, "Tart floral herbal infusion" },
    { "Masala chai",             10, "Slow spiced black tea preparation" },
    { "Peppermint strong",       13, "Bold peppermint herbal infusion" },
    { "Cinnamon bark decoct.",   14, "Sweet spiced bark simmer" },
    { "Ginger root decoction",   15, "Root simmer for deep extraction" },
    { "Turmeric decoction",      18, "Long simmered turmeric root brew" },
    { "Dandelion root decoct.",  23, "Slow roasted root decoction" },
    { "Mixed roots decoction",   25, "Strong multi-root herbal decoction" },
    { "Overnight cold mugicha", 480, "Cold-steeped roasted barley tea" },
    { "Cold brew coffee",       720, "Long cold coffee extraction" },
};


/* Return the complete preset table */
const TimerPreset *presets_get_all(gsize *count)
{
    const TimerPreset *presets;

    presets = timer_presets;
    if (count != NULL) {
        *count = sizeof(timer_presets) / sizeof(timer_presets[0]);
    }

    return presets;
}


/* Return the default preset used at startup */
const TimerPreset *presets_get_default(void)
{
    return &timer_presets[0];
}


/* Return a preset by zero-based index */
const TimerPreset *presets_get_by_index(guint index)
{
    gsize count;

    count = 0;
    presets_get_all(&count);
    if (index >= count) {
        return NULL;
    }

    return &timer_presets[index];
}


/* Find the zero-based index of a preset pointer*/ 
gboolean presets_get_index(const TimerPreset *preset, guint *index)
{
    gsize count;
    gsize current;

    if (preset == NULL || index == NULL) {
        return FALSE;
    }

    count = 0;
    presets_get_all(&count);
    for (current = 0; current < count; current++) {
        if (&timer_presets[current] == preset) {
            *index = (guint) current;
            return TRUE;
        }
    }

    return FALSE;
}
