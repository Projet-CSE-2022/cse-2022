# cse-2022

IP donkey car 192.168.1.3

mdp : redsreds

## Installation

### Canopen

- `sudo nano /boot/firmware/usercfg.txt` et ajouter les paramètres suivant pour configurer le module can :

  - `dtparam=spi=on`
  - `dtoverlay=mcp2515-can0,oscillator=16000000,interrupt=25`
  - `dtoverlay=spi-bcm2835-overlay`

- `sudo /etc/init.d` 

- `sudo touch can.sh` Créer un fichier pour démarrer le canopen au démarrage

- `sudo chmod 777 can.sh`

- `sudo nano can.sh`

  ```bash
  #!/bin/sh
  ### BEGIN INIT INFO
  # Provides:          can.sh
  # Required-Start:    $all
  # Required-Stop:
  # Should-Start:
  # Default-Start:     2 3 4 5
  # Default-Stop:
  # X-Interactive:     true
  # Short-Description: Start can on boot
  ### END INIT INFO
  sudo ip link set can0 type can bitrate 500000
  sudo ip link set up can0
  ```

- `sudo update-rc.d can.sh default` pour init le fichier au démarrage

- `cd ..`

- `sudo find -iname '*can'` pour voir si le système a créé les fichier `rc2`, `rc3`, `rc4` et `rc5`

### ROS

- `git clone git@github.com:Projet-CSE-2022/cse-2022.git`
- `cd ~/cse_2022/catkin_ws`
- `source /opt/ros/noetic/setup.bash`
- `catkin_make -DCATKIN_WHITELIST_PACKAGES=""` pour compiler tous les packages
- `source ./devel/setup.bash`
- `roslaunch launch/all.launch` pour lancer tous les packages

## Contrôles

- i tourner
- l avancer
- j reculer
- l avancer
- tout arrêter
- q augmenter vitesse
- z diminuer vitesse
