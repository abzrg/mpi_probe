#!/usr/bin/env bash

set -ex

mpif90 -Wpedantic -Wall -Wextra -Wshadow main.f90
