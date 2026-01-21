# Unit Dimension System

*Note: This document is AI-translated.*

The unit dimension system is a core utility module in the ast library for handling units and dimensions of physical quantities. This system supports numerical operations with units, automatic unit conversion, and dimension consistency checks to ensure the correctness of physical calculations.

## Core Concepts

### Dimension

Dimension is the basic attribute of a physical quantity, representing the type of the physical quantity. For example, length, mass, and time are fundamental dimensions, while velocity and acceleration are derived dimensions.

::: warning
Currently supported derived dimensions must be in the form of products of powers of fundamental dimensions, where exponents must be integers ranging from -8 to 7.
:::

#### Fundamental Dimensions

The ast library defines the following fundamental dimensions:
- Mass
- Length
- Angle
- Time
- Temperature
- Current
- Amount
- Luminous Intensity

#### Dimension Operations

Dimensions support multiplication, division, and power operations to create derived dimensions:

@[code](/examples/util/quantity/dimension_op.cpp)

### Unit

Unit is the specific representation of a dimension, used to measure the size of physical quantities. Each unit has a corresponding dimension and scaling factor.

#### Predefined Units

The ast library provides a rich set of predefined units, including:

- Length: mm, cm, m, km, in, ft, yd, mi
- Time: s, min, h, day
- Mass: kg, g, mg, lb
- Angle: rad, deg
- Force: N
- Current: A
- Area: m²
- Volume: m³, L

#### Unit Operations

Units can be multiplied, divided, and raised to powers to create composite units:

@[code](/examples/util/quantity/unit_op.cpp)

### Quantity

Quantity is a numerical value with a unit, combining numerical value and unit information, and supporting numerical operations with units.

#### Creating Quantities

@[code](/examples/util/quantity/quantity_create.cpp)

#### Quantity Operations

Quantities support addition, subtraction, multiplication, and division, but require compatible dimensions:

@[code](/examples/util/quantity/quantity_op.cpp)

## Dimension Consistency Check

The system automatically checks dimension consistency during operations to prevent physically unreasonable calculations:

@[code](/examples/util/quantity/quantity_op_invalid.cpp)