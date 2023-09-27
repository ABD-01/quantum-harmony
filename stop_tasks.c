#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

gboolean UpdateTimer(GtkLabel *timerLabel);

// Function to show a warning dialog
void ShowWarningDialog() {
    GtkWidget *dialog;

    gtk_init(NULL, NULL);

    // Create a dialog with a message and OK button
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Your working time is about to end.");
    
    // Set dialog window size to match the screen width (1366)
    GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(dialog));
    int screenWidth = gdk_screen_get_width(screen);
    int screenHeight = gdk_screen_get_height(screen);
    gtk_window_set_default_size(GTK_WINDOW(dialog), screenWidth, screenHeight/3);

    // Set the dialog color to Windows blue (#357EC7)
    GdkRGBA color;
    gdk_rgba_parse(&color, "#357EC7");
    gtk_widget_override_background_color(GTK_WIDGET(dialog), GTK_STATE_NORMAL, &color);

    // Set the dialog's window type hint to resemble a system message
    gtk_window_set_type_hint(GTK_WINDOW(dialog), GDK_WINDOW_TYPE_HINT_NOTIFICATION);

    // Set dialog title and content area text
    gtk_window_set_title(GTK_WINDOW(dialog), "Warning");

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
        gtk_window_move(GTK_WINDOW(timerWindow), screenWidth - 150, 0);

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
        // system("taskkill /F /IM msedge.exe");
        // system("taskkill /F /IM Code.exe");
        printf("taskkill /F /IM msedge.exe\n");
        printf("taskkill /F /IM Code.exe\n");
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

int main() {
    // Show a warning dialog
    ShowWarningDialog();

    return 0;
}
