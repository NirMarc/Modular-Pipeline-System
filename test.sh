#!/bin/bash
#Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() { echo -e "${BLUE}[INFO]${NC} $1"; }

print_status() { echo -e "${GREEN}[TEST]${NC} $1"; }

print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }

print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

print_info "Building project (build.sh)"
./build.sh

OUTDIR="./output"
BIN="$OUTDIR/analyzer"

assert_eq() {
    local actual="$1" expected="$2" msg="$3" input="$4"
    if [ "$actual" != "$expected" ]; then
        print_error "FAIL: $msg"
        print_error "Input: $input"
        print_error "Expected: \n$expected"
        print_error "Actual: \n$actual"
        exit 1
    fi
}

help_msg() {
    printf "Usage: ./output/analyzer <queue_size> <plugin1> ... <pluginN>

Arguments:
  <queue_size>: The maximum number of items in each plugin's queue
  <plugin1> ... <pluginN>: Name of plugins to load (without .so extension)

Available plugins:
logger: Log all strings that pass through
typewriter: Simulates typewriter effect with delays
uppercaser: Converts all characters to uppercase
rotator: Moves every character to the right, Last character moves to the front
flipper: Reverses the order of characters in each string
expander: Expands each character with spaces

Example:
  ./output/analyzer 20 uppercaser rotator logger"
}

# Single Test Case
print_info "Testing logger"
EXPECTED="[logger] Single Test Case
Pipeline shutdown complete"
INPUT="Single Test Case"
ACTUAL="$(printf "$INPUT\n<END>\n" | "$BIN" 20 logger)"
assert_eq "$ACTUAL" "$EXPECTED" "logger did not match expected output" "$INPUT"
print_status "logger: PASS"

# Multiple Test Cases
print_info "Testing uppercaser expander logger"
EXPECTED="[logger] U P P E R C A S E R   E X P A N D E R   L O G G E R
Pipeline shutdown complete"
INPUT="uppercaser expander logger"
ACTUAL="$(printf "$INPUT\n<END>\n" | "$BIN" 20 uppercaser expander logger)"
assert_eq "$ACTUAL" "$EXPECTED" "uppercaser expander logger did not match expected output" "$INPUT"
print_status "uppercaser expander logger: PASS"

print_info "Testing rotator flipper typewriter"
EXPECTED="[typewriter] etirwepyt reppilf rotatorr
Pipeline shutdown complete"
INPUT="rotator flipper typewriter"
ACTUAL="$(printf "$INPUT\n<END>\n" | "$BIN" 20 rotator flipper typewriter)"
assert_eq "$ACTUAL" "$EXPECTED" "rotator flipper typewriter did not match expected output" "$INPUT"
print_status "rotator flipper typewriter: PASS"

print_status "-- Tested all plugins --"

print_info "Testing pipeline with same plugin multiple times"
EXPECTED="[logger] emit elpitlum nigulp emas htiw enilepip tseTs
[logger] E M I T   E L P I T L U M   N I G U L P   E M A S   H T I W   E N I L E P I P   T S E T S
Pipeline shutdown complete"
INPUT="Test pipeline with same plugin multiple times"
ACTUAL=$(printf "$INPUT\n<END>\n" | "$BIN" 20 rotator flipper logger expander uppercaser logger flipper expander)
assert_eq "$ACTUAL" "$EXPECTED" "pipeline with same plugin multiple times did not match expected output" "$INPUT"
print_status "pipeline with same plugin multiple times: PASS"


print_info "-- Testing Input Queue Size --"

print_info "Testing queue size must be greater than 0"
EXPECTED="Queue size must be greater than 0
$(help_msg)"
ACTUAL=$("$BIN" 0 logger 2>&1)
assert_eq "$ACTUAL" "$EXPECTED" "Expected error message when queue size is 0" "./output/analyzer 0 logger"
print_status "queue size must be greater than 0: PASS"

print_info "Testing queue size is not an integer"
EXPECTED="Queue size must be greater than 0
$(help_msg)"
ACTUAL=$("$BIN" a logger 2>&1)
assert_eq "$ACTUAL" "$EXPECTED" "Expected error message when queue size is not an integer" "./output/analyzer a logger"
print_status "queue size must be an integer: PASS"

print_info "Testing queue size is missing"
EXPECTED="$(help_msg)"
ACTUAL=$("$BIN" logger 2>&1)
assert_eq "$ACTUAL" "$EXPECTED" "Expected error message when queue size is missing" "./output/analyzer logger"
print_status "queue size must be an integer: PASS"

print_status "--Test input queue size: PASS --"

print_info "Testing invalid plugin"
EXPECTED="Failed to load plugin notplug: ./output/notplug.so: cannot open shared object file: No such file or directory
$(help_msg)"
ACTUAL=$("$BIN" 20 notplug 2>&1)
assert_eq "$ACTUAL" "$EXPECTED" "invalid plugin did not match expected output" "invalid"
print_status "invalid plugin: PASS"