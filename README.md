# Code source du TP synthèse du cours 420-401-AL (session H15)

## Énoncé

L'énoncé se trouve sur Moodle.

## Utilisation de valgrind pour détecter les erreurs et fuites de mémoire

Puisque le travail utilise la librairie Qt, plusieurs erreurs seront détectées si vous lancez simplement valgrind avec la commande habituelle :

	valgrind ./simul_server -l

À la place, utilisez le fichier de suppression fourni qui n'affichera que les erreurs de votre code source et ignorera les erreurs liées à Qt, avec la commande suivante dans le répertoire `build` :

	 valgrind --suppressions=../valgrind.supp --leak-check=full ./simul_server -l

