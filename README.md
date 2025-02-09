# Expression_Solver
Evaluates set of expressions using flex and bison tool 

## Overview
    A calculator built using Flex and Bison that reads mathematical expressions from an input file and evaluates them. This demonstrates basic compiler design principles like lexical analysis and parsing.
  
## Implementation Details

Basic Arithmetic Expressions

    Description:
     Focuses on evaluating simple arithmetic expressions.

    Key Features:
        Supports basic operators: addition (+), subtraction (-), multiplication (*), and division (/).
        No handling of variables or dependencies.

Arithmetic with Variable Dependencies

    Description:
    Expands on the basic arithmetic evaluator by allowing variables to be defined and used within expressions.

    Key Features:
        Supports variable assignment and expression evaluation where variables can depend on other variables.
        Manages dependency resolution to ensure that variables are computed in the correct order.

Logical Expressions with Predicates

    Description:
    Integrates logical and predicates operators into the expression evaluation.

    Key Features:
        Supports logical operators such as AND, OR, and NOT.
        Combines arithmetic computations with logical evaluations, enabling conditional expression processing.
        Useful for scenarios where expressions involve both numerical calculations and logical decision-making.

## Requirements
    GCC Compiler
    Flex
    Bison
    
## Compile and execute the program
    1. Navigate to the desired folder
    2. command to build: make


