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

/* Project includes */
#include <defs.h>

/* Local includes */
#include <app_state.h>


/* Initialize an 'AppState' structure */
void app_state_init(AppState *app, GtkApplication *gtk_app)
{
    app->gtk_app = gtk_app;
    app->window = NULL;
    app->status_label = NULL;
    app->preset_combo = NULL;
    app->start_button = NULL;
    app->stop_button = NULL;
    app->tray_menu = NULL;
    app->xembed_window = NULL;
    app->xembed_event_box = NULL;
    app->xembed_image = NULL;
    app->active_preset = presets_get_default();
    app->remaining_seconds = 0;
    app->total_seconds = 0;
    app->timer_source_id = 0;
    app->is_running = FALSE;
    app->bus = NULL;
    app->sni_registration_id = 0;
    app->sni_bus_name_id = 0;
    app->sni_bus_name = NULL;
    app->tooltip_text = g_strdup(APP_TIMER_OFF_TEXT);
    app->xembed_ready = FALSE;
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

    if (app->tooltip_text != NULL) {
        g_free(app->tooltip_text);
        app->tooltip_text = NULL;
    }

    if (app->sni_bus_name != NULL) {
        g_free(app->sni_bus_name);
        app->sni_bus_name = NULL;
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


/* Replace the tray tooltip text */
void app_state_set_tooltip(AppState *app, const char *text)
{
    g_free(app->tooltip_text);
    app->tooltip_text = g_strdup(text);
}


/* Return the current tray tooltip text */
const char *app_state_get_tooltip(AppState *app)
{
    if (app->tooltip_text == NULL) {
        return APP_TIMER_OFF_TEXT;
    }

    return app->tooltip_text;
}
