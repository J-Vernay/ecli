# ecli
ecli is a simple CLI parser for C++. It consists of a single header file which you can copy in your project.
The following complete example illustrates the simplicity of ecli (from `examples/hello.cpp`):

```cpp
#include "../ecli.hpp"

struct MyOptions : Ecli<MyOptions> {
    Option
        hello{"--hello", Argument, "Greets the given name."},
        helloworld{"--hello-world", Switch},
        french{"--french", Switch, "Greets in French."};
};

int main(int argc, char** argv) {
    MyOptions opt;
    int nb_positional = opt.parse(argc, argv, "Small program to output some greetings.");
    if (opt.helloworld) {
        std::printf(opt.french ? "\tBonjour tout le monde !\n" : "\tHello, world!\n");
    } else if (opt.hello) {
        std::printf(opt.french ? "\tBonjour, %s!\n" : "\tHello, %s!\n", opt.hello.value);
    } else {
        std::printf("Positional arguments:\n");
        for (int i = 0; i < nb_positional; ++i) {
            std::printf("\t%s\n", argv[i]);
        }
    }
    return 0;
}
```

This results in the following outputs:
```
$ ./out --help
Small program to output some greetings.

        --hello ...     Greets the given name.
        --hello-world  
        --french        Greets in French.

$ ./out --hello=Julien --french
        Bonjour, Julien!
$ ./out --french --hello Madame
        Bonjour, Madame!
$ ./out --not --arg abc
Positional arguments:
        ./out
        --not
        --arg
        abc
```

As you can see, the API is really simple, however the API is lacking validation
and conversion tools. ecli can be useful for prototyping and simple CLI applications.

