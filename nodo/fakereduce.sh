#!/bin/bash
while read line; do
  echo "reduce ${line}"
done < /dev/stdin
