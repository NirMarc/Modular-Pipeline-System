set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[TEST]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_status "Building project..."
./build.sh

print_status "Test 1: Basic uppercaser + logger pipeline"
EXPECTED="[logger] HELLO"
ACTUAL=$(echo -e "hello\n<END>" | ./output/analyzer 10 uppercaser logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 1 PASSED"
else
    print_error "Test 1 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi

print_status "Test 2: Complex pipeline (uppercaser -> rotator -> logger)"
EXPECTED="[logger] OHELL"
ACTUAL=$(echo -e "hello\n<END>" | ./output/analyzer 10 uppercaser rotator logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 2 PASSED"
else
    print_error "Test 2 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi

print_status "Test 3: Flipper functionality"
EXPECTED="[logger] olleh"
ACTUAL=$(echo -e "hello\n<END>" | ./output/analyzer 10 flipper logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 3 PASSED"
else
    print_error "Test 3 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi

print_status "Test 4: Expander functionality"
EXPECTED="[logger] h e l l o"
ACTUAL=$(echo -e "hello\n<END>" | ./output/analyzer 10 expander logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 4 PASSED"
else
    print_error "Test 4 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi

print_status "Test 5: Empty string handling"
EXPECTED="[logger] "
ACTUAL=$(echo -e "\n<END>" | ./output/analyzer 10 logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 5 PASSED"
else
    print_error "Test 5 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi

print_status "Test 6: Error handling - invalid arguments"
./output/analyzer 2>/dev/null && {
    print_error "Test 6 FAILED: Should have failed with no arguments"
    exit 1
} || {
    print_status "Test 6 PASSED: Correctly failed with no arguments"
}

print_status "Test 7: Error handling - invalid queue size"
./output/analyzer 0 logger 2>/dev/null && {
    print_error "Test 7 FAILED: Should have failed with invalid queue size"
    exit 1
} || {
    print_status "Test 7 PASSED: Correctly failed with invalid queue size"
}

print_status "Test 8: Error handling - non-existent plugin"
./output/analyzer 10 nonexistent 2>/dev/null && {
    print_error "Test 8 FAILED: Should have failed with non-existent plugin"
    exit 1
} || {
    print_status "Test 8 PASSED: Correctly failed with non-existent plugin"
}

print_status "Test 9: Multiple uppercase plugins"
EXPECTED="[logger] HELLO"
ACTUAL=$(echo -e "hello\n<END>" | ./output/analyzer 10 uppercaser uppercaser logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 9 PASSED"
else
    print_error "Test 9 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi

print_status "Test 10: Single character handling"
EXPECTED="[logger] A"
ACTUAL=$(echo -e "a\n<END>" | ./output/analyzer 10 uppercaser logger 2>/dev/null | grep "\[logger\]" | head -1)

if [ "$ACTUAL" == "$EXPECTED" ]; then
    print_status "Test 10 PASSED"
else
    print_error "Test 10 FAILED: Expected '$EXPECTED', got '$ACTUAL'"
    exit 1
fi


print_status "Test 11: Multiple input lines"
OUTPUT=$(echo -e "hello\nworld\ntest\n<END>" | ./output/analyzer 5 uppercaser logger 2>/dev/null | grep "\[logger\]")
LINE_COUNT=$(echo "$OUTPUT" | wc -l)

if [ "$LINE_COUNT" -eq 3 ]; then
    print_status "Test 11 PASSED: Processed 3 lines correctly"
else
    print_error "Test 11 FAILED: Expected 3 lines, got $LINE_COUNT"
    exit 1
fi

print_status "Test 12: Typewriter plugin functionality"
timeout 10s bash -c 'echo -e "hi\n<END>" | ./output/analyzer 5 typewriter >/dev/null 2>&1' && {
    print_status "Test 12 PASSED: Typewriter completed within timeout"
} || {
    print_error "Test 12 FAILED: Typewriter test timed out or failed"
    exit 1
}

print_status "========================================="
print_status "All tests passed successfully!"
print_status "========================================="

print_status "Example usage:"
print_status "./output/analyzer 20 uppercaser rotator logger flipper"
print_status ""
print_status "Try it:"
print_status "echo 'hello world' | ./output/analyzer 10 uppercaser rotator logger"