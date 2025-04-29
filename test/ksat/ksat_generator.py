from pysat.formula import CNF
from pysat.solvers import Glucose3
import random
import argparse

def check_sat(cnf):
    """Check if a CNF formula is satisfiable using Glucose3 solver."""
    with Glucose3(bootstrap_with=cnf.clauses) as solver:
        is_sat = solver.solve()
        return is_sat

def generate_ksat(num_vars, num_clauses, k):
    """Generate a random k-SAT formula."""
    cnf = CNF()
    for _ in range(num_clauses):
        clause = []
        vars = random.sample(range(1, num_vars + 1), k)
        for var in vars:
            clause.append(var if random.random() > 0.5 else -var)
        cnf.append(clause)
    
    # Check satisfiability
    is_sat = check_sat(cnf)
    cnf.comments = [f"c Random {k}-SAT instance",
                   f"c Variables: {num_vars}, Clauses: {num_clauses}",
                   f"c Status: {'UNSAT' if is_sat else 'SAT'}"]
    return cnf

def main():
    parser = argparse.ArgumentParser(description="Generate a random k-SAT problem in CNF format.")
    parser.add_argument("filepath", help="The name of the .cnf file to store k-SAT problem")
    parser.add_argument("num_vars", type=int, help="Number of variables")
    parser.add_argument("num_clauses", type=int, help="Number of clauses")
    parser.add_argument("k", type=int, help="Number of literals per clause")
    parser.add_argument("--seed", type=int, default=None, help="Random seed for reproducibility")

    args = parser.parse_args()
    
    # Set random seed if provided
    if args.seed is not None:
        random.seed(args.seed)

    # Check file type
    if not args.filepath.lower().endswith(".cnf"):
        print("Invalid file type: .cnf expected")
        return
    
    # Generate and store the k-SAT instance
    cnf = generate_ksat(args.num_vars, args.num_clauses, args.k)
    cnf.to_file(args.filepath)

if __name__ == "__main__":
    main()