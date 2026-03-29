#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ETATS 100
#define MAX_SYMBOLES 26

/* MAX_NOM : taille max d'un nom d'état.
 * Les états composés après déterminisation peuvent avoir des noms longs :
 * ex "0.1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21" = 55 chars.
 * On prend 60 avec marge. */
#define MAX_NOM 60

typedef struct {
    int nb_symboles;
    char symboles[MAX_SYMBOLES][5]; // ⚠️ support ε

    int nb_etats;
    char etats[MAX_ETATS][MAX_NOM];

    int nb_initiaux;
    char initiaux[MAX_ETATS][MAX_NOM];

    int nb_finaux;
    char finaux[MAX_ETATS][MAX_NOM];

    // transitions
    char transitions[MAX_ETATS][MAX_SYMBOLES][MAX_ETATS][MAX_NOM];
    int nb_transitions[MAX_ETATS][MAX_SYMBOLES];

} Automate;

// fonctions
int est_initial(Automate *A, char *etat);
int est_final(Automate *A, char *etat);
void afficher_automate(Automate *A);
Automate* lire_automate_sur_fichier(const char *nom_fichier);
int est_standard(Automate *A);
int est_deterministe(Automate *A);
int est_complet(Automate *A);
Automate* completion(Automate *A);
Automate* determinisation_et_completion_automate(Automate *A);
Automate* standardisation(Automate *A);
void afficher_automate_deterministe_complet(Automate *A);
void epsilon_fermeture(Automate *A, int src, int fermeture[], int *taille);

#endif

// Minimisation
Automate* minimisation(Automate *AFDC);
void afficher_automate_minimal(Automate *AFDCM, Automate *AFDC, int correspondance[], int nb_etats_AFDC);
int* get_correspondance(void);
int  get_nb_classes(void);