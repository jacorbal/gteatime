/**
 * @file notify.c
 *
 * @brief Desktop notification implementation using the notification
 *        D-Bus API
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

/* Project includes */
#include <defs.h>

/* Local includes */
#include <notify.h>


/* Send a desktop notification for a completed timer */
void notify_timer_finished(AppState *app, const char *preset_name)
{
    GVariantBuilder actions_builder;
    GVariantBuilder hints_builder;
    GError *error;
    gchar *body;

    error = NULL;
    body = g_strdup_printf("%s is ready.", preset_name);

    if (app->bus == NULL) {
        g_free(body);
        return;
    }

    g_variant_builder_init(&actions_builder, G_VARIANT_TYPE("as"));
    g_variant_builder_init(&hints_builder, G_VARIANT_TYPE("a{sv}"));

    g_dbus_connection_call(app->bus,
                           "org.freedesktop.Notifications",
                           "/org/freedesktop/Notifications",
                           "org.freedesktop.Notifications",
                           "Notify",
                           g_variant_new("(susssasa{sv}i)",
                                         APP_TITLE,
                                         0,
                                         APP_ICON_NAME,
                                         "Timer finished",
                                         body,
                                         &actions_builder,
                                         &hints_builder,
                                         5000),
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           NULL,
                           NULL);

    if (error != NULL) {
        g_error_free(error);
    }

    g_free(body);
}
