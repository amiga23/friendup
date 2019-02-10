#!/bin/bash
sudo docker run --rm -ti friendup mysql -h "$1" -u friendup -p friendup --execute="SOURCE db/FriendCoreDatabase.sql"
