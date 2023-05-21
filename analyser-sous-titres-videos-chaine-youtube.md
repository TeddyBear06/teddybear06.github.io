# Analyser les sous-titres des vidéos d'une chaîne YouTube

J'ai récemment eu l'occasion de travailler sur un projet permettant d'extraire les sous-titres des vidéos d'une chaîne YouTube.

Il s'agit de [yt-fts](https://github.com/NotJoeMartinez/yt-fts).

Quel intérêt ?

Je vais prendre comme exemple un Youtubeur que j'apprécie particulièrement, [Lolo Cochet](https://www.youtube.com/channel/UCaUsBCtdRYgeu-2AQQZ5Y7Q).

En quelques lignes de commandes, on peut récupérer tous les sous-titres automatiques générés par YouTube et chercher des mots ou expressions dans l'ensemble de ces vidéos.

Pour récupérer tous les sous-titres de ces vidéos :

```
$ python yt_fts.py download "https://www.youtube.com/@LoloCochetMoto/videos" --language fr
```

Une fois le téléchargement et le processing effectué (stockage dans une base de données SQLite), si par exemple, je souhaite savoir dans quelles vidéos est évoqué le terme "suzuki", il me suffit d'éxécuter la commande suivante :

```
$ python yt_fts.py search UCaUsBCtdRYgeu-2AQQZ5Y7Q "suzuki"
Searching for quotes in channel UCaUsBCtdRYgeu-2AQQZ5Y7Q for text suzuki

Video title"('ACHARNEMENT ►  EP 2  ► LE MANS  ► ENDURANCE ► 1 SAISON AVEC LA HONDA N°1 - YouTube',)"


    Quote: "suzuki la route à son ombre pour la"
    Time Stamp: 00:02:46.610
    Link: https://youtu.be/8gYuNtOjlyM?t=163

Video title"('SUSPENS ► EP 6 ► SUZUKA 8 HOURS  ►  ENDURANCE  ► 1 SAISON AVEC LA HONDA N°1 - YouTube',)"


    Quote: "mais suzuki"
    Time Stamp: 00:00:11.209
    Link: https://youtu.be/bjI2c3bA1xI?t=8


    Quote: "suzuki puis certes virtuellement"
    Time Stamp: 00:22:04.060
    Link: https://youtu.be/bjI2c3bA1xI?t=1321

Video title"("J'AI MIS LE FEU AU BOL D'OR MOTO ► lolo cochet moto - YouTube",)"


    Quote: "disputées contre kawasaki suzuki et"
    Time Stamp: 00:05:54.310
    Link: https://youtu.be/gCeXmqwtynU?t=351
```

Je peux même m'amuser à utiliser des outils GNU/Linux pour compter le nombre d'occurences "suzuki" :

```
python yt_fts.py search UCaUsBCtdRYgeu-2AQQZ5Y7Q "suzuki" | grep Quote | wc -l
```

Mon intervention sur ce projet était liée à la gestion de la page de consentement affichée par YouTube si vous vous y connecté pour la première fois à partir d'une IP en Europe (RGPD).

Le cheminement et la pull request sont disponibles sur GitHub :

- [Issue](https://github.com/NotJoeMartinez/yt-fts/issues/1)
- [Pull request](https://github.com/NotJoeMartinez/yt-fts/pull/6)

J'ai également ajouté la possibilité de spécifier la langue attendue pour les sous-titres :

- [Pull request](https://github.com/NotJoeMartinez/yt-fts/pull/7)

Happy hacking folks!
