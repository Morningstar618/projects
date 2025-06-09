# fileDB

`fileDB` is a lightweight command-line database application written in C for managing employee records. It supports basic operations such as creating a new database, listing existing employees, and adding new entries.

## 🛠 Build

To build the project, simply run:

```bash
make


# Create a new database
./fileDB -n -f ./employees.db

# Add a new employee
./fileDB -f ./employees.db -a "John Doe|San Francisco|120000"

# List all employees
./fileDB -f ./employees.db -l


