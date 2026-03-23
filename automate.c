#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "automate.h"

Automate lire_automate(char *nom) {

    FILE *f = fopen(nom, "r");
    if (f == NULL) {
        printf("Erreur ouverture fichier : %s\n", nom);
        exit(1);
    }

    Automate A;

    fscanf(f, "%d", &A.nb_symboles);
    fscanf(f, "%d", &A.nb_etats);

    fscanf(f, "%d", &A.nb_initiaux);
    for(int i=0;i<A.nb_initiaux;i++)
        fscanf(f, "%d", &A.initiaux[i]);

    fscanf(f, "%d", &A.nb_finaux);
    for(int i=0;i<A.nb_finaux;i++)
        fscanf(f, "%d", &A.finaux[i]);

    int nb_trans;
    fscanf(f, "%d", &nb_trans);

    // init
    for(int i=0;i<100;i++)
        for(int j=0;j<26;j++)
            A.nb_transitions[i][j] = 0;

    // lecture transitions
    for(int i=0;i<nb_trans;i++) {
        int depart, arrivee;
        char symbole;

        fscanf(f, "%d%c%d", &depart, &symbole, &arrivee);

        int s = symbole - 'a';

        int index = A.nb_transitions[depart][s];
        A.transitions[depart][s][index] = arrivee;
        A.nb_transitions[depart][s]++;
    }

    fclose(f);
    return A;
}

void afficher_automate(Automate A) {

    // alphabet
    printf("      ");
    for(int i=0;i<A.nb_symboles;i++)
        printf("%-8c", 'a'+i);
    printf("\n");

    for(int i=0;i<A.nb_etats;i++) {

        int isInit = 0, isFinal = 0;

        for(int j=0;j<A.nb_initiaux;j++)
            if(A.initiaux[j]==i) isInit=1;

        for(int j=0;j<A.nb_finaux;j++)
            if(A.finaux[j]==i) isFinal=1;

        if(isInit) printf("E ");
        else if(isFinal) printf("S ");
        else printf("  ");

        printf("%d ", i);

        for(int s=0;s<A.nb_symboles;s++) {

            if (A.nb_transitions[i][s] == 0) {
                printf("%-8s", "-");
            } else {
                char buffer[50] = "";
                for(int k=0; k<A.nb_transitions[i][s]; k++) {
                    char temp[10];
                    sprintf(temp, "%d", A.transitions[i][s][k]);
                    strcat(buffer, temp);

                    if(k < A.nb_transitions[i][s]-1)
                        strcat(buffer, ",");
                }
                printf("%-8s", buffer);
            }
        }
        printf("\n");
    }
}