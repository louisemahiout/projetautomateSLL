//
// Created by louis on 23/03/2026.
//

#ifndef AUTOMATE_H
#define AUTOMATE_H


#define MAX_ETATS 100
#define MAX_SYMBOLES 26
#define MAX_TRANSITIONS 10

// Structure automate
typedef struct {
    int nbSymboles;
    int nbEtats;

    int nbInitiaux;
    int initiaux[MAX_ETATS];

    int nbFinaux;
    int finaux[MAX_ETATS];

    int table[MAX_ETATS][MAX_SYMBOLES][MAX_TRANSITIONS];
    int nbTransitions[MAX_ETATS][MAX_SYMBOLES];

} Automate;

// Fonctions

void lire_automate(const char *nomFichier, Automate *A);
int est_initial(Automate *A, int etat);
int est_final(Automate *A, int etat);
int est_final(Automate *A, int etat);
void afficher_ligne(Automate *A, int i, int largeur);
void afficher_automate(Automate *A);
int est_standard(Automate *A);
int est_deterministe(Automate *A);
int est_complet(Automate *A);
//void standardisation(Automate *A);


#endif

