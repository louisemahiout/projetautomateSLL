

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


/*Affiche UNE seule ligne */
void afficher_ligne(Automate *A, int i, int largeur) {
    if (est_initial(A, i)) {
        printf("E ");
    } else {
        printf("  ");
    }

    if (est_final(A, i)) {
        printf("S ");
    } else {
        printf("  ");
    }

    printf("%-2d ", i);

    for (int j = 0; j < A->nbSymboles; j++) {
        if (A->nbTransitions[i][j] == 0) {
            printf("%-*s", largeur, "-");
        } else {
            int len = 0;

            for (int k = 0; k < A->nbTransitions[i][j]; k++) {
                printf("%d", A->table[i][j][k]);
                len++;

                if (k < A->nbTransitions[i][j] - 1) {
                    printf(",");
                    len++;
                }
            }

            for (int s = len; s < largeur; s++) {
                printf(" ");
            }
        }
    }

    printf("\n");
}


/* =========================
   Affichage de l'automate
   ========================= */
void afficher_automate(Automate *A) {
    int largeur = 8;

    /* En-tête */
    printf("     ");
    for (int i = 0; i < A->nbSymboles; i++) {
        printf("%-*c", largeur, 'a' + i);
    }
    printf("\n");

    /* 1) Etats initiaux */
    for (int i = 0; i < A->nbEtats; i++) {
        if (est_initial(A, i) && !est_final(A, i)) {
            afficher_ligne(A, i, largeur);
        }
    }

    /* 2) Etats normaux */
    for (int i = 0; i < A->nbEtats; i++) {
        if (!est_initial(A, i) && !est_final(A, i)) {
            afficher_ligne(A, i, largeur);
        }
    }

    /* 3) Etats à la fois initiaux et finaux */
    for (int i = 0; i < A->nbEtats; i++) {
        if (est_initial(A, i) && est_final(A, i)) {
            afficher_ligne(A, i, largeur);
        }
    }

    /* 4) Etats finaux seulement */
    for (int i = 0; i < A->nbEtats; i++) {
        if (!est_initial(A, i) && est_final(A, i)) {
            afficher_ligne(A, i, largeur);
        }
    }
}

/*est-il standard ? */
int est_standard(Automate *A) {

    if (A->nbInitiaux != 1)
        return 0;

    int init = A->initiaux[0];

    for (int i = 0; i < A->nbEtats; i++) {
        for (int j = 0; j < A->nbSymboles; j++) {
            for (int k = 0; k < A->nbTransitions[i][j]; k++) {

                if (A->table[i][j][k] == init) {
                    printf("Non standard : transition vers l'état initial (%d)\n", init);
                    return 0;
                }
            }
        }
    }

    return 1;
}
/*est-il déterministe ? */
int est_deterministe(Automate *A) {

    int deterministe = 1;

    for (int i = 0; i < A->nbEtats; i++) {
        for (int j = 0; j < A->nbSymboles; j++) {

            if (A->nbTransitions[i][j] > 1) {
                printf("Non déterministe : état %d avec symbole %c -> plusieurs transitions\n",
                       i, 'a' + j);

                deterministe = 0;
            }
        }
    }

    return deterministe;
}

/*est-il complet ? */
int est_complet(Automate *A) {

    int complet = 1;

    for (int i = 0; i < A->nbEtats; i++) {
        for (int j = 0; j < A->nbSymboles; j++) {

            if (A->nbTransitions[i][j] == 0) {
                printf("Non complet : etat %d avec symbole %c -> aucune transition\n",
                       i, 'a' + j);

                complet = 0;
            }
        }
    }

    return complet;
}