===============
Stereo Parallel
===============

.. image:: https://travis-ci.org/char-lie/stereo-parallel.svg?branch=master
    :target: https://travis-ci.org/char-lie/stereo-parallel
.. image:: https://codedocs.xyz/char-lie/stereo-parallel.svg
    :target: https://codedocs.xyz/char-lie/stereo-parallel

.. contents::

Contributing
============

Before adding any changes, read
`Code of Conduct`_ and CONTRIBUTING_ guide.

Using the code
==============

The project is licensed under `MIT License`_.
You can freely copy and use the code in your project
(even if it's commercial and its source code is closed).
When you modify some files,

- leave ``Copyright (c) 2018 char-lie`` at the top of modified file(s),
- write your own copyright under it,
- write license text you want to use (or leave `MIT License`_ if you use it).

It's explained in answer to
`How to manage a copyright notice in an open source project?`_.

Build the project
=================

The project uses CMake_ of version 3 and higher.
To build the project in Linux,

- clone it,
- create a directory to build the project in,
- go there and call ``cmake`` specifying root as the parameter,
- build the project using ``cmake --build`` or your build system (``make``?).

.. code-block:: bash

    git clone https://github.com/char-lie/stereo-parallel.git
    mkdir stereo-parallel/build
    cd build
    cmake ..
    cmake --build .

After build, it's recommended to run tests
in order to make sure that all works fine.
This can be done by executing ctest_ in build directory.
Here is a bash script for download, build and test

.. code-block:: bash

    git clone https://github.com/char-lie/stereo-parallel.git
    mkdir stereo-parallel/build
    cd build
    cmake ..
    cmake --build .
    ctest

Documentation
=============

You can build the documentation locally
and read it offline from ``build/docs`` directory.
First, you should install Doxygen_ and Graphviz_.
Under Ubuntu you can do it by

.. code-block:: bash

    sudo apt install doxygen graphviz

Then, you can build the documentation using CMake_.
Just enable ``BUILD_DOC`` flag

.. code-block:: bash

    cmake .. -DBUILD_DOC=ON
    cmake --build .

.. _CMake:
    https://cmake.org
.. _ctest:
    https://cmake.org/cmake/help/v3.0/manual/ctest.1.html
.. _CONTRIBUTING:
    https://github.com/char-lie/stereo-parallel/blob/master/CONTRIBUTING.rst
.. _Code of Conduct:
    https://github.com/char-lie/stereo-parallel/blob/master/CODE_OF_CONDUCT.md
.. _Doxygen:
    http://www.doxygen.org
.. _Graphviz:
    https://www.graphviz.org
.. _How to manage a copyright notice in an open source project?:
    https://softwareengineering.stackexchange.com/a/158011
.. _MIT License:
    https://github.com/char-lie/stereo-parallel/blob/master/LICENSE
