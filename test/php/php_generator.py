from pysat.examples.genhard import PHP
import argparse

def main():
    parser = argparse.ArgumentParser(description="Generate a Pigeonhole Principle (PHP) problem in CNF format.")
    parser.add_argument("filepath", help = "The name of the .cnf file to store PHP problem")
    parser.add_argument("num_holes", help = "Number of holes in PHP")
    parser.add_argument("k_mult", help = "The value of k multiplier")

    # parse arguments
    args = parser.parse_args()
    filepath = args.filepath
    num_holes = int(args.num_holes)
    k_mult = int(args.k_mult)

    # check file type
    if not filepath.lower().endswith(".cnf"):
        print("Invalid file type: .cnf expected")
        return
    
    # store file
    cnf = PHP(num_holes, k_mult)
    cnf.to_file(filepath)


if __name__ == "__main__":
    main()