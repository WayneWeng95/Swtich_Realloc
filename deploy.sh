#!/bin/bash
sudo apt-get update

sudo apt-get install zsh

zsh

sudo apt-get install unzip

curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"

unzip awscliv2.zip

sudo ./aws/install

sudo apt-get install gcc

git clone https://github.com/WayneWeng95/Swtich_Realloc.git

cd Swtich_Realloc

./compile.sh

echo "aws s3 cp 18.txt s3://allocatortest"

