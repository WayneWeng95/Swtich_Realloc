#!/bin/bash
sudo apt-get update

sudo apt-get upgrade

sudo apt-get install unzip

curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"

unzip awscliv2.zip

sudo ./aws/install

rm awscliv2.zip

sudo apt-get install gcc

./compile.sh

base64 /dev/urandom | head -c 2147000000 > file.txt

echo "aws s3 cp 18.txt s3://allocatortest"

