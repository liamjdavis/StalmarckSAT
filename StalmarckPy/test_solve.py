#!/usr/bin/env python
from stalmarckpy import solve
import sys

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python test_solve.py <cnf-file>")
        sys.exit(1)
    
    cnf_path = sys.argv[1]
    print(f"Testing StalmarckPy solver with file: {cnf_path}")
    
    try:
        result = solve(cnf_path)
        print(f"Result: {'SAT' if result else 'UNSAT'}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)