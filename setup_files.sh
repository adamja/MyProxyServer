#!/bin/bash

Nclients=3  # Specify the number of client folders you want to create
Nservers=3  # Specify the number of server folders you want to create

# REMOVE OLD FOLDERS
rm -R proxy client* server*
rm obj/*

# COMPILE proxy, server and client code
make all

# Create proxy folder
mkdir "proxy"
mkdir "proxy/my_cache"
cp obj/proxy.out "proxy"


# Create server folders
for (( c=1; c<=Nservers; c++ ))
do
    mkdir "server$c"
    mkdir "server$c/my_file"
    cp obj/server.out "server$c"

    # Create dummy files (5)
    for (( f=1; f<=5; f++ ))
    do
        touch "server$c/my_file/file$f"
        echo "This is the contents of file $f on server $c..." > "server$c/my_file/file$f"
    done

done


# Create client folders
for (( c=1; c<=Nclients; c++ ))
do
    mkdir "client$c"
    mkdir "client$c/download-file"
    cp obj/client.out "client$c"
done

