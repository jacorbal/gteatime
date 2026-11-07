/**
 * @file timer.c
 *
 * @brief Countdown timer implementation
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

/* Project includes */
#include <notify.h>

/* Local includes */
#include <timer.h>


/**
 * @brief Handle one countdown tick
 *
 * @param user_data AppState pointer
 *
 * @return @c G_SOURCE_CONTINUE while the timer remains active
 */
static gboolean s_timer_tick_cb(gpointer user_data);


/* Start or resets the countdown timer */
void timer_start(AppState *app, const TimerPreset *preset)
{
    if (app->timer_source_id != 0) {
        g_source_remove(app->timer_source_id);
        app->timer_source_id = 0;
    }

    app->active_preset = preset;
    app->remaining_seconds = preset->minutes * 60;
    app->is_running = TRUE;
    app->timer_source_id = g_timeout_add_seconds(1, s_timer_tick_cb, app);
    app_state_emit_changed(app);
}


/* Stop the countdown timer */
void timer_stop(AppState *app)
{
    if (app->timer_source_id != 0) {
        g_source_remove(app->timer_source_id);
        app->timer_source_id = 0;
    }

    app->remaining_seconds = 0;
    app->is_running = FALSE;
    app_state_emit_changed(app);
}


/* Handle one countdown tick */
static gboolean s_timer_tick_cb(gpointer user_data)
{
    AppState *app;
    const char *preset_name;

    app = user_data;
    if (app->remaining_seconds > 0) {
        app->remaining_seconds--;
    }

    if (app->remaining_seconds == 0) {
        preset_name = "Timer";
        if (app->active_preset != NULL) {
            preset_name = app->active_preset->name;
        }

        app->timer_source_id = 0;
        app->is_running = FALSE;
        app_state_emit_changed(app);
        notify_timer_finished(app, preset_name);
        return G_SOURCE_REMOVE;
    }

    app_state_emit_changed(app);
    return G_SOURCE_CONTINUE;
}
