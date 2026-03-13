# Compilateur et options
# --------------------------
CC = gcc                 # Le compilateur C à utiliser (ici GCC)
CFLAGS = -Wall -MMD      # Options de compilation : -Wall active tous les avertissements, -MMD génère automatiquement un fichier .d pour les dépendances des fichiers .h
LIBS =         # Bibliothèques à lier (ici X11 pour l'affichage graphique)

LIBSDIR =                # Répertoires supplémentaires pour les bibliothèques (vide ici)

# Fichiers du projet
SRC = $(wildcard *.c)    # Liste de tous les fichiers source .c du dossier courant
OBJ = $(SRC:.c=.o)       # Liste des fichiers objets correspondants aux .c
DEP = $(OBJ:.o=.d)       # Fichiers de dépendances générés automatiquement

# Nom de l’exécutable selon l’OS et insertion des bibliotheque différentes
ifeq ($(OS),Windows_NT)
    EXEC = main.exe
	LIBS =
else
    EXEC = main.out
	LIBS = -lX11 -lm  

endif

# Règle par défaut : tout compiler
all: $(EXEC)             # La cible 'all' dépend de l'exécutable
                         # Quand tu tapes 'make', c'est cette règle qui est exécutée par défaut

# Compilation de chaque .c en .o
%.o: %.c                 # Règle générique : comment transformer un .c en .o
	$(CC) $(CFLAGS) -c $< -o $@  
                         # $< = le fichier source (.c)
                         # $@ = le fichier cible (.o)
                         # Cette ligne compile chaque .c en .o

# Edition de liens pour créer l'exécutable
$(EXEC): $(OBJ)          # L'exécutable dépend de tous les objets
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LIBSDIR)  
                         # $@ = nom de l'exécutable (main.out)
                         # $^ = tous les fichiers objets
                         # Ajoute les bibliothèques nécessaires (-lX11)
                         # Cette ligne assemble les .o en exécutable

# Nettoyage des fichiers objets uniquement
# Supprime tous les fichiers objets .o

#Sous linux
clean:
	rm -f *.o             
#Sous Windows
cleanwin:
	powershell -Command "Remove-Item *.o, *.d -ErrorAction SilentlyContinue"

# Nettoyage complet : fichiers objets + exécutable
# Supprime l'exécutable et tous les fichiers objets

# Sous linux
mproper:
	rm -f $(EXEC) *.o *.out *.d      		
# Sous Windows
mproperwin:
	powershell -Command "Remove-Item *.exe, *.o, *.out, *.d -ErrorAction SilentlyContinue"

# Règle pour compiler et lancer le programme
run: $(EXEC)             # La cible 'run' dépend de l'exécutable
	./$(EXEC)             
# Exécute le programme


# Inclure automatiquement les fichiers de dépendances
# Grâce à cette commande, Make sait quels fichiers .o dépendent des .h modifiés
# et ne recompile que ceux qui sont réellement affectés
-include $(DEP)

