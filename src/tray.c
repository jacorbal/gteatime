/**
 * @file tray.c
 *
 * @brief XApp status icon tray integration
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

/* XApp includes */
#include <libxapp/xapp-status-icon.h>

/* Project includes */
#include <defs.h>
#include <timer.h>

/* Local includes */
#include <tray.h>


static XAppStatusIcon *s_tray_icon = NULL;


/**
 * @brief Choose the tray icon name for the current timer state
 *
 * @param app Shared application state
 *
 * @return Icon name suitable for @a xapp_status_icon_set_icon_name()
 */
static const char *s_tray_icon_name(AppState *app)
{
    GtkIconTheme *theme;

    if (app->is_running == FALSE) {
        return TRAY_ICON_IDLE_NAME;
    }

    theme = gtk_icon_theme_get_default();
    if (theme != NULL &&
        gtk_icon_theme_has_icon(theme, TRAY_ICON_RUNNING_NAME) == TRUE) {
        return TRAY_ICON_RUNNING_NAME;
    }

    if (theme != NULL &&
        gtk_icon_theme_has_icon(theme,
            TRAY_ICON_RUNNING_FALLBACK_NAME) == TRUE) {
        return TRAY_ICON_RUNNING_FALLBACK_NAME;
    }

    return TRAY_ICON_IDLE_NAME;
}


/**
 * @brief Start a preset selected from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_preset_cb(GtkMenuItem *item, gpointer user_data)
{
    AppState *app;
    gpointer index_pointer;
    const TimerPreset *preset;

    app = user_data;
    index_pointer = g_object_get_data(G_OBJECT(item), "preset-index");
    preset = presets_get_by_index(GPOINTER_TO_UINT(index_pointer));
    if (preset != NULL) {
        timer_start(app, preset);
    }
}


/**
 * @brief Stop the timer from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_stop_cb(GtkMenuItem *item, gpointer user_data)
{
    AppState *app;

    (void) item;

    app = user_data;
    timer_stop(app);
}


/**
 * @brief Quit the application from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_quit_cb(GtkMenuItem *item, gpointer user_data)
{
    AppState *app;

    (void) item;

    app = user_data;
    g_application_quit(G_APPLICATION(app->gtk_app));
}


/**
 * @brief Build the tray popup menu
 *
 * @param app Shared application state
 *
 * @return New menu, floating until the status icon takes it
 */
static GtkWidget *s_tray_build_menu(AppState *app)
{
    GtkWidget *menu;
    GtkWidget *item;
    GtkWidget *separator;
    const TimerPreset *presets;
    gsize count;
    gsize index;
    gchar *label;

    menu = gtk_menu_new();
    presets = presets_get_all(&count);

    for (index = 0; index < count; index++) {
        label = g_strdup_printf("%s (%u min)",
                                presets[index].name,
                                presets[index].minutes);
        item = gtk_menu_item_new_with_label(label);
        g_object_set_data(G_OBJECT(item),
                          "preset-index",
                          GUINT_TO_POINTER((guint) index));
        g_signal_connect(item,
                         "activate",
                         G_CALLBACK(s_tray_menu_preset_cb),
                         app);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
        g_free(label);
    }

    separator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);

    item = gtk_menu_item_new_with_label("Stop timer");
    g_signal_connect(item, "activate", G_CALLBACK(s_tray_menu_stop_cb),
                     app);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(item, "activate", G_CALLBACK(s_tray_menu_quit_cb),
                     app);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    gtk_widget_show_all(menu);
    return menu;
}


/* Initialize tray integration */
void tray_init(AppState *app)
{
    GtkWidget *menu;

    if (s_tray_icon != NULL) {
        return;
    }

    s_tray_icon = xapp_status_icon_new_with_name(APP_DESKTOP_ID);
    xapp_status_icon_set_icon_name(s_tray_icon, s_tray_icon_name(app));

    /* The same menu answers both mouse buttons; the status icon keeps
     * its own reference for each of them */
    menu = s_tray_build_menu(app);
    xapp_status_icon_set_primary_menu(s_tray_icon, GTK_MENU(menu));
    xapp_status_icon_set_secondary_menu(s_tray_icon, GTK_MENU(menu));
}


/* Shut down tray integration and release related resources */
void tray_shutdown(AppState *app)
{
    (void) app;

    g_clear_object(&s_tray_icon);
}


/* Refresh the tray icon for the current timer state */
void tray_update(AppState *app)
{
    if (s_tray_icon == NULL) {
        return;
    }

    xapp_status_icon_set_icon_name(s_tray_icon, s_tray_icon_name(app));
}
