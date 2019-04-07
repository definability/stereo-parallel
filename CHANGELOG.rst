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

0.1.1 - 2019-04-07
==================

Added
-----

- Support of OpenMP_ for CSP solution,
  namely for ``sp::graph::constraint::solve_csp``.

0.1.0 - 2019-03-29
==================

Added
-----

- Project build with CMake_.
- Project testing with ctest_.
- Namespace ``sp`` (stereo parallel), containing all other namespaces.

- ``types`` with the basic types.

  - ``Pixel`` structure to represent position of a pixel.
    Contains

    - ``x`` (horizontal offset, column) of a pixel.
    - ``y`` (vertical offset, row) of a pixel,

  - ``Node`` of the ``DisparityGraph`` stores

    - ``pixel`` it belongs to,
    - ``disparity`` assigned to the ``pixel`` in the node.

  - ``Edge`` of the ``DisparityGraph`` is a tuple of two ``Node`` instances

    - ``node`` is the start,
    - ``neighbor`` is the end of the edge.

- ``image`` namespace with image processing utilities.

  - ``Image`` structure to store information about images.
    Contains

    - ``width`` of an image,
    - ``height`` of an image,
    - ``max_value`` limit for intensity,
    - ``data`` array of intensities of pixels in row-major order.

  - ``PGM_IO`` class to read and write the ``Image`` to and from
    the plain grayscale PGM files.
    Contains

    - ``get_image`` getter for the read ``Image``,
    - ``set_image`` setter for the ``Image`` to write,
    - ``operator>>`` to read an ``Image`` from a file,
    - ``operator<<`` to write an ``Image`` to a file,

  - ``image_valid`` function to check validity of ``Image``.

- ``indexing`` contains functions for getting access to arrays
  by abstract indices like ``Pixels``, ``Nodes``, etc.

  - ``pixel_index`` to calculate index of given pixel
    in 1D intensities array.
  - ``pixel_value`` to fetch intensity of given pixel
    from 1D intensities array.
  - ``node_index`` to get index of the node in ``nodes_availability`` array.
  - ``neighbor_index`` allows to get index of neighboring pixel for fast access.
  - ``neighbor_by_index`` allows to get a neighboring pixel
    of the ``Node`` by index of the neighbor.
  - ``reparametrization_index_fast`` to get an index of
    reparametrization element for given a node and its neighbor index.
  - ``reparametrization_index`` to get an index of reparametrization element
    using a node and a pixel.
  - ``reparametrization_index_slow`` to get an index of
    reparametrization element by an edge.
  - ``reparametrization_value_fast`` to get a value of reparametrization element
    for given a node and its neighbor value.
  - ``reparametrization_value`` to get a value of reparametrization element
    using a node and a pixel.
  - ``reparametrization_value_slow`` to get a value of reparametrization element
    by an edge.
  - ``neighborhood_index_fast`` to get index of a neighborhood
    from ``neighborhoods`` of ``LowestPenalties``
    using pixel coordinates and its neighbor index.
  - ``neighborhood_index`` to get index of a neighborhood
    from ``neighborhoods`` of ``LowestPenalties``
    using coordinates of pixel and its neighbor.
  - ``neighborhood_index_slow`` to get index of a neighborhood
    from ``neighborhoods`` of ``LowestPenalties``
    using ``Edge`` instance.

- ``indexing::check`` with utilities to check availability of provided indices.

  - ``neighborhood_exists`` checks whether two pixels are really neighbors.
  - ``neighborhood_exists_fast`` checks whether a pixel has a neighbor
    with specified index.
  - ``node_exists`` checks existence of a node.
  - ``edge_exists`` checks existence of an edge.

- ``graph::disparity`` to support graph representation of disparity map.

  - ``DisparityGraph`` to represent the problem using graphical model.
    Contains

    - ``left`` image,
    - ``right`` image
      (made on a camera located to the right of the ``left`` image),
    - ``disparity_levels`` is the number of different disparity levels
    - ``cleanness`` weight of ``Node`` penalty.
    - ``smoothness`` weight of ``Edge`` penalty.
      (the more it is, the more memory the graph consumes),
    - ``reparametrization`` that allows to solve the dual problem.

  - ``edge_penalty`` calculates penalty of an edge.
  - ``node_penalty`` calculates penalty of a node.

- ``graph::constraint`` with utilities to solve CSP.

  - ``ConstraintGraph`` to build final disparity map
    after optimization of ``DisparityGraph``.
    Contains

    - Constructor to build a ``ConstraintGraph`` given ``DisparityGraph``.
    - ``nodes_availability`` array that contains information about ability
      of each ``Node`` to be chosen,
    - ``disparity_graph`` constant pointer to ``DisparityGraph`` instance
      from which the ``ConstraintGraph`` was built.
    - ``threshold`` parameter to calculate `epsilon`-consistent nodes.

  - ``make_node_available`` to mark specified ``Node``
    as available in ``ConstraintGraph``.
  - ``make_node_unavailable`` to mark specified ``Node``
    as unavailable in ``ConstraintGraph``.
  - ``make_all_nodes_unavailable`` to mark all ``Node`` instances
    as unavailable in ``ConstraintGraph``.
  - ``solve_csp`` to solve a CSP problem given ``ConstraintGraph``.
  - ``csp_solution_iteration`` to make a step in ``solve_csp``.
  - ``should_remove_node`` to check whether we need to remove the node.
  - ``is_edge_available`` to check edge existence.
  - ``is_node_available`` to check node existence.
  - ``check_nodes_left`` to check whether there are nodes left
    in ``ConstraintGraph``.

- ``graph::lowest_penalties`` with utilities
  to find locally best nodes and edges.

  - ``LowestPenalties`` to store minimal penalties of pixels and neighborhoods.
    Contains

    - ``graph`` constant reference to ``DisparityGraph`` instance
      from which the ``LowestPenalties`` was built.
    - ``pixels`` array with minimal penalties of pixels
      calculated from correspondent nodes.
    - ``neighborhoods`` array with minimal penalties of neighborhoods
      calculated from correspondent edges.

  - ``calculate_lowest_pixel_penalty`` to calculate minimal penalty of a pixel.
  - ``calculate_lowest_neighborhood_penalty`` to calculate minimal penalty
    of a neighborhood given coordinates of a pixel and its neighbor.
  - ``calculate_lowest_neighborhood_penalty_fast`` to calculate minimal penalty
    of a neighborhood using corresponding ``Edge`` instance.
  - ``calculate_lowest_neighborhood_penalty_slow`` to calculate minimal penalty
    of a neighborhood given pixel coordinates and index of its neighbor.
  - ``lowest_pixel_penalty`` to get minimal penalty of a pixel
    from ``pixels`` of ``LowestPenalties``.
  - ``lowest_neighborhood_penalty`` to get minimal penalty
    of a neighborhood from ``neighborhoods`` of ``LowestPenalties``
    using corresponding ``Edge`` instance.
  - ``lowest_neighborhood_penalty_fast`` to get minimal penalty
    of a neighborhood from ``neighborhoods`` of ``LowestPenalties``
    given coordinates of a pixel and its neighbor.

- ``labeling::finder`` with functions to find a consistent labeling.

  - ``fetch_pixel_available_penalties``
    to find the available penalties of all nodes.
  - ``fetch_edge_available_penalties``
    to find the available penalties of all edges.
  - ``fetch_available_penalties``
    to find and fuse both nodes' and edges' penalties.
  - ``calculate_minimal_consistent_threshold``
    to find the minimal threshold of the ``ConstraintGraph``
    for the problem to still be solvable.
  - ``choose_best_node``
    to leave only one node with the lowest penalty at specific pixel.
    if a pixel has two nodes with the same penalty,
    the one with the lower disparity will be chosen.
  - ``find_labeling``
    to remove all non-best nodes.
  - ``build_disparity_map``
    to build a grayscale image with the solution to the problem.

.. _Unreleased:
    https://github.com/char-lie/stereo-parallel/compare/v0.1.1...HEAD
.. _0.1.1:
    https://github.com/char-lie/stereo-parallel/compare/v0.1.0...v0.1.1

.. _Keep a Changelog:
    http://keepachangelog.com/en/1.0.0
.. _Semantic Versioning:
    http://semver.org/spec/v2.0.0

.. _CMake:
    https://cmake.org
.. _ctest:
    https://cmake.org/cmake/help/v3.0/manual/ctest.1.html
.. _OpenMP:
    https://www.openmp.org
