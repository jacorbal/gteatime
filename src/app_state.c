/**
 * @file app_state.c
 *
 * @brief Shared application state lifecycle and helpers
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

/* GTK includes */
#include <gtk/gtk.h>

/* Local includes */
#include <app_state.h>


/* Initialize an 'AppState' structure */
void app_state_init(AppState *app, GtkApplication *gtk_app)
{
    app->gtk_app = gtk_app;
    app->active_preset = presets_get_default();
    app->remaining_seconds = 0;
    app->timer_source_id = 0;
    app->is_running = FALSE;
    app->is_activated = FALSE;
    app->state_changed_cb = NULL;
    app->state_changed_data = NULL;
}


/* Release resources owned by an 'AppState' structure */
void
app_state_clear(AppState *app)
{
    if (app->timer_source_id != 0) {
        g_source_remove(app->timer_source_id);
        app->timer_source_id = 0;
    }
}


/* Register a callback for state changes */
void app_state_set_callback(AppState *app,
        AppStateCallback callback, void *user_data)
{
    app->state_changed_cb = callback;
    app->state_changed_data = user_data;
}


/* Emit the state changed callback when one is registered */
void app_state_emit_changed(AppState *app)
{
    if (app->state_changed_cb != NULL) {
        app->state_changed_cb(app, app->state_changed_data);
    }
}

