#!/bin/sh

echo "hello script"
for i in $(kill -l | awk '{print $2}') ; do
	echo "trap $i"
	trap "echo -e '\nrecv $i\n'" $i
done

echo "finish trap"

while true ; do
	sleep 1
	echo -ne "\t$(date)\n\e[A"
done

echo "I will quit"
