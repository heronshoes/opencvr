#!/usr/bin/env bash
set -e

# install
rbenv install --verbose $1
rbenv rehash
rbenv global $1
