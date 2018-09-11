===============================
Contributing to Stereo Parallel
===============================

Improvements to `Stereo Parallel`_ are welcome!
First of all, read the `Code of Conduct`_.

.. contents::

Pull request processing
=======================

Create pull request
-------------------

#. Discuss the change you wish to make by creating `new issue`_.
#. Create new branch with name that satisfies the following
    * The name consists of two parts separated by slash.
    * The first part of the name should contain name of label
      assigned to the task.
      If multiple labels were assigned,
      choose the one which is more suitable by your opinion.
      **Exception** is a `bug label`_:
      branches that close bugs should be named
      **bugfix**.
    * The second part is a number of the issue.
    * Examples: ``feature/123``, ``build/7``, ``bugfix/666``.
#. Create new `pull request`_
    * Name of the pull request should consist of three parts.
    * The first part is ``Fix`` for ``bugfix`` branches
      and ``Close`` for another branches.
    * The second part is a number of the issue lead by hash sign.
    * The third part is a title of the issue.
    * For example, ``Close #4: Create CONTRIBUTING file``.
#. Description of the pull request should contain detailed summary
   of what you've done in it.

Your changes will be squashed
-----------------------------

All accepted pull requests are `squashed and merged`_
into the ``master`` branch.
Keep this in mind when writing description of your pull request.

This means that all your commits finally will be squashed into one.
Title of the resulting commit
will be the title of the pull request you've created
and its description will be copied from description of the pull request.

If your pull request is complex and covers several tasks,
you should either split it into several pull requests
or request to close some issues as duplicates.

Before merge
------------

* Write new tests if needed.
* Make sure that all tests pass.
* Change CHANGELOG_ if needed.
* Change README_ if needed.
* Check again grammar and lexicon of description of the pull request.

How to bump version of the project
==================================

The project adheres to `Semantic Versioning`_.
Read it to know how to name the next version of the project.

- Switch to ``master`` branch.
- Change version in CHANGELOG_ file,
  commit the change with ``Stereo Parallel ${VERSION}`` tag,
  where ``${VERSION}`` is a new version of the project,
  but don't push it for now.
- Create tag with name ``v ${VERSION}``,
  (``git tag -a "v${VERSION}"`` in ``bash``)
  with title ``Stereo Parallel ${VERSION}``,
  and add corresponding section of CHANGELOG_ to tag description.
- Push your changes using (``git push --follow-tags`` in ``bash``).

.. _bug label:
    https://github.com/char-lie/stereo-parallel/labels/bug
.. _CHANGELOG:
    https://github.com/char-lie/stereo-parallel/blob/master/CHANGELOG.rst
.. _Code of Conduct:
    https://github.com/char-lie/stereo-parallel/blob/master/CODE_OF_CONDUCT.md
.. _Keep a Changelog:
    https://keepachangelog.com
.. _new issue:
    https://github.com/char-lie/stereo-parallel/issues/new
.. _pull request:
    https://github.com/char-lie/stereo-parallel/pulls
.. _README:
    https://github.com/char-lie/stereo-parallel/blob/master/README.rst
.. _Semantic Versioning:
    http://semver.org/spec/v2.0.0.html
.. _squashed and merged:
    https://help.github.com/articles/about-pull-request-merges/ #squash-and-merge-your-pull-request-commits
.. _Stereo Parallel:
    https://github.com/char-lie/stereo-parallel/
