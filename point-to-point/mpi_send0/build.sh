#!/usr/bin/env bash

set -ex

mpicc -std=c23 -Wpedantic -Wall -Wextra -Wshadow main.c
