#!/bin/bash

wget https://ghproxy.cn/https://github.com/goharbor/harbor/releases/download/v2.11.1/harbor-offline-installer-v2.11.1.tgz
tar -zxvf harbor-offline-installer-v2.11.1.tgz
cd harbor
vi harbor.yml
vi docker-compose.yml
./install.sh