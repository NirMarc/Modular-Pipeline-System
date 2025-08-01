#!/bin/bash
set -e

gcc tests/monitor_test.c plugins/sync/monitor.c -lpthread -o tests/monitor_test

./tests/monitor_test

rm tests/monitor_test