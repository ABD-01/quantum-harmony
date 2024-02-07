#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <gtk/gtk.h>

gboolean UpdateTimer(GtkLabel *timerLabel);

// Function to show a warning dialog
void ShowWarningDialog() {
    GtkWidget *warningWindow;
    GdkScreen *screen;

    gtk_init(NULL, NULL);

    // Get the screen
    screen = gdk_screen_get_default();

    // Set dialog window size to match the screen width and height (1/3 of screen height)
    int screenWidth = gdk_screen_get_width(screen);
    int screenHeight = gdk_screen_get_height(screen);

    // Create a transient window (to appear as a popup)
    warningWindow = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(warningWindow), screenWidth, (int)(screenHeight / 4));
    gtk_window_move(GTK_WINDOW(warningWindow), 0, (int)(screenHeight / 3));

    // Increase font size
    GtkWidget *warningLabel = gtk_label_new("A quick brown fox jumped over the lazy dog.");
    PangoFontDescription *font_desc = pango_font_description_from_string("Verdana Bold 32");

    // Create a label with the warning message
    gtk_widget_override_font(warningLabel, font_desc);

    // Set the dialog color to Windows blue (#357EC7) using CSS
    const gchar *css = "* { background-color: #357EC7; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Add the fixed container to the window
    gtk_container_add(GTK_CONTAINER(warningWindow), warningLabel);

    // Show the warning window
    gtk_widget_show_all(warningWindow);

    // Add a timer to close the warning window after 3 seconds
    g_timeout_add_seconds(3, (GSourceFunc)gtk_widget_destroy, warningWindow);

    // Start the GTK main loop for the warning window
    // gtk_main();
}

void ShowTimerWindow() {
    GtkWidget *timerWindow;
    GdkScreen *screen;

    // Create a new GTK application context for the timer window
    gtk_init(NULL, NULL);

    // Get the screen
    screen = gdk_screen_get_default();

    timerWindow = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(timerWindow), 100, 50);
    // Set initial position of the timer window (top left corner)
    gtk_window_move(GTK_WINDOW(timerWindow), 50, 100);

    GtkWidget *timerLabel = gtk_label_new("15:00"); // Initial time (15 minutes)

    gtk_container_add(GTK_CONTAINER(timerWindow), timerLabel);

    // Set the background color of the window to blue
    const gchar *css = "* { background-color: #357EC7; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Show the timer window
    gtk_widget_show_all(timerWindow);

    g_timeout_add_seconds(1, (GSourceFunc)UpdateTimer, timerLabel);

    // Start the GTK main loop for the timer window
    // gtk_main();
}

gboolean UpdateTimer(GtkLabel *timerLabel) {
    static int minutes = 15;
    static int seconds = 0;

    seconds--;

    if (seconds < 0) {
        minutes--;
        seconds = 59;
    }

    if (minutes < 0) {
        // Terminate specified applications (e.g., Edge, VSCode, etc.)
        system("taskkill /F /IM msedge.exe");
        system("taskkill /F /IM Code.exe");
        system("taskkill /F /IM mcuxpressoide.exe");
        system("taskkill /F /IM notepad++.exe");
        system("taskkill /F /IM Acrobat.exe");
        system("taskkill /F /IM nvim.exe");
        system("taskkill /F /IM WindowsTerminal.exe");
        system("shutdown /s /t 005");
        // Add more applications as needed

        // Close the timer window
        gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(timerLabel)));
        gtk_main_quit();
    } else {
        char timerText[6];
        snprintf(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);

        gtk_label_set_text(timerLabel, timerText);

        return TRUE;
    }

    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Show a warning dialog
    ShowWarningDialog();

    ShowTimerWindow();

    gtk_main();

    return 0;
}
