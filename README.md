libdabdemod
===========

**libdabdemod** provides a subsytem to demodulate a stream of complex baseband
samples into OFDM symbols. The baseband symbols are can be acquired using
**libdabdevice** for example. The library is designed to be run on a separate
thread, since demodulation is a computationally very expensive process.

The demodulated symbols will be placed in a user-provided queue, which can
be used to feed them to **libdabdecode** in order to acquire the transported
payload data from a DAB/DAB+ ensemble.

Code example:
-------------

```cpp
#include <dabdemod.h>
#include <types/common_types.h>

#include <future>

int main(int argc, char * * argv)
  {
  using namespace dab::literals;

  dab::sample_queue_t samples{};
  dab::symbol_queue_t symbols{};

  /*
   * Asynchronously fill the sample queue
   */

  dab::demodulator demod{samples, symbols, dab::transmission_modes::kTransmissionMode1};
  auto demodRunner = std::async(std::launch::async, [&]{ demod.run(); });

  /*
   * Asynchronously process the recovered symbols
   */
  }
```

Building:
---------

You first need to check out the source code and the respective submodules:

```
$ git clone --recursive https://github.com/Opendigitalradio/libdabdemod.git
```

Like the other members of the **libdabXYZ** family, **libdabdemod** uses CMake
as it build environment. There are several configuration option that
you can choose from when building **libdabdemod**:

| Option                         | Default | Comment                                                 |
| ------------------------------ | ------- | ------------------------------------------------------- |
| `BUILD_DOCUMENTATION`          | **OFF**     | Build the **libdabdemod** documentation.                    |
| `BUILD_DOCUMENTATION_ONLY`     | **OFF**     | Only build the documentation.                           |
| `BUILD_INTERNAL_DOCUMENTATION` | **OFF**     | Generate the developer documentation.                   |
| `CMAKE_BUILD_TYPE`             | **Debug**   | The type of binary to produce.                          |
| `DOCUMENTATION_FOR_THESIS`     | **OFF**     | Build the documentation for the inclusion in the thesis |
| `WITH_ADDRESS_SANITIZER`       | **OFF**     | Include additional memory checks (**slow**)                 |
| `WITH_COMMON_TESTS`            | **OFF**     | Build and run the common library tests.                 |
| `WITH_UNDEFINED_SANITIZER`     | **OFF**     | Check for undefined behavior.                           |

To build the library, enter the `build` directory and run cmake or ccmake to
configure the build environment to you liking. Afterward, you can use
`cmake --build .` to build the library. Depending on your selection, the built
library will be in the directory `products/{Debug|Release}/lib`. If you chose
to build the documentation, the pdf will be found in
`dabdemod_doc/latex/refman.pdf`.

You can use this library as a subproject in any CMake based project.
