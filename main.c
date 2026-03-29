// main.c
#include <stdio.h>
#include <stdlib.h>
#include "automate.h"

int main() {
    int choix;
    char chemin[256];

    printf("\nQuel automate tu veux utiliser?:  ");
    scanf("%d", &choix);

    sprintf(chemin, "../Automates/automate%d.txt", choix);
    Automate *AF = lire_automate_sur_fichier(chemin);

    afficher_automate(AF);
    if (est_standard(AF)) {
        printf("\nL'automate est STANDARDISE\n");
    } else {
        printf("\n L'automate n'est PAS standardise\n");
        Automate *AS = standardisation(AF);

        printf("\n===== AUTOMATE STANDARDISE =====\n");
        afficher_automate(AS);

        free(AS);
    }

    Automate *AFDC=NULL;

    if (est_deterministe(AF)) {

        printf(" Automate DETERMINISTE\n");

        if (est_complet(AF)) {
            printf(" Automate COMPLET\n");
            AFDC = AF;
        } else {
            printf("Automate NON complet\n");
            AFDC = completion(AF);

            printf(" Automate NON deterministe\n");
            AFDC = determinisation_et_completion_automate(AF);

            printf("\n===== AUTOMATE DETERMINISTE COMPLET =====\n");
            afficher_automate_deterministe_complet(AFDC);
        }

    } else {
        printf(" Automate NON deterministe -> determinisation +completion\n");
        AFDC=determinisation_et_completion_automate(AF);

        printf("\n=====AUTOMATE DETERMINISTE COMPLET =====\n");
        afficher_automate_deterministe_complet(AFDC);
    }

    // ---- Minimisation ----
    Automate *AFDCM = minimisation(AFDC);
    afficher_automate_minimal(AFDCM, AFDC, get_correspondance(), AFDC->nb_etats);

    if (AFDCM != AFDC) free(AFDCM);
    if (AFDC != AF) free(AFDC);
    free(AF);

    return 0;
}