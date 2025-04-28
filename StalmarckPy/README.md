# StalmarckPy

A lightweight Python API for the StalmarckSAT solver.

## Installation

```bash
pip install stalmarckpy
```

## Usage

```python
from stalmarckpy import solve

# Solve a DIMACS CNF file
result = solve("path/to/cnf_file.cnf")
print("SAT" if result else "UNSAT")
```