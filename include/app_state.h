/**
 * @file app_state.h
 *
 * @brief Shared app. state used by the timer and tray modules
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

#ifndef APP_STATE_H
#define APP_STATE_H


/* Glib includes */
#include <gio/gio.h>

/* GTK includes */
#include <gtk/gtk.h>

/* Project includes */
#include <presets.h>

struct app_state;

/**
 * @brief Opaque-friendly alias for the shared application state
 */
typedef struct app_state AppState;

/**
 * @brief Callback invoked whenever visible application state changes
 *
 * @param app       Shared application state
 * @param user_data Caller-provided callback data
 */
typedef void (*AppStateCallback) (AppState *app, void *user_data);

/**
 * @brief Stores all runtime state shared across modules
 */
struct app_state {
    GtkApplication *gtk_app;            /**< GTK application instance */
    const TimerPreset *active_preset;   /**< Currently selected timer preset */
    guint remaining_seconds;            /**< Remaining timer in seconds */
    guint timer_source_id;              /**< GLib timeout source ID */
    gboolean is_running;                /**< Timer is active */
    gboolean is_activated;              /**< App. was already activated */
    AppStateCallback state_changed_cb;  /**< Callback for state changes */
    void *state_changed_data;           /**< User data passed state callback */
};


/**
 * @brief Initialize an @a AppState structure
 *
 * @param app     State structure to initialize
 * @param gtk_app GTK application instance
 */
void app_state_init(AppState *app, GtkApplication *gtk_app);

/**
 * @brief Release resources owned by an @a AppState structure
 *
 * @param app State structure to clear
 */
void app_state_clear(AppState *app);

/**
 * @brief Register a callback for state changes
 *
 * @param app       Shared application state
 * @param callback  Callback to invoke
 * @param user_data Caller-provided callback data
 */
void app_state_set_callback(AppState *app,
        AppStateCallback callback, void *user_data);

/**
 * @brief Emit the state changed callback when one is registered
 *
 * @param app Shared application state
 */
void app_state_emit_changed(AppState *app);


#endif  /* ! APP_STATE_H */
