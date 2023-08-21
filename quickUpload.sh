PORT=$1

if [ "$PORT" == "" ]; then
    echo Please enter a port
    exit 1
fi

if [ "$(ls | grep build)" == "" ]; then
    mkdir build
fi

cd build

cmake ..

make

avrdude -q -v -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -D -U flash:w:GermanEquitorialMount-atmega328p.hex:i