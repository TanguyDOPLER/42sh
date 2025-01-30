#!/bin/bash

echo "Boucle until infinie avec continue"

until false; do
    echo "Avant continue"
    continue
    echo "Après continue"
done

