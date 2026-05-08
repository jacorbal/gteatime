/**
 * @file main.c
 *
 * @brief Application entry point and top-level GTK callbacks
 *
 * A lightweight GTK3 system-tray tea and coffee timer written in
 * modular C99 and  inspired by KDE’s classic `kteatime`, a tool I used
 * in the early 2000s before switching to Openbox and a GTK3-only
 * environment.
 *
 * @author J. A. Corbal <jacorbal@gmail.com>
 * @date Creation date: Fri May  8 18:52 UTC 2026
 * @version 1.0.0
 * @copyright Copyright (c) 2026, J. A. Corbal.
 *            ISC License <https://opensource.org/license/isc-license-txt>
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'ISC License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

/* GTK includes */
#include <gtk/gtk.h>

/* Project includes */
#include <app_state.h>
#include <defs.h>
#include <timer.h>
#include <tray.h>
#include <window.h>


/**
 * @brief Handle GtkApplication activation
 *
 * @param gtk_app   GTK application instance
 * @param user_data AppState pointer
 */
static void s_app_activate_cb(GtkApplication *gtk_app,
                              gpointer user_data);

/**
 * @brief Handle GtkApplication shutdown
 *
 * @param gtk_app   GApplication instance
 * @param user_data AppState pointer
 */
static void s_app_shutdown_cb(GApplication *gtk_app,
                              gpointer user_data);

/**
 * @brief Propagate shared state changes to visible UI components
 *
 * @param app       Shared application state
 * @param user_data Unused callback data
 */
static void s_state_changed_cb(AppState *app, void *user_data);


/* Main entry */
int main(int argc, char **argv)
{
    GtkApplication *gtk_app;
    AppState app;
    int status;

    gtk_app = gtk_application_new(APP_ID,
                                  (GApplicationFlags)0);
    app_state_init(&app, gtk_app);
    app_state_set_callback(&app, s_state_changed_cb, NULL);
    g_application_hold(G_APPLICATION(gtk_app));

    g_signal_connect(gtk_app, "activate",
            G_CALLBACK(s_app_activate_cb), &app);
    g_signal_connect(gtk_app, "shutdown",
            G_CALLBACK(s_app_shutdown_cb), &app);

    status = g_application_run(G_APPLICATION(gtk_app), argc, argv);

    app_state_clear(&app);
    g_object_unref(gtk_app);

    return status;
}


/* Handle GtkApplication activation */
static void s_app_activate_cb(GtkApplication *gtk_app,
                              gpointer user_data)
{
    AppState *app;
    gboolean tray_ready;

    (void) gtk_app;

    app = user_data;
    tray_ready = tray_init(app);
    if (tray_ready == FALSE) {
        window_show(app);
    }
}


/* Handle GtkApplication shutdown */
static void s_app_shutdown_cb(GApplication *gtk_app,
                              gpointer user_data)
{
    AppState *app;

    (void) gtk_app;

    app = user_data;
    tray_shutdown(app);
}


/* Propagate shared state changes to visible UI components */
static void s_state_changed_cb(AppState *app, void *user_data)
{
    (void) user_data;

    window_update(app);
    tray_update(app);
}
