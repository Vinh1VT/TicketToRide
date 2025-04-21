#!/bin/bash

N_PROCESS=10
PROJET="./stat"

for i in $(seq 0 $((N_PROCESS-1))); do
  NOM="TGVinhStatV$i"
  "$PROJET" "$NOM" &
done

wait

echo "Fini!"