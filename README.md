# Big Integer Computation

The class is aimed at easy adoption for big integers computation with medium performance in c++ programming.

## Notification

The class is under development, which means that the functionalities presented might not fit your needs.
Feal free to customize your own version, or pull a feature request to let me know what is missing.


## Prerequisites

Place the ./src and ./include documents inside your project directory and include the BigInteger.h file in your *.cpp

```
#include "./include/BigInteger.h"
```


## Basic usage

```
#include <iostream>
#include "./include/BigInteger.h"

using namespace std;

int main(void)
{
    BigInteger bigint_1("+12345678987654321");
    BigInteger bigint_2("-98765432123456789");
    
    cout << "bigint_1 - bigint_2 = " << bigint_1 - bigint_2 << endl;
    
    return 0;
}
```

That's it!

See ./demo.cpp for more ideas about how to use the BigInteger class.


## Performance

The following picture demonstrates the performance of using the BigInteger class to work with arbitrary large integer numbers:

<p align="center">
  <img src="./img/demo.png" width="75%" height="75%" />	
</p>

Note 1: The results are obtained from ./demo.cpp
Note 2: The FFT/KARATSUBA label below each panel is the indication of the "MULTIPLY" setting defined in .\include\define.h

## Author

* **Jason Lai** - jason99.lai@gmail.com
