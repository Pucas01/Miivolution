# Miivolution Tests

## Building Tests

```bash
cmake -B build -G Ninja -DMIIVOLUTION_BUILD_TESTS=ON -DMIIVOLUTION_FETCH_AURORA=ON
ninja -C build
```

## Running Tests

```bash
# Run all tests
./build/tests/miivolution_tests

# Run specific test cases
./build/tests/miivolution_tests "[logging]"
./build/tests/miivolution_tests "[rfl]"

# Verbose output
./build/tests/miivolution_tests -s

# List all tests
./build/tests/miivolution_tests --list-tests
```
