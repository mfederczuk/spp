# spp #

[version_shield]: https://img.shields.io/badge/version-v0.1.1-informational.svg
[release_page]: https://github.com/mfederczuk/spp/releases/tag/v0.1.1 "Release v0.1.1"
[![version: v0.1.1][version_shield]][release_page]
[![Changelog](https://img.shields.io/badge/-Changelog-informational.svg)](CHANGELOG.md "Changelog")

## About ##

> Script Preprocessor

**spp** is a preprocessor program for script files.

**spp** is mainly intended to be used to build big scripts that have been split up into several pieces.

## Usage ##

The executable can be passed a single file argument.  
The entire file is processed and the output will be written to `stdout`.
If no argument is specified or `-` is passed down, **spp** will read `stdin` instead.

### Directives ###

The preprocessor directives of **spp** look similar to the directives of the **C** and **C++** preprocessor.

**Synopsis:** `[whitespace]#<command>[<whitespace><argument>]`

_WHITESPACE_ may be either spaces or tabs.  
_COMMAND_ may be any characters BUT spaces and tabs.  
_ARGUMENT_ may be ANY character. Note that trailing whitespace will also count to the argument.

The following commands are defined:

* `insert <file>`  
  Inserts contents of _FILE_ into this position.
* `include <file>`  
  Inserts contents of _FILE_ into this position after running **spp** through it.
* `ignore` and `end-ignore`  
  Delete this and the following lines from the final output until `end-ignore` is seen.
* `ignorenext`  
  Deletes this and the following line from the final output.

## Installation ##

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
