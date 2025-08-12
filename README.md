# Shift
Shift is a custom programming language compiler implemented in C++ using LLVM. It supports features like user-defined functions, variables, expressions, and external function calls. The compiler lexes, parses, analyzes, and generates LLVM IR, which is then compiled to native code.

## Building
### Prerequisites
- C++20 compatible compiler
- CMake (version 3.10+)
- LLVM (with development headers and libraries installed)

### Steps
These steps will create the executable ```shift``` inside the build directory.

1. Clone the repository:

    ```
    git clone https://github.com/BvHand487/shift.git
    cd shift
    ```

2. Create a build directory and run CMake:

    ```
    mkdir build
    cd build
    cmake ..
    ```

3. Build the project:

    ```
    make
    ```

## Running
To run the compiler on a Shift source file:

```
./shift path/to/source_file.shf
```

This will compile the source and generate the corresponding output (for now an object file and an executable in the same location as the source file).


## Language
This language was aimed to be similar to C-like languages, whilst offering a clean syntax and compile-time guarantees without sacrificing too much runtime speed.

### Syntax
- Functions are indicated with the `fn` keyword followed by the function name. A `extern` before the `fn` keyword marks a function as extern and a `..` at the end of the parameter list marks it as having variable args.

- All variables must be declared and that is done with the `let` keyword. Depending on the context, a type annotation might be required:
    ```
    let a: int = 123;
    let b = 123;
    ```

- Type annotations for variables and function paramaters are indicated by a colon, followed by the type. The only exception to this are the return types of a function, where the type is preceded by an arrow:
    
    ```
    fn foo(x: int, y: int) -> bool { ... }
    ...
    let x: int = 5;
    ```

    *Type annotations are not needed when the type can be inferred, e.g. you can write `let x = 5;`.*

- Calling functions is as simple as writing the function name followed by parentheses which contain the arguments:
    ```
    fn add(x: int, y: int) -> int
    {
        return x + y;
    }
    ...
    add(5, 4);  // should be 9
    ```

- If statements are similar to the ones in other languages:
    ```
    let x = 1234;

    if (x == 0)
        printf("zero!\n");
    else if (x > 0)
        printf("positive!\n");
    else
        printf("negative!\n");
    ```

- The logical operators `&&`, `||` and `!` are interchangable with `and`, `or` and `not` respectively.

- While should also be familiar. The only problem is that `do { ... } while` doesn't exist:
    ```
    let x = 0;

    while (x < 100)
    {
        printf("%d\n", x);
        x = x + 1;
    }
    ```


## Examples
### Example 1 - Printing the N-th Fibonacci number:
The following program computes the N-th Fibonacci number and prints it to stdout using `printf`.
<br/>
In this case it prints the 6th number, which is 8.
The table below shows the Fibonacci sequence from F<sub>0</sub> to F<sub>7</sub>.
| F<sub>0</sub> | F<sub>1</sub> | F<sub>2</sub> | F<sub>3</sub> | F<sub>4</sub> | F<sub>5</sub> | F<sub>6</sub> | F<sub>7</sub> |
|---|---|---|---|---|---|---|---|
| 0 | 1 | 1 | 2 | 3 | 5 | 8 |13 |

#### Code
```
extern fn printf(fmt: str, ..) -> int;

fn fib(n: int) -> int
{
    if (n == 0)
        return 0;

    if (n == 1)
        return 1;

    return fib(n - 2) + fib(n - 1);
}

fn main() -> int
{
    let x = 6;
    let res = fib(x);
    printf("%d\n", res);
}
```

### Example 2 - Finding prime numbers:
The following program computes which numbers are primes from 0-200 and prints them out using `printf`.
<br/>
The output format is as follows:
```
0 -> FALSE
1 -> FALSE
2 -> TRUE
3 -> TRUE
4 -> FALSE
5 -> TRUE
...
```

#### Code
```
extern fn printf(fmt: str, ..) -> int;

fn is_prime(n: int) -> bool
{
    if (n <= 1) return false;
    if (n == 2 or n == 3) return true;

    let div = 2;
    while (div <= n / 2)
    {
        if (n % div == 0)
            return false;

        div = div + 1;
    }

    return true;
}

fn main() -> int
{
    let x = 0;
    while (x < 200)
    {
        let is_prime = is_prime(x);

        printf("%d -> ", x);
        if (is_prime)
            printf("TRUE\n");
        else
            printf("FALSE\n");

        x = x + 1;
    }
}
```