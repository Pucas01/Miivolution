# RevoMii Tests

## Building Tests

```bash
cmake -B build -G Ninja -DREVOMII_BUILD_TESTS=ON -DREVOMII_FETCH_AURORA=ON
ninja -C build
```

## Running Tests

```bash
# Run all tests
./build/tests/revomii_tests

# Run specific test cases
./build/tests/revomii_tests "[logging]"
./build/tests/revomii_tests "[rfl]"

# Verbose output
./build/tests/revomii_tests -s

# List all tests
./build/tests/revomii_tests --list-tests
```
