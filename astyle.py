#!/bin/python3

import os

def apply_astyle(directory, extension, command):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(extension):
                file_path = os.path.join(root, file)
                print(f"{command} {file_path}")
                os.system(f"{command} {file_path}")

def supprimer_fichiers_orig(repertoire):
    for root, _, files in os.walk(repertoire):
        for file in files:
            if file.endswith(".orig"):
                chemin_fichier = os.path.join(root, file)
                os.remove(chemin_fichier)

# Remplacez par votre répertoire et la commande astyle
directory = "."
command_cpp = "astyle -A4 -S -xw -t4"
command_hpp = "astyle -A4 -S -xw -xl -t4"

apply_astyle(directory, ".cpp", command_cpp)
apply_astyle(directory, ".hpp", command_hpp)
supprimer_fichiers_orig(directory)
