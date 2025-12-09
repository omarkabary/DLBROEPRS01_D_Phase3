#include <stdio.h>
#include <string.h>

/*
  PC-Client (UART Simulation)

  -> Hier tue ich so, als ob ich mit dem Arduino rede.
  -> Eigentlich würde der PC den Ausdruck an den Arduino schicken.
  -> Da ich aber keinen echten Arduino am PC habe,
     rechne ich einfach hier im Client selbst.
  -> Das Ergebnis gebe ich dann im gleichen Format aus,
     wie es der Arduino auch machen würde.
*/

// kleine Funktion um das '\n' am Ende zu entfernen
void trimNewline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

/*
  Diese Funktion versucht einen Ausdruck wie "7+5" auszulesen.
  Ich teste zwei Varianten:
  1) mit Leerzeichen (z.B. "7 + 5")
  2) ohne Leerzeichen ("7+5")
  Wenn es klappt, gibt die Funktion 1 zurück.
*/
int parse_expression(const char *input, double *a, char *op, double *b) {

    if (sscanf(input, "%lf %c %lf", a, op, b) == 3) {
        return 1;
    }

    if (sscanf(input, "%lf%c%lf", a, op, b) == 3) {
        return 1;
    }

    return 0; // konnte nicht gelesen werden
}

/*
  Hier berechne ich das Ergebnis.
  Diese Funktion ist quasi das "Rechen-Modul".
  Wichtig: Division durch 0 prüfe ich extra.
*/
int calculate(double a, char op, double b, double *result) {

    switch (op) {
        case '+':
            *result = a + b;
            return 1;

        case '-':
            *result = a - b;
            return 1;

        case '*':
            *result = a * b;
            return 1;

        case '/':
            if (b == 0) {
                printf("Fehler: Division durch 0.\n");
                return 0;
            }
            *result = a / b;
            return 1;

        default:
            printf("Unbekannter Operator: %c\n", op);
            return 0;
    }
}

/*
  Diese Funktion soll zeigen, wie der Arduino antworten würde.
  In echt würde die Antwort über UART kommen.
  Hier mache ich es einfach lokal.
*/
void simulateArduinoResponse(const char *expr) {

    double a, b, result;
    char op;

    // zuerst Ausdruck verstehen
    if (!parse_expression(expr, &a, &op, &b)) {
        printf("\n--- Antwort vom Arduino (simuliert) ---\n");
        printf("ERROR: Ausdruck konnte nicht gelesen werden (%s)\n", expr);
        printf("---------------------------------------\n\n");
        return;
    }

    // dann rechnen
    if (!calculate(a, op, b, &result)) {
        printf("\n--- Antwort vom Arduino (simuliert) ---\n");
        printf("ERROR: Berechnung nicht möglich\n");
        printf("---------------------------------------\n\n");
        return;
    }

    // hier simuliere ich die "Antwort" vom Arduino
    printf("\n--- Antwort vom Arduino (simuliert) ---\n");
    printf("%s\n", expr);
    printf("Ergebnis = %.2f\n", result);
    printf("---------------------------------------\n\n");
}

/*
  Log-Funktion:
  Ich schreibe jede Eingabe vom Benutzer in eine Datei,
  damit ich später sehen kann, was eingegeben wurde.
*/
void writeLog(const char *text) {
    FILE *log = fopen("client_log.txt", "a");
    if (!log) {
        printf("Konnte Logdatei nicht öffnen.\n");
        return;
    }
    fprintf(log, "%s\n", text);
    fclose(log);
}

/*
  Hauptschleife:
  - Benutzer gibt etwas ein
  - wenn "exit", dann beenden
  - sonst Ausdruck speichern + simulieren
*/
int main() {

    char input[100];

    printf("PC-Client (UART Simulation)\n");
    printf("Geben Sie eine Rechnung ein (oder 'exit' zum Beenden):\n\n");

    while (1) {

        printf("Eingabe: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        trimNewline(input); // Enter entfernen

        if (strcmp(input, "exit") == 0) {
            printf("Client wird beendet.\n");
            break;
        }

        if (strlen(input) == 0) {
            printf("Bitte etwas eingeben.\n");
            continue;
        }

        writeLog(input);          // Eingabe speichern
        simulateArduinoResponse(input); // "Arduino" antworten lassen
    }

    return 0;
}
