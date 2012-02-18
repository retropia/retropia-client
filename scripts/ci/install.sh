#!/bin/bash

sudo echo "deb http://apt.retropia.org/ubuntu natty main contrib non-free" >> /etc/apt/sources.list
wget -q http://apt.retropia.org/key.gpg -O- | sudo apt-key add -
sudo apt-get update
sudo apt-get install qt4-dev-tools libsqlite3-dev libssl-dev libpthread-stubs0-dev libdisasm-dev libgdsl-dev libgtest-dev