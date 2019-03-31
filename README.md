# Implémentation d'un parallel_for dans la bibliothèque "mthread"

Le but était d'implémenter le maximum de fonctionnalité du *Open MP parallel_for*  dans la bibliothèque **mthread**.

Voici les fonctionnalités du *parallel_for* que j'ai réussi à implémenter dans la bibliothèque **mthread**.

- Parallélisation d'une boucle *for* avec un mode de répartition statique des itérations.
- Parallélisation d'une boucle *for* avec un mode de répartition dynamique des itérations.

## Localisation & Fonctionnement

La définition de la structure de données contenant les variables dont l'on a besoin pour la parallélisation d'une boucle *for* se trouvent dans le fichier **mthread.h:35**.

```C
struct mthread_parallel_s {
    	   volatile int nb_thread; // Nombre de threads en parallèle pour la boucle for
    	   int schedule; // Le mode de répartition des itérations : (1 => Statique) & (2 => Dynamique)
	       int chunk_size; 
   };
```

La fonction principale **mthread_parallel_for** se trouve dans le fichier du **mthread_parallel_for.c**.

Dans cette fonction, j'ai implémenté deux types de mode de répartition des itérations:

- **Statique** : Remplissage d'un tableau qui permettra à chaque thread de connaitre exactement les itérations qu'il va exécuter. Le *chunk-size* est par défaut égal à 1.
- **Dyamique** : On définit le nombre d'itérations au total, puis chaque thread, un après l'autre prendra un nombre d'itérations égalent à la valeur du *chunk_size* jusqu'à ce qu'il ne reste plus d'itérations.

## Tests

Pour tester ces fonctionnalités du *parallel for* dans la bibliothèque **mthread**, deux fichiers tests ont été créé pour chaque type de scheduling.

Ces deux fichiers se trouvent dans le répertoire: **test/parallel_for**:

- parallel_for_test_static_sched.c
- parallel_for_test_dynamic_sched.c

Vous pouvez lancer ces tests en lancant les commandes suivantes:
```C
~/mthread/$ cd test
~/mthread/test$ make parallel_for/parallel_for_test_static_sched
~/mthread/test$ make parallel_for/parallel_for_test_dynamic_sched
```

