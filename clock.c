#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <string.h>

struct alarmInfo info;
char date[20];
int hour, min, sec;
char am_pm[3] = ""; // Initialize with an empty string
int showHelp = 0;
int clockType = 0; // 0 for 24h, 1 for 12h
int needAlarm = 0;
int haveAlarm = 0;

// =============================================================
// =                     AlarmInfo struct                      =
// =============================================================

struct alarmInfo
{
    int alarmHour;
    int alarmMin;
    char clockType[3];
    char alarmMessage[100];
    int isSet;
};

// =============================================================
// =                   Initialising Functions                  =
// =============================================================
void currentDateTime(int *clockType, char *dateBuffer, int *hour, int *min, int *sec, char *am_pm);
void helpMenu();
void messageContent(int *alarmHour, int *alarmMin, char clockType[3]);
void confirmingAlarm(struct alarmInfo info);
void setAlarm12();
void setAlarm24();
void setAlarm(int* clockType);
int userInput(char* key, int* showHelp, int* clockType, int* needAlarm, int* haveAlarm);
void delAlarm(int* haveAlarm);

// =============================================================
// =                        Main Function                      =
// =============================================================
int main() {
    while (1) {
        // clear console
        system("cls");

        // setting alarm
        if (needAlarm) {
            setAlarm(&clockType);
            needAlarm = 0;
            haveAlarm = 1;
        }
        // replace to help menu
        else if (showHelp) {
            helpMenu();
        }

        // time
        else {
            // Get current date and time
            currentDateTime(&clockType, date, &hour, &min, &sec, am_pm);
            printf("Press 'h' for menu :D\n");
            printf("%s", date);

            // Print time based on clock type
            if (clockType == 0) {
                printf("Time: %02d:%02d:%02d\n", hour, min, sec);
            }
            else {
                printf("Time: %02d:%02d:%02d %s\n", hour, min, sec, am_pm);
            }

            if (info.isSet == 1) {
                if (info.alarmHour == hour && info.alarmMin == min) {
                    printf("ALARM: %s", info.alarmMessage);
                }
                if (strcmp(info.clockType, "XX") == 0) {
                    printf("\n(24 hour)\nAlarm: %02d:%02d\nMessage: %s\n", info.alarmHour, info.alarmMin, info.alarmMessage);
                }
                else {
                    printf("\n(12 hour)\nAlarm: %02d:%02d %s\nMessage: %s\n", info.alarmHour, info.alarmMin, info.clockType, info.alarmMessage);
                }
            }
            else {
                printf("\nNo alarm set. 'a' to set\n");
            }
        }

        // check if a key has been pressed
        if (_kbhit()) {
            // read the input character
            char key = _getch();
            userInput(&key, &showHelp, &clockType, &needAlarm, &haveAlarm);
        }

        // update every second (1000)
        Sleep(100);
    }
}

// =============================================================
// =                      currentDateTime                      =
// =============================================================
void currentDateTime(int *clockType, char *dateBuffer, int *hour, int *min, int *sec, char *am_pm) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Store the date in dateBuffer
    sprintf(dateBuffer, "Date: %d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    switch (*clockType) {
    // 24-hour clock
    case 0:
        *hour = tm.tm_hour;
        *min = tm.tm_min;
        *sec = tm.tm_sec;
        *am_pm = '\0'; // No AM/PM for 24-hour clock
        break;

    // 12-hour clock
    case 1:
        *hour = tm.tm_hour;
        if (*hour >= 12) {
            strcpy(am_pm, "PM");
            if (*hour > 12) {
                *hour -= 12;
            }
        }
        else {
            strcpy(am_pm, "AM");
            if (*hour == 0) {
                *hour = 12; // midnight
            }
        }
        *min = tm.tm_min;
        *sec = tm.tm_sec;
        break;
    }
}

// =============================================================
// =                         helpMenu                          =
// =============================================================
void helpMenu() {
    printf("Menu:\n");
    printf("h -> toggle help menu\n");
    printf("q -> exit clock\n");
    printf("c -> toggle 24h/12h\n");
    printf("a -> set alarm\n");
    printf("d -> remove alarm\n");
}

// =============================================================
// =                    confirmingAlarm                        =
// =============================================================
void confirmingAlarm(struct alarmInfo info) {
    // clear console
    system("cls");
    if (strcmp(info.clockType, "XX") != 0) {
        printf("Alarm set: %02d:%02d %s", info.alarmHour, info.alarmMin, info.clockType);
    }
    else {
        printf("Alarm set: %02d:%02d", info.alarmHour, info.alarmMin);
    }
    printf("\nMessage: %s\n", info.alarmMessage);
    printf("Setting Alarm...\n");

    Sleep(2500);
}

// =============================================================
// =                    messageContent                         =
// =============================================================
void messageContent(int *alarmHour, int *alarmMin, char clockType[3]) {
    system("cls");
    char alarmMessage[100];
    printf("[ENTER] to continue :D\n");
    printf("Alarm Message: ");
    scanf("%99s", alarmMessage);

    // Check if the message is too long
    if (strlen(alarmMessage) >= 100) {
        printf("Error: The message is too long! Please enter a shorter message.\n");
    }
    else {
        printf("Setting alarm...");
        info.alarmHour = *alarmHour;
        info.alarmMin = *alarmMin;
        strcpy(info.clockType, clockType);
        strcpy(info.alarmMessage, alarmMessage);
        info.isSet = 1;
        confirmingAlarm(info);
    }
}

// =============================================================
// =                       setAlarm12                          =
// =============================================================
void setAlarm12() {
    int alarmHour = 0;
    int alarmMin = 0;
    char clockType[3] = "AM";
    int confirmAlarm = 0;
    int place = 0;

    // blinking effect
    int blinkInterval = 500;
    DWORD lastBlinkTime = GetTickCount();
    int showHour = 1; // show, 0=hide
    int showMin = 1;
    int showType = 1;

    while (!confirmAlarm) {
        // clear console
        system("cls");

        printf("Press 'a' to stop, arrow buttons to move, [ENTER] to set alarm\n");

        // display time (blinking effect)
        if (place == 0) {
            if (showHour) {
                printf("%02d:%02d %s\n", alarmHour, alarmMin, clockType);
                printf("Editing Hour...\n");
            }
            else {
                printf("--:%02d %s\n", alarmMin, clockType);
                printf("Editing Hour...\n");
            }
        }
        else if (place == 1) {
            if (showMin) {
                printf("%02d:%02d %s\n", alarmHour, alarmMin, clockType);
                printf("Editing Minutes...\n");
            }
            else {
                printf("%02d:-- %s\n", alarmHour, clockType);
                printf("Editing Minutes...\n");
            }
        }
        else {
            if (showType) {
                printf("%02d:%02d %s\n", alarmHour, alarmMin, clockType);
                printf("Editing Clock...\n");
            }
            else {
                printf("%02d:%02d --\n", alarmHour, alarmMin);
                printf("Editing Clock...\n");
            }
        }

        // handle input
        if (_kbhit()) {
            // get input
            char key = _getch();
            switch (key) {
            case 'a':
                confirmAlarm = 1;
                break;

            case 13:
                confirmAlarm = 1;
                messageContent(&alarmHour, &alarmMin, clockType);
                break;

            case 'q':
                system("cls");
                printf("Exiting time0ut clock...");
                exit(1);

            case 'c':
                system("cls");
                setAlarm24();
                return;

            // up arrow
            case 0x48: {
                if (place == 0) {
                    if (alarmHour == 11 && strcmp(clockType, "PM") == 0) {
                        alarmHour = 0;
                        strcpy(clockType, "AM");
                    }
                    else if (alarmHour == 11 && strcmp(clockType, "AM") == 0) {
                        alarmHour = 12;
                        strcpy(clockType, "PM");
                    }
                    else {
                        alarmHour = (alarmHour + 1) % 12;
                    }
                }
                else if (place == 1) {
                    {
                        if (alarmMin == 59) {
                            alarmMin = 0;
                        }
                        else {
                            alarmMin += 1 % 60;
                        }
                    }
                }
                else {
                    if (alarmHour == 0 && strcmp(clockType, "AM") == 0) {
                        alarmHour = 12;
                        strcpy(clockType, "PM");
                    }
                    else if (alarmHour == 12 && strcmp(clockType, "PM") == 0) {
                        alarmHour = 0;
                        strcpy(clockType, "AM");
                    }
                    else if (strcmp(clockType, "PM") == 0) {
                        strcpy(clockType, "AM");
                    }
                    else {
                        strcpy(clockType, "PM");
                    }
                }
                break;
            }

            // down arrow
            case 0x50:
                if (place == 0) {
                    if (alarmHour == 0 && strcmp(clockType, "AM") == 0) {
                        alarmHour = 11;
                        strcpy(clockType, "PM");
                    }
                    else if (alarmHour == 12 && strcmp(clockType, "PM") == 0) {
                        alarmHour = 11;
                        strcpy(clockType, "AM");
                    }
                    else if (alarmHour == 1 && strcmp(clockType, "PM") == 0) {
                        alarmHour = 11;
                        strcpy(clockType, "AM");
                    }
                    else if (alarmHour == 0 && strcmp(clockType, "AM") == 0) {
                        alarmHour = 12;
                        strcpy(clockType, "PM");
                    }
                    else {
                        alarmHour = (alarmHour - 1) % 12;                        
                    }
                }
                else if (place == 1) {
                    {   
                        if (alarmMin == 0) {
                            alarmMin = 59;
                        }
                        else {
                            alarmMin -= 1;
                        }
                    }
                }
                else {
                    if (alarmHour == 0 && strcmp(clockType, "AM") == 0) {
                        alarmHour = 12;
                        strcpy(clockType, "PM");
                    }
                    else if (alarmHour == 12 && strcmp(clockType, "PM") == 0) {
                        alarmHour = 0;
                        strcpy(clockType, "AM");
                    }
                    else if (strcmp(clockType, "PM") == 0) {
                        strcpy(clockType, "AM");
                    }
                    else {
                        strcpy(clockType, "PM");
                    }
                }
                break;

            // left arrow
            case 0x4B:
                if (place != 0) {
                    place -= 1;
                }
                break;

            // right arrow
            case 0x4D:
                if (place != 2) {
                    place += 1;
                }
                break;
            }
        }

        // if it's time to blink
        DWORD currentTime = GetTickCount();
        if (currentTime - lastBlinkTime > blinkInterval) {
            lastBlinkTime = currentTime;
            if (place == 0) {
                showHour = !showHour; // blink hour
            }
            else if (place == 1) {
                showMin = !showMin; // blink min
            }
            else {
                showType = !showType; // blink type
            }
        }

        // every 100 ms
        Sleep(100);
    }
}

// =============================================================
// =                       setAlarm24                          =
// =============================================================
void setAlarm24() {
    int alarmHour = 0;
    int alarmMin = 0;
    int confirmAlarm = 0;
    int place = 0;

    // blinking effect
    int blinkInterval = 500;
    DWORD lastBlinkTime = GetTickCount();
    int showHour = 1; // show, 0=hide
    int showMin = 1;

    while (!confirmAlarm) {
        // clear console
        system("cls");

        printf("Press 'a' to stop, arrow buttons to move, [ENTER] to set alarm\n");

        // display time (blinking effect)
        if (place == 0) {
            if (showHour) {
                printf("%02d:%02d\n", alarmHour, alarmMin);
                printf("Editing Hour...\n");
            }
            else {
                printf("--:%02d\n", alarmMin);
                printf("Editing Hour...\n");
            }
        }
        else {
            if (showMin) {
                printf("%02d:%02d\n", alarmHour, alarmMin);
                printf("Editing Minutes...\n");
            }
            else {
                printf("%02d:--\n", alarmHour);
                printf("Editing Minutes...\n");
            }
        }

        // handle input
        if (_kbhit()) {
            // get input
            char key = _getch();
            switch (key) {
            case 'a':
                confirmAlarm = 1;
                break;

            case 13:
                confirmAlarm = 1;
                char clockType[3] = "XX";
                messageContent(&alarmHour, &alarmMin, clockType);
                break;

            case 'q':
                system("cls");
                printf("Exiting time0ut clock...");
                exit(1);

            case 'c':
                system("cls");
                setAlarm12();
                return;

            // up arrow
            case 0x48: {
                if (place == 0) {
                    alarmHour = (alarmHour + 1) % 24;
                }
                else {
                    if (alarmMin == 59) {
                        alarmMin = 0;
                    }
                    else {
                        alarmMin += 1;
                    }
                }
                break;
            }

            // down arrow
            case 0x50: {
                if (place == 0) {
                    alarmHour = (alarmHour - 1 + 24) % 24;
                }
                else {
                    if (alarmMin == 0) {
                        alarmMin = 59;
                    }
                    else {
                        alarmMin -= 1;
                    }
                }
                break;
            }

            // left arrow
            case 0x4B:
                place = 0; // to hour
                break;

            // right arrow
            case 0x4D:
                place = 1; // to min
                break;
            }
        }

        // if it's time to blink
        DWORD currentTime = GetTickCount();
        if (currentTime - lastBlinkTime > blinkInterval) {
            lastBlinkTime = currentTime;
            if (place == 0) {
                showHour = !showHour; // blink hour
            }
            else {
                showMin = !showMin; // blink min
            }
        }

        // every 100 ms
        Sleep(100);
    }
}

// =============================================================
// =                         setAlarm                          =
// =============================================================
void setAlarm(int *clockType) {   
    switch (*clockType) {

    // 12 hour clock
    case 0:
        setAlarm24();
        break;

    // 24 hour clock
    case 1:
        setAlarm12();
        break;
    }
}

// =============================================================
// =                         delAlarm                          =
// =============================================================

void delAlarm(int* haveAlarm)  {
    system("cls");
    *haveAlarm = !(*haveAlarm);
    printf("Removed Alarm!");
    memset(&info, 0, sizeof(info));
    Sleep(1200);
}

// =============================================================
// =                         userInput                         =
// =============================================================
int userInput(char *key, int *showHelp, int *clockType, int *needAlarm, int* haveAlarm) {
    // determine result
    switch (*key) {
    // exit
    case 'q':
        system("cls");
        printf("Exiting time0ut clock...");
        exit(1);

    // help menu
    case 'h':
        *showHelp = !(*showHelp);
        break;

    // change clock type
    case 'c':
        *clockType = !(*clockType);
        break;

    // set alarm
    case 'a':
        *needAlarm = !(*needAlarm);
        break;

    // delete alarm
    case 'd':
        if (*haveAlarm == 1) {
            delAlarm(haveAlarm);
        }
        else {
            system("cls");
            printf("No alarm set!");
            Sleep(1200);
        }
        break;

    // other input
    default:
        printf("no such command");
    }
}
