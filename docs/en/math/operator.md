# Mathematical Operations

*Note: This document aims to help understand the module functionality. Please refer to the source code for specific APIs and behaviors.*

## Overview

`MathOperator.hpp` is the mathematical operation tool module in the ast project, providing a series of general vector and matrix operation functions.

This module adopts a templated design, supporting various data container types, including standard library containers, native arrays, and pointers.

## Features

### Supported Operation Types

- **Basic mathematical functions**: `sign`, `cot`, `acot`, `eps`
- **Vector operations**:
  - Dot product (`dot`)
  - Cross product (`cross`)
  - Norm (`norm`)
  - Normalization (`normalize`, `normalized`)
- **Arithmetic operators**: Support for vector-scalar addition, subtraction, multiplication, and division
- **Matrix multiplication**: Support for multiplication of fixed-size matrices

### Multi-container Support

The module is designed to support various data container types:

- Standard library containers like `std::vector`, `std::array`
- Native C arrays `double[N]`
- Native pointers `double*`
- Any custom container that supports `size()` and subscript access `[]`

## Usage Examples

### Basic Vector Operations

@[code](/examples/util/operator/vector_op_basic.cpp)

### Arithmetic Operations

@[code](/examples/util/operator/vector_op_arithmetic.cpp)

### Matrix Operations

@[code](/examples/util/operator/matrix_op_basic.cpp)

## Design Notes

### Templated Design

The module is implemented using template functions. As long as a type supports standardized interfaces (`size()` and subscript access `[]`), it can use these functions. This design provides great flexibility while maintaining type safety.

### Performance Considerations

- For operations with known dimensions (e.g., `dot3`, `cross3`), specialized functions are provided to avoid loop overhead
- Support for in-place operations (`normalize`) and operations that return copies (`normalized`)
- Compile-time size checks for native array versions

### Error Handling

- Use `assert` to check vector size consistency
- Use `static_assert` to check array sizes at compile time
- Safe handling for normalization of zero vectors

## Dependencies

- `AstGlobal.h`: Project global definitions
- `math.h`: Mathematical functions
- `assert.h`: Assertion checks
- `type_traits`: Type traits
- `array`: Standard array containers