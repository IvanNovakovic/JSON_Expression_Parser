# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Detect OS and set executable path
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    EXECUTABLE="./build/Debug/json_eval.exe"
else
    EXECUTABLE="./build/json_eval"
fi

# Parse command line arguments
VERBOSE=false
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -v|--verbose) VERBOSE=true ;;
        *) echo "Unknown parameter: $1"; exit 1 ;;
    esac
    shift
done

# Test counter
PASSED=0
TOTAL=0

# Print directory information
echo "Script location: $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
echo "Working directory: $(pwd)"

# Build the project
echo "Building project..."
cmake -S . -B build
cmake --build build -j $(nproc)

# Verify executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}Error: json_parser executable not found${NC}"
    echo "Build may have failed or executable path is incorrect"
    exit 1
fi

# Create test directory
TEST_DIR="test_data"
mkdir -p $TEST_DIR

# Helper function to run test
run_test() {
    local desc="$1"
    local input_file="$2"
    local expression="$3"
    local expected="$4"
    
    TOTAL=$((TOTAL + 1))
    
    if [ "$VERBOSE" = true ]; then
        echo "Testing: $desc"
        echo "Command: $EXECUTABLE '$input_file' '$expression'"
        result=$($EXECUTABLE "$input_file" "$expression" 2>&1)
        echo "Output : $result"
    else
        echo "Testing: $desc... "
        result=$($EXECUTABLE "$input_file" "$expression" 2>&1)
    fi
    
    if [ "$result" == "$expected" ]; then
        echo -e "${GREEN}PASSED${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}FAILED${NC}"
        echo "Expected: $expected"
        echo "Got     : $result"
    fi
    echo "-------------------"
}

# Create test JSON files
cat > $TEST_DIR/basic.json << EOF
{
    "a": {
        "b": [
            1,
            2,
            {
                "c": "test"
            },
            [
                11,
                12
            ]
        ]
    }
}
EOF

cat > $TEST_DIR/numbers.json << EOF
{
    "a": [
        1,
        -1,
        1.5,
        -1.5
    ]
}
EOF

echo "Starting tests..."
echo "================="
echo "Basic Path Expressions"
echo "================="

# Basic path expressions
run_test "Access array element by index" "$TEST_DIR/basic.json" "a.b[1]" "2"
run_test "Access nested object property" "$TEST_DIR/basic.json" "a.b[2].c" "\"test\""
run_test "Get complete array contents" "$TEST_DIR/basic.json" "a.b" "[1, 2, {\"c\": \"test\"}, [11, 12]]"
run_test "Dynamic array indexing using path" "$TEST_DIR/basic.json" "a.b[a.b[1]].c" "\"test\""
run_test "Access nested array element" "$TEST_DIR/basic.json" "a.b[3][1]" "12"

echo "================="
echo "Built-in Functions"
echo "================="

# Function tests
run_test "Max of two array elements" "$TEST_DIR/basic.json" "max(a.b[0], a.b[1])" "2"
run_test "Min of nested array" "$TEST_DIR/basic.json" "min(a.b[3])" "11"
run_test "Size of object" "$TEST_DIR/basic.json" "size(a)" "1"
run_test "Size of array" "$TEST_DIR/basic.json" "size(a.b)" "4"
run_test "Size of string from dynamic path" "$TEST_DIR/basic.json" "size(a.b[a.b[1]].c)" "4"

echo "================="
echo "Number Operations"
echo "================="

# Numeric operations
run_test "Max with mixed array values and literals" "$TEST_DIR/basic.json" "max(a.b[0], 10, a.b[1], 15)" "15"

echo "================="
echo "Number Types"
echo "================="

# Number type handling
run_test "Positive integer access" "$TEST_DIR/numbers.json" "a[0]" "1"
run_test "Negative integer access" "$TEST_DIR/numbers.json" "a[1]" "-1"
run_test "Positive float access" "$TEST_DIR/numbers.json" "a[2]" "1.5"
run_test "Negative float access" "$TEST_DIR/numbers.json" "a[3]" "-1.5"

# Error handling tests
# run_test "Nonexistent file" "nonexistent.json" "value" "Error: Cannot open file nonexistent.json"
# run_test "Invalid path" "$TEST_DIR/basic.json" "nonexistent" "Error: Path not found: nonexistent"

# Print summary
echo
echo "================="
echo "Test Summary:"
echo "Passed: $PASSED/$TOTAL"
if [ $PASSED -eq $TOTAL ]; then
    echo -e "${GREEN}All tests passed!${NC}"
else
    echo -e "${RED}Some tests failed${NC}"
    exit 1
fi

# Cleanup
rm -rf $TEST_DIR
