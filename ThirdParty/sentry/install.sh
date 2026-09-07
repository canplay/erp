#!/bin/bash

VERSION="24.1.0"
git clone https://ghproxy.cn/https://github.com/getsentry/self-hosted.git
cd sentry
mv ./config.example.yml ./config.yml
mv ./enhance-image.example.sh ./enhance-image.sh
mv ./sentry.conf.example.py ./sentry.conf.py
vi ./config.yml
sed -i "s/SENTRY_BIND=9000/SENTRY_BIND=8993/g" ./env
cat >> ./sentry.conf.py <<EOF
CSRF_TRUSTED_ORIGINS = ["https://sentry.cm.example.com", "http://127.0.0.1:8993"]
EOF
cat >> ./enhance-image.sh <<EOF
pip install sentry-plugin-dingtalk
EOF
cd ..
./install.sh