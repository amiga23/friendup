#!/bin/bash
#source env.sh || exit 1

sudo docker run -d -v friendup-config:/friendup/cfg -v friendup-storage:/friendup/storage --name friendup-storage busybox:latest

sudo docker cp cfg.ini friendup-storage:/friendup/cfg/cfg.ini
sudo docker cp cert.pem friendup-storage:/friendup/cfg/crt/certificate.pem
sudo docker cp privkey.pem friendup-storage:/friendup/cfg/crt/key.pem

sudo docker run \
  -d \
  -p 6500:6500 \
  -p 6502:6502 \
  -p 443:6502 \
  --restart=always \
  --name=friendup \
  --volumes-from friendup-storage \
  friendup
