#!/usr/bin/env bash
set -e

# Install OpenCV and opencvr
sudo apt-get update
sudo apt-get install -y libopencv-dev

make -C dummycv
./dev-tools/gen-headers-txt.rb > headers.txt
./gen2rb.py
ruby extconf.rb
make

export RUBYLIB=/workspaces/opencvr
