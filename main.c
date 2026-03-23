#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "automate.h"

int main() {

    int choix;
    char nom_fichier[100];

    printf("Quel automate voulez-vous utiliser ? ");
    scanf("%d", &choix);

    sprintf(nom_fichier, "Automates/automate%d.txt", choix);

    Automate A = lire_automate(nom_fichier);

    printf("\n--- AUTOMATE CHARGE ---\n");
    afficher_automate(A);

    return 0;
}