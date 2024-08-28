FROM gcc

RUN <<EOF
apt-get update -yqq
apt-get install -yqq libgpgme-dev cmake clang qt6-base-dev clang-tidy clang-format dbus
EOF

WORKDIR /usr/src/app
COPY . .
RUN <<EOF
make clean
make
EOF
