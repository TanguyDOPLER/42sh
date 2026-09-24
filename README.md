# 42SH — Subject

version #

ASSISTANTS C/UNIX 2025 <assistants@tickets.assistants.epita.fr>

### Copyright

This document is for internal use at EPITA (website) only.

Copyright © 2024-2025 Assistants <assistants@tickets.assistants.epita.fr>

The use of this document must abide by the following rules: ▷ You downloaded it from the assistants’ intranet.* ▷ This document is strictly personal and must not be passed onto someone else. ▷ Non-compliance with these rules can lead to severe sanctions.

### Contents

1 The Ultimate Answer 4

2 The reaction 4

3 Preamble 4

3. 1 The UNIX Wars. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 4

3. 2 The POSIX Standard. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 5

3. 3 UNIX Shell and Standardization. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 5

4 Instructions 6

4. 1 42sh. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 6

4. 2 Builtins . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 7

4. 3 Compilation . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 7

4. 4 Testsuite Testing. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 8

5 Assignment 9

5. 1 Step 1 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 9

5. 1.1 Getting Started . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 9

5. 1.2 Usage. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 10

5. 1.3 Exit Status and Errors . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 11

5. 1.4 Simple Commands. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 12

5. 1.5 Command Lists . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 12

5. 1.6 “If” Commands . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 13

5. 1.7 Compound Lists. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 14

5. 1.8 Single Quotes. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 14

5. 1.9 True and False Builtins. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 15

5. 1.10 The echo Builtin . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 15

5. 1.11 Comments. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 15

5. 2 Step 2 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 16

5. 2.1 Redirections. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 16

- https://intra.forge.epita.fr

2

5. 2.2 Pipelines. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 17

5. 2.3 Negation. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 17

5. 2.4 “while” and “until” Commands. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 18

5. 2.5 Operators . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 18

5. 2.6 Double Quotes and Escape Character. . . . . . . . . . . . . . . . . . . . . . . . . . . 18

5. 2.7 Variables. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 18

5. 2.8 “for” Commands . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 19

5. 3 Step 3. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 20

5. 3.1 Built-in commands. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 20

5. 3.2 Command Blocks. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 21

5. 3.3 Functions . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 21

5. 3.4 Command Substitution . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 22

5. 3.5 Subshells. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 22

5. 4 Step 4. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 23

5. 4.1 “Case” Commands . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 23

5. 4.2 Aliases . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 23

5. 4.3 Field Splitting. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 24

5. 5 Advice. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 24

5. 5.1 Testsuite. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 24

5. 5.2 Build Systems. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 25

5. 6 Bonus Features . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 25

5. 6.1 Tilde Expansion. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 25

5. 6.2 Path Expansion. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 25

5. 6.3 Arithmetic Expansion . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 26

5. 6.4 Here-Document. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 26

5. 7 Going Further . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 26

5. 7.1 Prompt. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 27

5. 7.2 Job Control. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 27

6 Bibliography 28

7 Epilogue 29

7. 1 The Search for The Ultimate Question. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 29

7. 2 Douglas Adams’ View. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 29

3

### 1 The Ultimate Answer

According to The Hitchhiker’s Guide to the Galaxy, researchers from a pan-dimensional, hyperintelligent race of beings constructed the second greatest computer in all of time and space, Deep Thought, to calculate the Ultimate Answer to Life, the Universe, and Everything. After seven and a half million years of pondering the question, Deep Thought provides the answer: “forty-two”.

### 2 The reaction

“Forty-two! Is that all you’ve got to show for seven and a half million years work?” yelled Loonquawl. “I checked it very thoroughly,” said the computer, “and that quite definitely is the answer. I think the problem, to be quite honest with you, is that you’ve never actually known what the question is.”

- --Douglas Adams

### 3 Preamble

3. 1 The UNIX Wars

When command line operating systems were first written, every single one had its own kind of command line interface and language, each fairly different from the others.

One amongst many, UNIX was developed by Bell Labs (a research center) for internal use by AT&T (a telephone company).

In the late 1970s, AT&T sold UNIX licenses to academics, which included access to the source code. UNIX variants became popular among academics 1 , and eventually, spread into the computer system business.

By the early 1980s, a number of slightly incompatible UNIX variants were competing. Users could port their programs over from an UNIX system to another without much effort.

Soon, some variants became more popular than others. To avoid becoming irrelevant, smaller vendors decided to build common standards their products should meet (those products were then dubbed “Open Systems”).

Despite initial competition between standards, most of these vanished or merged, and today, only one truly remains: POSIX.

1 Some of the descendants of these operating systems are still in use today, such as OpenBSD, FreeBSD, or NetBSD.

4

3. 2 The POSIX Standard

POSIX stands for Portable Operating System Interface. The most recent POSIX specification defines a standard interface and environment that can be used by an OS to provide access to POSIX-compliant applications. This standard also defines a command interpreter, shell, and common utility programs. POSIX supports application portability at the source code level so they can be built to run on any POSIX-compliant OS.

A brief history of POSIX:

- In 1988, the first version of POSIX was published by the IEEE. Amongst many things, it defined

how much of the C standard library shall work.

- In 1992, the XPG4 standard was integrated into POSIX. Published the same year by the

X/Open company, it included an attempt to define how the UNIX shell 2 language works.

POSIX, as most other standards of the kind, evolves over time. That is why when programming in C, you may have to #define either _POSIX_C_SOURCE or _XOPEN_SOURCE to declare what revision of the standard you expect headers to be compliant to.

By making sure your project is as POSIX-compliant as possible, you can ensure that your 42sh is portable and usable on any POSIX-compliant OS. To check the compliance of your project with the POSIX standard, make sure to compare its behavior with that of the POSIX mode of Bash, which can be started with bash --posix. bash --posix is not always POSIX-compliant, but it is one of the most POSIX-compliant shells. To learn more about what is and what is not POSIX-compliant in bash --posix, you can take a look at the documentation.

Be careful!

When implementing features, prioritize the specifications of the subject over other sources, even if it means not exactly matching the behavior of the real features.

3. 3 UNIX Shell and Standardization

The first release of UNIX in 1971 came with a shell known as the Thompson Shell. In the following 20 years, UNIX derivatives developed their own, better (and sometimes slightly incompatible) shell programs.

Bash, the most used shell in the world nowadays, was first released in 1989, before any specification of the shell programming language was published 3 . The --posix option of Bash was later added to make it POSIX-compliant. Most of Bash’s non-standard features are still available even with this option.

Later, other shells such as Dash (1997) were written with POSIX compliance in mind. However, even Dash deviates from the POSIX specification by a small amount, by including some features that are not part of the specification, and by missing features that are part of it.

The goal of this project is to write a POSIX shell 4 , which is why the subject will often refer you to the POSIX specification. More specifically, you will often have to refer to the Shell Command Language

2 A shell is a user interface. In the UNIX world, shell mostly means “command line user interface”. 3 XPG4 was not published until 1992, so its specification of the UNIX Shell Command Language (SCL) was not integrated into POSIX yet. 4 You will not have time to implement a full POSIX shell, and this subject also contains a few non-standard (but ubiquitous) features.

5

chapter of the Shell and Utilites (XCU) volume of the POSIX specification, which you are highly encouraged to read, as it is essential to the understanding of the project.

### 4 Instructions

- You only have to implement what is required by the subject. If you are unsure, ask.

- You can test your programs using bash --posix, but beware: you do not have to implement

everything it can do. When in doubt, ask.

- You must not use _GNU_SOURCE extensions.

- You must regularly checkout the Assistants ING - Projets newsgroup, where your project

managers can amend the subject, announce conferences and events.

- Everything your program allocates must be freed.

- Your program must not crash or exhibit unreliable behavior.

- You must not strip your program, nor link it statically.

- When submitting your program, you must remove all debugging and logging messages displayed

on standard outputs. Otherwise, you will be penalized.

Some slight changes apply compared to the default EPITA coding style:

- You are allowed to use the goto keyword for error handling only.

- You are allowed to use explicit casts BUT beware that they can cause issues and difficult errors

to debug. Use them wisely.

newpage

4. 1 42sh

File Tree

./

Makefile.am (to submit)

configure.ac (to submit)

src/

- */

- .{c,h} (to submit)

Makefile.am (to submit)

Makefile.am (to submit)

tests/

- (to submit)

6

Makefile.am (to submit)

tests/

- (to submit)

Compilation: Your code must compile with the following flags

- -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

Autotools

- install: Produces and installs the 42sh binary

- all: Produces the src/42sh binary

- check: Runs the testsuite

- clean: Deletes everything produced by make

Forbidden functions: You can use all the functions of the standard C library except

- glob(3)

- regexec(3)

- wordexp(3)

- popen(3)

- syscall(2)

- system(3)

4. 2 Builtins

During the project, you will have to implement several commands, referred to as “builtins”.

Most of the time, when given a command, a shell performs a fork(2) and calls one of the exec(3) functions to execute it. However, this is not the case for some commands called builtins. These are coded directly in the shell, so it does not have to fork and execute it.

The same goes for your 42sh: you must not call one of the exec(3) functions to execute any of the builtin commands this subject mandates.

You can find information about builtins in the bash(1) manual page.

4. 3 Compilation

For your project, you will need to compile with the GNU Autotools build system. Have a look at the documentation or watch the conference replay.

An article is available on the trove to help you configure your build system.

With Autotools, your 42sh will be compiled using the following commands and must appear under src/42sh:

42sh$ autoreconf --install ... 42sh$ ./configure ... (continues on next page)

7

(continued from previous page) 42sh$ make ... 42sh$ ./src/42sh

Your binary must be created under src/42sh. When testing, your 42sh will be installed at a given path using the following commands:

42sh$ autoreconf --install ... 42sh$ ./configure --prefix=/install/at/this/path ... 42sh$ make install ... 42sh$ /install/at/this/path/bin/42sh

You can then retrieve the 42sh binary at the chosen path (/install/at/this/path/bin/42sh).

Be careful!

Please note that the install rule is automatically generated by autotools. You do not have to implement it yourself.

Be careful!

Make sure that no building dependency is necessary to build your project (ex: Criterion) or else you risk failing your submission.

Consider that the environment in which your project will be tested has nothing.

4. 4 Testsuite Testing

You will have to write a testsuite for your project. This testsuite will be evaluated to assess its quality. Your build system must be able to run your testsuite using the following commands:

42sh$ BIN_PATH="$(pwd)/bin/42sh" COVERAGE="yes" make check 42sh$ BIN_PATH="$(pwd)/bin/42sh" OUTPUT_FILE="$(pwd)/out" make check

The first command will run your testsuite in coverage mode and the second one will run your testsuite in regular mode.

In coverage mode, your testsuite must run your unit and functional tests. In regular mode, your testsuite must only run your functional tests.

Be careful!

Your testsuite must not fail (ie. make check must return 0) even if it does not pass all the tests. It must also complete its execution in less than 60 seconds. You should add timeouts to your tests to prevent infinite loops in case you test certain edge cases the sample binaries might not handle. If you are using a shell testsuite, you might want to take a look at the timeout(1) command. If you are using a testing framework (ie. pytest), the documentation surely mentions a way to add timeouts to your tests.

8

Be careful!

BIN_PATH is the absolute path to your 42sh binary. OUTPUT_FILE is the absolute path of the file in which you must write the percentage of tests passed as an integer between 0 and 100 included (no floating point numbers). You must only write the percentage, nothing else. For example, if

42. 5% of the tests passed, you must write 42 in the OUTPUT_FILE. If COVERAGE is set to yes, then

you are in coverage mode. Otherwise, you are in regular mode.

Be careful!

To test your testsuite, make check will be called with random 42sh binaries, as well as other implementations like bash –posix. Hence, your make check in regular mode (ie. without COVER- AGE=”yes”) must not call the binary with your implementation’s custom options.

To make sure your make check works as expected, your testsuite must run when called like so:

42sh$ BIN_PATH="$(which bash)" OUTPUT_FILE="$(pwd)/out" make check

### 5 Assignment

5. 1 Step 1

5. 1.1 Getting Started

This section does not give any assignment but rather advice on how to properly start the project.

Lexer / Parser

For this step, you already need to execute commands. To do so, you will need an AST. However, this AST is given by a parser which itself depends on a lexer.

You cannot afford to wait until you have a fully functional lexer to start writing your parser. Likewise, you cannot wait for the parser to handle every rule to start writing execution functions.

You should first write a temporary rudimentary lexer and parser to be able to execute something early in the step, even if it means rewriting a better lexer and parser later.

This first lexer should only lex what is needed this step, that is:

- if

- then

- elif

- else

- fi

- ;

- \n

9

- '

- words

5. 1.2 Usage

There are three ways 42sh must read an input Shell program:

- It must read its input from a string given using -c.

42sh$ ./42sh -c "echo Input as string" Input as string

- It must read from a file directly given as a positional argument.

42sh$ cat -e script.sh echo Input as file$ 42sh$ ./42sh script.sh Input as file

- When no other source is provided, it must read commands from standard input.

42sh$ cat -e script.sh echo Input through stdin$ 42sh$ ./42sh < script.sh Input through stdin 42sh$ cat script.sh | ./42sh Input through stdin

Your project needs to interpret its command line arguments. If an invalid option is detected, you must print an error message and a usage message, both on the error output, and exit with an error 1 .

The command line syntax is: 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...].

Your 42sh must accept at least the following option:

- -c [SCRIPT]: instead of reading the script from a file, directly interpret the argument as a shell

script.

There are no other mandatory options but you are free to implement any you believe would ease your development or debugging process.

For instance, you could implement a --verbose option for logging, or the --pretty-print option, introduced in this subject.

Tips

Consider using fmemopen(3) to handle string inputs as FILEs.

Pitfalls:

- Implement all three ways of reading input as we will test them.

1 An exit status represents an error if it is not zero.

10

Pretty Printer

During development, the ASTs your parser outputs might contain mistakes. In order to uncover those mistakes, understand which part of your code is at fault and fix it. In order to do that, you have to compare what you expected to parse with what you actually parsed. This process is a whole lot easier when you can display the output of your parser.

Pretty-printing is a very efficient way to diagnose these bugs: it is the process of writing your AST (or any data structure) as easy-to-read text. As an example, the following command:

if echo ok; then echo foobar > example.txt; fi

Could be pretty-printed as:

if { command "echo" "ok" }; then { redir ">example.txt" command "echo" "foobar"; }

The format is up to you and will not be tested, of course.

Directly seeing the shape and contents of your AST will save you a lot of time during debugging sessions on your own or with the assistants. To visualize the AST you can use tools such as Graphviz.

You should print an output you can reparse. By doing that, you can test the output by giving it as argument to your 42sh.

You should enable this feature through a command-line option or an environment variable when calling your program.

For instance:

./42sh --pretty-print example.sh

or

PRETTY_PRINT=1 ./42sh example.sh

This feature does not take a lot of time to implement, and brings immense improvements during development. You should probably implement it as soon as you have an AST. Do not restrain yourself and feel free to add any information you deem useful.

5. 1.3 Exit Status and Errors

You have to handle errors as described in the corresponding section of the SCL.

Going further...

In 42sh, we use the exit code 2 for command line argument and grammar errors.

11

5. 1.4 Simple Commands

Tips

Most features have a few lines of grammar at the top which describe what the minimum grammar requirements for this feature are.

Please read the section of the guide about shell syntax to learn more.

To avoid copying the entire shell grammar for every feature, every grammar block only shows the changes compared to the previous feature.

In case this evolution is unclear, the guide contains a full shell grammar.

input = list '\n' | list EOF | '\n' | EOF ;

(* Of course, many of these rules make little sense for now... *) list = and_or; and_or = pipeline; pipeline = command; command = simple_command; simple_command = WORD { element} ; element = WORD;

To implement the execution of simple commands such as ls /bin, you will need:

- a lexer which produces word tokens.

- a parser that accepts word tokens and produces at least a simple_command AST node.

- an execution module which travels through your AST, executes the simple_command node, waits

for its status code, and returns it.

Tips

Use the execvp(3) version of exec(3): it searches the location of the executable in the PATH for you.

5. 1.5 Command Lists

(* list makes more sense now, right? *) list = and_or { ';' and_or} [ ';' ] ; and_or = pipeline; pipeline = command; command = simple_command;

At this stage of the project, command, pipeline, and and_or are the same as simple_command. As you implement more features, this will change.

Your shell has to be able to group commands together in a list. At this stage, you only have to handle command lists as follows:

12

# this line must be represented in a single `command_list` AST node echo foo; echo bar

# just like this one with a semicolon at the end echo foo; echo bar;

In order to handle command lists:

- your lexer must recognize; tokens.

- you need to parse and design a special AST node for command lists.

5. 1.6 “If” Commands

(* a command can now also be a shell_command *) command = simple_command | shell_command ;

(* for the time being, it is limited to a single rule_if *) shell_command = rule_if;

rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;

else_clause = 'else' compound_list | 'elif' compound_list 'then' compound_list [else_clause] ;

compound_list = and_or [';'] {'\n'} ;

You have to handle if commands, as in the SCL:

- your lexer must recognize if, then, elif and else as special tokens.

- your AST has to have a node for conditions.

- your parser must handle the if token returned by the lexer, parse the condition, the then token,

the true branch, a series of elif branches, the else branch, and finally, fi.

- your execution module must evaluate the condition, and run the then or the else compound_list

depending on its exit code.

Pitfalls:

- The condition and body of ifs are compound lists. We’ll implement them in the next part

but note how we do not simply reduce the rule to and_or. We need separators to delimit commands and recognize if’s separator tokens such as then and fi.

13

5. 1.7 Compound Lists

compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or} [';'] {'\n'} ;

Compound lists are just like command lists, only with a few tweaks:

- this variant only appears inside code blocks such as conditions or functions.

- compound list can separate commands using newlines instead of;.

Compound lists are what enables conditions like this:

if false; true; then echo a echo b; echo c; fi

Or even:

if false true then echo a echo b; echo c fi

In order to handle compound lists:

- your lexer must recognize newline tokens.

- you do not need another AST type, as compound lists are executed just like lists.

Tips

This tip only applies if you are writing a recursive descent parser.

When you meet a keyword which ends a control flow structure (such as then or fi), you have to stop parsing your compound list, and the function which called parse_compound_list decides if this keyword is appropriate.

5. 1.8 Single Quotes

Implementing this feature takes two main changes:

- implement lexing of single quotes.

- implement expansion of single quotes during execution.

The behavior of single quotes is specified by the SCL.

14

5. 1.9 True and False Builtins

As builtins, true and false are parsed as simple commands.

true (resp. false) must do nothing and return 0 (resp. 1).

Be careful!

Builtins are not executed in the same manner as other simple commands: they are directly executed inside your shell, and thus do not require fork or exec.

5. 1.10 The echo Builtin

The echo builtin command prints its arguments separated by spaces, and prints a final newline.

Your implementation of this command does not have to comply with POSIX.

You have to handle the following options:

- -n inhibits printing a newline.

- -e enable the interpretation of \n, \t and \\ escapes.

- -E disable the interpretation of \n, \t and \\ escapes.

Be careful!

You need to call fflush(stdout) after running builtins.

5. 1.11 Comments

Implement the recognition of comments. As specified by the SCL, a comment always start with a # and can only span one line.

# This is a comment echo "Hello world" # This is also a comment

Be careful!

The # character is not considered as the beginning of a comment if it is quoted, escaped or not the first character of a word.

42sh$ echo \#escaped "#"quoted not#first #commented #escaped #quoted not#first

15

5. 2 Step 2

5. 2.1 Redirections

command = simple_command | shell_command { redirection} ;

simple_command = prefix { prefix} | { prefix} WORD { element} ;

prefix = redirection;

redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' ) WORD;

element = WORD | redirection ;

Implement the execution of redirections as described in the SCL. You do not have to handle Here-Documents in this module.

Of course, redirections must work correctly with any command.

Be careful with your file descriptors, and do not forget to test this part thoroughly as there may be some corner cases you did not handle in your first implementation.

Pitfalls:

- Redirections must work for builtins and functions. Consider the following code:

echo tofile >file.txt echo tostdout

This shell program only uses builtins: fork will not be called, no new processes will be created.

If your shell performs the >file.txt redirection for the first command and does not reverse it, the second command will write to file.txt too.

You have to save the file descriptors you override, and restore those to their former value when undoing the redirection.

- File descriptors are a scarce resource. On many systems, you can only have 1024 open file

descriptors at once. If your redirection code inadvertently leaves file descriptors open, you may run out and get an error.

- When a new process is created using fork, it gets a copy of all of its parent’s file descriptors.

By default, the same thing occurs with exec.

When you run an external program like ls, it does not need the saves of file descriptors you made for redirections.

16

You can configure a file descriptor to be automatically closed on exec using fcntl(fd, F_SETFD, FD_CLOEXEC) (you also close these by hand if you really want to).

Tips

To check which file descriptors are open for a given process, you can run ls -l /proc/ ${PID_YOUR_PROCESS}/fd. You can also run ls -l /proc/self/fd to get which file descriptors are open for ls itself.

It can be combined with a command which gets the PID of a process by name: ls -l "/proc/ $(pgrep -n 42sh)/fd".

5. 2.2 Pipelines

pipeline = command { '|' {'\n'} command} ;

Implement pipelines as specified by the SCL.

The exit status of the pipeline is the exit status of the last command:

- true | false exits 1

- false | true exits 0

Pitfalls:

- waitpid must be called on all started processes.

- Please refer to the guide for detailed explanations of pipe related pitfalls.

5. 2.3 Negation

pipeline = ['!'] command { '|' {'\n'} command} ;

Adding a! reverses the exit status of the pipeline (even if there is no pipe):

- true exits 0

- ! true exits 1

- false exits 1

- ! false exits 0

You will need to:

- Handle! as a reserved word (not an operator!) in your lexer.

- Add a new AST node type.

- Parse and execute it.

Pitfalls:

17

- Even if negation appears inside pipelines in the grammar, it makes little sense to perform

negation inside a pipeline AST node. You can just have a separate AST node and only create it when needed.

5. 2.4 “while” and “until” Commands

shell_command = rule_if | rule_while | rule_until ;

rule_while = 'while' compound_list 'do' compound_list 'done' ; rule_until = 'until' compound_list 'do' compound_list 'done' ;

Implement the execution of while and until loops, as specified by the SCL.

5. 2.5 Operators

and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline} ;

Implement the execution of the “&&“ and “||“ operators, as specified by the SCL.

5. 2.6 Double Quotes and Escape Character

Implement the lexing and expansion of double quotes and escape characters, as described in the SCL.

Pitfalls:

- This part has complicated interactions with later features, such as subshells.

- You have to make sure your expansion algorithm is the same as your lexing algorithm. If

the two disagree, weird bugs will ensue.

5. 2.7 Variables

prefix = ASSIGNMENT_WORD | redirection ;

Implement variable assignment and simple variable subsitutions, as described by the SCL.

ASSIGNMENT_WORD is a special WORD as specified by the SCL.

You do not have to implement expansion modifiers, only the “$name” and “${name}” formats will be tested.

The following special variables must be properly expanded:

18

- $@

- $*

- $?

- $$

- $1 … $n

- $#

- $RANDOM

- $UID

The following environment variables 1 must also be properly expanded:

- $OLDPWD

- $PWD

- $IFS

Tips

You do not have to handle IFS splitting yet as it is a Step 4 feature.

Pitfalls:

- Because of $@, expansion outputs an array of strings.

5. 2.8 “for” Commands

shell_command = rule_if | rule_while | rule_until | rule_for ;

rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD} ( ';' | '\n' ) ] ) {'\n'} 'do' compound_list ↪→'done' ;

Implement the execution of for loops, as specified by the SCL.

1 To learn more about environment variables, you can take a look at the SCL section about them.

19

5. 3 Step 3

5. 3.1 Built-in commands

“exit”

Implement the exit builtin.

For more information about exit, please refer to the SCL

Pitfalls:

- All resources (allocated memory and file descriptors) should be released before calling exit.

The easiest way to handle this is to have a special kind of “error” which stops execution and exits normally.

“cd”

Implement the cd builtin. You do not have to implement the -L and -P options, nor follow the required behavior for the CDPATH variable. However, you have to implement cd -.

For more information about cd, please refer to the SCL.

You must also update the PWD and OLDPWD environment variables.

Beware, the shell keeps track of the path which was taken through symlinks:

mkdir -p /tmp/test_dir ln -s /tmp/test_dir /tmp/link cd /tmp/link

# the shell knows the current directory is also known as /tmp/link echo "$PWD"

# pwd does not, as it is an external command. env -i ensures the PWD # environment variable is not passed down, and avoids executing a potential # builtin implementation of pwd env -i pwd

“export”

Implement the export builtin. You do not have to handle printing all exported variables, only the export NAME=VALUE and export NAME uses will be tested.

For more information about export, please refer to the SCL.

20

“continue” and “break”

Implement the continue and break builtins.

For more information about these, please refer to the SCL:

- SCL specification for continue

- SCL specification for break

Pitfalls:

- Mind the corner cases of breaking / continuing out of more loops than are currently active.

“dot”

Implement the. builtin, as specified by the SCL.

“unset”

Implement the unset builtin with all its options, as specified by the SCL.

5. 3.2 Command Blocks

shell_command = '{' compound_list '}' | rule_if | rule_while | rule_until | rule_for ;

Command Blocks are a way to explicitly create command lists. They are useful for creating function bodies, as well as grouping commands together in redirections. However, they can be used anywhere.

{ echo a; echo b; } | tr b h

foo() { echo this is inside a command block; }

5. 3.3 Functions

command = simple_command | shell_command { redirection} | funcdec { redirection} ;

funcdec = WORD '(' ')' {'\n'} shell_command;

21

Implement function definition and execution. This includes, of course, redirections to functions and argument transmission. Don’t forget to take a look at the SCL section on function definition.

Pitfalls:

- Functions have to hold a reference to some part of your AST which should otherwise be

freed at the end of each command. It means that you either have to make a copy of part of your AST (the body of the function), or prevent it from being freed at the end of the “line”. This can be accomplished fairly easily using reference counting.

- Functions can be defined in any command. This is valid:

foo() { bar() { echo foobar } }

# defines bar foo

# prints foobar bar

5. 3.4 Command Substitution

Implement command substitution, as described by the SCL.

Pitfalls:

- At this point, your lexer needs to be recursive and remember the context that is currently

active. The context is saved when entering a new context and restored when leaving a context.

- Your will have a hard time keeping your lexer and expansion in sync unless you create some

kind of library.

5. 3.5 Subshells

shell_command = '{' compound_list '}' | '(' compound_list ')' | rule_for | rule_while | rule_until | rule_if ;

Subshells run commands in a new process.

42sh$ a=sh; (a=42; echo -n $a);echo $a 42sh

22

Please refer to the SCL specification.

### 5.4 Step 4

5. 4.1 “Case” Commands

shell_command = '{' compound_list '}' | '(' compound_list ')' | rule_for | rule_while | rule_until | rule_case | rule_if ;

rule_case = 'case' WORD {'\n'} 'in' {'\n'} [case_clause] 'esac' ;

case_clause = case_item { ';;' {'\n'} case_item} [';;'] {'\n'} ;

case_item = ['('] WORD { '|' WORD} ')' {'\n'} [compound_list] ;

Implement the case construct, as specified by the SCL.

5. 4.2 Aliases

Implement alias handling, as specified by the SCL.

It requires:

- Implementing the alias and unalias builtins, abiding by the SCL specification (for alias and for

unalias).

- Using the alias list for substitutions inside the lexer.

Substitutions are performed at the token level:

alias funcdec='foo(' funcdec) { echo ok; } foo

Your have to lex, parse, and execute one line at a time. Otherwise, your lexer will not know about your aliases in time.

You do not have to follow the behavior specified in the SCL when the alias ends with trailing spaces. You also do not have to implement alias listing (when called without argument).

# this does not work, as the whole line is lexed and # parsed as a whole (it matches the list grammar rule) alias foo=ls; foo

# this works, as the bar alias was registered before bar was lexed (continues on next page)

23

(continued from previous page) alias bar=ls bar

5. 4.3 Field Splitting

Implement field splitting, as specified by the SCL.

5. 5 Advice

42sh is not an easy project to implement, let alone to debug. This section highlights some techniques and features meant to ease your development and debugging.

5. 5.1 Testsuite

You might have realized how important a strong testsuite is for a project of this kind. We strongly advise you to build one which at least:

- implements all the needed functions to really test your program.

- prevents regression.

- gives you the possibility to follow the progress you made.

You can write unit tests to check the behaviour of some functions, but functional tests are handier for this project. You should focus on them.

Here are some useful milestones to help you determine how advanced your testsuite is:

Test Program

We recommend you to write a test program whose output would follow an easily readable format. Here are some tips:

- issue only one line per test.

- you must be able to clearly understand the result of the test. This means that at least failure

or success should be printed. It is desirable to display the cause of a failure: standard output, error output, exit value or any combination among those three pieces of information.

- group tests into categories: you want to be able to clearly identify the category of the running

test. Before testing a category, we advise you to display its name followed by a blank line, and after the tests, a blank line and the result of the category (with a percentage or the number of successful and failed tests).

- display the global result of your tests after the execution.

A category represents a whole set of tests aiming at evaluating a particular part of your 42sh.

24

Tests Format

As your tests are distributed into categories, you might want to have the tests of a category grouped in a directory whose name matches the test category.

For instance:

42sh$ ls -l tests/categories total 20K drwx------ 2 login_x epita 4,0K 2023-01-03 18:42 echo/ drwx------ 2 login_x epita 4,0K 2023-01-03 18:42 pipes/ drwx------ 2 login_x epita 4,0K 2023-01-03 18:42 simple_commands/

We also advise you to have one file per test. This file must at least contain:

- a test description.

- the input.

If you want to compare the standard output and standard error of your tests with bash --posix, you do not need to store that of the latter in files. However you can make your script generate the expected output into a file.

5. 5.2 Build Systems

Using a build system for 42sh is not a constraint. Use it wisely to integrate various development tools and coordinate everything in one place. Incorporate some rules into your build system to gain efficiency: generating documentation, bindings, launching a testsuite, linting…

5. 6 Bonus Features

5. 6.1 Tilde Expansion

Implement “~” expansion as specified by the SCL.

This only works in a limited number of contexts, so some indication has to be given to expansion to limit its scope.

5. 6.2 Path Expansion

Implement the expansion of the following special parameters (also known as metacharacters and wildcards):

- *

- ?

- [], with the special meaning of the “-“ and “!” characters

You should also handle all globbing character classes ([:alnum:] etc.).

25

5. 6.3 Arithmetic Expansion

Implement the expansion of arithmetic expressions, which are wrapped by $(()).

You have to handle variables and the following operators: -, +, *, /, **, &, |, ^, &&, ||,! and ~. You do not have to handle other operators.

For more information, please refer to the SCL.

Pitfalls:

- Your lexer and expansion have do deal with an ambiguity for expansions beginning with

$((. Please refer to the SCL.

5. 6.4 Here-Document

redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' ) WORD | [IONUMBER] ( '<<' | '<<-' ) HEREDOC ;

Implement execution of Here-Documents, as described in the SCL. Once again, be careful with your file descriptors.

Pitfalls:

- You have to both read and write to the same fd in the same process. The easiest way to

do that is to write the content of the heredoc in a temporary file, and read it afterwards. You can also use a pipe, as long as the size of the expanded variable does not exceed the capacity of the pipe (your process will block as soon as the pipe is full).

5. 7 Going Further

If you implemented all previous modules and are confident about them you can consider doing the following.

Be careful!

These features are not tested, and you will not get any bonus points for implementing these.

You will only succeed in impressing friends, family, and teaching assistants.

26

5. 7.1 Prompt

Your 42sh must show a prompt if the shell is in interactive mode. You only have to implement PS1 and PS2 prompts as described by the SCL.

5. 7.2 Job Control

list = and_or { ( ';' | '&' ) and_or} [ ';' | '&' ] ;

compound_list = {'\n'} and_or { ( ';' | '&' | '\n' ) {'\n'} and_or} [ ';' | '&' ] {'\n'} ;

Your 42sh must be able to launch and manage more than one command at the same time. You should add the & operator.

You shall also implement the following builtin commands:

- jobs (with option -l)

- wait [n]

and the $! variable.

Switching between processes must be apparent, and no zombie processes shall be left behind.

You can read this manual to get hints on how to do it.

27

### 6 Bibliography

- Shell Resources:

– https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html

– https://shell.multun.net/

– https://en.wikipedia.org/wiki/Recursive_descent_parser

– https://www.gnu.org/software/bash/manual/

– https://zsh.sourceforge.io/Doc/

– http://www.kornshell.com/doc/

– https://www.tcsh.org/

- Open Source Shells

– https://github.com/emersion/mrsh/

– https://github.com/Swoorup/mysh/

– https://github.com/brenns10/lsh

– https://github.com/Fedjmike/tush (not POSIX)

- Crafting Interpreters

– https://craftinginterpreters.com/contents.html

- Programming Philosophies:

– https://martinfowler.com/agile.html

– https://wiki.c2.com/?ExtremeProgrammingRoadmap

– https://www.agilealliance.org/agile101/subway-map-to-agile-practices/

– https://manifesto.softwarecraftsmanship.org/

- Git and Related Tools:

– https://git-scm.com

– https://learngitbranching.js.org/

– https://docs.gitlab.com/ee/user/project/repository/repository_mirroring.html

– https://docs.gitlab.com/ee/ci/

- Build Systems:

– https://www.lrde.epita.fr/~adl/dl/autotools.pdf

– https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.71/html_ node/index.html

– https://www.gnu.org/software/automake/manual/automake.html

- Documentation:

– https://www.doxygen.nl/index.html

28

### 7 Epilogue

7. 1 The Search for The Ultimate Question

Deep Thought informs the researchers that it will design a second and greater computer, incorporating living beings as part of its computational matrix, to tell them what the question is. That computer was called Earth and was so big that it was often mistaken for a planet. The researchers themselves took the apparent form of mice to run the program. The question was lost, five minutes before it was to have been produced, due to the Vogons demolition of the Earth, supposedly to build a hyperspace bypass. Later in the series, it is revealed that the Vogons had been hired to destroy the Earth by a consortium of philosophers and psychiatrists who feared for the loss of their jobs when the meaning of life became common knowledge.

- --Douglas Adams

7. 2 Douglas Adams’ View

Douglas Adams was asked many times during his career why he chose the number “forty-two”. Many theories were proposed, but he rejected them all. On November 3rd, 1993, he gave an answer on alt. fan.douglas-adams:

The answer to this is very simple. It was a joke. It had to be a number, an ordinary, smallish number, and I chose that one. Binary representations, base thirteen, Tibetan monks are all complete nonsense. I sat at my desk, stared into the garden and thought ‘42 will do’. I typed it out. End of story.

- --Wikipedia

Seek strength. The rest will follow.

29
