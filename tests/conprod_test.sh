#!/bin/bash
set -e

gcc tests/consumer_producer_test.c plugins/sync/consumer_producer.c plugins/sync/monitor.c -lpthread -o tests/consumer_producer_test
./tests/consumer_producer_test

rm tests/consumer_producer_test