# JSON Expression Evaluator

A C++ application that parses JSON files and evaluates expressions on JSON data. Built with modern C++17 and optimized with multithreading support.

## Features

- Parse and evaluate JSON files
- Support for path expressions (`a.b[1]`)
- Dynamic array indexing (`a.b[a.b[1]]`)
- Built-in functions:
  - `min()` - Finds minimum value across arguments
  - `max()` - Finds maximum value across arguments
  - `size()` - Returns length of strings/arrays/objects

## Building

Use the provided build script:

```bash
./build.sh
```

Or manually with CMake:

```bash
cmake -S . -B build
cmake --build build -j
```

## Usage

The `json_eval` executable accepts a JSON file path and an optional expression:

```bash
./build/json_eval <json_file> [expression]
```

Examples:

```bash
# Print JSON content
./build/json_eval data/test.json

# Access array element
./build/json_eval data/test.json "a.b[1]"

# Use built-in functions
./build/json_eval data/test.json "max(a.b[0], a.b[1])"
./build/json_eval data/test.json "size(a.b)"
```

## Testing

Run the test suite using:

```bash
./test_eval.sh
```

Add -v flag for verbose output:

```bash
./test_eval.sh -v
```

## Requirements

- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.28 or higher