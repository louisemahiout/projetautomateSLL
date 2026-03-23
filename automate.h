//
// Created by louis on 23/03/2026.
//

#ifndef AUTOMATE_H
#define AUTOMATE_H

typedef struct {
    int nb_symboles;
    int nb_etats;

    int nb_initiaux;
    int initiaux[100];

    int nb_finaux;
    int finaux[100];

    int transitions[100][26][100]; // [etat][symbole][liste]
    int nb_transitions[100][26];   // nb d'états par case

} Automate;

Automate lire_automate(char *nom);
void afficher_automate(Automate A);

#endif
