# Se connecter à une base de données

La question peut sembler triviale car nous le faisons tous depuis des années et à partir d'environnement divers et variés.

Cependant, comprendre le fonctionnement exact d'une connexion à une base de données peut nous aider à mieux comprendre certains mécanismes.

C'est parti, commençons par le commencement, les prérequis à une connexion à une base de données.

## I. Le(s) driver(s)

Il existe 2 types de driver (il existait quelque chose avec ODBC mais je ne vais pas faire d'archéologie cette fois) pour établir une connexion à une base de données :

- ODBC (Open Database connectivity);
- OLE DB (Object Linking and Embedding database).

ODBC est le connecteur de base de données standard de l'industrie pour un accès natif aux base de données relationnelles.

Un extrait [Wikipédia](https://fr.wikipedia.org/wiki/Open_Database_Connectivity) : 

> ODBC (sigle de Open Database Connectivity) est un intergiciel qui permet à une application informatique, par un procédé unique, de manipuler plusieurs bases de données qui sont mises à disposition par des systèmes de gestion de bases de données (SGBD) ayant chacun un procédé propre.
>
> Ce logiciel, fondé sur le cahier des charges du SQL Access Group, a été mis en œuvre en 1992 par Microsoft pour les systèmes d'exploitation Windows, puis plus tard par d'autres éditeurs pour d'autres systèmes d'exploitation tels que Unix et la plateforme Java.

On ne va pas s'intéresser à OLE DB car il n'est pas largement utilisé et a déjà été déprécié dans le passé avant d'être dé-déprécié par Microsoft car ils se sont rendus compte qu'il était encore utile dans certains cas et qu'ils ne pouvaient pas le faire disparaître. Sale histoire.

## II. Le fichier de configuration des drivers installés

Quand vous installez un driver ODBC (peut être que vous n'en avez jamais installé alors que vous effectuez des connexions à des bases de données mais si vous êtes sous Windows c'est peut être que le pilote est déjà installé par défaut par Windows ou que c'est le l'ordinateur d'un collègue qui l'a déjà fait), un fichier de configuration va être créé quelque part dans votre système de fichier.

Suivant l'environement (Windows/GNU/Linux/AIX/...), il n'est pas au même endroit mais son nom est identique : 

> odbcinst.ini

Un exemple de ce que peut contenir ce fichier (un fichier .ini, comme on en a l'habitude, qui liste les drivers disponibles et le chemin pour y accéder (plus des paramètres de configuration supplémentaires, of course)) :

```ini
[ODBC Driver 18 for SQL Server]
Description=Microsoft ODBC Driver 18 for SQL Server
Driver=/opt/microsoft/msodbcsql18/lib64/libmsodbcsql-18.3.so.1.1

[PostgreSQL]
Description=PostgreSQL driver for GNU/Linux
Driver=/usr/lib/psqlodbcw.so
Setup=/usr/lib/libodbcpsqlS.so
FileUsage=1
```

Sous Windows, il existe un utilitaire ["ODBC Data Source Administrator"](https://learn.microsoft.com/fr-fr/sql/database-engine/configure-windows/open-the-odbc-data-source-administrator?view=sql-server-ver16) qui permet d'avoir une GUI pour configurer les DSNs (Data Source Name) pour tous les utilisateurs ou l'utilisateur en cours.

Sous GNU/Linux, il existe [odbcinst](https://manpages.ubuntu.com/manpages/focal/man1/odbcinst.1.html).

### III. La connexion

Le moment tant attendu... La connexion proprement parlé à la base de données.

Elle peut s'effectuer de pulisuers façons différentes :

- Via la couche TCP/IP
- Via un socket
- Via un chemin sur un système de fichier (pour une base SQLite par exemple)

La plupart du temps (quoique) la connexion se fera par la couche TCP/IP.

J'ai trouvé un [repository qui s'apelle "Chinook"](https://github.com/lerocha/chinook-database) qui fournit un script SQL pour créer une base de données sur laquelle on va faire nos tests (ça change de [AdventureWorks](https://learn.microsoft.com/en-us/sql/samples/adventureworks-install-configure?view=sql-server-ver16&tabs=ssms)).

Il faut installer la librairie SQLite3 dans sa version de développement pour nous permettre de compiler notre code source et le driver ODBC pour SQLite :

```
$ sudo apt-get install lisqlite3odbc lisqlite3-dev
```