

#include <stdio.h>
#include <stdlib.h>
#include "automate.h"

void lire_automate(const char *nomFichier, Automate *A) {
    FILE *f = fopen(nomFichier, "r");

    if (f == NULL) {
        printf("Erreur ouverture fichier\n");
        exit(1);
    }

    fscanf(f, "%d", &A->nbSymboles);
    fscanf(f, "%d", &A->nbEtats);

    fscanf(f, "%d", &A->nbInitiaux);
    for (int i = 0; i < A->nbInitiaux; i++)
        fscanf(f, "%d", &A->initiaux[i]);

    fscanf(f, "%d", &A->nbFinaux);
    for (int i = 0; i < A->nbFinaux; i++)
        fscanf(f, "%d", &A->finaux[i]);

    int nbTrans;
    fscanf(f, "%d", &nbTrans);

    // Initialisation
    for (int i = 0; i < A->nbEtats; i++) {
        for (int j = 0; j < A->nbSymboles; j++) {
            A->nbTransitions[i][j] = 0;
        }
    }

    // Lecture transitions
    for (int i = 0; i < nbTrans; i++) {
        int depart, arrivee;
        char symbole;

        fscanf(f, "%d%c%d", &depart, &symbole, &arrivee);

        int col = symbole - 'a';

        A->table[depart][col][A->nbTransitions[depart][col]] = arrivee;
        A->nbTransitions[depart][col]++;
    }

    fclose(f);
}

// Fonction pour vérifier si un état est initial
int est_initial(Automate *A, int etat) {
    for (int i = 0; i < A->nbInitiaux; i++)
        if (A->initiaux[i] == etat) return 1;
    return 0;
}

// Fonction pour vérifier si un état est final
int est_final(Automate *A, int etat) {
    for (int i = 0; i < A->nbFinaux; i++)
        if (A->finaux[i] == etat) return 1;
    return 0;
}

void afficher_automate(Automate *A) {
    int largeur = 8;

    // En-tête
    printf("     ");
    for (int i = 0; i < A->nbSymboles; i++) {
        printf("%-*c", largeur, 'a' + i);
    }
    printf("\n");

    // Lignes
    for (int i = 0; i < A->nbEtats; i++) {

        if (est_initial(A, i)) printf("E ");
        else printf("  ");

        if (est_final(A, i)) printf("S ");
        else printf("  ");

        printf("%-2d ", i);

        for (int j = 0; j < A->nbSymboles; j++) {

            if (A->nbTransitions[i][j] == 0) {
                printf("%-*s", largeur, "-");
            } else {
                for (int k = 0; k < A->nbTransitions[i][j]; k++) {
                    printf("%d", A->table[i][j][k]);
                    if (k < A->nbTransitions[i][j] - 1)
                        printf(",");
                }

                int len = A->nbTransitions[i][j] * 2;
                for (int s = len; s < largeur; s++)
                    printf(" ");
            }
        }

        printf("\n");
    }
}
