#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>

// Function to show a warning dialog
void ShowWarningDialog() {
    GtkWidget *dialog;

    gtk_init(NULL, NULL);

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
        "Your working time is about to end.");

    gtk_window_set_title(GTK_WINDOW(dialog), "Warning");

    gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    gtk_main_quit();
}

int main()
{ 
  int hours, minutes;
  printf("Enter the time (HH:MM): ");
  scanf("%d:%d", &hours, &minutes);

  printf("You entered: %02d:%02d\n", hours, minutes);

  ShowWarningDialog();

  system("C:/Users/Muhammed/projects/wp/main.exe");
  return 0;
}

// ref: https://stackoverflow.com/a/39266950
// gcc -o main.exe main.c `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`