This is a compiler written in C++ for a non-standard programming language. An example of the syntax of which is available in sample_code.ru.
symbol_table.txt would be helpful in observing some of key syntax construct for this language.

This was a personal project done to play with my interest in the fundamentals of compiler construction. Therefore, it is focused more on correctly implementing the various stages of the compilation process rather on efficiency. This project should be taken as a prototype, the ideas of which can be utilised later on.

To run this program you will require the GNU compiler. An example format is:
```
g++ main.cpp <path_to_source_file> -o <out_file_name>
./<out_file_name>
```
