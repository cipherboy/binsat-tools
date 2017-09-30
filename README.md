# binsat-tools

Binary SAT and Circuit tools

Licensed under the GPLv3.


Translates SAT output to/from a bitpacked binary representation.

To build and run tests:

	make

To convert to a bitpacked representation:

	cryptominisat problem.cnf | sat2bin > problem.binsat

To restore solutions:

	bin2sat < problem.binsat > problem.out

`bin2sat` will return the appropriate code (10/20) of SAT/UNSAT.
