#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <gtk/gtk.h>

gboolean UpdateTimer(GtkLabel *timerLabel);

// Function to show a warning dialog
void ShowWarningDialog() {
    GtkWidget *dialog;
    GdkScreen *screen;

    gtk_init(NULL, NULL);

    // Get the screen
    screen = gdk_screen_get_default();

    // Create a dialog with a message and OK button
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Your working time is about to end.");

    // Set dialog window size to match the screen width and height (1/3 of screen height)
    int screenWidth = gdk_screen_get_width(screen);
    int screenHeight = gdk_screen_get_height(screen);
    gtk_window_set_default_size(GTK_WINDOW(dialog), screenWidth, (int)(screenHeight / 3));

    // Increase font size
    GtkLabel *label = GTK_LABEL(gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(dialog)));
    PangoFontDescription *font_desc = pango_font_description_from_string("Verdana Bold 32");
    gtk_widget_override_font(GTK_WIDGET(label), font_desc);

    // Set the dialog's window type hint to resemble a system message
    gtk_window_set_type_hint(GTK_WINDOW(dialog), GDK_WINDOW_TYPE_HINT_NOTIFICATION);

    // Set dialog title and content area text
    gtk_window_set_title(GTK_WINDOW(dialog), "Warning");

    // Set the dialog color to Windows blue (#357EC7) using CSS
    const gchar *css = "* { background-color: #357EC7; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Show the dialog and wait for user response
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    // Close the dialog
    gtk_widget_destroy(dialog);

    gtk_main_quit();

    // If the OK button is clicked, show a timer window in the top right corner
    if (result == GTK_RESPONSE_OK) {
        GtkWidget *timerWindow;

        gtk_init(NULL, NULL);

        timerWindow = gtk_window_new(GTK_WINDOW_POPUP);
        gtk_window_set_default_size(GTK_WINDOW(timerWindow), 100, 50);
        // Set initial position of the timer window (top left corner)
        gtk_window_move(GTK_WINDOW(timerWindow), 50, 100);

        GtkWidget *timerLabel = gtk_label_new("15:00"); // Initial time (15 minutes)

        gtk_container_add(GTK_CONTAINER(timerWindow), timerLabel);

        gtk_widget_show_all(timerWindow);

        g_timeout_add_seconds(1, (GSourceFunc)UpdateTimer, timerLabel);

        gtk_main();
    }
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

// int main() {
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Show a warning dialog
    ShowWarningDialog();

    return 0;
}
