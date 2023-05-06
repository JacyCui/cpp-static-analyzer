# Contributing Guide

## Commit Message Convention

Grammar for commit messages:

```text
<type>: <description>

[optional body]
```

The commit type `<type>` can include the following:

- `feat` – a new feature is introduced with the changes
- `fix` – a bug fix has occurred 
- `chore` – changes that do not relate to a fix or feature and don't modify src or test files (for example updating dependencies)
- `refactor` – refactored code that neither fixes a bug nor adds a feature
- `docs` – updates to documentation such as the README or other markdown files
- `style` – changes that do not affect the meaning of the code, likely related to code formatting such as white-space, missing semi-colons, and so on.
- `test` – including new or correcting previous tests
- `perf` – performance improvements
- `ci` – continuous integration related
- `build` – changes that affect the build system or external dependencies
- `revert` – reverts a previous commit

The commit type should be all lowercase and limited to the above 11 types. `<description>` should be a brief description of the commit and should not exceed one line. If you have more to describe, put them in
the optional body.

## Comment Convention

Doxygen recognizable comments should be provided for each class and methods declared in the header files. 

Method that overrides a base class method can just inherit comments in the base class.

You can take `include/World.h` as an example. 
