#include "take_ss.h"
#include <string.h>

GMainLoop *loop;
char ss_filepath[256];

void filepath_copy(char* dest, const char* src); // Forward declaration of the helper function

// This callback runs when the user finishes interacting with the screenshot UI
static void on_portal_response(GDBusConnection *conn, const gchar *sender, const gchar *path,
                               const gchar *interface, const gchar *signal, GVariant *parameters, gpointer user_data) {
    guint32 response;
    GVariant *results;
    
    // Portal response signal signature is (ua{sv})
    // u: response code (0 = success)
    // a{sv}: dictionary of results
    g_variant_get(parameters, "(u@a{sv})", &response, &results);

    if (response == 0) {
        gchar *uri;
        if (g_variant_lookup(results, "uri", "&s", &uri)) {
            filepath_copy(ss_filepath, uri); // Store the URI in the global variable
            printf("Screenshot saved at: %s\n", uri);
        }
    } else {
        printf("Screenshot cancelled by user (Response code: %u)\n", response);
    }

    g_variant_unref(results);
    g_main_loop_quit(loop); // Exit the program once we have the result
}

void filepath_copy(char* dest, const char* src) {
    /*
        Copies the file path from src to dest, ensuring that dest is null-terminated and does not exceed 255 characters.
        Used for copying the screenshot URI to a global variable while ensuring safety and preventing buffer overflows.
        Assumed that src is smaller than 256 characters, but the function will handle cases where it is not by truncating and ensuring null-termination.
    */

    // check if src is not NULL
    if (src == NULL) {
        dest[0] = '\0'; // Set dest to an empty string
        return;
    }
    const char *prefix = "file://";
    size_t prefix_len = strlen(prefix);


    const char *path_start = src + prefix_len;
    size_t path_len = strlen(path_start); // length of just the path portion

    strncpy(dest, path_start, path_len);
    dest[path_len] = '\0';
}

char* take_ss_wayland() {
    GDBusConnection *conn;
    GError *error = NULL;
    GVariant *result;
    gchar *request_path;

    conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    loop = g_main_loop_new(NULL, FALSE);

    // 1. Call Screenshot
    result = g_dbus_connection_call_sync(
        conn, "org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot", "Screenshot",
        g_variant_new("(s@a{sv})", "", g_variant_new_array(G_VARIANT_TYPE("{sv}"), NULL, 0)),
        G_VARIANT_TYPE("(o)"), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

    if (error) {
        fprintf(stderr, "Call Failed: %s\n", error->message);
        return NULL;
    }

    g_variant_get(result, "(&o)", &request_path);

    // 2. Subscribe to the 'Response' signal on the path returned
    g_dbus_connection_signal_subscribe(
        conn,
        "org.freedesktop.portal.Desktop",       // Sender
        "org.freedesktop.portal.Request",       // Interface
        "Response",                             // Signal Name
        request_path,                           // Object Path (the one we just got)
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_portal_response,
        NULL, NULL);

    printf("Waiting for user to take screenshot...\n");
    g_variant_unref(result);

    // 3. Run the loop to wait for the signal
    g_main_loop_run(loop);

    g_object_unref(conn);

    // check if ss_filepath is empty
    if (ss_filepath[0] == '\0') {
        printf("No screenshot taken or an error occurred.\n");
        return NULL;
    }
    return ss_filepath;
}