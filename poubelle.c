
// int phaseRemontee(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor, int *chemin, int max_chemin)
// {
//     int sizeX = laby->sizeX;
//     int sizeY = laby->sizeY;

//     // =================================================================
//     // 1. AFFICHAGE DU TABLEAU DES DISTANCES (Pour le débuggage)
//     // =================================================================
//     printf("\n--- MAP DES DISTANCES (laby->copy_laby_update) ---\n");
//     for (int y = 0; y < sizeY; y++)
//     {
//         for (int x = 0; x < sizeX; x++)
//         {
//             // On affiche un point si la case vaut 0 (inaccessible)
//             if (laby->copy_laby_update[x][y] == 0)
//             {
//                 printf("  . ");
//             }
//             else
//             {
//                 printf("%3d ", laby->copy_laby_update[x][y]);
//             }
//         }
//         printf("\n");
//     }
//     printf("--------------------------------------------------\n\n");

//     // Coordonnées de la destination (le trésor ciblé)
//     int destX = tuiles_tresor->x[tuiles_tresor->num_tresor];
//     int destY = tuiles_tresor->y[tuiles_tresor->num_tresor];

//     printf("[DEBUG] DEPART  (Joueur) : (%d, %d) -> Valeur: %d\n", yek->x, yek->y, laby->copy_laby_update[yek->x][yek->y]);
//     printf("[DEBUG] ARRIVEE (Trésor) : (%d, %d) -> Valeur: %d\n", destX, destY, laby->copy_laby_update[destX][destY]);

//     // On commence la remontée depuis l'arrivée
//     int x = destX;
//     int y = destY;
//     int idx = 0;

//     // Sécurité : si l'arrivée possède une distance de 0, elle est strictement inaccessible
//     if (laby->copy_laby_update[x][y] == 0)
//     {
//         printf("[ERREUR] La destination (%d, %d) vaut 0. Impossible de remonter.\n", x, y);
//         return 0;
//     }

//     // =================================================================
//     // 2. BOUCLE DE REMONTEE (De l'arrivée vers le départ)
//     // =================================================================
//     while (!(x == yek->x && y == yek->y))
//     {
//         int current_value = laby->copy_laby_update[x][y];
//         int found = 0;

//         printf("[REMONTEE] Case actuelle: (%d, %d) avec distance = %d\n", x, y, current_value);

//         // On cherche un voisin qui possède la valeur (distance actuelle - 1)
//         for (int dir = 0; dir < 4; dir++)
//         {
//             int nx, ny;

//             // Vérifie si le voisin est accessible (murs + limites de la grille)
//             // voisin_accessible(laby, x, y, dir, &nx, &ny);
//             if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
//                 continue;

//             // Si ce voisin valide a bien la valeur inférieure
//             if (laby->copy_laby_update[nx][ny] == current_value - 1)
//             {
//                 if (idx < max_chemin)
//                 {
//                     chemin[idx++] = dir; // On enregistre la direction
//                 }

//                 // On se déplace sur cette case voisine
//                 x = nx;
//                 y = ny;
//                 found = 1;
//                 break; // Quitte la boucle des directions pour passer à la case suivante
//             }
//         }

//         // Si aucun voisin avec (current_value - 1) n'a été trouvé : blocage logique
//         if (!found)
//         {
//             printf("[ERREUR REMONTEE] Bloqué sur la case (%d, %d) de valeur %d. Aucun voisin ne vaut %d\n",
//                    x, y, current_value, current_value - 1);
//             return 0;
//         }
//     }

//     printf("[REMONTEE] Succès ! Arrivé au joueur à la case (%d, %d)\n", x, y);

//     // =================================================================
//     // 3. INVERSION DU CHEMIN
//     // =================================================================
//     // Le chemin a été construit à l'envers (Arrivée -> Départ)
//     // On l'inverse pour obtenir l'ordre logique (Départ -> Arrivée)
//     for (int i = 0; i < idx / 2; i++)
//     {
//         int tmp = chemin[i];
//         chemin[i] = chemin[idx - 1 - i];
//         chemin[idx - 1 - i] = tmp;
//     }

//     return idx; // Renvoie la taille finale du chemin trouvé
// }




// void simulate_chemin_court()
// {
//     copie_laby(&laby);

//     t_case_strat chemin;
//     bool chemin_existe = phaseExpansion(&laby, &yek, &tuiles_tresor);
//     if (chemin_existe)
//     {
//         printf("Chemin trouvé !!! B)\n");
//     }
//     else
//     {
//         printf("Pas de chemin possible entre départ et arrivée\n");
//     }

//     int len = phaseRemontee(&laby, &yek, &tuiles_tresor, &chemin, 500);

//     if (len <= 0)
//     {
//         printf("Erreur remontée\n");
//     }
//     else
//     {
//         printf("La longueur totale est de  %d\n", len);
//     }
// }

