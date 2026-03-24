

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

    if (!est_standard(&A)) {
        printf("Automate NON standard\n");

        int choix;
        printf("Voulez-vous le standardiser ? (1=oui / 0=non) : ");
        scanf("%d", &choix);

    }
    else {
        printf("Automate standard\n");
    }
    if (est_deterministe(&A)) {
        printf("Automate deterministe\n");
    } else {
        printf("Automate NON deterministe\n");
    }
    if (est_complet(&A)) {
        printf("Automate complet\n");
    } else {
        printf("Automate NON complet\n");
    }
    //if (choix == 1) {
        //standardisation(&A);
        //printf("\nAutomate standardisé :\n");
        //afficher_automate(&A);
    //}

    return 0;
}
