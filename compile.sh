#source /opt/gcc-beaglebone/environment-setup-armv7at2hf-neon-poky-linux-gnueabi
export CC="arm-linux-gnueabihf-gcc"

export CFLAGS=""
export LDFLAGS="-lpthread"
export CPPFLAGS=""
export SERVER_MAIN="server_main.c"
export CLIENT_MAIN="client_main.c"
export SERVER_TARGET="server"
export CLIENT_TARGET="client"
export SRC=$(ls | grep -v main.c | grep .c | tr '\n' ' ')

make -j

cp $SERVER_TARGET $CLIENT_TARGET ~/share