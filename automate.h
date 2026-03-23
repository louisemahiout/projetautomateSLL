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
void afficher_automate(Automate *A);

#endif

