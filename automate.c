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

    // 🔹 1. Vérifier qu'il y a un seul état initial
    if (A->nb_initiaux != 1) {
        return 0;
    }

    char *etat_initial = A->initiaux[0];

    // 🔹 2. Vérifier qu'aucune transition n'arrive vers l'état initial
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
 * ---------------------------------------------------------
 * Calcule l'ε-fermeture d'un état src (indice dans A->etats).
 * Remplit fermeture[] avec les indices des états atteignables
 * via ε uniquement (src inclus). *taille = nb d'éléments.
 * Utilisée uniquement pour les automates asynchrones.
 * ========================================================= */
void epsilon_fermeture(Automate *A, int src, int fermeture[], int *taille) {

    // Chercher la colonne ε dans l'alphabet
    int col_eps = -1;
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "\xce\xb5") == 0 ||
            strcmp(A->symboles[i], "ε")         == 0) {
            col_eps = i;
            break;
        }
    }

    // DFS itératif
    int pile[MAX_ETATS];
    int sommet = 0;
    int vu[MAX_ETATS] = {0};

    pile[sommet++] = src;
    vu[src]        = 1;
    fermeture[0]   = src;
    *taille        = 1;

    while (sommet > 0) {
        int courant = pile[--sommet];
        if (col_eps == -1) break; // pas d'ε dans cet automate

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

/* ε-fermeture d'un ensemble d'états (indices) */
static void eps_fermeture_ens(Automate *A,
                               int src[], int nb,
                               int res[], int *taille) {
    int vu[MAX_ETATS] = {0};
    *taille = 0;
    for (int i = 0; i < nb; i++) {
        int ef[MAX_ETATS], tef = 0;
        epsilon_fermeture(A, src[i], ef, &tef);
        for (int j = 0; j < tef; j++)
            if (!vu[ef[j]]) { vu[ef[j]] = 1; res[(*taille)++] = ef[j]; }
    }
}

/* Construit le nom "i.j.k" trié depuis un tableau d'indices */
static void indices_vers_nom(Automate *A, int idx[], int nb, char res[]) {
    // tri croissant
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
 * ---------------------------------------------------------
 * Fonctionne pour les automates SYNCHRONES et ASYNCHRONES.
 *
 * Automate synchrone (sans ε) : l'ε-fermeture de chaque état
 *   est juste {état lui-même}, comportement identique à avant.
 * Automate asynchrone (avec ε) :
 *   1. état initial = ε-fermeture des initiaux
 *   2. pour chaque symbole a : successeurs = ε-fermeture(δ(q,a))
 * ========================================================= */
Automate* determinisation_et_completion_automate(Automate *A) {

    Automate *B = malloc(sizeof(Automate));
    if (!B) { printf("Erreur malloc\n"); exit(1); }

    // Initialiser les compteurs de transitions de B
    for (int i = 0; i < MAX_ETATS; i++)
        for (int j = 0; j < MAX_SYMBOLES; j++)
            B->nb_transitions[i][j] = 0;

    // Alphabet de B = alphabet de A sans ε
    B->nb_symboles = 0;
    for (int i = 0; i < A->nb_symboles; i++) {
        if (strcmp(A->symboles[i], "\xce\xb5") != 0 &&
            strcmp(A->symboles[i], "ε")         != 0)
            strcpy(B->symboles[B->nb_symboles++], A->symboles[i]);
    }

    // sous_etats[i] = indices (dans A) des sous-états composant l'état i de B
    // static + memset pour éviter stack overflow et résidus entre appels
    static int sous_etats[MAX_ETATS][MAX_ETATS];
    static int nb_sous[MAX_ETATS];
    memset(sous_etats, 0, sizeof(sous_etats));
    memset(nb_sous,    0, sizeof(nb_sous));
    B->nb_etats = 0;

    // ---- État initial de B = ε-fermeture des initiaux de A ----
    {
        int init_idx[MAX_ETATS], nb_init = 0;
        for (int i = 0; i < A->nb_initiaux; i++)
            for (int x = 0; x < A->nb_etats; x++)
                if (strcmp(A->etats[x], A->initiaux[i]) == 0)
                    { init_idx[nb_init++] = x; break; }

        int ef[MAX_ETATS], tef = 0;
        eps_fermeture_ens(A, init_idx, nb_init, ef, &tef);

        char nom[MAX_NOM];
        indices_vers_nom(A, ef, tef, nom);

        strcpy(B->etats[0], nom);
        for (int i = 0; i < tef; i++) sous_etats[0][i] = ef[i];
        nb_sous[0]     = tef;
        B->nb_etats    = 1;
        strcpy(B->initiaux[0], nom);
        B->nb_initiaux = 1;
    }

    // ---- Exploration en largeur ----
    int traite = 0;
    while (traite < B->nb_etats) {

        for (int s = 0; s < B->nb_symboles; s++) {

            // Colonne de ce symbole dans A
            int col_A = -1;
            for (int x = 0; x < A->nb_symboles; x++)
                if (strcmp(A->symboles[x], B->symboles[s]) == 0)
                    { col_A = x; break; }
            if (col_A == -1) continue;

            // 1. Successeurs directs par symbole s (sans doublons)
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

            if (nb_succ == 0) continue; // sera géré par completion()

            // 2. ε-fermeture des successeurs
            int ef[MAX_ETATS], tef = 0;
            eps_fermeture_ens(A, succ, nb_succ, ef, &tef);

            // 3. Construire le nom du nouvel état
            char nom[MAX_NOM];
            indices_vers_nom(A, ef, tef, nom);

            // 4. Chercher si cet état existe déjà dans B
            int idx_dest = -1;
            for (int i = 0; i < B->nb_etats; i++)
                if (strcmp(B->etats[i], nom) == 0) { idx_dest = i; break; }

            // 5. Nouvel état → l'enregistrer
            if (idx_dest == -1) {
                idx_dest = B->nb_etats;
                strcpy(B->etats[idx_dest], nom);
                for (int i = 0; i < tef; i++)
                    sous_etats[idx_dest][i] = ef[i];
                nb_sous[idx_dest] = tef;
                B->nb_etats++;
            }

            // 6. Enregistrer la transition traite --symbole_s--> idx_dest
            strcpy(B->transitions[traite][s][0], nom);
            B->nb_transitions[traite][s] = 1;
        }
        traite++;
    }

    // ---- États finaux de B ----
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

    // Compléter seulement si nécessaire
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

    // 🔹 I devient l'unique initial
    strcpy(B->initiaux[0], nouvel_initial);
    B->nb_initiaux = 1;

    return B;
}

/* =========================================================
 * minimisation
 * ---------------------------------------------------------
 * Algorithme de Moore (raffinement de partitions).
 *
 * Entrée  : AFDC — automate déterministe complet
 * Sortie  : AFDCM — automate minimal équivalent
 *
 * Affiche chaque partition numérotée + transitions en termes de classes.
 * La correspondance état_AFDC → classe est stockée dans _corr[].
 * ========================================================= */

/* Tableau statique partagé avec main via get_correspondance() */
static int _corr[MAX_ETATS];
static int _nb_classes_global;

/* Affiche une partition et les transitions exprimées en classes */
static void afficher_partition(Automate *AFDC, int classe[],
                                int nb_classes, int num_partition) {
    int n = AFDC->nb_etats;

    printf("\n  Partition P%d :\n", num_partition);
    for (int c = 0; c < nb_classes; c++) {
        printf("    G%d = { ", c);
        for (int i = 0; i < n; i++)
            if (classe[i] == c) printf("%s ", AFDC->etats[i]);
        printf("}\n");
    }

    /* Transitions exprimées en classes */
    printf("\n  Transitions par classes (P%d) :\n", num_partition);
    /* Largeur de colonne : 12 pour l'état, 12 par symbole */
    printf("  %-12s", "Etat");
    for (int s = 0; s < AFDC->nb_symboles; s++)
        printf("  %-10s", AFDC->symboles[s]);
    printf("  Classe\n");
    printf("  ");
    for (int x = 0; x < 12 + 12 * AFDC->nb_symboles + 8; x++) printf("-");
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("  %-12s", AFDC->etats[i]);
        for (int s = 0; s < AFDC->nb_symboles; s++) {
            if (AFDC->nb_transitions[i][s] == 0) {
                printf("  %-10s", "--");
            } else {
                char *dest = AFDC->transitions[i][s][0];
                int cl = -1;
                for (int x = 0; x < n; x++)
                    if (strcmp(AFDC->etats[x], dest) == 0) { cl = classe[x]; break; }
                char buf[16];
                sprintf(buf, "G%d", cl);
                printf("  %-10s", buf);
            }
        }
        printf("  G%d\n", classe[i]);
    }
}

Automate* minimisation(Automate *AFDC) {

    int n = AFDC->nb_etats;

    /* --------------------------------------------------
     * 1. Partition initiale P0
     *    Principe : séparer finaux (G1) et non-finaux (G0).
     *    Cas dégénérés : tous finaux → une seule classe G0
     *                    aucun final → une seule classe G0
     * -------------------------------------------------- */
    int classe[MAX_ETATS];
    int nb_classes;

    /* Compter finaux et non-finaux */
    int nb_finaux_AFDC = 0, nb_non_finaux_AFDC = 0;
    for (int i = 0; i < n; i++) {
        if (est_final(AFDC, AFDC->etats[i])) nb_finaux_AFDC++;
        else nb_non_finaux_AFDC++;
    }

    if (nb_finaux_AFDC == 0 || nb_non_finaux_AFDC == 0) {
        /* Une seule classe */
        nb_classes = 1;
        for (int i = 0; i < n; i++) classe[i] = 0;
    } else {
        /* Deux classes : 0 = non-finaux, 1 = finaux */
        nb_classes = 2;
        for (int i = 0; i < n; i++)
            classe[i] = est_final(AFDC, AFDC->etats[i]) ? 1 : 0;
    }

    printf("\n========== MINIMISATION ==========\n");
    afficher_partition(AFDC, classe, nb_classes, 0);

    /* --------------------------------------------------
     * 2. Raffinement itératif jusqu'à stabilité
     * -------------------------------------------------- */
    int iteration = 1;
    int stable = 0;

    while (!stable) {

        int new_classe[MAX_ETATS];
        int new_nb = 0;
        int marque[MAX_ETATS];
        for (int i = 0; i < n; i++) marque[i] = -1;

        for (int i = 0; i < n; i++) {
            if (marque[i] != -1) continue;

            /* Nouvelle classe pour l'état i */
            int ma_classe = new_nb++;
            marque[i] = ma_classe;
            new_classe[i] = ma_classe;

            /* Regrouper tous les j de même ancienne classe
               ET même profil de transitions (destinations dans même classe) */
            for (int j = i + 1; j < n; j++) {
                if (marque[j] != -1) continue;
                if (classe[j] != classe[i]) continue;

                int meme = 1;
                for (int s = 0; s < AFDC->nb_symboles && meme; s++) {
                    /* Classe destination de i par s */
                    int cl_i = -1;
                    if (AFDC->nb_transitions[i][s] > 0) {
                        char *d = AFDC->transitions[i][s][0];
                        for (int x = 0; x < n; x++)
                            if (strcmp(AFDC->etats[x], d) == 0) { cl_i = classe[x]; break; }
                    }
                    /* Classe destination de j par s */
                    int cl_j = -1;
                    if (AFDC->nb_transitions[j][s] > 0) {
                        char *d = AFDC->transitions[j][s][0];
                        for (int x = 0; x < n; x++)
                            if (strcmp(AFDC->etats[x], d) == 0) { cl_j = classe[x]; break; }
                    }
                    if (cl_i != cl_j) meme = 0;
                }

                if (meme) {
                    marque[j] = ma_classe;
                    new_classe[j] = ma_classe;
                }
            }
        }

        /* La partition est stable si aucun état n'a changé de groupe */
        stable = 1;
        for (int i = 0; i < n && stable; i++)
            for (int j = i + 1; j < n && stable; j++)
                if ((classe[i] == classe[j]) != (new_classe[i] == new_classe[j]))
                    stable = 0;

        for (int i = 0; i < n; i++) classe[i] = new_classe[i];
        nb_classes = new_nb;

        if (!stable) {
            afficher_partition(AFDC, classe, nb_classes, iteration);
            iteration++;
        }
    }

    printf("\n  ->Partition stable atteinte en %d iteration(s).\n", iteration);

    /* Sauvegarder la correspondance */
    for (int i = 0; i < n; i++) _corr[i] = classe[i];
    _nb_classes_global = nb_classes;

    /* --------------------------------------------------
     * 3. Construire l'automate minimal AFDCM
     * -------------------------------------------------- */
    Automate *M = malloc(sizeof(Automate));
    if (!M) { printf("Erreur malloc minimisation\n"); exit(1); }

    for (int i = 0; i < MAX_ETATS; i++)
        for (int j = 0; j < MAX_SYMBOLES; j++)
            M->nb_transitions[i][j] = 0;

    /* Alphabet identique (sans ε, AFDC en est déjà débarrassé) */
    M->nb_symboles = AFDC->nb_symboles;
    for (int i = 0; i < AFDC->nb_symboles; i++)
        strcpy(M->symboles[i], AFDC->symboles[i]);

    /* Un état par classe, nommé "q0", "q1", ... */
    M->nb_etats = nb_classes;
    for (int c = 0; c < nb_classes; c++) {
        char nom[16];
        sprintf(nom, "q%d", c);
        strcpy(M->etats[c], nom);
    }

    /* État initial = classe contenant l'état initial de AFDC */
    M->nb_initiaux = 0;
    for (int i = 0; i < AFDC->nb_initiaux; i++) {
        for (int x = 0; x < n; x++) {
            if (strcmp(AFDC->etats[x], AFDC->initiaux[i]) == 0) {
                char nom[16];
                sprintf(nom, "q%d", classe[x]);
                int doublon = 0;
                for (int k = 0; k < M->nb_initiaux; k++)
                    if (strcmp(M->initiaux[k], nom) == 0) { doublon = 1; break; }
                if (!doublon)
                    strcpy(M->initiaux[M->nb_initiaux++], nom);
                break;
            }
        }
    }

    /* États finaux = classes contenant au moins un état final de AFDC */
    M->nb_finaux = 0;
    int classe_finale[MAX_ETATS] = {0};
    for (int i = 0; i < n; i++) {
        if (est_final(AFDC, AFDC->etats[i]) && !classe_finale[classe[i]]) {
            classe_finale[classe[i]] = 1;
            char nom[16];
            sprintf(nom, "q%d", classe[i]);
            strcpy(M->finaux[M->nb_finaux++], nom);
        }
    }

    /* Transitions : représentant de chaque classe */
    for (int c = 0; c < nb_classes; c++) {
        int rep = -1;
        for (int i = 0; i < n; i++)
            if (classe[i] == c) { rep = i; break; }
        if (rep == -1) continue;

        for (int s = 0; s < AFDC->nb_symboles; s++) {
            if (AFDC->nb_transitions[rep][s] == 0) continue;
            char *dest = AFDC->transitions[rep][s][0];
            for (int x = 0; x < n; x++) {
                if (strcmp(AFDC->etats[x], dest) == 0) {
                    char nom_dest[16];
                    sprintf(nom_dest, "q%d", classe[x]);
                    strcpy(M->transitions[c][s][0], nom_dest);
                    M->nb_transitions[c][s] = 1;
                    break;
                }
            }
        }
    }

    return M;
}

/* =========================================================
 * afficher_automate_minimal
 * ---------------------------------------------------------
 * Affiche AFDCM + message "déjà minimal" si applicable
 * + table de correspondance AFDC → AFDCM.
 * ========================================================= */
void afficher_automate_minimal(Automate *AFDCM, Automate *AFDC,
                                int correspondance[], int nb_etats_AFDC) {
    if (AFDCM->nb_etats == AFDC->nb_etats)
        printf("\n  L'automate etait deja minimal (aucun etat fusionne).\n");
    else
        printf("\n  %d etat(s) fusionne(s) : %d -> %d etats.\n",
               AFDC->nb_etats - AFDCM->nb_etats,
               AFDC->nb_etats, AFDCM->nb_etats);

    printf("\n===== AUTOMATE MINIMAL (AFDCM) =====\n");
    afficher_automate(AFDCM);

    printf("\n  Table de correspondance AFDC -> AFDCM :\n");
    printf("  %-28s -> %s\n", "Etat AFDC", "Etat AFDCM");
    printf("  ");
    for (int i = 0; i < 40; i++) printf("-");
    printf("\n");
    for (int i = 0; i < nb_etats_AFDC; i++) {
        char nom_classe[16];
        sprintf(nom_classe, "q%d", correspondance[i]);
        printf("  %-28s -> %s\n", AFDC->etats[i], nom_classe);
    }
}

/* Accesseurs pour récupérer la correspondance depuis main.c */
int* get_correspondance(void)     { return _corr; }
int  get_nb_classes(void)         { return _nb_classes_global; }