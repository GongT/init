#!/bin/bash

trap "echo 'got USR1!!!' ; exit" USR1
trap "echo 'got USR2!!!' ; exit" USR2

while true ; do
	sleep infinity &
	wait $!
done
