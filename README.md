# Injila

`Injila` is a hindi word which means `scripture`.

![alt text](https://github.com/DO-CV/injila/blob/master/doc/injila_screenshot.jpg?raw=true "Annotation result")

## Bugs?
Please let me know by writing an issue ticket in GitHub.

## Installing `injila`
[`injila`](https://github.com/DO-CV/injila) depends on [`sara`](https://github.com/DO-CV/sara).

You need to install `sara`. Currently installation instructions are available for **Linux Debian-based OSes only** .
Please follow the two steps.

- Installing `sara` third-party library:

  ```
  # Install `Sara` dependencies.
  sudo apt-get install -qq cmake libjpeg-dev libpng-dev libtiff5-dev qtbase5-dev

  # Clone `sara` repository
  git clone https://github.com/DO-CV/sara.git

  # Build `sara` release libraries.
  cd sara
  mkdir build && cd build
  cmake ..
  make  -j4
  make package

  # Re-build `sara` in debug libraries (necessary as well).
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  make -j4
  make package

  # Install the `sara` debian packages and that's it.
  sudo dpkg -i libDO-Sara-1.1.0-Linux.deb
  sudo dpkg -i libDO-Sara-dbg-1.1.0-Linux.deb
  ```

- Building the `injila` annotation tool:
  Make sure you leave the `sara` directory (so type: `cd ../..`)

  ```
  git clone https://github.com/DO-CV/injila.git

  # Build the annotation program.
  cd injila
  cmake -DCMAKE_BUILD_TYPE=Release .. # Don't forget `Release` otherwise the program will be slow...
  make -j4

  # Run the program.
  cd ImageAnnotator
  ./ImageAnnotator
  ```
