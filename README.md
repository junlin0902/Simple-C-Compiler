# Waterloo Language Plus Pointers Plus Procedures Compiler

## Introduction
The Waterloo Language Plus Pointers Plus Procedures (WLP4) Compiler is a project that compiles WLP4, a simplified version of the C programming language. This compiler is structured to process code in multiple stages, ensuring efficient and accurate compilation.

## WLP4 Language Overview
WLP4 Programming Language Tutorial: https://student.cs.uwaterloo.ca/~cs241/wlp4/WLP4tutorial.html

## Compiler Components 
The compiler consists of several components, each handling a specific phase of the compilation process:
1. Scanner (wlp4scan.cc): Tokenizes the input source code.
2. Parser (wlp4parse.cc): Analyzes the syntactic structure.
3. Type Checker (wlp4type.cc): Ensures type correctness.
4. Code Generator (wlp4gen.cc): Converts the validated code into assembly.
5. Assembler (asm.cc): Translates assembly code into machine code.
Each component's output serves as the input for the subsequent component, maintaining a streamlined compilation process.

## Compilation Process
The correct order of execution for the compilation process is as follows:
1. Scan
2. Parse
3. Type Check
4. Generate Code
5.Assemble

## Code Generation Optimization
The compiler implements several optimization techniques during the code generation phase, including:
1. Constant Folding  
2. Constant Propagation  
3. Common Sub-expression elimination  
4. Dead Code Elimination  
5. Strength Reduction
These optimizations enhance the efficiency and performance of the compiled code.

## Installation and Usage
1. Clone the repository to your local machine.
2. Compile the source files using a suitable C++ compiler.
3. Run the compiler with the WLP4 source code as input.
