# Comprendre ce qu'est une requête paramétrée

J'ai toujours été étonné de ne pas pouvoir obtenir le résultat (dans le sens "la requête SQL complète", celle qui va être exécutée dans mon SGBD) d'une requête paramétrée avant de l'exécuter.

J'ai naïvement cru pendant - trop - longtemps que du SQL était généré avant l'exécution d'une requête paramétrée.

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