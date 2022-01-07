// 'ecli', a simple C++ library for parsing command line arguments.
// Repository: https://github.com/J-Vernay/ecli
// Julien Vernay 2022 - contact@jvernay.fr
// Boost Software License - Version 1.0 - August 17th, 2003


#ifndef JVERNAY_ECLI_
#define JVERNAY_ECLI_

#include <cstring>
#include <cstdio>
#include <cstdlib>

/// Empty structure which you must inherit to create your options
/// by adding members of type 'Option'.
/// The template argument mut be your derived class, this is known
/// as Curiously-Recurring Template Pattern.
template<typename CRTP = void>
struct Ecli {
    /// Encodes option kind and state.
    /// 'Argument' is default and requires an additional value.
    /// 'Switch' is for flags not requiring additional values.
    enum OptionState { Argument = 1, Switch = 2, _processed = -1 };

    /// Structure representing an option. Overwritten on parsing,
    /// 'state' and 'value' are modified.
    struct Option {
        /// Name of the option.
        char const* name;
        /// Kind of option, overwritten on parsing.
        OptionState state = Argument;
        /// Description of option, overwritten on parsing with its
        /// value or 'nullptr' if unspecified.
        char const* value;
        /// Implicit conversion to 'char const*' to return 'value'.
        operator char const*() const noexcept { return value; }
    };

    /// Parse given arguments into given options.
    /// This will 'destruct' argv by moving first the positional arguments.
    /// Returns the number of positional arguments.
    int parse(int argc, char** argv, char const* intro_msg = "Some program...") noexcept {
        static constexpr int nb_options = sizeof(CRTP) / sizeof(Option);
        static_assert(sizeof(Option[nb_options]) == sizeof(CRTP),
            "Derived structure must contain only 'Option' members.");
            
        Option* options = reinterpret_cast<Option*>(this);
        int nb_positional = 0;
        // Find matching option name and argument.
        for (int i = 0; i < argc; ++i) {
            if (std::strcmp(argv[i], "--help") == 0) {
                CRTP::help(intro_msg);
                std::exit(EXIT_SUCCESS);
            }
            int j;
            for (j = 0; j < nb_options; ++j) {
                Option& opt = options[j];
                if (opt.state == _processed)
                    continue; // Already processed option, skip.
                
                int opt_size = std::strlen(opt.name);
                if (std::strncmp(opt.name, argv[i], opt_size) != 0)
                    continue; // Different option name, skip.
                
                if (argv[i][opt_size] == '=') {
                    // The option name ends there, a value is provided in same argument.
                    opt.value = argv[i] + opt_size + 1;
                } else if (argv[i][opt_size] == '\0') {
                    // The option name ends there, the potential value is provided after.
                    if (opt.state == Argument) opt.value = (i + 1 == argc ? "" : argv[++i]);
                    if (opt.state == Switch) opt.value = "";
                } else {
                    continue; // The option name did not end there, false positive, skip.
                }
                opt.state = _processed;
                break;
            }
#ifndef JVERNAY_ECLI_PREVENT_DESTRUCTIVE
            if (j == nb_options) {
                // Not found, this is a positional argument:
                argv[nb_positional++] = argv[i];
            }
#endif
        }
        // Non-processed options have value 'nullptr'.
        for (int j = 0; j < nb_options; ++j) {
            if (options[j].state != _processed)
                options[j].value = nullptr;
        }
        return nb_positional;
    }

    /// Emit the help and exit the program. May be overriden in derived structure.
    static void help(char const* intro_msg) {
        static constexpr int nb_options = sizeof(CRTP) / sizeof(Option);
        // Create a copy to ensure initial values.
        CRTP initial_options{};
        // Find max size for options name and description.
        int name_size = 0;
        Option* options = reinterpret_cast<Option*>(&initial_options);
        for (int j = 0; j < nb_options; ++j) {
            int size = std::strlen(options[j].name);
            if (options[j].state == Argument)
                size += 4; // size of " ..."
            if (name_size < size)
                name_size = size;
        }
        std::printf("%s\n\n", intro_msg);
        // Display all options.
        for (int j = 0; j < nb_options; ++j) {
            if (options[j].value == nullptr)
                options[j].value = "";
            if (options[j].state == Argument)
                std::printf("\t%s %-*s  \t%s\n", options[j].name, name_size - (int)std::strlen(options[j].name) - 1, "...", options[j].value);
            else
                std::printf("\t%-*s  \t%s\n", name_size, options[j].name, options[j].value);
        }
        std::printf("\n");
    }
};


#endif