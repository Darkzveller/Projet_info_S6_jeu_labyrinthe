#include "Variable.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"
#include "affichage.h"
#include "Mouvement.h"
void copie_laby(t_laby *laby)
{
    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {

            laby->copy_laby_update[x][y] = laby->laby_update[x][y];
        }
    }
}
// Renvoie les coordonnées de la case voisine à partir d'une case (x, y) et d'une direction d
// d = 0 : Nord, 1 : Est, 2 : Sud, 3 : Ouest
//  void coord_case_voisine(int coord_x_actu, int coord_y_actu, int direction_voulue, int *coord_x_voisine, int *coord_y_voisine)
#define NORD 0
#define EST 1
#define SUD 2
#define OUEST 3
#define MUR_NORD (1 << SHIFT_BIT_NORD)
#define MUR_EST (1 << SHIFT_BIT_EST)
#define MUR_SUD (1 << SHIFT_BIT_SUD)
#define MUR_OUEST (1 << SHIFT_BIT_OUEST)
bool voisin_accessible(t_laby *laby, int x, int y, int dir, int *nx, int *ny)
{
    static const int dx[4] = {0, 1, 0, -1};
    static const int dy[4] = {-1, 0, 1, 0};
    
    // Détermine les coordonnées de la case voisine
    *nx = x + dx[dir];
    *ny = y + dy[dir];
    
    // Vérifie si on sort du labyrinthe
    if (*nx < 0 || *ny < 0 || *nx >= laby->sizeX || *ny >= laby->sizeY)
    {
        return false;
    }
    
    // 1. Récupération et vérification des murs de la case actuelle
    int cell_actu = laby->copy_laby_update[x][y]; // ou laby_update selon votre structure
    if (dir == NORD  && (cell_actu & MUR_NORD))  return false;
    if (dir == EST   && (cell_actu & MUR_EST))   return false;
    if (dir == SUD   && (cell_actu & MUR_SUD))   return false;
    if (dir == OUEST && (cell_actu & MUR_OUEST)) return false;
    
    // 2. CORRECTION CRITIQUE : Vérification des murs de la case VOISINE
    int cell_voisine = laby->copy_laby_update[*nx][*ny];
    // On vérifie le mur opposé sur la case voisine
    if (dir == NORD  && (cell_voisine & MUR_SUD))   return false;
    if (dir == EST   && (cell_voisine & MUR_OUEST)) return false;
    if (dir == SUD   && (cell_voisine & MUR_NORD))  return false;
    if (dir == OUEST && (cell_voisine & MUR_EST))   return false;
    
    return true;
}

int phaseExpansion(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    int coord_x_arrivee = tuiles_tresor->x[tuiles_tresor->num_tresor+1];
    int coord_y_arrivee = tuiles_tresor->y[tuiles_tresor->num_tresor+1];

    // CORRECTION : pas de '&' ici car laby est déjà un pointeur
    copie_laby(laby);

    int tab[sizeX][sizeY];
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            tab[x][y] = 0;
        }
    }

    int startX = yek->x;
    int startY = yek->y;
    tab[startX][startY] = 1;

    int distance = 1;
    int changed = 1;

    while (!tab[coord_x_arrivee][coord_y_arrivee] && changed)
    {
        changed = 0;

        // CORRECTION : Remplacement de size - 1 par size pour parcourir tout le plateau
        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; x++)
            {
                if (tab[x][y] == distance)
                {
                    for (int dir = 0; dir < 4; dir++)
                    {
                        int nx, ny;
                        if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
                            continue;

                        if (tab[nx][ny] == 0)
                        {
                            tab[nx][ny] = distance + 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
        distance++;
    }

    // Sauvegarde de la carte des distances
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            laby->copy_laby_update[x][y] = tab[x][y];
        }
    }

    return tab[coord_x_arrivee][coord_y_arrivee] > 0;
}
int phaseRemontee(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor, int *chemin, int max_chemin)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    // =================================================================
    // 1. AFFICHAGE DU TABLEAU DES DISTANCES (Pour le débuggage)
    // =================================================================
    printf("\n--- MAP DES DISTANCES (laby->copy_laby_update) ---\n");
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            // On affiche un point si la case vaut 0 (inaccessible)
            if (laby->copy_laby_update[x][y] == 0) {
                printf("  . ");
            } else {
                printf("%3d ", laby->copy_laby_update[x][y]);
            }
        }
        printf("\n");
    }
    printf("--------------------------------------------------\n\n");

    // Coordonnées de la destination (le trésor ciblé)
    int destX = tuiles_tresor->x[tuiles_tresor->num_tresor+1];
    int destY = tuiles_tresor->y[tuiles_tresor->num_tresor+1];
    
    printf("[DEBUG] DEPART  (Joueur) : (%d, %d) -> Valeur: %d\n", yek->x, yek->y, laby->copy_laby_update[yek->x][yek->y]);
    printf("[DEBUG] ARRIVEE (Trésor) : (%d, %d) -> Valeur: %d\n", destX, destY, laby->copy_laby_update[destX][destY]);

    // On commence la remontée depuis l'arrivée
    int x = destX;
    int y = destY;
    int idx = 0;

    // Sécurité : si l'arrivée possède une distance de 0, elle est strictement inaccessible
    if (laby->copy_laby_update[x][y] == 0)
    {
        printf("[ERREUR] La destination (%d, %d) vaut 0. Impossible de remonter.\n", x, y);
        return 0;
    }

    // =================================================================
    // 2. BOUCLE DE REMONTEE (De l'arrivée vers le départ)
    // =================================================================
    while (!(x == yek->x && y == yek->y))
    {
        int current_value = laby->copy_laby_update[x][y];
        int found = 0;

        printf("[REMONTEE] Case actuelle: (%d, %d) avec distance = %d\n", x, y, current_value);

        // On cherche un voisin qui possède la valeur (distance actuelle - 1)
        for (int dir = 0; dir < 4; dir++)
        {
            int nx, ny;
            
            // Vérifie si le voisin est accessible (murs + limites de la grille)
            if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
                continue;

            // Si ce voisin valide a bien la valeur inférieure
            if (laby->copy_laby_update[nx][ny] == current_value - 1)
            {
                if (idx < max_chemin) 
                {
                    chemin[idx++] = dir; // On enregistre la direction
                }
                
                // On se déplace sur cette case voisine
                x = nx;
                y = ny;
                found = 1;
                break; // Quitte la boucle des directions pour passer à la case suivante
            }
        }

        // Si aucun voisin avec (current_value - 1) n'a été trouvé : blocage logique
        if (!found)
        {
            printf("[ERREUR REMONTEE] Bloqué sur la case (%d, %d) de valeur %d. Aucun voisin ne vaut %d\n", 
                    x, y, current_value, current_value - 1);
            return 0; 
        }
    }

    printf("[REMONTEE] Succès ! Arrivé au joueur à la case (%d, %d)\n", x, y);

    // =================================================================
    // 3. INVERSION DU CHEMIN
    // =================================================================
    // Le chemin a été construit à l'envers (Arrivée -> Départ)
    // On l'inverse pour obtenir l'ordre logique (Départ -> Arrivée)
    for (int i = 0; i < idx / 2; i++)
    {
        int tmp = chemin[i];
        chemin[i] = chemin[idx - 1 - i];
        chemin[idx - 1 - i] = tmp;
    }

    return idx; // Renvoie la taille finale du chemin trouvé
}
void simulate_chemin_court()
{
    int chemin[500]; // stockage du chemin

    bool chemin_existe = phaseExpansion(&laby, &yek, &tuiles_tresor);
    if (chemin_existe)
    {
        printf("Chemin trouvé !!! B)\n");
    }
    else
    {
        printf("Pas de chemin possible entre départ et arrivée\n");
    }

    int len = phaseRemontee(&laby, &yek, &tuiles_tresor, chemin, 500);

    if (len <= 0)
    {
        printf("Erreur remontée\n");
    }
    else
    {
        printf("La longueur totale est de  %d\n", len);
    }
}
int main()
{
    printf("caca\n");
#if DEBUG_CONNECT_SERV
    printf("Tentative de connexion au serveur \n");
#endif
    connectToServer(nom_serveur, port_serveur, nom_bot_moi);
#if DEBUG_CONNECT_SERV
    printf("Connecter au serveur de jeu en tant que %s \n", nom_bot_moi);
    printf("Choix partie %s\n", type_partie_choisi[BOT_BOUGE_PAS]);
#endif

    waitForLabyrinth(type_partie_choisi[BOT_BOUGE_PAS], laby.labyrinthName, &laby.sizeX, &laby.sizeY);

#if DEBUG_DATA_STRUCT_LABY
    printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", laby.labyrinthName, laby.sizeX, laby.sizeY);
#endif

    taille_buffer = (laby.sizeX * laby.sizeY + 5) * 11;
    laby.labyData = malloc(taille_buffer * sizeof(char));
    laby.tour_joueur = getLabyrinth(laby.labyData);
    transfer_labydata_to_laby_update(&laby);
    print_laby(&laby, true);

#if DEBUG_DATA_STRUCT_LABY
    printf("Données recu du serveur, mon numéro du joueur : %d\n", laby.tour_joueur);
    printf("Les data du labyrinthe : \n%s\n\n", laby.labyData);
#endif

#if ACTIVE_AFFICHAGE_LABY
    initAffichage(laby.sizeX, laby.sizeY);
#endif

    t_return_code resultat_move = NORMAL_MOVE;

    yek.x = 0;
    yek.y = 0;
    adversaire.x = laby.sizeX - 1;
    adversaire.y = laby.sizeY - 1;

    while (resultat_move == NORMAL_MOVE)
    {
        printf("\nAffichage du labyrinthe :\n");
        printLabyrinth();
        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);
        // printf("labydata \n%d\n", laby.labyData);
        // printf("laby_update \n%d\n", laby.laby_update);

        position_tresor(&laby, &tuiles_tresor);

        if (laby.tour_joueur == 0)
        {
            simulate_chemin_court();

            update_laby(&laby, &yek);
            printf("Entre coup chef : ");
            scanf("%d %d %d %d %d", &yek.type_insertion, &yek.indice, &yek.rotation, &yek.x, &yek.y);
            sprintf(yek.coup_envoi, "%d %d %d %d %d", yek.type_insertion, yek.indice, yek.rotation, yek.x, yek.y);

            resultat_move = sendMove(yek.coup_envoi, laby.message_serveur);
            laby.tour_joueur = 1;
        }
        else
        {
            update_laby(&laby, &adversaire);

            printf("Attente du coup de l'adversaire\n");

            // On récupère le coup de l'adversaire
            resultat_move = getMove(adversaire.coup_recu, laby.message_serveur);
            printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

            laby.tour_joueur = 0;
            // On met à jour les coordonnées de l'adversaire
            sscanf(adversaire.coup_recu, "%d %d %d %d %d", &adversaire.type_insertion, &adversaire.indice, &adversaire.rotation, &adversaire.x, &adversaire.y);

            printf("L'adversaire a joué vers : type : %d indice : %d rotation: %d x : %d y : %d\n", adversaire.type_insertion, adversaire.indice, adversaire.rotation, adversaire.x, adversaire.y);
        }
        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);

        print_laby(&laby, true);
    }

    // Affichage de la raison de la fin (gagné ou autre)
    printf("Fin de la partie ! Raison : %s\n", laby.message_serveur);

    free(laby.labyData);
    closeConnection();
    return 0;
}
