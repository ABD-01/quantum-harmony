#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main() {
    SYSTEMTIME currentTime;
    char startTime[10];
    int workHours = 8;
    int workMinutes = 45;
    int warningMinutes = 15;
    int startHour, startMinute, endHour, endMinute;

    GetLocalTime(&currentTime);

    printf("Current time: %02d:%02d\n", currentTime.wHour, currentTime.wMinute);
    printf("Is this your start time? (y/n): ");
    
    char confirm;
    scanf(" %c", &confirm);

    if (confirm == 'n' || confirm == 'N') {
        printf("Enter your start time (HH:MM): ");
        scanf("%s", startTime);
        sscanf(startTime, "%d:%d", &startHour, &startMinute);
    } else {
        startHour = currentTime.wHour;
        startMinute = currentTime.wMinute;
    }

    // Calculate the end time of working hours
    endHour = startHour + workHours;
    endMinute = startMinute + workMinutes - warningMinutes;

    if (endMinute < 0) {
        endHour--;
        endMinute += 60;
    }

    printf("Scheduled task will run at: %02d:%02d\n", endHour, endMinute);

    char schtaskCommand[200];

    // Create the schtasks command
    sprintf(schtaskCommand, "schtasks /create /tn stop_tasks /tr C:\\path\\to\\stop_tasks.exe /sc ONCE /st %02d:%02d", endHour, endMinute);

    // Execute the schtasks command to create the scheduled task
    // system(schtaskCommand);
    printf("%s\n", schtaskCommand);

    return 0;
}
