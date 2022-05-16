# cse-2022

IP donkey car 192.168.1.3

mdp : redsreds

faire donkey_llc
cf https://github.com/tleyden/donkey-ros

**Contrôle de la donkey car via teleop**

1. Se connecter en ssh sur la voiture avec la commande "ssh ubuntu@192.168.1.3"
2. Aller dans le dossier `catkin_ws` et entrer la commande `source /opt/ros/noetic/setup.bash` et `source ./devel/setup.bash` dans chacun des terminaux utilisés
3.lancer `rosrun i2cpwm_board i2cpwm_board`, `rosrun donkey_llc low_level_control.py` et `rosrun teleop_twist_keyboard teleop_twist_keyboard.py` dans trois terminaux différents.
4. On peut maintenant contrôler la donkey car avec le clavier en ayant le focus sur le terminal où l'on a lancételeop_twist_keyboard.

**raspberry pi 3 with CAN** 
https://www.youtube.com/watch?v=fXiOIUZtV10 
http://wiki.ros.org/ros_canopen
http://wiki.ros.org/canopen_chain_node


**Sustentation tracteur** 
https://tel.archives-ouvertes.fr/tel-01063386/document


**configurer CAN rpi3**
sudo nano /boot/firmware/usercfg.txt
ajouter :
dtparam=spi=on
dtoverlay=mcp2515-can0,oscillator=16000000,interrupt=25
dtoverlay=spi-bcm2835-overlay

**Activer CAN**
sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0
checker photo

**Définition des commandes** 

|      |      |      |
| ---- | ---- | ---- |
|      |      |      |
|      |      |      |
|      |      |      |
