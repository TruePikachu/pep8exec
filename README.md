pep8exec
========

Object-oriented pep/8 Simulator


Due to my encountering of a couple bugs in `Pep8Term` (the `ROLr` and `RORr` instructions not working as intended, and `^D` causing an infinite loop), I have had to make modifications to `Pep8Term`. However, that code is a mess, demonstrating all the reasons _not_ to use global variables for everything.

My goal is to write a new version of the simulator which is object-oriented instead of based around huge numbers of global variables. It would also help me understand the architecture a bit better, but that is merely a side-effect.

An additional goal is to help make it easier to understand the code; rather than showing how the operation is written in C++, it would use a syntax similar to RTL. For example, having `Pep8FlagRegister FLAG` and `Pep8Register& r`:
```C++
// ROLr
FLAG.C = r[0];
r.bits(0,14) = r.bits(1,15);
r[15] = FLAG.C;
```
(note that I just wrote that code before I even have headers written up, so actual code is subject to change)

One more goal is to make a program which is more terminal-interactive than `Pep8Term`; one can't, for instance, specify the program to run on the command line in `Pep8Term`, instead needing to use its "menu". `pep8exec` would allow specifying all the `Pep8Term` options on the command line, permitting much easier automation for testing.
