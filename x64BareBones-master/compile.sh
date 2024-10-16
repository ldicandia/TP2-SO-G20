#!/bin/bash
sudo docker start TPE_2024_2
sudo docker exec -it TPE_2024_2 make clean -C /root/Toolchain
sudo docker exec -it TPE_2024_2 make clean -C /root/
sudo docker exec -it TPE_2024_2 make -C /root/Toolchain
sudo docker exec -it TPE_2024_2 make -C /root/
sudo docker stop TPE_2024_2
#sudo docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it --name TPE_2024_2 agodio/itba-so:2.0