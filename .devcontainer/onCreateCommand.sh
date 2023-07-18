#!/usr/bin/env bash
set -e

# Install IRuby 
# IRuby is installed on system Ruby (RVM)
sudo apt-get update
sudo apt-get install -y libczmq-dev libzmq3-dev

gem install iruby
iruby register --force

# Setup rbenv
#   Install rustc to build yjit
sudo apt-get update
sudo apt-get install -y rustc
export RUBY_CONFIGURE_OPTS='--enable-yjit --disable-install-doc'
echo 'eval "$(rbenv init - bash)"' >> ~/.bashrc
source $HOME/.bashrc
.devcontainer/install_ruby.sh 3.2.2
# .devcontainer/install_ruby.sh 3.1.4
# .devcontainer/install_ruby.sh 3.0.6
