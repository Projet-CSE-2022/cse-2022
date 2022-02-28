## CSE - 

Nils Basset, Adrien Jaquiéry, Gabrielle Thurnherr

### Objectif 

Faire en sorte que le fauteuil détecte le point critique de renversement. Réagir en fonction de cette détection.

### Cahier des charges

- utiliser un bus can
- détection du point critique de renversement de la chaise
- arrêter le fauteuil en cas de détection du point critique
- revenir en arrière si on détecte une position trop dangereuse
- prédiction / prévenir que la pente est dangereuse
- Rendre le système modulable

### Spécification 

- utilisation d'une rpi/arduino
- faire des recherches sur les microcontrôleurs
- faire première version sur rpi/arduino
- si le temps le permet porter sur microcontrôleur
- détecter/définir le centre de gravité
- détection point crtitique statiquement d'abord puis dynamiquement
- définition des commandes de réaction
- Envoi des commandes (stop, danger,...)
- réception des infos par bus can? 
- simulation avec la donkey car
- réception des commandes de mouvements (avancer, reculer,...) via can
- prédiction des mouvements



**Points critiques**

- faire gaffe au centre de gravité -> rendre configurable?
- Calcul de stabilité
- emplacement du capteur, voir l'influence, initialisation dans un premier temps?
- étudier la donkey pour démontrer que notre système est faisable et implémentable 



#### Matériel nécessaire

- 1x rpi4 
- 1x arduino
- 1x IMU
- 1x hat can pour rpi4
- bus can -> fils à choper au fablab 
- 1x donkey car
- 1x module can pour arduino



### TODO

- Check le matériel
- Commande du matériel
- Tout installer sur la donkey car 
- soft arduino
- soft rpi4
- tests



### Répartition du travail

#### Nils



#### Adrien



#### Gabrielle



