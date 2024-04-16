#!/bin/bash

# this script launches the app in windows and can be run from git bash
cd src/
export PYTHONPATH="$PWD/uds_stack"
python3 main.py
