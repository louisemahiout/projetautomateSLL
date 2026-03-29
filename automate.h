#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ETATS    100
#define MAX_SYMBOLES 26
#define MAX_NOM       60

// Structure représentant un automate fini
typedef struct {
    int  nb_symboles;                         // Nombre de symboles dans l'alphabet
    char symboles[MAX_SYMBOLES][5];           // Liste des symboles (chaînes de 1 à 4 caractères + 'ε')

    int  nb_etats;                            // Nombre total d'états dans l'automate
    char etats[MAX_ETATS][30];                /* Noms des états : jusqu'à 30 caractères pour gérer des
                                                 états composés du type "0.1.3.5" */

    int  nb_initiaux;                         // Nombre d'états initiaux
    char initiaux[MAX_ETATS][30];             // Noms des états initiaux

    int  nb_finaux;                           // Nombre d'états finaux
    char finaux[MAX_ETATS][30];               // Noms des états finaux

    char transitions[MAX_ETATS][MAX_SYMBOLES][MAX_ETATS][30];
    /* Table des transitions :
       transitions[i][j][k] = nom de l'état k atteint
       depuis l'état i avec le symbole j */

    int  nb_transitions[MAX_ETATS][MAX_SYMBOLES];
    /* Nombre de transitions existantes depuis l'état i
       avec le symbole j */
} Automate;

/* fonctions */
int est_initial(Automate *A, char *etat);
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
void lire_mot(char *mot);
int reconnaitre_mot(Automate *A, char *mot);

// Minimisation
Automate* minimisation(Automate *AFDC);
void afficher_automate_minimal(Automate *AFDCM, Automate *AFDC, int correspondance[], int nb_etats_AFDC);
int* get_correspondance(void);
int  get_nb_classes(void);
Automate* automate_complementaire(Automate *A);
#endif


