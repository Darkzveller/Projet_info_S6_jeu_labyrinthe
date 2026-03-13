 /*
+------------------------------------------------------------------------------------+
|                                                                                    |
|                                                                                    |
|                                                                                    |
|   ___      _______  _______  __   __  ______    ___   __    _  _______  __   __    |
|  |   |    |   _   ||  _    ||  | |  ||    _ |  |   | |  |  | ||       ||  | |  |   |
|  |   |    |  |_|  || |_|   ||  |_|  ||   | ||  |   | |   |_| ||_     _||  |_|  |   |
|  |   |    |       ||       ||       ||   |_||_ |   | |       |  |   |  |       |   |
|  |   |___ |       ||  _   | |_     _||    __  ||   | |  _    |  |   |  |       |   |
|  |       ||   _   || |_|   |  |   |  |   |  | ||   | | | |   |  |   |  |   _   |   |
|  |_______||__| |__||_______|  |___|  |___|  |_||___| |_|  |__|  |___|  |__| |__|   |
|                                                                                    |
|                                                                                    |
|                                                                                    |
| API to play to classic Labyrinth game                                              |
| (with the Coding Game Server)                                                      |
|                                                                                    |
|                                                                                    |
+------------------------------------------------------------------------------------+


Authors: T. Hilaire
Licence: GPL

File: labyrinthAPI.h
	Contains the client API for the Labyrinth game
	-> based on clientAPI.h

Copyright 2021 T. Hilaire
*/


#ifndef __API_CLIENT_LABYRINTH__
#define __API_CLIENT_LABYRINTH__
#include "clientAPI.h"


/* -------------------------------------
 * Initialize connection with the server
 * Quit the program if the connection to the server 
 * cannot be established
 *
 * Parameters:
 * - serverName: (string) address of the server 
 *   (it should be "vps-1a2cee88.vps.ovh.net" if the server runs there)
 * - port: (int) port number used for the connection
 * - name: (string) name of this bot (max 20 characters)
 */
void connectToServer(const char* serverName, int port, char* name);


/* ----------------------------------
 * Close the connection to the server
 * to do, because we are polite
 *
 * Parameters: None
*/
void closeConnection();


/* ------------------------------------------------------------------------------
 * Wait for a Game, and retrieve its name and first data (size of the labyrinth)
 *
 * Parameters:
 * - gameType: string (max 200 characters) type of the game we want to play (empty string for regular game)
 * - labyrinthName: string (max 50 characters), corresponds to the game name (filled by the function)
 * - sizeX, sizeY: sizes of the labyrinth (filled by the function)
 *
 * gameType is a string like "<GAME> key1=value1 key2=value1 ..."
 * - It indicates the type of the game you want to plys
 *   it could be "TRAINING <BOT>" to play against bot <BOT>
 *   or "TOURNAMENT <xxxx"> to join the tournament <xxxx>
 *   or "" (empty string) to wait for an opponent (decided by the server)
 * - key=value pairs are used for options (each training player has its own options)
 *   invalid keys are ignored, invalid values leads to error
 *   the following options are common to every training player:
 *   - timeout: allows an define the timeout when training (in seconds)
 *   - 'seed': allows to set the seed of the random generator
 *   - 'start': allows to set who starts ('0' or '1')
 *   - 'margin': if set to 1, then add spaces between lines and columns when the labyrinth is displayed
 *   - 'display': if equal to "display", then the labyrinth is not displayed, but instead its data is (5 numbers per tile)
 *
 * The bot name <BOT> could be:
 * - "DONTMOVE" for a player that insert randomly the extra tile and don't move
 * - "RANDOM" for a player that insert randomly the extra tile and move to the next item if reachable
 * - "BASIC" for a player that search for the best insertion in order to move to the next item
 *
 */
void waitForLabyrinth(const char* gameType, char* labyrinthName, int* sizeX, int* sizeY);


/* -------------------------------------
 * Get the labyrinth and tell who starts
 * It fills the char* labyData with the data of the labyrinth
 *
 * Parameters:
 * - labyData: the data of labyrinth (the pointer data MUST HAVE allocated with at least (sizeX*sizeY+5)*11 characters)
 * The data is stored in a string and is composed of sizeX*sizeY tiles, plus the extra tile.
 * Each tile is composed by 5 integers, one of each wall on North, East, South and West (1 if there is a wall, 0 if none), plus the item number (0 if no item)
 *
 * Returns 0 if you begin, or 1 if the opponent begins
 */
 int getLabyrinth(char* labyData);



/* ----------------------
 * Get the opponent move
 *
 * Parameters:
 * - move: a string describing the move - filled by the function
 * - msg: a message (string) filed by the function
 *      -> returns the tile (5 integers) and the next item to be reached
 *      -> in case of winning/losing move, it contains a message explaining why the game ends
 * these two strings must be allocated accordingly
 *
 * Returns:
 * - NORMAL_MOVE for normal move,
 * - WINNING_MOVE for a winning move, -1
 * - LOOSING_MOVE for a losing (or illegal) move
 * - this code is relative to the opponent (WINNING_MOVE if HE wins, ...)
 */
 t_return_code getMove(char* move, char* msg);



/* -----------
 * Send a move
 *
 * Parameters:
 * - move: a string describing the move
 * - msg: a message (string) filed by the function
 *      -> returns the tile (5 integers) and the next item to be reached
 *      -> in case of winning/losing move, it contains a message explaining why the game ends
 * these last string must be allocated accordingly
 *
 * Returns a return_code
 * NORMAL_MOVE for normal move,
 * WINNING_MOVE for a winning move, -1
 * LOSING_MOVE for a losing (or illegal) move
 * this code is relative to your program (WINNING_MOVE if YOU win, ...)
 */
t_return_code sendMove(const char* move, char* msg);



/* ----------------------
 * Display the labyrinth
 * in a pretty way (ask the server what to print)
 */
void printLabyrinth();



/* ----------------------------
 * Send a comment to the server
 *
 * Parameters:
 * - comment: (string) comment to send to the server (max 100 char.)
 */
void sendComment(const char* comment);



#endif
