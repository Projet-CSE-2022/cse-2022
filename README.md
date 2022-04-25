# cse-2022

IP donkey car 192.168.1.3

mdp : redsreds

faire donkey_llc
cf https://github.com/tleyden/donkey-ros


raspberry pi 3 with CAN : https://www.youtube.com/watch?v=fXiOIUZtV10 
http://wiki.ros.org/ros_canopen
http://wiki.ros.org/canopen_chain_node


Sustentation tracteur : https://tel.archives-ouvertes.fr/tel-01063386/document


configurer CAN rpi3 :
sudo nano /boot/firmware/usercfg.txt
ajouter :
dtparam=spi=on
dtoverlay=mcp2515-can0,oscillator=16000000,interrupt=25
dtoverlay=spi-bcm2835-overlay

Activer CAN :
sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0
checker photo
