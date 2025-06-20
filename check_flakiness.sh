#!/bin/bash
set -e
for i in $(seq $1); do
  echo "Executing $i";
	$2;
done
