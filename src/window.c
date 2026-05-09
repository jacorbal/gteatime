/**
 * @file window.c
 * @brief GTK configuration window implementation.
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
#include <timer.h>

/* Local includes */
#include <window.h>

/**
 * @brief Start or reset the timer from the selected preset
 *
 * @param button    Button that emitted the signal
 * @param user_data AppState pointer
 */
static void s_start_clicked_cb(GtkButton *button, gpointer user_data);

/**
 * @brief Stop the timer from the window
 *
 * @param button    Button that emitted the signal
 * @param user_data AppState pointer
 */
static void s_stop_clicked_cb(GtkButton *button, gpointer user_data);

/**
 * @brief Handle preset selection change
 *
 * @param combo     Combo box that emitted the signal
 * @param user_data AppState pointer
 */
static void s_preset_changed_cb(GtkComboBox *combo, gpointer user_data);

/**
 * @brief Hides the window instead of destroying it
 *
 * @param widget    Window widget
 * @param event     Delete event
 * @param user_data AppState pointer
 *
 * @return @c true to stop default destruction
 */
static gboolean s_window_delete_event_cb(GtkWidget *widget,
                                       GdkEvent *event,
                                       gpointer user_data);

/**
 * @brief Handle key press events (ESC to hide window)
 *
 * @param widget    Window widget
 * @param event     Key press event
 * @param user_data AppState pointer
 *
 * @return @c true if the event was handled
 */
static gboolean s_window_key_press_cb(GtkWidget *widget,
                                      GdkEventKey *event,
                                      gpointer user_data);

/**
 * @brief Show About dialog
 *
 * @param button    Button that emitted the signal
 * @param user_data AppState pointer
 */
static void s_about_clicked_cb(GtkButton *button, gpointer user_data);



/* Create the main configuration window */
void window_create(AppState *app)
{
    GtkWidget *box;
    GtkWidget *button_box;
    GtkWidget *label;
    GtkAccelGroup *accel_group;
    const TimerPreset *presets;
    gsize count;
    gsize index;
    gchar *row_text;

    app->window = gtk_application_window_new(app->gtk_app);
    gtk_window_set_title(GTK_WINDOW(app->window), APP_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(app->window), 325, 215);
    gtk_window_set_resizable(GTK_WINDOW(app->window), FALSE);
    gtk_window_set_icon_name(GTK_WINDOW(app->window), APP_ICON_NAME);
    g_signal_connect(app->window,
                     "delete-event",
                     G_CALLBACK(s_window_delete_event_cb),
                     app);
    g_signal_connect(app->window,
                     "key-press-event",
                     G_CALLBACK(s_window_key_press_cb),
                     app);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 12);
    gtk_container_add(GTK_CONTAINER(app->window), box);

    label = gtk_label_new("Choose a tea or coffee timer:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    app->preset_combo = gtk_combo_box_text_new();
    presets = presets_get_all(&count);
    for (index = 0; index < count; ++index) {
        row_text = g_strdup_printf("%s (%u min)",
                                   presets[index].name,
                                   presets[index].minutes);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->preset_combo),
                                       row_text);
        g_free(row_text);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->preset_combo), 0);
    gtk_box_pack_start(GTK_BOX(box), app->preset_combo, FALSE, FALSE, 0);

    /* Description label */
    app->desc_label = gtk_label_new("");
    gtk_widget_set_halign(app->desc_label, GTK_ALIGN_START);
    gtk_widget_set_valign(app->desc_label, GTK_ALIGN_START);
    gtk_widget_set_size_request(app->desc_label, -1, 40);
    gtk_label_set_lines(GTK_LABEL(app->desc_label), 2);
    gtk_label_set_line_wrap(GTK_LABEL(app->desc_label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(app->desc_label), 50);
    gtk_label_set_xalign(GTK_LABEL(app->desc_label), 0.0);
    gtk_box_pack_start(GTK_BOX(box), app->desc_label, FALSE, FALSE, 0);

    app->status_label = gtk_label_new(APP_TIMER_OFF_TEXT);
    gtk_widget_set_halign(app->status_label, GTK_ALIGN_START);
    gtk_widget_set_valign(app->status_label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(app->status_label, 10);
    gtk_box_pack_start(GTK_BOX(box), app->status_label, FALSE, FALSE, 0);

    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(button_box, GTK_ALIGN_END);
    gtk_box_set_spacing(GTK_BOX(button_box), 8);
    gtk_box_pack_end(GTK_BOX(box), button_box, FALSE, FALSE, 0);

    app->start_button = gtk_button_new_with_label("Start");
    app->stop_button = gtk_button_new_with_label("Stop");
    app->about_button = gtk_button_new_with_label("About");
    gtk_widget_set_tooltip_text(app->start_button,
            "Start or reset the timer (Alt+P)");
    gtk_widget_set_tooltip_text(app->stop_button,
            "Stop the timer (Alt+S)");
    gtk_widget_set_tooltip_text(app->about_button,
            "About (Alt+A)");
    gtk_container_add(GTK_CONTAINER(button_box), app->start_button);
    gtk_container_add(GTK_CONTAINER(button_box), app->stop_button);
    gtk_container_add(GTK_CONTAINER(button_box), app->about_button);

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(app->window), accel_group);
    gtk_widget_add_accelerator(app->start_button,
                               "clicked",
                               accel_group,
                               GDK_KEY_p,
                               GDK_MOD1_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(app->stop_button,
                               "clicked",
                               accel_group,
                               GDK_KEY_s,
                               GDK_MOD1_MASK,
                               GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(app->about_button,
                               "clicked",
                               accel_group,
                               GDK_KEY_a,
                               GDK_MOD1_MASK,
                               GTK_ACCEL_VISIBLE);
    g_object_unref(accel_group);

    g_signal_connect(app->about_button,
                     "clicked",
                     G_CALLBACK(s_about_clicked_cb),
                     app);
    g_signal_connect(app->start_button,
                     "clicked",
                     G_CALLBACK(s_start_clicked_cb),
                     app);
    g_signal_connect(app->stop_button,
                     "clicked",
                     G_CALLBACK(s_stop_clicked_cb),
                     app);
    g_signal_connect(app->preset_combo,
                     "changed",
                     G_CALLBACK(s_preset_changed_cb),
                     app);

    window_update(app);
}


/* Show and present the main configuration window */
void window_show(AppState *app)
{
    if (app->window == NULL) {
        window_create(app);
    }

    window_update(app);
    gtk_widget_show_all(app->window);
    gtk_window_present(GTK_WINDOW(app->window));
}


/* Toggle main window visibility */
void window_toggle_visible(AppState *app)
{
    if (app->window == NULL) {
        window_show(app);
        return;
    }

    if (gtk_widget_get_visible(app->window) == TRUE) {
        gtk_widget_hide(app->window);
    } else {
        window_show(app);
    }
}


/* Update widgets from the current application state */
void window_update(AppState *app)
{
    char *status;
    guint preset_index;

    if (app->status_label == NULL) {
        return;
    }

    if (app->is_running == TRUE) {
        status = timer_format_remaining(app->remaining_seconds);
        gtk_label_set_text(GTK_LABEL(app->status_label), status);
        g_free(status);
    } else {
        gtk_label_set_text(GTK_LABEL(app->status_label), APP_TIMER_OFF_TEXT);
    }

    if (app->preset_combo != NULL &&
        presets_get_index(app->active_preset, &preset_index) == TRUE) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(app->preset_combo),
                                 (gint) preset_index);
    }

    if (app->desc_label != NULL && app->active_preset != NULL) {
        gtk_label_set_text(GTK_LABEL(app->desc_label),
                app->active_preset->desc);
    }

    if (app->start_button != NULL) {
        gtk_widget_set_sensitive(app->start_button, TRUE);
        if (app->is_running == TRUE) {
            gtk_button_set_label(GTK_BUTTON(app->start_button), "Reset");
        } else {
            gtk_button_set_label(GTK_BUTTON(app->start_button), "Start");
        }
    }

    if (app->stop_button != NULL) {
        gtk_widget_set_sensitive(app->stop_button, app->is_running);
    }
}


/* Start or reset the timer from the selected preset */
static void s_start_clicked_cb(GtkButton *button, gpointer user_data)
{
    AppState *app;
    gint active;
    const TimerPreset *preset;

    (void) button;

    app = user_data;
    active = gtk_combo_box_get_active(GTK_COMBO_BOX(app->preset_combo));
    if (active < 0) {
        active = 0;
    }

    preset = presets_get_by_index((guint) active);
    if (preset != NULL) {
        timer_start(app, preset);
    }
}


/* Stop the timer from the window */
static void s_stop_clicked_cb(GtkButton *button, gpointer user_data)
{
    AppState *app;

    (void) button;

    app = user_data;
    timer_stop(app);
}


/* Handle preset selection change */
static void s_preset_changed_cb(GtkComboBox *combo, gpointer user_data)
{
    AppState *app;
    gint active;
    const TimerPreset *preset;

    (void) combo;

    app = user_data;
    active = gtk_combo_box_get_active(GTK_COMBO_BOX(app->preset_combo));
    if (active >= 0) {
        preset = presets_get_by_index((guint) active);
        if (preset != NULL) {
            app->active_preset = preset;
            if (app->desc_label != NULL) {
                gtk_label_set_text(GTK_LABEL(app->desc_label),
                                  preset->desc);
            }
        } /* ! (preset) */
    } /* ! if (acitve) */
}


/* Hides the window instead of destroying it */
static gboolean s_window_delete_event_cb(GtkWidget *widget,
                                         GdkEvent *event,
                                         gpointer user_data)
{
    (void) event;
    (void) user_data;
  
    gtk_widget_hide(widget);

    return TRUE;
}


/* Handle key press events (ESC to hide window) */
static gboolean s_window_key_press_cb(GtkWidget *widget,
                                      GdkEventKey *event,
                                      gpointer user_data)
{
    (void) user_data;

    if (event->keyval == GDK_KEY_Escape) {
        gtk_widget_hide(widget);
        return TRUE;
    }

    return FALSE;
}


/* Show About dialog */
static void s_about_clicked_cb(GtkButton *button, gpointer user_data)
{
    AppState *app;
    GtkWidget *about_dialog;

    (void) button;

    app = user_data;

    about_dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog),
                                       APP_TITLE);
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog),
                                  APP_VERSION);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog),
                                   APP_DESCRIPTION);
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog),
                                  (const gchar *[]) {
                                      APP_AUTHOR " <" APP_AUTHOR_EMAIL ">",
                                      NULL
                                  });
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about_dialog),
                                       GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog),
                                  "https://github.com/jacorbal/gteatime");
    gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog),
                                         APP_ICON_NAME);
    gtk_window_set_transient_for(GTK_WINDOW(about_dialog),
                                  GTK_WINDOW(app->window));

    gtk_dialog_run(GTK_DIALOG(about_dialog));
    gtk_widget_destroy(about_dialog);
}
