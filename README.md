# Projet Automates 

## Description

Ce projet implémente en C un programme complet de traitement d'automates finis.
Il permet de lire un automate depuis un fichier, d'effectuer plusieurs transformations
et de reconnaître des mots sur l'automate déterministe complet obtenu.

---

## Structure du projet

```
projetautomateSLL/
│
├── main.c              # Programme principal
├── automate.c          # Implémentation des fonctions
├── automate.h          # Déclarations et structure Automate
│
└── Automates/          # Dossier contenant les fichiers automates
    ├── automate1.txt
    ├── automate18.txt
    ├── automate31.txt
    ├── automate32.txt
    └── ...
```

---

## Format du fichier automate (.txt)

```
nombres de symboles symboles1  symboles2  ...
nombres d'états  états1  états2 ...
nombres d'états initiaux etat initial
nombres d'états finaux etat final
nombres de transitions
<src><symbole><dest>
...
```

### Exemple (automate18.txt)

```
4 a b c d
5 0 1 2 3 4
1 1
1 0
13
1a2
1b3
1c4
1d0
2a2
2b3
2c4
2d0
3b3
3c4
3d0
4c4
4d0
```

> Les transitions ε sont encodées en UTF-8 (2 octets : `0xCE 0xB5`).

---

## Fonctionnalités

### Lecture et affichage
| Fonction | Description |
|---|---|
| `lire_automate_sur_fichier(fichier)` | Lit un automate depuis un fichier `.txt` |
| `afficher_automate(A)` | Affiche la table de transitions |
| `afficher_automate_deterministe_complet(A)` | Affiche l'automate déterministe complet |
| `afficher_automate_minimal(AFDCM, AFDC, correspondance, nb_etats)` | Affiche l'automate minimisé |

### Propriétés
| Fonction | Description |
|---|---|
| `est_initial(A, etat)` | Vérifie si un état est initial |
| `est_final(A, etat)` | Vérifie si un état est final |
| `est_standard(A)` | Vérifie si l'automate est standardisé |
| `est_deterministe(A)` | Vérifie si l'automate est déterministe |
| `est_complet(A)` | Vérifie si l'automate est complet |

### Transformations
| Fonction | Description |
|---|---|
| `standardisation(A)` | Crée un nouvel état initial `I` sans transition entrante |
| `completion(A)` | Ajoute un état puits `P` pour les transitions manquantes |
| `determinisation_et_completion_automate(A)` | Déterminise (algorithme des sous-ensembles) et complète |
| `automate_complementaire(A)` | Inverse les états finaux et non finaux |
| `minimisation(AFDC)` | Minimise l'automate déterministe complet (algorithme des classes) |
| `epsilon_fermeture(A, src, fermeture, taille)` | Calcule la fermeture ε d'un état |

### Minimisation
| Fonction | Description |
|---|---|
| `get_correspondance()` | Retourne le tableau de correspondance état → classe |
| `get_nb_classes()` | Retourne le nombre de classes de la partition finale |

### Reconnaissance
| Fonction | Description |
|---|---|
| `lire_mot(mot)` | Lit un mot entier au clavier avec `fgets` |
| `reconnaitre_mot(A, mot)` | Retourne `1` (OUI) ou `0` (NON) sur un automate **déterministe complet**.|

---

---

## Utilisation

```
Quel automate tu veux utiliser ? : 18

===== AUTOMATE INITIAL =====
...
➡️ L'automate est STANDARDISE / n'est PAS standardise
✔️ / ❌ Automate déterministe
✔️ / ❌ Automate complet

===== AUTOMATE DETERMINISTE COMPLET =====
...

===== AUTOMATE COMPLEMENTAIRE =====
...

===== RECONNAISSANCE DE MOTS =====
(tapez 'fin' pour arreter la reconnaissance)

Entrez un mot : ad
 "ad" : OUI

Entrez un mot : fin

Voulez-vous traiter un autre automate ? (oui/non) : non
Au revoir !
```

---

## Exemples de mots reconnus

| Automate | Mots acceptés | Mots rejetés |
|---|---|---|
| automate18 | `d`, `ad`, `bd`, `cd`, `aad` | `a`, `b`, `dd` |

---

## Auteur
Lindsay ELLEPO
Sarra GHEDIRA
Louise MAHIOUT
