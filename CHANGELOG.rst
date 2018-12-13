=========
Changelog
=========

All notable changes to this project will be documented in this file.

The format is based on `Keep a Changelog`_
and this project adheres to `Semantic Versioning`_.

.. contents::
    :backlinks: none

Unreleased_
===========

Added
-----

- Project build with CMake_.
- Project testing with ctest_.
- ``Image`` structure to store information about images.
  Contains

  - ``width`` of an image,
  - ``height`` of an image,
  - ``max_value`` limit for intensity,
  - ``data`` array of intensities of pixels in row-major order.

- ``Pixel`` structure to represent position of a pixel.
  Contains

  - ``row`` (vertical offset) of a pixel,
  - ``column`` (horizontal offset) of a pixel.

- ``image_valid`` function to check validity of ``Image``.
- ``get_pixel_index`` to calculate index of given pixel
  in 1D intensities array.
- ``get_pixel_value`` to fetch intensity of given pixel
  from 1D intensities array.
- ``DisparityGraph`` to represent the problem using graphical model.
  Contains

  - ``left`` image,
  - ``right`` image
    (made on a camera located to the right of the ``left`` image),
  - ``maximal_disparity`` is a limit for disparity
    (the more it is, the more memory the graph consumes),
  - ``reparametrization`` that allows to solve the dual problem.

- ``Node`` of the ``DisparityGraph`` stores

  - ``pixel`` it belongs to,
  - ``disparity`` assigned to the ``pixel`` in the node.

- ``Edge`` of the ``DisparityGraph`` is a tuple of two ``Node`` instances

  - ``node`` is the start,
  - ``neighbor`` is the end of the edge.

- ``neighbor_index`` allows to get index of neighboring pixel for fast access.
- ``neighborhood_exists`` checks whether two pixels are really neighbors.
- ``neighborhood_exists_fast`` checks whether a pixel has a neighbor
  with specified index.
- ``node_exists`` checks existence of a node.
- ``edge_exists`` checks existence of an edge.
- ``edge_penalty`` calculates penalty of an edge.
- ``node_penalty`` calculates penalty of a node.
- ``reparametrization_index_fast`` to get an index of reparametrization element
  for given a node and its neighbor index.
- ``reparametrization_index`` to get an index of reparametrization element
  using a node and a pixel.
- ``reparametrization_index_slow`` to get an index of reparametrization element
  by an edge.
- ``reparametrization_value_fast`` to get a value of reparametrization element
  for given a node and its neighbor value.
- ``reparametrization_value`` to get a value of reparametrization element
  using a node and a pixel.
- ``reparametrization_value_slow`` to get a value of reparametrization element
  by an edge.

- ``ConstraintGraph`` to build final disparity map
  after optimization of ``DisparityGraph``.
  Contains

  - ``nodes_availability`` array that contains information about ability
    of each ``Node`` to be chosen,
  - ``disparity_graph`` constant pointer to ``DisparityGraph`` instance
    from which the ``ConstraintGraph`` was built.
  - ``threshold`` parameter to calculate `epsilon`-consistent nodes.

- ``node_exists`` function to check whether specified ``Node``
  is marked as available in ``ConstraintGraph``.
- ``make_node_available`` mark specified ``Node``
  as available in ``ConstraintGraph``.
- ``make_node_unavailable`` mark specified ``Node``
  as unavailable in ``ConstraintGraph``.
- ``disparity2constraint`` function
  to construct a ``ConstraintGraph`` given ``DisparityGraph``.

.. Remove these two lines and one indentation level of the next two lines
    when you will release the first version.
    .. _Unreleased:
        https://github.com/char-lie/stereo-parallel/compare/v0.0.1...HEAD

.. _CMake:
    https://cmake.org
.. _ctest:
    https://cmake.org/cmake/help/v3.0/manual/ctest.1.html
.. _Keep a Changelog:
    http://keepachangelog.com/en/1.0.0
.. _Semantic Versioning:
    http://semver.org/spec/v2.0.0
