#!/bin/bash

N_PROCESS=10
PROJET="./stat"
NOM="TGVinhStatV7"
DIR=/home/vinh/Documents/Programmation/TTRProjet/LogStat/V7

mkdir -p "$DIR"

# Lancer les processus
for i in $(seq 0 $((N_PROCESS-1))); do
  NOMI="$NOM$i"
  "$PROJET" "$NOMI" &
done

wait

echo "Collecting results"

# Télécharger les logs
for i in $(seq 0 $((N_PROCESS-1))); do
  NOMI="$NOM$i"
  wget -P "$DIR" "http://82.29.170.160:8889/logs/player/$NOMI"
done

wait

# Analyse des logs
cd "$DIR" || exit 1
grep "^Total score:" $NOM* > lignes.txt
awk 'NR % 2 == 1' lignes.txt > total.txt

echo "Fini!"
