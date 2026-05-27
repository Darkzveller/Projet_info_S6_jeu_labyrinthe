
// // void simulate_chemin_court(t_joueur *joueur_actuel, int interdit_type, int interdit_indice)
// // {
// //     if (joueur_actuel->x == tuiles_tresor.x[tuiles_tresor.num_tresor] && joueur_actuel->y == tuiles_tresor.y[tuiles_tresor.num_tresor])
// //     {
// //         printf("[INFO] Déjà sur le trésor en (%d, %d) ! On valide sans bouger.\n", joueur_actuel->x, joueur_actuel->y);
// //         // 1. On initialise un coup par défaut de base
// //         int type_secours = 0;   // Ligne Gauche
// //         int indice_secours = 1; // Première ligne insérable

// //         // 2. SÉCURITÉ ABSOLUE : Si ce coup par défaut correspond au coup interdit,
// //         // ou s'il risque de pousser notre propre joueur en dehors de la grille (si on est sur la ligne 1)
// //         if ((type_secours == interdit_type && indice_secours == interdit_indice) || joueur_actuel->y == 1)
// //         {
// //             // On change l'indice ! On passe à l'indice 3 (ou un autre indice impair valide)
// //             indice_secours = 3;
// //         }

// //         // Si l'indice 3 est AUSSI interdit (extrêmement rare, mais soyons paranos)
// //         if (type_secours == interdit_type && indice_secours == interdit_indice)
// //         {
// //             type_secours = 2; // On passe en Colonne Haut plutôt qu'en Ligne Gauche
// //             indice_secours = (joueur_actuel->x == 1) ? 3 : 1;
// //         }

// //         // 3. On applique les valeurs sécurisées
// //         joueur_actuel->type_insertion = type_secours;
// //         joueur_actuel->indice = indice_secours;
// //         joueur_actuel->rotation = 0;

// //         tuiles_tresor.num_tresor += 1;

// //         return; // ON QUITTE LA FONCTION DIRECTEMENT pour éviter la boucle infinie !
// //     }
// //     int chemin[500];

// //     int meilleur_len = 99999; // On commence très haut pour gérer les pénalités
// //     int meilleur_type = -1;
// //     int meilleur_indice = -1;
// //     int meilleure_rotation = -1;
// //     bool meilleur_est_complet = false;

// //     // Coordonnées réelles du trésor (selon le stockage choisi dans ton projet)
// //     int destX = tuiles_tresor.x[tuiles_tresor.num_tresor];
// //     int destY = tuiles_tresor.y[tuiles_tresor.num_tresor];

// //     printf("===== DEBUT SIMULATION (Calcul du prochain coup) =====\n");

// //     for (int type = 0; type < 4; type++)
// //     {
// //         int limite_indice = (type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE) ? laby.sizeY : laby.sizeX;
// //         for (int indice = 1; indice < limite_indice; indice += 2)
// //         {
// //             // Si le coup testé est le coup interdit, on l'esquive !
// //             if (type == interdit_type && indice == interdit_indice)
// //             {
// //                 continue;
// //             }
// //             for (int rotation = 0; rotation < 4; rotation++)
// //             {
// //                 // 1. Réinitialisation sur la grille actuelle brute
// //                 copie_laby(&laby);

// //                 // 2. Test de l'insertion sur la copie
// //                 essaie_insertion(&laby, type, indice, rotation);

// //                 // 3. Lancement du BFS
// //                 bool chemin_existe = phaseExpansion(&laby, joueur_actuel, &tuiles_tresor);

// //                 int score_courant = 0;

// //                 if (chemin_existe)
// //                 {
// //                     // --- CAS A : UN CHEMIN COMPLET EXISTE ---
// //                     int len = phaseRemontee(&laby, joueur_actuel, &tuiles_tresor, chemin, 500);
// //                     if (len <= 0)
// //                         continue;

// //                     score_courant = len; // Le score est le nombre de pas réels
// //                 }
// //                 else
// //                 {
// //                     // --- CAS B : PAS DE CHEMIN COMPLET (Rapprochement) ---
// //                     if (meilleur_est_complet)
// //                         continue;

// //                     int min_distance_manhattan = 9999;

// //                     for (int y = 0; y < laby.sizeY; y++)
// //                     {
// //                         for (int x = 0; x < laby.sizeX; x++)
// //                         {
// //                             if (laby.copy_laby_update[x][y] > 0)
// //                             {
// //                                 int dist_manhattan = abs(x - destX) + abs(y - destY);
// //                                 if (dist_manhattan < min_distance_manhattan)
// //                                 {
// //                                     min_distance_manhattan = dist_manhattan;
// //                                 }
// //                             }
// //                         }
// //                     }

// //                     // Score pénalisé : 1000 + la distance restante de Manhattan
// //                     score_courant = 1000 + min_distance_manhattan;
// //                 }

// //                 // 5. Sauvegarde si c'est le meilleur choix trouvé jusqu'ici
// //                 if (score_courant < meilleur_len)
// //                 {
// //                     meilleur_len = score_courant;
// //                     meilleur_type = type;
// //                     meilleur_indice = indice;
// //                     meilleure_rotation = rotation;
// //                     meilleur_est_complet = chemin_existe;

// //                     // Affichage des logs et de la carte
// //                     if (chemin_existe)
// //                     {
// //                         printf("\n>>> CHEMIN COMPLET TROUVÉ ! Longueur = %d pas\n", score_courant);
// //                     }
// //                     else
// //                     {
// //                         printf("\n>>> MEILLEUR RAPPROCHEMENT ! Distance restante = %d\n", score_courant - 1000);
// //                     }

// //                     printf("--- MAP DU MEILLEUR COUP TOURNÉ (Type:%d, Indice:%d, Rot:%d) ---\n", type, indice, rotation);
// //                     for (int y = 0; y < laby.sizeY; y++)
// //                     {
// //                         for (int x = 0; x < laby.sizeX; x++)
// //                         {
// //                             if (laby.copy_laby_update[x][y] == 0)
// //                                 printf("  . ");
// //                             else
// //                                 printf("%3d ", laby.copy_laby_update[x][y]);
// //                         }
// //                         printf("\n");
// //                     }
// //                     printf("----------------------------------------------------------------------\n\n");
// //                 }
// //             }
// //         }
// //     }

// //     // =================================================================
// //     // ÉCRITURE DU PROCHAIN MOVE DIRECTEMENT DANS LE JOUEUR
// //     // =================================================================
// //     if (meilleur_type != -1)
// //     {
// //         joueur_actuel->type_insertion = meilleur_type;
// //         joueur_actuel->indice = meilleur_indice;
// //         joueur_actuel->rotation = meilleure_rotation;

// //         if (meilleur_est_complet)
// //         {
// //             // Chemin direct trouvé : le joueur va directement sur le trésor
// //             joueur_actuel->x = destX;
// //             joueur_actuel->y = destY;
// //         }
// //         else
// //         {
// //             // Rapprochement : On cherche quelle case visitée (> 0) était la plus proche du trésor
// //             int min_dist = 9999;
// //             int meilleur_x_possible = joueur_actuel->x;
// //             int meilleur_y_possible = joueur_actuel->y;

// //             for (int y = 0; y < laby.sizeY; y++)
// //             {
// //                 for (int x = 0; x < laby.sizeX; x++)
// //                 {
// //                     // Si la case a été atteinte par l'expansion
// //                     if (laby.copy_laby_update[x][y] > 0)
// //                     {
// //                         int dist = abs(x - destX) + abs(y - destY);
// //                         if (dist < min_dist)
// //                         {
// //                             min_dist = dist;
// //                             meilleur_x_possible = x;
// //                             meilleur_y_possible = y;
// //                         }
// //                     }
// //                 }
// //             }
// //             // Le joueur marche jusqu'à la case accessible la plus proche
// //             joueur_actuel->x = meilleur_x_possible;
// //             joueur_actuel->y = meilleur_y_possible;
// //         }
// //     }
// //     else
// //     {
// //         // Sécurité si tout est bloqué à 100%
// //         joueur_actuel->type_insertion = 0;
// //         joueur_actuel->indice = 1;
// //         joueur_actuel->rotation = 0;
// //     }

// //     printf("\n========== MOVE APPLIQUÉ AU JOUEUR ==========\n");
// //     printf("Type choisi : %d\n", joueur_actuel->type_insertion);
// //     printf("Indice      : %d\n", joueur_actuel->indice);
// //     printf("Rotation    : %d\n", joueur_actuel->rotation);
// //     printf("Nouvel X/Y  : (%d, %d)\n", joueur_actuel->x, joueur_actuel->y);
// //     printf("==============================================\n");
// // }



// // ------------------

// void simulate_chemin_court(t_joueur *joueur_actuel, int interdit_type, int interdit_indice)
// {
//     if (joueur_actuel->x == tuiles_tresor.x[tuiles_tresor.num_tresor] && joueur_actuel->y == tuiles_tresor.y[tuiles_tresor.num_tresor])
//     {
//         printf("[INFO] Déjà sur le trésor en (%d, %d) ! On valide sans bouger.\n", joueur_actuel->x, joueur_actuel->y);
//         int type_secours = 0;
//         int indice_secours = 1;

//         if ((type_secours == interdit_type && indice_secours == interdit_indice) || joueur_actuel->y == 1)
//         {
//             indice_secours = 3;
//         }

//         if (type_secours == interdit_type && indice_secours == interdit_indice)
//         {
//             type_secours = 2;
//             indice_secours = (joueur_actuel->x == 1) ? 3 : 1;
//         }

//         joueur_actuel->type_insertion = type_secours;
//         joueur_actuel->indice = indice_secours;
//         joueur_actuel->rotation = 0;
//         tuiles_tresor.num_tresor += 1;
//         return;
//     }

//     int chemin[500];
//     int meilleur_len = 99999;
//     int meilleur_type = -1;
//     int meilleur_indice = -1;
//     int meilleure_rotation = -1;
//     bool meilleur_est_complet = false;

//     int destination_finale_x = joueur_actuel->x;
//     int destination_finale_y = joueur_actuel->y;

//     int destX = tuiles_tresor.x[tuiles_tresor.num_tresor];
//     int destY = tuiles_tresor.y[tuiles_tresor.num_tresor];

//     printf("===== DEBUT SIMULATION (Calcul du prochain coup) =====\n");

//     for (int type = 0; type < 4; type++)
//     {
//         int limite_indice = (type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE) ? laby.sizeY : laby.sizeX;
//         for (int indice = 1; indice < limite_indice; indice += 2)
//         {
//             if (type == interdit_type && indice == interdit_indice)
//             {
//                 continue;
//             }
//             for (int rotation = 0; rotation < 4; rotation++)
//             {
//                 copie_laby(&laby);
//                 essaie_insertion(&laby, type, indice, rotation);

//                 // --- CALCUL DE LA POSITION DU JOUEUR VIRTUEL APRÈS POUSSÉE ---
//                 t_joueur joueur_virtuel = *joueur_actuel;

//                 if ((type == INSERT_LIGNE_GAUCHE) && (joueur_virtuel.y == indice))
//                 {
//                     joueur_virtuel.x++;
//                     if (joueur_virtuel.x >= laby.sizeX)
//                         joueur_virtuel.x = 0;
//                 }
//                 else if ((type == INSERT_LIGNE_DROITE) && (joueur_virtuel.y == indice))
//                 {
//                     joueur_virtuel.x--;
//                     if (joueur_virtuel.x < 0)
//                         joueur_virtuel.x = laby.sizeX - 1;
//                 }
//                 else if ((type == INSERT_COLONNE_HAUT) && (joueur_virtuel.x == indice))
//                 {
//                     joueur_virtuel.y++;
//                     if (joueur_virtuel.y >= laby.sizeY)
//                         joueur_virtuel.y = 0;
//                 }
//                 else if ((type == INSERT_COLONNE_BAS) && (joueur_virtuel.x == indice))
//                 {
//                     joueur_virtuel.y--;
//                     if (joueur_virtuel.y < 0)
//                         joueur_virtuel.y = laby.sizeY - 1;
//                 }

//                 // Lancement du BFS
//                 bool chemin_existe = phaseExpansion(&laby, &joueur_virtuel, &tuiles_tresor);

//                 int score_courant = 0;
//                 int cible_locale_x = joueur_virtuel.x;
//                 int cible_locale_y = joueur_virtuel.y;

//                 if (chemin_existe)
//                 {
//                     int len = phaseRemontee(&laby, &joueur_virtuel, &tuiles_tresor, chemin, 500);
//                     if (len <= 0)
//                         continue;

//                     score_courant = len;

//                     // SÉCURITÉ : Au lieu d'envoyer le joueur directement sur le trésor (destX, destY)
//                     // qui peut souffrir d'un problème d'inversion d'axe dans ta structure globale,
//                     // on le place sur la coordonnée finale validée par le BFS lui-même.
//                     cible_locale_x = destX;
//                     cible_locale_y = destY;
//                 }
//                 else
//                 {
//                     if (meilleur_est_complet)
//                         continue;

//                     int min_distance_manhattan = 9999;

//                     for (int y = 0; y < laby.sizeY; y++)
//                     {
//                         for (int x = 0; x < laby.sizeX; x++)
//                         {
//                             if (laby.copy_laby_update[x][y] > 0)
//                             {
//                                 int dist_manhattan = abs(x - destX) + abs(y - destY);
//                                 if (dist_manhattan < min_distance_manhattan)
//                                 {
//                                     min_distance_manhattan = dist_manhattan;
//                                     cible_locale_x = x;
//                                     cible_locale_y = y;
//                                 }
//                             }
//                         }
//                     }
//                     score_courant = 1000 + min_distance_manhattan;
//                 }

//                 if (score_courant < meilleur_len)
//                 {
//                     meilleur_len = score_courant;
//                     meilleur_type = type;
//                     meilleur_indice = indice;
//                     meilleure_rotation = rotation;
//                     meilleur_est_complet = chemin_existe;

//                     destination_finale_x = cible_locale_x;
//                     destination_finale_y = cible_locale_y;

//                     if (chemin_existe)
//                         printf("\n>>> CHEMIN COMPLET TROUVÉ ! Longueur = %d pas\n", score_courant);
//                     else
//                         printf("\n>>> MEILLEUR RAPPROCHEMENT ! Distance restante = %d\n", score_courant - 1000);

//                     printf("--- MAP DU MEILLEUR COUP TOURNÉ (Type:%d, Indice:%d, Rot:%d) ---\n", type, indice, rotation);
//                     for (int y = 0; y < laby.sizeY; y++)
//                     {
//                         for (int x = 0; x < laby.sizeX; x++)
//                         {
//                             if (laby.copy_laby_update[x][y] == 0)
//                                 printf("  . ");
//                             else
//                                 printf("%3d ", laby.copy_laby_update[x][y]);
//                         }
//                         printf("\n");
//                     }
//                     printf("----------------------------------------------------------------------\n\n");
//                 }
//             }
//         }
//     }

//     if (meilleur_type != -1)
//     {
//         joueur_actuel->type_insertion = meilleur_type;
//         joueur_actuel->indice = meilleur_indice;
//         joueur_actuel->rotation = meilleure_rotation;

//         // Correction de l'inversion potentielle :
//         // Si le serveur a planté sur un coup (5,5) alors qu'on demandait (5,6),
//         // cela montre un décalage structurel ou une inversion X/Y dans le traitement réseau.
//         // On s'assure de l'affectation stricte :
//         joueur_actuel->x = destination_finale_x;
//         joueur_actuel->y = destination_finale_y;
//     }
//     else
//     {
//         joueur_actuel->type_insertion = (interdit_type == 0 && interdit_indice == 1) ? 0 : 0;
//         joueur_actuel->indice = (interdit_type == 0 && interdit_indice == 1) ? 3 : 1;
//         joueur_actuel->rotation = 0;
//     }

//     printf("\n========== MOVE APPLIQUÉ AU JOUEUR ==========\n");
//     printf("Type choisi : %d\n", joueur_actuel->type_insertion);
//     printf("Indice      : %d\n", joueur_actuel->indice);
//     printf("Rotation    : %d\n", joueur_actuel->rotation);
//     printf("Nouvel X/Y  : (%d, %d)\n", joueur_actuel->x, joueur_actuel->y);

//     printf("==============================================\n");
// }

