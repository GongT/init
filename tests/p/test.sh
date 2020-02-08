#!/bin/bash

echo "Hello stdout! Arguments: $# -- $*." >&1
echo "Hello stderr! Arguments: $# -- $*." >&2

if [[ -z "$*" ]]; then
    echo "I will sleep and quit"
    sleep 1
    exit 111
else
    echo "I will run forever, but sleep when quit"
    trap '
i=5
while [[ "$i" -gt 0 ]]; do
	echo "quitting ($i)..."
	i=$((i - 1))
	sleep 1
done
echo "quit now."
exit
    ' INT
    while true; do
        echo "working..."
        sleep 1
    done
fi


