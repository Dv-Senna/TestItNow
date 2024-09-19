<p align="center">
	<img src="https://img.shields.io/badge/version-1.0.0-informational" />
</p>

# TestItNow
A C++ header-only unit testing library.

## Why ?
Why an other testing framework when we have a dozen different already well established.
Beacause I wanted a small testing suite, that can be integrated into any project really
easily. This is accomplished with the header-only nature of TestItNow and its minimal
interface.

## How to install ?
Just add this repo as a submodule, or just put it into your project and insert `./include`
in your include path, so that you can include TestItNow with `TestItNow/TestItNow.hpp`.

## How to use ?
A testing program is composed of tests, each of which are composed of several checks.
There are 2 types of checks in TestItNow, optional and mandatory checks. An optional check
can fail without preventing the whole test from running. A mandatory check, on the other
hand, stops the current test if it fails. It's usefull when you want to check that your
matrix equality operator works, and if not, there is no point in checking the addition,
multiplication, and so on.

In only one `cpp` file, you must tell TestItNow to define the main function and all other
functions of the library. You do so by adding a special define before including `TestItNow.hpp`

```cpp
// main.cpp
#define TIN_DEFINE_MAIN
#include <TestItNow/TestItNow.hpp>
```

In all your other files, just use `#include <TestItNow/TestItNow.hpp>`.

Here is an example of a test for some matrix library :

```cpp
// matrix.cpp
#include <TestItNow/TestItNow.hpp>

#include "myAwesomeMathLibrary/matrix.hpp"


TIN_START_TEST(matrix2D)
	Mat2 mat1 {1, 2, 3, 4};
	Mat2 mat2 {1, 2, 3, 4};

	// a mandatory check. If you don't want to give it a name, use `TIN_MANDATORY_CHECK`
	TIN_NAMED_MANDATORY_CHECK("equality", mat1 == mat2 && mat2 == mat1);

	// from now, we're sure that the equality operator works, ie our further checks are accurate

	Mat2 mat3 {2, 4, 6, 8};
	Mat2 mat4 {7, 10, 15, 22};

	TIN_CHECK(mat1 + mat2 == mat3);
	TIN_NAMED_CHECK("multiplication", mat1 * mat2 == mat4);

TIN_END_TEST(matrix2D, "This test checks that the 2x2 matrix implementation is working");
```


## CLI
Once you compile your test project, you get an executable. This is supposed to be run as a CLI.
If you want to run all tests, use :

```bash
.\test(.exe) -a
```

If you want to run some specific tests, you can do this :

```bash
.\test(.exe) test1 test2 test5
```