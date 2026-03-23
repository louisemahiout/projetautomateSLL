

#include <stdio.h>
#include "automate.h"



int main() {
    Automate A;
    int choix;

    printf("Quel automate voulez-vous utiliser ? ");
    scanf("%d", &choix);

    char chemin[100];

    // ⚠️ adapte selon ton cas (test si besoin)
    sprintf(chemin, "../Automates/automate%d.txt", choix);

    // Debug (tu peux enlever après)
    printf("Chargement du fichier : %s\n", chemin);

    lire_automate(chemin, &A);

    printf("\n=== AFFICHAGE DE L'AUTOMATE %d ===\n\n", choix);
    afficher_automate(&A);

    return 0;
}
