#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ETATS    100
#define MAX_SYMBOLES 26

typedef struct {
    int  nb_symboles;
    char symboles[MAX_SYMBOLES][5];

    int  nb_etats;
    char etats[MAX_ETATS][30];      /* 30 au lieu de 10 : noms composés type "0.1.3.5" */

    int  nb_initiaux;
    char initiaux[MAX_ETATS][30];

    int  nb_finaux;
    char finaux[MAX_ETATS][30];

    char transitions[MAX_ETATS][MAX_SYMBOLES][MAX_ETATS][30];
    int  nb_transitions[MAX_ETATS][MAX_SYMBOLES];

} Automate;

/* fonctions */
int      est_initial(Automate *A, char *etat);
int      est_final(Automate *A, char *etat);
void     epsilon_fermeture(Automate *A, int src, int fermeture[], int *taille);
void     afficher_automate(Automate *A);
Automate* lire_automate_sur_fichier(const char *nom_fichier);
int      est_standard(Automate *A);
int      est_deterministe(Automate *A);
int      est_complet(Automate *A);
Automate* completion(Automate *A);
Automate* determinisation_et_completion_automate(Automate *A);
Automate* standardisation(Automate *A);
void     afficher_automate_deterministe_complet(Automate *A);

#endif