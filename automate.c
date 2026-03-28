#include "automate.h"

int est_initial(Automate *A, char *etat) {
    for (int i = 0; i < A->nb_initiaux; i++)
        if (strcmp(A->initiaux[i], etat) == 0)
            return 1;
    return 0;
}

int est_final(Automate *A, char *etat) {
    for (int i = 0; i < A->nb_finaux; i++)
        if (strcmp(A->finaux[i], etat) == 0)
            return 1;
    return 0;
}

void afficher_automate(Automate *A) {
    int i, j, k;

    // 🔹 entête
    printf("      ");
    for (j = 0; j < A->nb_symboles; j++)
        printf("%-8s", A->symboles[j]);
    printf("\n");

    // 🔹 lignes
    for (i = 0; i < A->nb_etats; i++) {

        // E / S
        if (est_initial(A, A->etats[i])) printf("E");
        else printf(" ");

        if (est_final(A, A->etats[i])) printf("S");
        else printf(" ");

        printf("%-3s ", A->etats[i]);

        // transitions
        for (j = 0; j < A->nb_symboles; j++) {

            if (A->nb_transitions[i][j] == 0) {
                printf("%-8s", "--");
            } else {
                for (k = 0; k < A->nb_transitions[i][j]; k++) {
                    printf("%s", A->transitions[i][j][k]);
                    if (k < A->nb_transitions[i][j] - 1)
                        printf(",");
                }
                printf("%*s", 8 - 2, "");
            }
        }

        printf("\n");
    }
}
Automate* lire_automate_sur_fichier(const char *nom_fichier) {
    Automate *A = malloc(sizeof(Automate));
    if (!A) { printf("Erreur allocation mémoire\n"); exit(1); }

    FILE *f = fopen(nom_fichier, "r");
    if (!f) { printf("Erreur ouverture fichier '%s'\n", nom_fichier); exit(1); }

    // 🔹 symboles
    fscanf(f, "%d", &A->nb_symboles);
    for (int i = 0; i < A->nb_symboles; i++)
        fscanf(f, "%s", A->symboles[i]); // support ε

    // 🔹 états
    fscanf(f, "%d", &A->nb_etats);
    for (int i = 0; i < A->nb_etats; i++)
        fscanf(f, "%s", A->etats[i]);

    // 🔹 initiaux
    fscanf(f, "%d", &A->nb_initiaux);
    for (int i = 0; i < A->nb_initiaux; i++)
        fscanf(f, "%s", A->initiaux[i]);

    // 🔹 finaux
    fscanf(f, "%d", &A->nb_finaux);
    for (int i = 0; i < A->nb_finaux; i++)
        fscanf(f, "%s", A->finaux[i]);

    // 🔹 nb transitions
    int nb;
    fscanf(f, "%d", &nb);

    // 🔹 init
    for (int i = 0; i < MAX_ETATS; i++)
        for (int j = 0; j < MAX_SYMBOLES; j++)
            A->nb_transitions[i][j] = 0;

    // 🔹 transitions
    for (int i = 0; i < nb; i++) {
        char ligne[30];
        fscanf(f, "%s", ligne);

        char src[10], dest[10], symbole[5];
        int k = 0, j = 0;

        // src
        while ((ligne[k] >= '0' && ligne[k] <= '9') || ligne[k]=='p')
            src[j++] = ligne[k++];
        src[j] = '\0';

        // symbole (UTF-8 ou normal)
        j = 0;
        if ((unsigned char)ligne[k] == 0xCE && (unsigned char)ligne[k+1] == 0xB5) {
            symbole[0] = ligne[k];
            symbole[1] = ligne[k+1];
            symbole[2] = '\0';
            k += 2;
        } else {
            symbole[0] = ligne[k++];
            symbole[1] = '\0';
        }

        // dest
        strcpy(dest, &ligne[k]);

        // trouver état source
        int s = -1;
        for (int x = 0; x < A->nb_etats; x++)
            if (strcmp(A->etats[x], src) == 0) { s = x; break; }

        if (s == -1) {
            printf("Etat source inconnu : %s\n", src);
            exit(1);
        }

        // trouver symbole
        int col = -1;
        for (int x = 0; x < A->nb_symboles; x++)
            if (strcmp(A->symboles[x], symbole) == 0) { col = x; break; }

        if (col == -1) {
            printf("Symbole inconnu : %s\n", symbole);
            exit(1);
        }

        // stocker
        strcpy(A->transitions[s][col][A->nb_transitions[s][col]++], dest);
    }

    fclose(f);
    return A;
}

int est_standard(Automate *A) {

    // 🔹 1. Vérifier qu’il y a un seul état initial
    if (A->nb_initiaux != 1) {
        return 0;
    }

    char *etat_initial = A->initiaux[0];

    // 🔹 2. Vérifier qu’aucune transition n’arrive vers l’état initial
    for (int i = 0; i < A->nb_etats; i++) {
        for (int j = 0; j < A->nb_symboles; j++) {
            for (int k = 0; k < A->nb_transitions[i][j]; k++) {
                if (strcmp(A->transitions[i][j][k], etat_initial) == 0) {
                    return 0;
                }
            }
        }
    }

    // 🔹 3. Vérifier qu’il n’y a pas de symbole ε
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "ε") == 0) {
            return 0;
        }
    }

    // ✅ Si tout est bon
    return 1;
}

int est_deterministe(Automate *A) {

    // 🔹 1. Un seul état initial
    if (A->nb_initiaux != 1) {
        return 0;
    }

    // 🔹 2. Pas de ε
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "ε") == 0) {
            return 0;
        }
    }

    // 🔹 3. Au plus une transition par symbole
    for (int i = 0; i < A->nb_etats; i++) {
        for (int j = 0; j < A->nb_symboles; j++) {
            if (A->nb_transitions[i][j] > 1) {
                return 0;
            }
        }
    }

    return 1;
}
int est_complet(Automate *A) {

    for (int i = 0; i < A->nb_etats; i++) {
        for (int j = 0; j < A->nb_symboles; j++) {


            if (strcmp(A->symboles[j], "ε") == 0)
                continue;

            if (A->nb_transitions[i][j] == 0) {
                return 0;
            }
        }
    }

    return 1;
}
Automate* completion(Automate *A) {

    Automate *B = malloc(sizeof(Automate));
    *B = *A;

    int puit = B->nb_etats;
    strcpy(B->etats[puit], "P");
    B->nb_etats++;

    for (int i = 0; i < B->nb_etats; i++) {
        for (int j = 0; j < B->nb_symboles; j++) {

            if (strcmp(B->symboles[j], "ε") == 0)
                continue;

            if (B->nb_transitions[i][j] == 0) {
                strcpy(B->transitions[i][j][0], "P");
                B->nb_transitions[i][j] = 1;
            }
        }
    }

    for (int j = 0; j < B->nb_symboles; j++) {
        if (strcmp(B->symboles[j], "ε") == 0)
            continue;

        strcpy(B->transitions[puit][j][0], "P");
        B->nb_transitions[puit][j] = 1;
    }

    return B;
}
Automate* determinisation_et_completion_automate(Automate *A) {

    Automate *B = malloc(sizeof(Automate));

    // copier alphabet sans ε
    B->nb_symboles = 0;
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "ε") != 0) {
            strcpy(B->symboles[B->nb_symboles++], A->symboles[i]);
        }
    }

    // état initial = ensemble des initiaux
    char etat_init[100] = "";
    for (int i = 0; i < A->nb_initiaux; i++) {
        strcat(etat_init, A->initiaux[i]);
        if (i < A->nb_initiaux - 1) strcat(etat_init, ".");
    }

    strcpy(B->etats[0], etat_init);
    B->nb_etats = 1;

    strcpy(B->initiaux[0], etat_init);
    B->nb_initiaux = 1;

    int traité = 0;

    while (traité < B->nb_etats) {

        char *etat = B->etats[traité];

        for (int s = 0; s < B->nb_symboles; s++) {

            char nouveau[100] = "";

            // parcourir chaque sous-état
            char tmp[100];
            strcpy(tmp, etat);

            char *token = strtok(tmp, ".");

            while (token) {

                // trouver index dans A
                for (int i = 0; i < A->nb_etats; i++) {
                    if (strcmp(A->etats[i], token) == 0) {

                        for (int k = 0; k < A->nb_transitions[i][s]; k++) {
                            strcat(nouveau, A->transitions[i][s][k]);
                            strcat(nouveau, ".");
                        }
                    }
                }

                token = strtok(NULL, ".");
            }

            if (strlen(nouveau) > 0) {
                nouveau[strlen(nouveau) - 1] = '\0';

                // vérifier si existe déjà
                int existe = -1;
                for (int i = 0; i < B->nb_etats; i++) {
                    if (strcmp(B->etats[i], nouveau) == 0)
                        existe = i;
                }

                if (existe == -1) {
                    strcpy(B->etats[B->nb_etats++], nouveau);
                }

                strcpy(B->transitions[traité][s][0], nouveau);
                B->nb_transitions[traité][s] = 1;
            }
        }

        traité++;
    }

    // états finaux
    B->nb_finaux = 0;
    for (int i = 0; i < B->nb_etats; i++) {
        for (int j = 0; j < A->nb_finaux; j++) {
            if (strstr(B->etats[i], A->finaux[j])) {
                strcpy(B->finaux[B->nb_finaux++], B->etats[i]);
                break;
            }
        }
    }

    return completion(B);
}
void afficher_automate_deterministe_complet(Automate *A) {
    printf("\n===== AUTOMATE DETERMINISTE COMPLET =====\n");
    afficher_automate(A);
}