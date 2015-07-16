#!/bin/bash
while read line; do
  echo "map ${line}"
done < /dev/stdin
