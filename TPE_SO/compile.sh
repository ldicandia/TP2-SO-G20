#!/bin/bash
sudo docker start TPESO
sudo docker exec -it TPESO make clean -C /root/Toolchain
sudo docker exec -it TPESO make clean -C /root/
sudo docker exec -it TPESO make -C /root/Toolchain
sudo docker exec -it TPESO make -C /root/
sudo docker stop TPESO
sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 
#sudo docker run -v "${PWD}:/root" --privileged -ti agodio/itba-so-multi-platform:3.0 