#!/bin/bash
echo "Compilando..."
sudo docker start TPE_2024
sudo docker exec -it TPE_2024 make clean -C /root/Toolchain
sudo docker exec -it TPE_2024 make clean -C /root/
sudo docker exec -it TPE_2024 make -C /root/Toolchain
sudo docker exec -it TPE_2024 make -C /root/
sudo docker stop TPE_2024
#sudo docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it --name TPE_2024 agodio/itba-so:2.0