#include <X11/Xlib.h>	/* pour XLib */
#include <X11/keysym.h> /* pour les touches XLib */
#include <X11/Xatom.h>	/* pour le message WM_DELETE_WINDOW */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "affichage.h"

/* taille des cases du labyrinthe augmentée pour la visibilité */
#define CASE 60
#define EPAIS_MUR 20

/* Variables X11 (identiques) */
Display *display;
int screen;
GC gc;
Window w;
Colormap cmap;
Atom wmDeleteMessage;
Pixmap pix;
Pixmap fond;

/* couleurs (identiques) */
unsigned int couleurs[9];
unsigned int coulFond;
unsigned int coulMur;
unsigned int coulExploration[151];
#define BLANC 0
#define NOIR 1
#define BLEU 2
#define ROUGE 3
#define VERT 4
#define JAUNE 5
#define VIOLET 6

int init = 0;
int aff = 0;
int down = 0;

/* Fonction "alloueCouleur" (Inchangée) */
unsigned int alloueCouleur(int red, int green, int blue)
{
	XColor coul;
	coul.green = green;
	coul.red = red;
	coul.blue = blue;
	XAllocColor(display, cmap, &coul);
	return coul.pixel;
}

/* Fonction 'initAffichage' */
int initAffichage(int TAILLE_X, int TAILLE_Y)
{
	if (init)
	{
		fprintf(stderr, "Erreur (initAffichage) : la fonction initAffichage ne doit être lancée qu'une seule fois!\n");
		return 0;
	}
	if ((display = XOpenDisplay(NULL)) == NULL)
	{
		fprintf(stderr, "Erreur (initAffichage) : connection avec X impossible : X est-il lancé ??\n");
		return 0;
	}
	screen = DefaultScreen(display);
	gc = DefaultGC(display, screen);
	cmap = DefaultColormap(display, screen);

	/* On prévoit une marge de 1 case autour pour la tuile supplémentaire */
	w = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, (TAILLE_X + 2) * CASE, (TAILLE_Y + 2) * CASE, 2, BlackPixel(display, screen), WhitePixel(display, screen));
	XStoreName(display, w, "Labyrinthe API");

	couleurs[BLANC] = alloueCouleur(0xFFFF, 0xFFFF, 0xFFFF);
	couleurs[NOIR] = alloueCouleur(0, 0, 0);
	couleurs[BLEU] = alloueCouleur(0, 0, 0xFFFF);
	couleurs[ROUGE] = alloueCouleur(0xFFFF, 0, 0);
	couleurs[VERT] = alloueCouleur(0, 0xFFFF, 0);
	couleurs[JAUNE] = alloueCouleur(0xFFFF, 0xFFFF, 0);
	couleurs[VIOLET] = alloueCouleur(0xCCCC, 0x4444, 0xFFFF);
	coulFond = alloueCouleur(50000, 50000, 50000);
	coulMur = alloueCouleur(0, 0, 0);

	int prof = XDefaultDepth(display, screen);
	pix = XCreatePixmap(display, RootWindow(display, screen), (TAILLE_X + 2) * CASE, (TAILLE_Y + 2) * CASE, prof);

	XSelectInput(display, w, ExposureMask | KeyPressMask | ButtonPressMask);
	wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, w, &wmDeleteMessage, 1);
	XMapWindow(display, w);

	init = 1;
	return 1;
}

/* Fonction afficheLabyrinthe */
/* Fonction afficheLabyrinthe */
int afficheLabyrinthe(char *labyData, int tempo, int TAILLE_X, int TAILLE_Y,
                      int x1, int y1, int x2, int y2)
{
    int fin_attente = 0;
    int x, y, n, e, s, o, item;
    char *ptr = labyData;
    int offset;

    if (down)
        return 1;
    if (!init)
    {
        fprintf(stderr, "Warning (afficheLabyrinthe) : initAffichage n'a pas encore été exécuté\n");
        return 0;
    }

    /* 1. Effacer le fond (on met tout en blanc) */
    XSetForeground(display, gc, couleurs[BLANC]);
    XFillRectangle(display, pix, gc, 0, 0, (TAILLE_X + 2) * CASE, (TAILLE_Y + 2) * CASE);

    /* 2. Dessin des tuiles du labyrinthe */
    for (y = 0; y < TAILLE_Y; y++)
    {
        for (x = 0; x < TAILLE_X; x++)
        {
            /* On lit les 5 entiers : n, e, s, o (les murs) et item (le trésor) */
            if (sscanf(ptr, "%d %d %d %d %d%n", &n, &e, &s, &o, &item, &offset) >= 5)
            {
                int px = (x + 1) * CASE;
                int py = (y + 1) * CASE;

                /* Fond de la case : Jaune si trésor, sinon Blanc */
                XSetForeground(display, gc, (item > 0) ? couleurs[JAUNE] : couleurs[BLANC]);
                XFillRectangle(display, pix, gc, px, py, CASE, CASE);

                /* DESSIN DES MURS : On ne dessine QUE si la valeur est 1 (le "trait gras") */
                XSetForeground(display, gc, couleurs[NOIR]);
                
                if (n == 1) /* Mur NORD présent */
                    XFillRectangle(display, pix, gc, px, py, CASE, 8);
                if (s == 1) /* Mur SUD présent */
                    XFillRectangle(display, pix, gc, px, py + CASE - 8, CASE, 8);
                if (o == 1) /* Mur OUEST présent */
                    XFillRectangle(display, pix, gc, px, py, 8, CASE);
                if (e == 1) /* Mur EST présent */
                    XFillRectangle(display, pix, gc, px + CASE - 8, py, 8, CASE);

                /* Numéro du trésor */
                if (item > 0)
                {
                    char str[4];
                    sprintf(str, "%d", item);
                    XDrawString(display, pix, gc, px + CASE / 2 - 4, py + CASE / 2 + 4, str, strlen(str));
                }
                
                ptr += offset;
            }
        }
    }

    /* 3. DESSIN DES PIONS */
    /* Joueur 0 en ROUGE */
    XSetForeground(display, gc, couleurs[ROUGE]);
    XFillArc(display, pix, gc, (x1 + 1) * CASE + 15, (y1 + 1) * CASE + 15, 30, 30, 0, 360 * 64);

    /* Joueur 1 en VERT */
    XSetForeground(display, gc, couleurs[VERT]);
    XFillArc(display, pix, gc, (x2 + 1) * CASE + 15, (y2 + 1) * CASE + 15, 30, 30, 0, 360 * 64);

    /* 4. Mise à jour de la fenêtre */
    XCopyArea(display, pix, w, gc, 0, 0, (TAILLE_X + 2) * CASE, (TAILLE_Y + 2) * CASE, 0, 0);
    XFlush(display);

    /* 5. Gestion de la pause (Espace ou Tempo) */
    clock_t debut_time = clock();
    while (!fin_attente)
    {
        if (XPending(display))
        {
            XEvent ev;
            XNextEvent(display, &ev);
            switch (ev.type)
            {
            case Expose:
                XCopyArea(display, pix, w, gc, 0, 0, (TAILLE_X + 2) * CASE, (TAILLE_Y + 2) * CASE, 0, 0);
                break;
            case KeyPress:
                if (XLookupKeysym(&ev.xkey, 0) == XK_space) fin_attente = 2;
                break;
            case ClientMessage:
                if (ev.xclient.data.l[0] == wmDeleteMessage) exit(0);
                break;
            }
        }
        if (tempo && ((clock() - debut_time) * 1000 / CLOCKS_PER_SEC) > tempo)
            fin_attente = 1;
        if (!tempo && fin_attente == 0) usleep(10000);
        if (tempo == 0 && fin_attente == 2) break;
    }
    return (fin_attente == 2);
}























