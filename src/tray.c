/**
 * @file tray.c
 *
 * @brief StatusNotifierItem and XEmbed tray integration
 */
/*
 * Copyright (c) 2026, J. A. Corbal.
 * All rights reserved.
 *
 * This file is licensed under the 'MIT License'.
 * Read the 'LICENSE' file in the root of this repository for details.
 */

/* Standard library includes */
#include <string.h>
#include <unistd.h>

/* GTK includes */
#include <gtk/gtk.h>

/* GDK/X11 includes */
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

/* Project includes */
#include <defs.h>
#include <timer.h>
#include <window.h>

/* Local includes */
#include <tray.h>


static const char s_sni_introspection_xml[] =
    "<node>"
    "  <interface name='org.freedesktop.DBus.Properties'>"
    "    <method name='Get'>"
    "      <arg name='interface_name' type='s' direction='in'/>"
    "      <arg name='property_name' type='s' direction='in'/>"
    "      <arg name='value' type='v' direction='out'/>"
    "    </method>"
    "    <method name='GetAll'>"
    "      <arg name='interface_name' type='s' direction='in'/>"
    "      <arg name='properties' type='a{sv}' direction='out'/>"
    "    </method>"
    "    <method name='Set'>"
    "      <arg name='interface_name' type='s' direction='in'/>"
    "      <arg name='property_name' type='s' direction='in'/>"
    "      <arg name='value' type='v' direction='in'/>"
    "    </method>"
    "    <signal name='PropertiesChanged'>"
    "      <arg name='interface_name' type='s'/>"
    "      <arg name='changed_properties' type='a{sv}'/>"
    "      <arg name='invalidated_properties' type='as'/>"
    "    </signal>"
    "  </interface>"
    "  <interface name='org.kde.StatusNotifierItem'>"
    "    <method name='ContextMenu'>"
    "      <arg name='x' type='i' direction='in'/>"
    "      <arg name='y' type='i' direction='in'/>"
    "    </method>"
    "    <method name='Activate'>"
    "      <arg name='x' type='i' direction='in'/>"
    "      <arg name='y' type='i' direction='in'/>"
    "    </method>"
    "    <method name='SecondaryActivate'>"
    "      <arg name='x' type='i' direction='in'/>"
    "      <arg name='y' type='i' direction='in'/>"
    "    </method>"
    "    <method name='Scroll'>"
    "      <arg name='delta' type='i' direction='in'/>"
    "      <arg name='orientation' type='s' direction='in'/>"
    "    </method>"
    "    <property name='Category' type='s' access='read'/>"
    "    <property name='Id' type='s' access='read'/>"
    "    <property name='Title' type='s' access='read'/>"
    "    <property name='Status' type='s' access='read'/>"
    "    <property name='WindowId' type='i' access='read'/>"
    "    <property name='IconName' type='s' access='read'/>"
    "    <property name='IconPixmap' type='a(iiay)' access='read'/>"
    "    <property name='AttentionIconName' type='s' access='read'/>"
    "    <property name='AttentionIconPixmap' type='a(iiay)' access='read'/>"
    "    <property name='AttentionMovieName' type='s' access='read'/>"
    "    <property name='ToolTip' type='(sa(iiay)ss)' access='read'/>"
    "    <property name='ItemIsMenu' type='b' access='read'/>"
    "    <property name='Menu' type='o' access='read'/>"
    "  </interface>"
    "</node>";

static GDBusNodeInfo *sni_node_info = NULL;

/**
 * @brief Handle D-Bus method calls for the StatusNotifierItem object
 *
 * @param connection     D-Bus connection receiving the call
 * @param sender         Unique bus name of the caller
 * @param object_path    Called object path
 * @param interface_name Called interface name
 * @param method_name    Called method name
 * @param parameters     Method parameters
 * @param invocation     D-Bus method invocation
 * @param user_data      AppState pointer
 */
static void s_sni_method_call_cb(GDBusConnection *connection,
                                 const gchar *sender,
                                 const gchar *object_path,
                                 const gchar *interface_name,
                                 const gchar *method_name,
                                 GVariant *parameters,
                                 GDBusMethodInvocation *invocation,
                                 gpointer user_data);

/**
 * @brief Supply StatusNotifierItem properties to GDBus
 *
 * @param connection     D-Bus connection receiving the query
 * @param sender         Unique bus name of the caller
 * @param object_path    Queried object path
 * @param interface_name Queried interface name
 * @param property_name  Queried property name
 * @param error          Output error location
 * @param user_data      AppState pointer
 *
 * @return Newly created property value, or @c NULL on error
 */
static GVariant *s_sni_get_property_cb(GDBusConnection *connection,
                                       const gchar *sender,
                                       const gchar *object_path,
                                       const gchar *interface_name,
                                       const gchar *property_name,
                                       GError **error,
                                       gpointer user_data);

/**
 * @brief Build one StatusNotifierItem property value
 *
 * @param app           Shared application state
 * @param property_name Property name
 *
 * @return Newly created property value, or @c NULL for unknown
 *         properties
 */
static GVariant *s_sni_get_property(AppState *app,
                                    const char *property_name);

/**
 * @brief Build the tray popup menu
 *
 * @param app Shared application state
 */
static void s_tray_build_menu(AppState *app);

/**
 * @brief Display the tray popup menu
 *
 * @param app Shared application state
 * @param x   Pointer x coordinate provided by the tray host
 * @param y   Pointer y coordinate provided by the tray host
 */
static void s_tray_popup_menu(AppState *app, gint x, gint y);

/**
 * @brief Start a preset selected from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_preset_cb(GtkMenuItem *item,
        gpointer user_data);

/**
 * @brief Stop the timer from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_stop_cb(GtkMenuItem *item, gpointer user_data);

/**
 * @brief Open the configuration window from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_open_cb(GtkMenuItem *item, gpointer user_data);

/**
 * @brief Quit the application from the tray menu
 *
 * @param item      Menu item that emitted the signal
 * @param user_data AppState pointer
 */
static void s_tray_menu_quit_cb(GtkMenuItem *item, gpointer user_data);

/**
 * @brief Initialize the XEmbed fallback tray icon
 *
 * @param app Shared application state
 *
 * @return @c true when the icon is docked or a dock request is sent
 */
static gboolean s_tray_xembed_init(AppState *app);

/**
 * @brief Shut down the XEmbed fallback tray icon
 *
 * @param app Shared application state
 */
static void s_tray_xembed_shutdown(AppState *app);

/**
 * @brief Update tooltip and icon for the XEmbed fallback tray icon
 *
 * @param app Shared application state
 */
static void s_tray_xembed_update(AppState *app);

/**
 * @brief Handle mouse button presses on the XEmbed tray icon
 *
 * @param widget    Event box receiving the event
 * @param event     Button press event
 * @param user_data AppState pointer
 *
 * @return @c true when the event is handled
 */
static gboolean s_tray_xembed_button_press_cb(GtkWidget *widget,
        GdkEventButton *event, gpointer user_data);

/**
 * @brief Choose the tray icon name for the current timer state
 *
 * @param app Shared application state
 *
 * @return Icon name suitable for @a gtk_image_set_from_icon_name()
 */
static const char *s_tray_icon_name(AppState *app);

/**
 * @brief Emit D-Bus property change notifications for the SNI host
 *
 * @param app Shared application state
 */
static void s_emit_properties_changed(AppState *app);

/**
 * @brief Register the exported item with the StatusNotifier watcher
 *
 * @param app Shared application state
 *
 * @return @c true when the watcher accepts the item
 */
static gboolean register_watcher(AppState *app);


/* SNI table */
static const GDBusInterfaceVTable s_sni_vtable = {
    s_sni_method_call_cb,
    s_sni_get_property_cb,
    NULL,
    { 0 }
};


/* Initialize tray integration */
gboolean tray_init(AppState *app)
{
    GError *error;
    GDBusInterfaceInfo *interface_info;
    gchar *bus_name;
    gboolean ok;

    if (app->sni_registration_id != 0 || app->xembed_ready == TRUE) {
        return TRUE;
    }

    error = NULL;
    app->bus = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (app->bus == NULL) {
        if (error != NULL) {
            g_error_free(error);
        }
        s_tray_build_menu(app);
        return s_tray_xembed_init(app);
    }

    if (sni_node_info == NULL) {
        sni_node_info = g_dbus_node_info_new_for_xml(s_sni_introspection_xml,
                                                     &error);
        if (sni_node_info == NULL) {
            if (error != NULL) {
                g_error_free(error);
            }
            s_tray_build_menu(app);
            return s_tray_xembed_init(app);
        }
    }

    interface_info = g_dbus_node_info_lookup_interface(sni_node_info,
                                                       SNI_INTERFACE);
    app->sni_registration_id = g_dbus_connection_register_object(app->bus,
                                                                 SNI_OBJECT_PATH,
                                                                 interface_info,
                                                                 &s_sni_vtable,
                                                                 app,
                                                                 NULL,
                                                                 &error);
    if (app->sni_registration_id == 0) {
        if (error != NULL) {
            g_error_free(error);
        }
        s_tray_build_menu(app);
        return s_tray_xembed_init(app);
    }

    bus_name = g_strdup_printf("org.kde.StatusNotifierItem-%lu-1",
                               (gulong) getpid());
    app->sni_bus_name = g_strdup(bus_name);
    app->sni_bus_name_id = g_bus_own_name_on_connection(app->bus,
                                                        bus_name,
                                                        G_BUS_NAME_OWNER_FLAGS_NONE,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        NULL);
    g_free(bus_name);

    s_tray_build_menu(app);
    ok = register_watcher(app);
    tray_update(app);
    if (ok == FALSE) {
        ok = s_tray_xembed_init(app);
    }

    return ok;
}


/* Shut down tray integration and release related resources */
void tray_shutdown(AppState *app)
{
    if (app->sni_registration_id != 0 && app->bus != NULL) {
        g_dbus_connection_unregister_object(app->bus,
                                            app->sni_registration_id);
        app->sni_registration_id = 0;
    }

    if (app->sni_bus_name_id != 0) {
        g_bus_unown_name(app->sni_bus_name_id);
        app->sni_bus_name_id = 0;
    }

    s_tray_xembed_shutdown(app);

    if (app->bus != NULL) {
        g_object_unref(app->bus);
        app->bus = NULL;
    }
}


/* Refresh tray properties, tooltip, and icon state */
void tray_update(AppState *app)
{
    s_emit_properties_changed(app);
    s_tray_xembed_update(app);
}


/* Register the exported item with the StatusNotifier watcher */
static gboolean register_watcher(AppState *app)
{
    GVariant *result;
    GError *error;

    error = NULL;
    result = g_dbus_connection_call_sync(app->bus,
                                         SNI_WATCHER_NAME,
                                         SNI_WATCHER_PATH,
                                         SNI_WATCHER_INTERFACE,
                                         "RegisterStatusNotifierItem",
                                         g_variant_new("(s)", SNI_OBJECT_PATH),
                                         NULL,
                                         G_DBUS_CALL_FLAGS_NONE,
                                         1000,
                                         NULL,
                                         &error);
    if (result == NULL) {
        if (error != NULL) {
            g_error_free(error);
        }
        return FALSE;
    }

    g_variant_unref(result);
    return TRUE;
}


/* Handle D-Bus method calls for the StatusNotifierItem object */
static void s_sni_method_call_cb(GDBusConnection *connection,
                                 const gchar *sender,
                                 const gchar *object_path,
                                 const gchar *interface_name,
                                 const gchar *method_name,
                                 GVariant *parameters,
                                 GDBusMethodInvocation *invocation,
                                 gpointer user_data)
{
    AppState *app;
    gint x;
    gint y;

    (void) connection;
    (void) sender;
    (void) object_path;

    app = user_data;

    if (g_strcmp0(interface_name, SNI_INTERFACE) == 0) {
        if (g_strcmp0(method_name, "Activate") == 0 ||
            g_strcmp0(method_name, "SecondaryActivate") == 0) {
            g_variant_get(parameters, "(ii)", &x, &y);
            (void) x;
            (void) y;
            window_toggle_visible(app);
            g_dbus_method_invocation_return_value(invocation, NULL);
            return;
        }

        if (g_strcmp0(method_name, "ContextMenu") == 0) {
            g_variant_get(parameters, "(ii)", &x, &y);
            s_tray_popup_menu(app, x, y);
            g_dbus_method_invocation_return_value(invocation, NULL);
            return;
        }

        if (g_strcmp0(method_name, "Scroll") == 0) {
            g_dbus_method_invocation_return_value(invocation, NULL);
            return;
        }
    }

    g_dbus_method_invocation_return_error(invocation,
                                          G_DBUS_ERROR,
                                          G_DBUS_ERROR_UNKNOWN_METHOD,
                                          "Unknown method");
}


/* Supply StatusNotifierItem properties to GDBus */
static GVariant *s_sni_get_property_cb(GDBusConnection *connection,
                                       const gchar *sender,
                                       const gchar *object_path,
                                       const gchar *interface_name,
                                       const gchar *property_name,
                                       GError **error,
                                       gpointer user_data)
{
    AppState *app;
    GVariant *value;

    (void) connection;
    (void) sender;
    (void) object_path;
    (void) interface_name;

    app = user_data;
    value = s_sni_get_property(app, property_name);
    if (value == NULL && error != NULL) {
        g_set_error(error,
                    G_DBUS_ERROR,
                    G_DBUS_ERROR_INVALID_ARGS,
                    "Unknown property");
    }

    return value;
}


/* Build one StatusNotifierItem property value */
static GVariant *s_sni_get_property(AppState *app,
        const char *property_name)
{
    GVariantBuilder pixmap_builder;
    GVariant *value;

    value = NULL;
    if (g_strcmp0(property_name, "Category") == 0) {
        value = g_variant_new_string(SNI_CATEGORY_APPLICATION_STATUS);
    } else if (g_strcmp0(property_name, "Id") == 0) {
        value = g_variant_new_string(APP_DESKTOP_ID);
    } else if (g_strcmp0(property_name, "Title") == 0) {
        value = g_variant_new_string(APP_TITLE);
    } else if (g_strcmp0(property_name, "Status") == 0) {
        value = g_variant_new_string(SNI_STATUS_ACTIVE);
    } else if (g_strcmp0(property_name, "WindowId") == 0) {
        value = g_variant_new_int32(0);
    } else if (g_strcmp0(property_name, "IconName") == 0) {
        value = g_variant_new_string(s_tray_icon_name(app));
    } else if (g_strcmp0(property_name, "AttentionIconName") == 0) {
        value = g_variant_new_string(APP_ICON_NAME);
    } else if (g_strcmp0(property_name, "AttentionMovieName") == 0) {
        value = g_variant_new_string("");
    } else if (g_strcmp0(property_name, "ItemIsMenu") == 0) {
        value = g_variant_new_boolean(FALSE);
    } else if (g_strcmp0(property_name, "Menu") == 0) {
        value = g_variant_new_object_path("/");
    } else if (g_strcmp0(property_name, "IconPixmap") == 0 ||
               g_strcmp0(property_name, "AttentionIconPixmap") == 0) {
        g_variant_builder_init(&pixmap_builder, G_VARIANT_TYPE("a(iiay)"));
        value = g_variant_builder_end(&pixmap_builder);
    } else if (g_strcmp0(property_name, "ToolTip") == 0) {
        g_variant_builder_init(&pixmap_builder, G_VARIANT_TYPE("a(iiay)"));
        value = g_variant_new("(sa(iiay)ss)",
                              s_tray_icon_name(app),
                              &pixmap_builder,
                              APP_TITLE,
                              app_state_get_tooltip(app));
    }

    return value;
}


/* Emit D-Bus property change notifications for the SNI host */
static void s_emit_properties_changed(AppState *app)
{
    GVariantBuilder changed_builder;
    GVariantBuilder invalidated_builder;
    GVariant *tooltip;
    GVariant *icon_name;
    GVariant *status;

    if (app->bus == NULL) {
        return;
    }

    tooltip = s_sni_get_property(app, "ToolTip");
    icon_name = s_sni_get_property(app, "IconName");
    status = s_sni_get_property(app, "Status");
    g_variant_builder_init(&changed_builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&changed_builder, "{sv}", "ToolTip", tooltip);
    g_variant_builder_add(&changed_builder, "{sv}", "IconName", icon_name);
    g_variant_builder_add(&changed_builder, "{sv}", "Status", status);
    g_variant_builder_init(&invalidated_builder, G_VARIANT_TYPE("as"));

    g_dbus_connection_emit_signal(app->bus,
                                  NULL,
                                  SNI_OBJECT_PATH,
                                  DBUS_PROPERTIES_INTERFACE,
                                  "PropertiesChanged",
                                  g_variant_new("(sa{sv}as)",
                                                SNI_INTERFACE,
                                                &changed_builder,
                                                &invalidated_builder),
                                  NULL);
}


/* Build the tray popup menu */
static void s_tray_build_menu(AppState *app)
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

    item = gtk_menu_item_new_with_label("Open configuration");
    g_signal_connect(item, "activate", G_CALLBACK(s_tray_menu_open_cb), app);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    item = gtk_menu_item_new_with_label("Stop timer");
    g_signal_connect(item, "activate", G_CALLBACK(s_tray_menu_stop_cb), app);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(item, "activate", G_CALLBACK(s_tray_menu_quit_cb), app);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    gtk_widget_show_all(menu);
    app->tray_menu = menu;
}


/* Display the tray popup menu */
static void s_tray_popup_menu(AppState *app, gint x, gint y)
{
    if (app->tray_menu == NULL) {
        s_tray_build_menu(app);
    }

    (void) x;
    (void) y;

    gtk_menu_popup_at_pointer(GTK_MENU(app->tray_menu), NULL);
}


/* Initializes the XEmbed fallback tray icon */
static gboolean s_tray_xembed_init(AppState *app)
{
#ifdef GDK_WINDOWING_X11
    GdkDisplay *gdk_display;
    GdkScreen *screen;
    GdkWindow *gdk_window;
    Display *display;
    Window manager_window;
    Window icon_window;
    XClientMessageEvent event;
    Atom selection_atom;
    Atom opcode_atom;
    char selection_name[64];
    int screen_number;
    const char *icon_name;

    gdk_display = gdk_display_get_default();
    if (gdk_display == NULL) {
        return FALSE;
    }

    if (GDK_IS_X11_DISPLAY(gdk_display) == FALSE) {
        return FALSE;
    }

    screen = gdk_display_get_default_screen(gdk_display);
    screen_number = gdk_x11_screen_get_screen_number(screen);
    display = gdk_x11_display_get_xdisplay(gdk_display);

    g_snprintf(selection_name,
               sizeof(selection_name),
               XEMBED_SYSTEM_TRAY_SELECTION_FORMAT,
               screen_number);
    selection_atom = XInternAtom(display, selection_name, False);
    manager_window = XGetSelectionOwner(display, selection_atom);
    if (manager_window == None) {
        return FALSE;
    }

    app->xembed_window = gtk_window_new(GTK_WINDOW_POPUP);
    icon_name = s_tray_icon_name(app);
    gtk_window_set_title(GTK_WINDOW(app->xembed_window), APP_TITLE);
    gtk_window_set_icon_name(GTK_WINDOW(app->xembed_window), icon_name);
    gtk_widget_set_size_request(app->xembed_window, 24, 24);
    gtk_widget_set_tooltip_text(app->xembed_window, app_state_get_tooltip(app));

    app->xembed_event_box = gtk_event_box_new();
    gtk_widget_add_events(app->xembed_event_box,
                          GDK_BUTTON_PRESS_MASK |
                          GDK_ENTER_NOTIFY_MASK |
                          GDK_LEAVE_NOTIFY_MASK);
    gtk_container_add(GTK_CONTAINER(app->xembed_window),
                      app->xembed_event_box);
    app->xembed_image =
        gtk_image_new_from_icon_name(icon_name,
                GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(app->xembed_event_box),
                      app->xembed_image);
    g_signal_connect(app->xembed_event_box,
                     "button-press-event",
                     G_CALLBACK(s_tray_xembed_button_press_cb),
                     app);

    gtk_widget_show_all(app->xembed_window);
    gtk_widget_realize(app->xembed_window);

    gdk_window = gtk_widget_get_window(app->xembed_window);
    if (gdk_window == NULL) {
        gtk_widget_destroy(app->xembed_window);
        app->xembed_window = NULL;
        app->xembed_event_box = NULL;
        app->xembed_image = NULL;
        return FALSE;
    }

    icon_window = gdk_x11_window_get_xid(gdk_window);
    opcode_atom = XInternAtom(display, XEMBED_SYSTEM_TRAY_OPCODE, False);

    memset(&event, 0, sizeof(event));
    event.type = ClientMessage;
    event.window = manager_window;
    event.message_type = opcode_atom;
    event.format = 32;
    event.data.l[0] = CurrentTime;
    event.data.l[1] = XEMBED_SYSTEM_TRAY_REQUEST_DOCK;
    event.data.l[2] = icon_window;
    event.data.l[3] = 0;
    event.data.l[4] = 0;

    XSendEvent(display,
               manager_window,
               False,
               NoEventMask,
               (XEvent *)&event);
    XSync(display, False);

    app->xembed_ready = TRUE;
    return TRUE;
#else
    (void) app;
    return FALSE;
#endif
}


/* Shut down the XEmbed fallback tray icon */
static void s_tray_xembed_shutdown(AppState *app)
{
    if (app->xembed_window != NULL) {
        gtk_widget_destroy(app->xembed_window);
        app->xembed_window = NULL;
        app->xembed_event_box = NULL;
        app->xembed_image = NULL;
    }

    app->xembed_ready = FALSE;
}


/* Update tooltip and icon for the XEmbed fallback tray icon */
static void s_tray_xembed_update(AppState *app)
{
    if (app->xembed_window != NULL) {
        gtk_widget_set_tooltip_text(app->xembed_window,
                                    app_state_get_tooltip(app));
    }

    if (app->xembed_event_box != NULL) {
        gtk_widget_set_tooltip_text(app->xembed_event_box,
                                    app_state_get_tooltip(app));
    }

    if (app->xembed_image != NULL) {
        gtk_image_set_from_icon_name(GTK_IMAGE(app->xembed_image),
                                     s_tray_icon_name(app),
                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
    }
}


/* Handle mouse button presses on the XEmbed tray icon */
static gboolean s_tray_xembed_button_press_cb(GtkWidget *widget,
        GdkEventButton *event, gpointer user_data)
{
    AppState *app;

    (void) widget;

    app = user_data;
    if (event->button == 1) {
        window_toggle_visible(app);
        return TRUE;
    }

    if (event->button == 3) {
        s_tray_popup_menu(app, (gint) event->x_root, (gint) event->y_root);
        return TRUE;
    }

    return FALSE;
}


/* Choose the tray icon name for the current timer state */
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


/* Start a preset selected from the tray menu */
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


/* Stop the timer from the tray menu */
static void s_tray_menu_stop_cb(GtkMenuItem *item, gpointer user_data)
{
    AppState *app;

    (void) item;

    app = user_data;
    timer_stop(app);
}


/* Open the configuration window from the tray menu */
static void s_tray_menu_open_cb(GtkMenuItem *item, gpointer user_data)
{
    AppState *app;

    (void) item;

    app = user_data;
    window_show(app);
}


/* Quit the application from the tray menu */
static void s_tray_menu_quit_cb(GtkMenuItem *item, gpointer user_data)
{
    AppState *app;

    (void) item;

    app = user_data;
    g_application_quit(G_APPLICATION(app->gtk_app));
}
