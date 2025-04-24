#!/bin/bash

N_PROCESS=10
PROJET="./stat"
NOM="TGVinhStatRate"
DIR=/home/vinh/Documents/Programmation/TTRProjet/LogStat/Rate

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
grep "^Total score:" "$DIR"/$NOM* > lignes.txt
awk 'NR % 2 == 1' lignes.txt > total.txt

echo "Fini!"
