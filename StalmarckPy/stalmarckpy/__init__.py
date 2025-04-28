"""
StalmarckPy - Python bindings for the StalmarckSAT solver
"""

from ._stalmarckpy import solve_file

def solve(cnf_path):
    """
    Solves a CNF formula file using the Stalmarck SAT solver
    
    Args:
        cnf_path (str): Path to a DIMACS CNF file
        
    Returns:
        bool: True if the formula is satisfiable, False otherwise
    """
    return solve_file(cnf_path)