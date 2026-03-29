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
/* =========================================================
 * epsilon_fermeture
 * Calcule l'ε-fermeture d'UN état (src = indice dans A->etats).
 * Remplit fermeture[] avec les indices des états atteignables
 * via ε uniquement (src inclus). *taille = nombre d'éléments.
 * ========================================================= */
void epsilon_fermeture(Automate *A, int src,
                       int fermeture[], int *taille)
{
    /* Chercher la colonne ε dans l'alphabet */
    int col_eps = -1;
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "\xce\xb5") == 0 ||
            strcmp(A->symboles[i], "ε")         == 0) {
            col_eps = i;
            break;
        }
    }

    int pile[MAX_ETATS];
    int sommet = 0;
    int vu[MAX_ETATS] = {0};

    pile[sommet++]   = src;
    vu[src]          = 1;
    fermeture[0]     = src;
    *taille          = 1;

    while (sommet > 0) {
        int courant = pile[--sommet];
        if (col_eps == -1) break;

        for (int k = 0; k < A->nb_transitions[courant][col_eps]; k++) {
            char *dest = A->transitions[courant][col_eps][k];
            int idx = -1;
            for (int x = 0; x < A->nb_etats; x++)
                if (strcmp(A->etats[x], dest) == 0) { idx = x; break; }
            if (idx != -1 && !vu[idx]) {
                vu[idx] = 1;
                fermeture[(*taille)++] = idx;
                pile[sommet++] = idx;
            }
        }
    }
}

/* Fermeture ε d'un ensemble d'états (indices) */
static void eps_fermeture_ens(Automate *A,
                               int src[], int nb,
                               int res[], int *taille)
{
    int vu[MAX_ETATS] = {0};
    *taille = 0;
    for (int i = 0; i < nb; i++) {
        int ef[MAX_ETATS], tef = 0;
        epsilon_fermeture(A, src[i], ef, &tef);
        for (int j = 0; j < tef; j++)
            if (!vu[ef[j]]) { vu[ef[j]] = 1; res[(*taille)++] = ef[j]; }
    }
}

/* Construit "i.j.k" trié depuis un tableau d'indices */
static void indices_vers_nom(Automate *A,
                              int idx[], int nb,
                              char res[])
{
    /* tri croissant sur les numéros d'indices */
    for (int i = 0; i < nb-1; i++)
        for (int j = i+1; j < nb; j++)
            if (idx[i] > idx[j]) { int t=idx[i]; idx[i]=idx[j]; idx[j]=t; }
    res[0] = '\0';
    for (int i = 0; i < nb; i++) {
        strcat(res, A->etats[idx[i]]);
        if (i < nb-1) strcat(res, ".");
    }
}

/* =========================================================
 * determinisation_et_completion_automate
 * Fonctionne pour automates SYNCHRONES et ASYNCHRONES (ε).
 * ========================================================= */
Automate* determinisation_et_completion_automate(Automate *A)
{
    Automate *B = malloc(sizeof(Automate));
    if (!B) { printf("Erreur malloc\n"); exit(1); }

    /* Initialiser les compteurs de transitions */
    for (int i = 0; i < MAX_ETATS; i++)
        for (int j = 0; j < MAX_SYMBOLES; j++)
            B->nb_transitions[i][j] = 0;

    /* Alphabet de B = alphabet de A sans ε */
    B->nb_symboles = 0;
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "\xce\xb5") != 0 &&
            strcmp(A->symboles[i], "ε")         != 0)
            strcpy(B->symboles[B->nb_symboles++], A->symboles[i]);
    }

    /*
     * sous_etats[i] = tableau des indices (dans A) composant l'état i de B
     * nb_sous[i]    = taille de ce tableau
     * Mis en static + memset pour éviter stack overflow et résidus entre appels
     */
    static int sous_etats[MAX_ETATS][MAX_ETATS];
    static int nb_sous[MAX_ETATS];
    memset(sous_etats, 0, sizeof(sous_etats));
    memset(nb_sous,    0, sizeof(nb_sous));
    B->nb_etats = 0;

    /* État initial de B = ε-fermeture des initiaux de A */
    {
        int init_idx[MAX_ETATS], nb_init = 0;
        for (int i = 0; i < A->nb_initiaux; i++)
            for (int x = 0; x < A->nb_etats; x++)
                if (strcmp(A->etats[x], A->initiaux[i]) == 0)
                    { init_idx[nb_init++] = x; break; }

        int ef[MAX_ETATS], tef = 0;
        eps_fermeture_ens(A, init_idx, nb_init, ef, &tef);

        char nom[100];
        indices_vers_nom(A, ef, tef, nom);

        strcpy(B->etats[0], nom);
        for (int i = 0; i < tef; i++) sous_etats[0][i] = ef[i];
        nb_sous[0] = tef;
        B->nb_etats = 1;
        strcpy(B->initiaux[0], nom);
        B->nb_initiaux = 1;
    }

    /* Exploration en largeur */
    int traite = 0;
    while (traite < B->nb_etats) {

        for (int s = 0; s < B->nb_symboles; s++) {

            /* Colonne de ce symbole dans A */
            int col_A = -1;
            for (int x = 0; x < A->nb_symboles; x++)
                if (strcmp(A->symboles[x], B->symboles[s]) == 0)
                    { col_A = x; break; }
            if (col_A == -1) continue;

            /* 1. Successeurs directs par symbole s */
            int succ[MAX_ETATS], nb_succ = 0;
            int vu_succ[MAX_ETATS] = {0};

            for (int q = 0; q < nb_sous[traite]; q++) {
                int idx = sous_etats[traite][q];
                for (int k = 0; k < A->nb_transitions[idx][col_A]; k++) {
                    char *dest = A->transitions[idx][col_A][k];
                    for (int x = 0; x < A->nb_etats; x++) {
                        if (strcmp(A->etats[x], dest) == 0) {
                            if (!vu_succ[x]) { vu_succ[x]=1; succ[nb_succ++]=x; }
                            break;
                        }
                    }
                }
            }

            if (nb_succ == 0) continue; /* sera géré par completion() */

            /* 2. ε-fermeture des successeurs */
            int ef[MAX_ETATS], tef = 0;
            eps_fermeture_ens(A, succ, nb_succ, ef, &tef);

            /* 3. Nom du nouvel état */
            char nom[100];
            indices_vers_nom(A, ef, tef, nom);

            /* 4. Chercher si cet état existe déjà */
            int idx_dest = -1;
            for (int i = 0; i < B->nb_etats; i++)
                if (strcmp(B->etats[i], nom) == 0) { idx_dest = i; break; }

            /* 5. Nouvel état → l'enregistrer */
            if (idx_dest == -1) {
                idx_dest = B->nb_etats;
                strcpy(B->etats[idx_dest], nom);
                for (int i = 0; i < tef; i++)
                    sous_etats[idx_dest][i] = ef[i];
                nb_sous[idx_dest] = tef;
                B->nb_etats++;
            }

            /* 6. Enregistrer la transition */
            strcpy(B->transitions[traite][s][0], nom);
            B->nb_transitions[traite][s] = 1;
        }
        traite++;
    }

    /* États finaux de B */
    B->nb_finaux = 0;
    for (int i = 0; i < B->nb_etats; i++) {
        int fin = 0;
        for (int q = 0; q < nb_sous[i] && !fin; q++) {
            for (int f = 0; f < A->nb_finaux; f++) {
                if (strcmp(A->etats[sous_etats[i][q]], A->finaux[f]) == 0)
                    { fin = 1; break; }
            }
        }
        if (fin) strcpy(B->finaux[B->nb_finaux++], B->etats[i]);
    }

    if (est_complet(B))
        return B;
    return completion(B);
}
void afficher_automate_deterministe_complet(Automate *A) {
    printf("\n===== AUTOMATE DETERMINISTE COMPLET =====\n");
    afficher_automate(A);
}

Automate* standardisation(Automate *A) {
    Automate *B = malloc(sizeof(Automate));
    memset(B, 0, sizeof(Automate));

    // copier automate
    *B = *A;

    // 🔹 nouvel état initial
    char nouvel_initial[10] = "I";

    // éviter conflit si I existe déjà
    int existe = 1;
    while (existe) {
        existe = 0;
        for (int i = 0; i < B->nb_etats; i++) {
            if (strcmp(B->etats[i], nouvel_initial) == 0) {
                strcat(nouvel_initial, "0");
                existe = 1;
                break;
            }
        }
    }

    int idxI = B->nb_etats;
    strcpy(B->etats[idxI], nouvel_initial);
    B->nb_etats++;

    // 🔹 copier transitions de tous les anciens initiaux
    for (int init = 0; init < A->nb_initiaux; init++) {

        char *ancien = A->initiaux[init];

        int idxAncien = -1;
        for (int i = 0; i < A->nb_etats; i++) {
            if (strcmp(A->etats[i], ancien) == 0) {
                idxAncien = i;
                break;
            }
        }

        if (idxAncien != -1) {
            for (int j = 0; j < A->nb_symboles; j++) {
                for (int k = 0; k < A->nb_transitions[idxAncien][j]; k++) {

                    char *dest = A->transitions[idxAncien][j][k];

                    // éviter doublons
                    int doublon = 0;
                    for (int x = 0; x < B->nb_transitions[idxI][j]; x++) {
                        if (strcmp(B->transitions[idxI][j][x], dest) == 0) {
                            doublon = 1;
                            break;
                        }
                    }

                    if (!doublon) {
                        strcpy(
                            B->transitions[idxI][j][B->nb_transitions[idxI][j]],
                            dest
                        );
                        B->nb_transitions[idxI][j]++;
                    }
                }
            }
        }
    }

    // 🔹 si un ancien initial était final → I devient final
    for (int i = 0; i < A->nb_initiaux; i++) {
        if (est_final(A, A->initiaux[i])) {
            strcpy(B->finaux[B->nb_finaux], nouvel_initial);
            B->nb_finaux++;
            break;
        }
    }

    // 🔹 I devient l’unique initial
    strcpy(B->initiaux[0], nouvel_initial);
    B->nb_initiaux = 1;

    return B;
}