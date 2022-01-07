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