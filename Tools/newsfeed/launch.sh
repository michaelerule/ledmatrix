#!/usr/bin/env bash

./livestream.py > text
./update_periodically.py &
./scroll.py


