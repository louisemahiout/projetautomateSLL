// main.c
#include <stdio.h>
#include <stdlib.h>
#include "automate.h"

/*
Lecture d’un fichier .txt décrivant un automate :

Ligne 1 :
    - Premier nombre : nombre de symboles dans l'alphabet
      puis? les symboles eux-mêmes
    - Sert à construire l’alphabet de l’automate

Ligne 2 :
    - Premier nombre : nombre total d’états
      Puis, les noms/indices des états
    - Sert à initialiser tous les états de l’automate

Ligne 3 :
    - Premier nombre : nombre d’états initiaux
    PUIS? LA liste des états initiaux
    - Sert à définir l’état de départ de l’automate

Ligne 4 :
    - Premier nombre : nombre d’états finaux
    puis la liste des états finaux (ici l’état '7')
    - Sert à définir les états acceptants de l’automate

Ligne 5 :
    - Indique le nombre total de transitions (ici 12)
    - Sert à savoir combien de lignes suivantes contiennent des transitions

Lignes suivantes sont les differents transitions
    - Format de chaque ligne : "état_source + symbole + état_destination"
    - Exemple :
        "0ε1"  -> de l’état 0, sur le symbole ε, on va à l’état 1
        "2b1"  -> de l’état 2, sur le symbole b, on va à l’état 1
Remarques :
    - 'ε' est un symbole spécial pour les transitions epsilon
    - Les premières lignes servent à préparer l’automate avant de lire les transitions
*/

int main() {
    int choix;
    char chemin[256];
    char mot[256];
    char reponse[10];

    //Boucle générale de traitement
    do {
        printf("\nQuel automate veux tu utiliser ?:  ");
        scanf("%d", &choix);

        printf("\n===== Affichage de l'automate =====\n");
        sprintf(chemin, "../Automates/automate%d.txt", choix);

        Automate *AF = lire_automate_sur_fichier(chemin);

        afficher_automate(AF);
        if (est_standard(AF)) {
            printf("L'automate est STANDARDISE\n");
        } else {
            printf("L'automate n'est PAS standardise\n");
            Automate *AS = standardisation(AF);

            printf("\n===== AUTOMATE STANDARDISE =====\n");
            afficher_automate(AS);

            free(AS);
        }

        Automate *AFDC = NULL;

        if (est_deterministe(AF)) {

            printf("Automate deterministe\n");

            if (est_complet(AF)) {
                printf("Automate complet\n");
                AFDC = AF;
            } else {
                printf("Automate non complet\n");
                AFDC = completion(AF);
            }

        } else {

            printf("Automate non deterministe\n");
            AFDC = determinisation_et_completion_automate(AF);
        }

        afficher_automate_deterministe_complet(AFDC);

        // ---- Complémentaire ----
        Automate *AComp = automate_complementaire(AFDC);

        printf("\n===== AUTOMATE COMPLEMENTAIRE =====\n");
        afficher_automate(AComp);

        free(AComp);

        // ---- Minimisation ----
        Automate *AFDCM = minimisation(AFDC);
        afficher_automate_minimal(AFDCM, AFDC,
                                  get_correspondance(), AFDC->nb_etats);
        while (getchar() != '\n');
        lire_mot(mot);
        while (strcmp(mot, "fin") != 0) {
            if (reconnaitre_mot(AFDC, mot))
                printf(" \"%s\" : OUI\n\n", mot);
            else
                printf(" \"%s\" : NON\n\n", mot);
            lire_mot(mot);
        }

        if (AFDCM != AFDC) free(AFDCM);
        if (AFDC != AF) free(AFDC);
        free(AF);

        // Continuer ?
        printf("\nVoulez-vous traiter un autre automate ? (oui/non) : ");
        scanf("%s", reponse);

    } while (strcmp(reponse, "oui") == 0);

    printf("Au revoir !\n");
    return 0;
}
