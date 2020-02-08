#!/bin/bash

trap "echo 'got USR1!!!' ; exit" USR1
trap "echo 'got USR2!!!' ; exit" USR2

sleep infinity
