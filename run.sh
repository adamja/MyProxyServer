#!/bin/bash

Proxytimeout=180  # timeout period to disconnect idle clients in seconds
Proxyaddress=127.0.0.1  # address of the proxy the clients will connect to
Proxyport=51020  # port of the proxy the clients will connect to

Nservers=3  # specify the number of server folders you want to run

# SETUP PROJECT FILES
./setup_files.sh

sleep 1

# RUN PROXY
cd proxy
gnome-terminal --title="PROXY" -e "./proxy.out $Proxytimeout"
cd ..
echo "Opened new terminal running proxy with timeout of $Proxytimeout seconds"
sleep 1

# RUN SERVERS
for (( c=1; c<=Nservers; c++ ))
do
  port=$(( $Proxyport+$c ))
  cd "server$c"
  gnome-terminal --title="SERVER$c" -e "./server.out $port"
  cd ..
  echo "Opened new terminal running server$c on port $port"
  sleep 1
done