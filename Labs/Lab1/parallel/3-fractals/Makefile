
# ----------------------------------------------------------------------------
# Déclaration de variables
# ----------------------------------------------------------------------------

# la commande $(wildcard src/*.c) nous renvoie la liste des fichiers '*.c' dans le dossier source
SRC := $(wildcard *.c)
# On prend tous les fichiers src/*.c dans la variable SRC, on remplace par build/*.o
OBJ := $(patsubst %.c,%.o,$(SRC))
# Le nom du programme final
BIN := fractals
LOG := $(BIN).log
CSV := $(BIN).csv
PGM := *.pgm

# Nombre de threads à utiliser 
THREAD_NUM = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20

# Notez ici les paramètres de compilation
CFLAGS := -O0 -g 	# pas d'optimisation, ajouter les symboles de debug
CFLAGS += -Iinc/ 	# headers .h dans inc/
CFLAGS += -Wall		# afficher tous les warnings

# Notez ici les paramètres de liaison (librairies nécessaires)
LDFLAGS := -lm -lpthread	

# Commandes utilisées pour compiler & linker
CC := gcc
LD := gcc

# ----------------------------------------------------------------------------
# Règles de compilation
# ----------------------------------------------------------------------------

# Structure générale d'une règle: pour construire 'cible', il faut d'abord
# construire ou récupérer plusieurs prérequis, puis appliquer une ou plusieurs 
# commandes.
#
# cible: dépendance1 dépendance2 ...
# 	commande1 (ligne commence par TAB ; attention pas d'espace!)
#	commande2
# 

# Quelques variables automatiques dans make:
# % : pattern matching
# $@ : la cible de la règle en cours
# $^ : toutes les dépendances listées
# $< : la 1° dépendance listée

# Règle de compilation pour le programme principal: il faut avoir préalablement
# compilé tous les objets, puis les assembler sans oublier les librairies
$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
 
run: $(BIN)
	time -p -a ./$(BIN) 1 < params.txt

# pour nettoyer: supprimer les fichiers générés
clean:
	rm -f $(OBJ) $(BIN) $(LOG) $(PGM) $(CSV)

# build/tutorial.o dépend du fichier src/tutorial.c
# pour compiler: gcc -O0 -g -Iinc/ -Wall -o build/tutorial.o -c src/tutorial.c
build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

# pour mesurer le temps en variant le nombre de threads num de 1 à 20
# for num in [1 2 3 4 5 6 7 8]:
#	time -a -o ./tutorial.log ./tutorial num
time: $(BIN)
	rm -f $(LOG)
	for nb_thread in $(THREAD_NUM); do echo "$(BIN) $$nb_thread:" >> $(LOG); time -p -a ./$(BIN) $$nb_thread < params.txt; done
 
time_log: $(BIN)
	rm -f $(LOG)
	for nb_thread in $(THREAD_NUM); do echo "$(BIN) $$nb_thread:" >> $(LOG); time -p -a -o $(LOG) ./$(BIN) $$nb_thread < params.txt; done
 
time_csv: $(BIN)
	rm -f $(CSV)
	echo "nb_thread,real,user,sys" > $(CSV)
	for nb_thread in $(THREAD_NUM); do time -p -a -f "$$nb_thread,%E,%U,%S" -o $(CSV) ./$(BIN) $$nb_thread < params.txt; done
 
# on indique que les cibles des règles 'clean' et 'time' ne sont pas des fichiers
.PHONY: clean time