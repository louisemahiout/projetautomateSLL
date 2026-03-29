// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "automate.h"

/*
 * Affiche le menu des actions disponibles pour l'utilisateur.
 * Appelée à chaque itération de la boucle principale.
 */
void afficher_menu() {
    printf("\n========== MENU ==========\n");
    printf("  afficher       - Afficher l'automate\n");
    printf("  standardiser   - Standardiser l'automate\n");
    printf("  determiniser   - Determiniser et completer l'automate\n");
    printf("  complementaire - Calculer l'automate complementaire\n");
    printf("  minimiser      - Minimiser l'automate\n");
    printf("  reconnaitre    - Reconnaitre un mot\n");
    printf("  changer        - Changer d'automate\n");
    printf("  sortir         - Quitter le programme\n");
    printf("===========================\n");
    printf("Que veux-tu faire ? : ");
}

int main() {

    /* --- Variables générales --- */
    int  choix;           // Numéro de l'automate choisi par l'utilisateur
    char chemin[256];     // Chemin vers le fichier .txt de l'automate
    char mot[256];        // Mot saisi par l'utilisateur pour la reconnaissance
    char commande[64];    // Commande saisie dans le menu

    /* --- Pointeurs sur les différentes formes de l'automate --- */
    Automate *AF    = NULL;  // Automate de base lu depuis le fichier
    Automate *AFDC  = NULL;  // Automate Fini Deterministe Complet
    Automate *AFDCM = NULL;  // Automate Fini Deterministe Complet Minimal

    /*
     * Drapeau indiquant si l'AFDC a déjà été calculé.
     * Certaines opérations (complémentaire, minimisation, reconnaissance)
     * nécessitent que la déterminisation ait été faite au préalable.
     */
    int afdc_pret = 0;

    /* ------------------------------------------------------------------ */
    /*  Chargement initial de l'automate                                   */
    /* ------------------------------------------------------------------ */
    printf("Quel automate veux tu utiliser ? : ");
    scanf("%d", &choix);
    sprintf(chemin, "../Automates/automate%d.txt", choix);
    AF = lire_automate_sur_fichier(chemin);

    /* ------------------------------------------------------------------ */
    /*  Boucle principale du menu                                          */
    /* ------------------------------------------------------------------ */
    while (1) {

        afficher_menu();
        scanf("%s", commande);

        /* -------------------------------------------------------------- */
        /*  AFFICHER : affiche l'automate brut tel que lu depuis le fichier */
        /* -------------------------------------------------------------- */
        if (strcmp(commande, "afficher") == 0) {

            printf("\n===== Affichage de l'automate =====\n");
            afficher_automate(AF);

        /* -------------------------------------------------------------- */
        /*  STANDARDISER : standardise l'automate s'il ne l'est pas déjà  */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "standardiser") == 0) {

            if (est_standard(AF)) {
                printf("L'automate est deja STANDARDISE.\n");
            } else {
                Automate *AS = standardisation(AF);
                printf("\n===== AUTOMATE STANDARDISE =====\n");
                afficher_automate(AS);
                free(AS);  // L'automate standardisé est temporaire, on le libère
            }

        /* -------------------------------------------------------------- */
        /*  DETERMINISER : déterminise et complète l'automate              */
        /*  Résultat stocké dans AFDC, nécessaire pour les étapes suivantes */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "determiniser") == 0) {

            /* Libère les anciens AFDC/AFDCM avant d'en recalculer de nouveaux */
            if (AFDCM != NULL && AFDCM != AFDC) free(AFDCM);
            if (AFDC  != NULL && AFDC  != AF)   free(AFDC);
            AFDCM = NULL;

            if (est_deterministe(AF)) {
                printf("Automate deterministe\n");

                if (est_complet(AF)) {
                    /* L'automate est déjà déterministe ET complet : AFDC == AF
                     * On ne crée pas de copie pour éviter un double free plus tard */
                    printf("Automate complet\n");
                    AFDC = AF;
                } else {
                    /* Déterministe mais incomplet : on complète uniquement */
                    printf("Automate non complet -> completion...\n");
                    AFDC = completion(AF);
                }
            } else {
                /* Non déterministe : déterminisation + complétion en une passe */
                printf("Automate non deterministe -> determinisation + completion...\n");
                AFDC = determinisation_et_completion_automate(AF);
            }

            afficher_automate_deterministe_complet(AFDC);
            afdc_pret = 1;  // L'AFDC est maintenant disponible pour la suite

        /* -------------------------------------------------------------- */
        /*  COMPLEMENTAIRE : calcule l'automate complémentaire de l'AFDC  */
        /*  Nécessite que la déterminisation ait été effectuée             */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "complementaire") == 0) {

            if (!afdc_pret) {
                printf("Lance d'abord 'determiniser' pour obtenir l'AFDC.\n");
            } else {
                Automate *AComp = automate_complementaire(AFDC);
                printf("\n===== AUTOMATE COMPLEMENTAIRE =====\n");
                afficher_automate(AComp);
                free(AComp);  // Résultat temporaire, libéré après affichage
            }

        /* -------------------------------------------------------------- */
        /*  MINIMISER : calcule l'automate minimal depuis l'AFDC           */
        /*  Utilise l'algorithme de partition (classes d'équivalence)      */
        /*  Nécessite que la déterminisation ait été effectuée             */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "minimiser") == 0) {

            if (!afdc_pret) {
                printf("Lance d'abord 'determiniser' pour obtenir l'AFDC.\n");
            } else {
                /* Libère un éventuel AFDCM calculé précédemment */
                if (AFDCM != NULL && AFDCM != AFDC) free(AFDCM);

                AFDCM = minimisation(AFDC);
                afficher_automate_minimal(AFDCM, AFDC,
                                         get_correspondance(), AFDC->nb_etats);
            }

        /* -------------------------------------------------------------- */
        /*  RECONNAITRE : teste si des mots sont reconnus par l'AFDC       */
        /*  L'utilisateur saisit des mots un par un.                       */
        /*  Taper "fin" pour revenir au menu.                              */
        /*  Nécessite que la déterminisation ait été effectuée             */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "reconnaitre") == 0) {

            if (!afdc_pret) {
                printf("Lance d'abord 'determiniser' pour obtenir l'AFDC.\n");
            } else {
                printf("(tape 'fin' pour revenir au menu)\n");

                /* Vide le buffer d'entrée avant de lire les mots */
                while (getchar() != '\n');

                lire_mot(mot);
                while (strcmp(mot, "fin") != 0) {
                    if (reconnaitre_mot(AFDC, mot))
                        printf(" \"%s\" : OUI\n\n", mot);
                    else
                        printf(" \"%s\" : NON\n\n", mot);
                    lire_mot(mot);
                }
                printf("Retour au menu.\n");
            }

        /* -------------------------------------------------------------- */
        /*  CHANGER : charge un nouvel automate depuis un fichier          */
        /*  Libère toute la mémoire allouée avant de recharger             */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "changer") == 0) {

            /* Libère dans l'ordre inverse de création pour éviter les fuites */
            if (AFDCM != NULL && AFDCM != AFDC) free(AFDCM);
            if (AFDC  != NULL && AFDC  != AF)   free(AFDC);
            free(AF);

            AFDC     = NULL;
            AFDCM    = NULL;
            afdc_pret = 0;

            printf("Quel automate veux tu utiliser ? : ");
            scanf("%d", &choix);
            sprintf(chemin, "../Automates/automate%d.txt", choix);
            AF = lire_automate_sur_fichier(chemin);

        /* -------------------------------------------------------------- */
        /*  SORTIR : libère toute la mémoire et quitte proprement          */
        /* -------------------------------------------------------------- */
        } else if (strcmp(commande, "sortir") == 0) {

            if (AFDCM != NULL && AFDCM != AFDC) free(AFDCM);
            if (AFDC  != NULL && AFDC  != AF)   free(AFDC);
            free(AF);

            printf("Au revoir !\n");
            return 0;

        /* -------------------------------------------------------------- */
        /*  Commande inconnue                                              */
        /* -------------------------------------------------------------- */
        } else {
            printf("Commande non reconnue. Retape une commande du menu.\n");
        }
    }
}