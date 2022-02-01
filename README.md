# spp #

[version_shield]: https://img.shields.io/badge/version-v0.1.1-informational.svg
[release_page]: https://github.com/mfederczuk/spp/releases/tag/v0.1.1 "Release v0.1.1"
[![version: v0.1.1][version_shield]][release_page]
[![Changelog](https://img.shields.io/badge/-Changelog-informational.svg)](CHANGELOG.md "Changelog")

## About ##

**spp** (**s**cript **p**re**p**rocessor) is a general-purpose preprocessor program, similar to [m4].

**spp** was originally created to combine big shellscripts, that have been split into several files, into a single file
for easier distribution, though **spp** may, of course, be used for any other purpose.

Its design and functionality was greatly based off of and inspired by **C** & **C++**'s preprocessor.

[m4]: <https://en.wikipedia.org/wiki/M4_(computer_language)> "m4 (computer language) - Wikipedia"

## Usage ##

**spp** will read through a file line-by-line, echoing each non-directive line to the output.

### Directives ###

Directives of **spp** are almost identical to the directives of the **C** and **C++** preprocessor:

<!-- TODO: instead of "directive name" -> "instruction" ? -->
```txt
    directive name
           │
           │       ┌──argument(s)
prefix──┐┌─┴──┐ ┌──┴──┐
        #define foo bar
        └──────┬──────┘
               │
           directive
```

The directive may be preceded by any amount of whitespace characters.  
The instruction name must immediately follow the directive prefix.  
How the argument(s) is/are interpreted depends on the instruction.  
Whitespace characters after the instruction name do *not* count towards the argument(s).
The whitespace between the instruction name and the argument(s), and any trailing
Trailing whitespace is stripped away.

A list of available instructions can be found in [`Instructions.md`](Instructions.md).

## Installation ##

**spp** needs to be manually built and installed:

```sh
git clone https://github.com/mfederczuk/spp.git &&
	cd spp &&
	make &&
	sudo make install
```

## Contributing ##

Read through the [Contribution Guidelines](CONTRIBUTING.md) if you want to contribute to this project.

## License ##

**spp** is licensed under [**GNU GPL v3.0**](licenses/GNU-GPL-v3.0.txt) or later.  
For more information about copying and licensing, see the [COPYING.txt](COPYING.txt) file.
