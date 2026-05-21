/**
 * @file presets.c
 *
 * @brief Static tea and coffee preset table
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

/* Glib includes */
#include <glib.h>

/* Local includes */
#include <presets.h>


/* Options */
static const TimerPreset timer_presets[] = {
    { "Matcha",                      1,   "Whisked powdered green tea (ceremonial)" },
    { "Green tea",                   2,   "Low-temperature infusion" },
    { "White tea",                   3,   "Delicate low-temperature infusion" },
    { "French press",                4,   "Full immersion coffee (3–5 min typical)" },
    { "Pu-erh tea",                  5,   "Earthy fermented tea infusion (short steeps common)" },
    { "Oolong tea",                  6,   "Partially oxidized tea (shorter gongfu or 3–5 min Western)" },
    { "Rooibos tea",                 7,   "Naturally caffeine-free infusion" },
    { "Chamomile tea",               8,   "Gentle floral herbal infusion (5–7 min)" },
    { "Ginger tea",                  9,   "Warming spicy herbal infusion (infusion; decoction longer)" },
    { "Hibiscus tea",               10,   "Tart floral herbal infusion (5–10 min)" },
    { "Masala chai",                11,   "Spiced black tea; if boiled with milk, 3–10 min depending on strength" },
    { "Peppermint extra strong",    12,   "Extra-potent peppermint herbal infusion with a bold, minty character" },
    { "Cinnamon bark decoction",    15,   "Sweet spiced cinnamon bark simmer (15 min)" },
    { "Ginger root decoction",      18,   "Deep ginger root decoction (18 min simmer)" },
    { "Turmeric root decoction",    20,   "Turmeric root decoction for fuller extraction (15-20+ min)" },
    { "Roasted chicory decoction",  25,   "Slow-simmered roasted chicory root for a coffee-like cup" },
    { "Dandelion root decoction",   30,   "Slow dandelion root decoction (about 30 min)" },
    { "Mixed roots long decoction", 60,   "Very long multi-root decoction (1-hour simmer)" },
    { "Overnight cold mugicha",    480,   "Cold-steeped roasted barley tea (8–12 h)" },
    { "Cold brew coffee",          720,   "Long cold coffee extraction (12–18 h)" },
    //{ "Sourdough bread",          1440,   "Naturally leavened bread with overnight cold proof (24 h)" },
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
