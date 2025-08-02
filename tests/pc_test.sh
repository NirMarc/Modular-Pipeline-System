#!/bin/bash
set -e

gcc ./tests/test_plugin_common.c -o test_plugin_common 

./test_plugin_common 

rm test_plugin_common