FROM ubuntu:22.04

RUN <<EOF
apt-get update -yqq
apt-get install -yqq clang cmake qt6-base-dev libgl1-mesa-dev dbus-x11 \
    libgpgme-dev libgmp-dev
EOF

WORKDIR /usr/src/app
COPY . .

RUN <<EOF
make clean
make
EOF
