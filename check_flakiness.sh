#!/bin/bash
set -e
for i in $(seq 100); do
	$1;
done
