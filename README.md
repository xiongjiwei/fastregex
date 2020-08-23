# Fastre

This is a regular expression engine.

## Feature

- `a*` spuuort star
- `a+` support plus
- `a?` support optional
- `a|b` support alternation
- `a{m,n}` support quantity
- `.` support any
- `\d` support escape
- `[a]` `\d` `\w` support Character classes
- `(abc)` support group

## TODO

1. Anchors
2. Lookaround
3. Groups & References

## About

Parse the pattern to abstract syntax tree, and then compile the abstract syntax tree to bytecode program. Use virture machine to run the bytecode program.
