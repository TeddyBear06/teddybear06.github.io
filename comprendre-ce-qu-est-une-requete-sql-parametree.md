# Comprendre ce qu'est une requête SQL paramétrée

J'ai toujours été étonné de ne pas pouvoir obtenir le résultat (dans le sens "la requête SQL complète", celle qui va être exécutée dans mon SGBD) d'une requête paramétrée avant de l'exécuter.

J'ai naïvement cru pendant - trop - longtemps que du SQL était généré avant l'exécution d'une requête paramétrée.

Qu'il y avait du code quelque part qui avait pour rôle d'échapper toute sorte de potentielles injections SQL (ce qui aurait été intéressant).

![Exploits of a Mom](https://imgs.xkcd.com/comics/exploits_of_a_mom.png)

On va prendre comme exemple un code PHP avec une connexion à SQLServer pour illustrer (largement inspiré de cette [page](https://www.php.net/manual/fr/function.sqlsrv-prepare.php#example-2056) mais adapté pour des raisons de simplicité et d'illustration du concept) :

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
//      $instruction->getSQLWithParameters() 
// Le nom de la méthode est arbitraire mais le concept serait de pouvoir obtenir 
// la requête SQL suivante :
//      UPDATE Table_1 SET OrderQty = 10 WHERE SalesOrderID = 99
```

La réponse est simple, parce que à aucun moment le code (au sens votre code PHP ou PDO ou le driver ODBC) ne va générer du SQL.

Le seul dans l'histoire qui a toutes les "billes" pour construire la requête SQL finale est ... Vous avez deviné ?

Le SGBD.

Quand on prépare une requête il nous faut olbigatoirement 2 éléments :

- Une connexion au SGBD (je ne comprenais pas d'ailleurs pourquoi on avait besoin de la connexion pour préparer une requête, mais vous allez comprendre par la suite)
- Un statement (une instruction)

Mais si on pourrait se passer d'un statement en utilisant un sqlsrv_exec() directement mais c'est l'objet de ce post donc... On va utiliser un statement.

Dans un premier temps, on établi une connexion avec le SGBD (le sqlsrv_connect() dans l'exemple précédent).

Ensuite, la magie opère dans cette méthode [sqlsrv_prepare()](https://github.com/microsoft/msphpsql/blob/master/source/sqlsrv/conn.cpp#L1085) (toujours dans l'exemple précédent).

Elle va réaliser plusieurs opérations :

1. Créer un statement qui va être stockée en mémoire pour être joué plus tard (c'est ça la clef, le "plus tard")
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

Une fois que le statement et créé, le code [visibe ici](https://github.com/microsoft/msphpsql/blob/master/source/shared/core_conn.cpp#L468) va ajouter le texte de la requête au statement :

```php
// core_sqlsrv_prepare
// Create a statement object and prepare the SQL query passed in for execution at a later time.
// Parameters:
// stmt - statement to be prepared
// sql - T-SQL command to prepare
// sql_len - length of the T-SQL string

void core_sqlsrv_prepare( _Inout_ sqlsrv_stmt* stmt, _In_reads_bytes_(sql_len) const char* sql, _In_ SQLLEN sql_len )
{
    // [...]
}
```

Pour finalement, [ajouter le N paramètres à notre statement](https://github.com/microsoft/msphpsql/blob/master/source/shared/core_stmt.cpp#L364) :

```php
// core_sqlsrv_bind_param
// Binds a parameter using SQLBindParameter.  It allocates memory and handles other details
// in translating between the driver and ODBC.
// Parameters:
// param_num      - number of the parameter, 0 based
// param_z        - zval of the parameter
// php_out_type   - type to return for output parameter
// sql_type       - ODBC constant for the SQL Server type (SQL_UNKNOWN_TYPE = 0 means not known, so infer defaults)
// column_size    - length of the field on the server (SQLSRV_UKNOWN_SIZE means not known, so infer defaults)
// decimal_digits - if column_size is valid and the type contains a scale, this contains the scale
// Return:
// Nothing, though an exception is thrown if an error occurs
// The php type of the parameter is taken from the zval.
// The sql type is given as a hint if the driver provides it.

void core_sqlsrv_bind_param( _Inout_ sqlsrv_stmt* stmt, _In_ SQLUSMALLINT param_num, _In_ SQLSMALLINT direction, _Inout_ zval* param_z,
                             _In_ SQLSRV_PHPTYPE php_out_type, _Inout_ SQLSRV_ENCODING encoding, _Inout_ SQLSMALLINT sql_type, _Inout_ SQLULEN column_size,
                             _Inout_ SQLSMALLINT decimal_digits)
{
    // [...]
}
```

La méthode utilisée est [SQLBindParameter](https://learn.microsoft.com/en-us/sql/odbc/reference/syntax/sqlbindparameter-function?view=sql-server-ver16).

Tout se passe sur un même statement et dans la mémoire du SGBD à l'aide de méthodes de l'API ODBC.

La documentation de [l'API est disponible ici](https://learn.microsoft.com/en-us/sql/odbc/reference/syntax/odbc-api-reference?view=sql-server-ver16).

Du coup, toute notre couche de driver PHP sert juste à faire ces quelques lignes de C ([visibles ici](https://learn.microsoft.com/en-us/sql/odbc/reference/develop-app/handles?view=sql-server-ver16)) et que je n'ai pas besoin de commenter vu qu'il y a déjà des commentaires et que maintenant que nous savons quelles sont les différentes étapes effectuées lors d'une requête préparée le code à beaucoup de sens :


```c
SQLHSTMT      hstmtOrder, hstmtLine; // Statement handles.  
SQLUINTEGER   OrderID;  
SQLINTEGER    OrderIDInd = 0;  
SQLRETURN     rc;  
  
// Prepare the statement that retrieves line number information.  
SQLPrepare(hstmtLine, "SELECT * FROM Lines WHERE OrderID = ?", SQL_NTS);  
  
// Bind OrderID to the parameter in the preceding statement.  
SQLBindParameter(hstmtLine, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 5, 0,  
               &OrderID, 0, &OrderIDInd);  
  
// Bind the result sets for the Order table and the Lines table. Bind  
// OrderID to the OrderID column in the Orders table. When each row is  
// fetched, OrderID will contain the current order ID, which will then be  
// passed as a parameter to the statement tofetch line number  
// information. Code not shown.  
  
// Create a result set of sales orders.  
SQLExecDirect(hstmtOrder, "SELECT * FROM Orders", SQL_NTS);  
  
// Fetch and display the sales order data. Code to check if rc equals  
// SQL_ERROR or SQL_SUCCESS_WITH_INFO not shown.  
while ((rc = SQLFetch(hstmtOrder)) != SQL_NO_DATA) {  
   // Display the sales order data. Code not shown.  
  
   // Create a result set of line numbers for the current sales order.  
   SQLExecute(hstmtLine);  
  
   // Fetch and display the sales order line number data. Code to check  
   // if rc equals SQL_ERROR or SQL_SUCCESS_WITH_INFO not shown.  
   while ((rc = SQLFetch(hstmtLine)) != SQL_NO_DATA) {  
      // Display the sales order line number data. Code not shown.  
   }  
  
   // Close the sales order line number result set.  
   SQLCloseCursor(hstmtLine);  
}  
  
// Close the sales order result set.  
SQLCloseCursor(hstmtOrder);
```

On constate que même au plus bas niveau (sans entrer dans du code machine, mais en C), la requête SQL complète n'est connue que par le SGBD, et surtout, qu'après son exécution par ce dernier.

D'ailleurs, je ne sais pas vous, mais ce code en C me parle bien plus que tout ce code de driver PHP.