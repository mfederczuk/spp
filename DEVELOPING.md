# Development Guidelines #

## General ##

The following rules **do not** apply to binary files.

* **Encoding & Character Set**  
  All files must be encoded using **UTF-8** and use **Unicode** as the character set.  
  The only **ASCII** control characters that are allowed in files are the horizontal tab and the line break character.  
  In source code, identifiers should never contain non-**ASCII** characters and string literals should only consist of
  printable **ASCII** characters if possible. (make use of escape sequences)

* **Use Unix-like line endings** (`LF`)

* **Max line length is 120 characters**

* **No trailing whitespace**
  (there are some exceptions to this rule, see [.editorconfig](.editorconfig))

* **Files must have exactly one trailing newline character**

## Coding ##

* **On the subject of tabs & spaces**

  Since this topic is largely based on the programming/development language that is being used, there are some
  exceptions to the these specific rules.

  * **Indent with tabs & align with spaces**

    ```c
    // wrong
    enum·color·{
    →	RED→	=·0xFF0000,
    →	GREEN→	=·0x00FF00,
    →	BLUE→	=·0x0000FF
    };
    ```

    ```c
    // wrong
    enum·color·{
    ····RED···=·0xFF0000,
    ····GREEN·=·0x00FF00,
    ····BLUE··=·0x0000FF
    };
    ```

    ```c
    // right
    enum·color·{
    →	RED···=·0xFF0000,
    →	GREEN·=·0x00FF00,
    →	BLUE··=·0x0000FF
    };
    ```

  * **Tab Width**

    When calculating the length of a line, a tab counts for 4 characters.  
    Locally, the tab width may be changed to any value needed, if it helps with visual clarity.

  * **Tabs must only appear at the beginning of lines**

* **Prefer guard clauses & avoid `else`**

  ```c
  // wrong
  void func(int x) {
  	if(x == 0) {
  		foo();
  	} else {
  		bar();
  	}
  }
  ```

  ```c
  // right
  void func(int x) {
  	if(x == 0) {
  		foo();
  		return;
  	}

  	bar();
  }
  ```

* **Don't rely on "truthy"/"falsy" values**

  ```js
  // wrong
  const str = "";
  if(str) { // hard to understand; what does this actually mean?
  }
  ```

  ```js
  // right
  const str = "";
  if(typeof(str) === "string" && str.length > 0) { // easy to understand; no ambiguity what this check is supposed to do
  }
  ```

## Git ##

* **Branching System**  
  This repository uses **[t3m v1.0.0-rc01]** as its branching system.  
  It doesn't have to be followed and won't be enforced 100%.

* **One commit should contain one logical change**  
  One commit should only be a single logical change, like fixing a bug or changing formatting.  
  This means that changes may also span several files.  
  Changes to documentation, based on the changed code (e.g.: API references, changelog files, …), can also be
  incorporated into the commit.

* **Prefer multiple smaller commits**  
  This point goes hand-in-hand with the previous one; split an entire feature into multiple smaller commits rather than
  having one big commit with everything.

* **Don't Squash**  
  It doesn't matter if a branch has 100 or more commits, as long as each one of those commits is one logical change,
  ***do not squash them down to a single commit***.

* **Never commit broken code**  
  Never commit code with wrong syntax, compilation errors, static semantic errors or missing assets/resources.  
  Every commit should be a functional build.

* **Commit Message guidelines**
  * The subject line should be capitalized (like in a sentence)
  * Use the imperative mood & present tense in the subject line
  * The subject line should consist of maximum 50 characters
  * The subject line should not end with a period
  * The subject line should be a short summary of the changes
  * Separate the subject line from the message body with an empty line
  * Wrap the message body after 72 characters
  * The message body should primarily be used to explain *what* changed and *why* this change was necessary instead of
    *how* the change was implemented
  * The entire message should not be just a reference to an issue that got fixed

* **Merge Commit Messages**

  When merging into the master branch...

  * the message of non-release commits should be: _"Merge in [feature/change/fix]"_
  * the message of release commits should be: _"Release v[version name]"_

[t3m v1.0.0-rc01]: https://github.com/mfederczuk/t3m/tree/v1.0.0-rc01
