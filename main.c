// main.c
#include <stdio.h>
#include <stdlib.h>
#include "automate.h"



int main() {
    int choix;
    char chemin[256];

    printf("\nQuel automate tu veux utilise?:  ");
    scanf("%d", &choix);

    sprintf(chemin, "../Automates/automate%d.txt", choix);
    Automate *AF = lire_automate_sur_fichier(chemin);

    afficher_automate(AF);
    if (est_standard(AF)) {
        printf("\n➡️ L'automate est STANDARDISE\n");
    } else {
        printf("\n➡️ L'automate n'est PAS standardise\n");
        Automate *AS = standardisation(AF);

        printf("\n===== AUTOMATE STANDARDISE =====\n");
        afficher_automate(AS);

        free(AF);
        AF = AS;
    }

    Automate *AFDC;

    if (est_deterministe(AF)) {

        printf("✔️ Automate déterministe\n");

        if (est_complet(AF)) {
            printf("✔️ Automate complet\n");
            AFDC = AF;
        } else {
            printf("❌ Automate non complet\n");
            AFDC = completion(AF);
        }

    } else {

        printf("❌ Automate non déterministe\n");
        AFDC = determinisation_et_completion_automate(AF);
    }

    afficher_automate_deterministe_complet(AFDC);

    if (AFDC != AF)
        free(AFDC);

    free(AF);
}