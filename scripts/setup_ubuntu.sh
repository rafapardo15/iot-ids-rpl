#!/usr/bin/env bash
set -euo pipefail

sudo apt update
sudo apt install -y \
  git \
  build-essential \
  ant \
  openjdk-17-jdk \
  python3 \
  python3-venv \
  python3-pip \
  libncurses-dev \
  libgtk-3-0 \
  libxtst6

