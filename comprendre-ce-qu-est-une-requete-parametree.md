# Comprendre ce qu'est une requête paramétrée

J'ai toujours été étonné de ne pas pouvoir obtenir le résultat (dans le sens "la requête SQL complète", celle qui va être exécutée dans mon SGBD) d'une requête paramétrée avant de l'exécuter.

J'ai naïvement cru pendant - trop - longtemps que du SQL était généré avant l'exécution d'une requête paramétrée.

Qu'il y avait du code quelque part qui avait pour rôle d'échapper toute sorte de potentielles injections SQL.

On va prendre comme exemple un code PHP avec une connexion à SQLServer pour illustrer (largement pompé de cette [page](https://www.php.net/manual/fr/function.sqlsrv-prepare.php#example-2056) mais adapté pour des raisons de simplicité et d'illustration du concept) :

```php
<?php

$nomDuServeur = "serverName\sqlexpress";

$informationsDeConnexion = array("Database"=>"dbName", "UID"=>"username", "PWD"=>"password");

$connexion = sqlsrv_connect($nomDuServeur, $informationsDeConnexion);

$requeteSQL = "UPDATE Table_1 SET OrderQty = ? WHERE SalesOrderID = ?";

$quantite = 10; 
$identifiant = 99;

$instruction = sqlsrv_prepare($connexion, $requeteSQL, array( $quantite, $identifiant));

// Pourquoi à partir de ce moment je ne peux pas faire quelque chose comme ...
//      $instruction->getSQL() 
// Le nom de la méthode est arbitraire mais le concept serait de pouvoir obtenir 
// la requête SQL suivante :
//      UPDATE Table_1 SET OrderQty = 10 WHERE SalesOrderID = 99
```

La réponse est simple, parce que à aucun moment le code (au sens votre code PHP ou PDO ou le driver ODBC) ne va générer du SQL.

Le seul dans l'histoire qui a toutes les billes pour construire la requête SQL finale est ... Vous avez deviné ?

Le SGBD.

Quand on prépare une requête il nous faut olbigatoirement 3 éléments :

- Une connexion au SGBD
- Une requête SQL (pas encore paramétrée)
- Des paramètres

Dans un premier temps, on établi une connexion avec le SGBD (le sqlsrv_connect() dans l'exemple précédent).

Ensuite, la magie opère dans cette méthode sqlsrv_prepare() (toujours dans l'exemple précédent).

Elle va réaliser plusieurs opérations :

1. Créer un statement (une instruction) qui va être stockée en mémoire pour être joué plus tard (c'est ça la clef, le "plus tard")
2. Ajouter le texte de la requête SQL à ce statement (l'instruction de l'étape n°1)
3. Ajouter les N paramètres (typés ou non) à ce statement (l'instruction de l'étape n°1)

Le code source en charge de l'étape n°1 est [visible ici](https://github.com/microsoft/msphpsql/blob/master/source/shared/core_stmt.cpp#L275).

Je colle la déclaration de la méthode pour mémoire :

```php
// core_sqlsrv_create_stmt
// Common code to allocate a statement from either driver.  Returns a valid driver statement object or
// throws an exception if an error occurs.
// Parameters:
// conn             - The connection resource by which the client and server are connected.
// stmt_factory     - factory method to create a statement.
// options_ht       - A HashTable of user provided options to be set on the statement.
// valid_stmt_opts  - An array of valid driver supported statement options.
// err              - callback for error handling
// driver           - reference to caller
// Return
// Returns the created statement

sqlsrv_stmt* core_sqlsrv_create_stmt( _Inout_ sqlsrv_conn* conn, _In_ driver_stmt_factory stmt_factory, _In_opt_ HashTable* options_ht,
                                      _In_opt_ const stmt_option valid_stmt_opts[], _In_ error_callback const err, _In_opt_ void* driver )
{
    // [...]
}
```