#include <bitset>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unordered_set>

#include "args.hxx"

#define MAX_HEIGHT 250

bool isPrime(int ss[], int period) {
    int ma = 0;
    for (int i = 0; i < period; ++i) {
        ma = std::max(ma, ss[i]);
    }
    assert(ma < MAX_HEIGHT);
    std::bitset<MAX_HEIGHT> state;
    int reps = ma / period + 1;
    for (int r = 0; r < reps; ++r) {
        for (int i = 0; i < period; ++i) {
            int lands = r * period + i + ss[i];
            if (lands >= reps * period)
                state.set(lands - reps * period);
        }
    }
    std::unordered_set<std::bitset<MAX_HEIGHT>> seen_states;
    seen_states.insert(state);
    for (int i = 0; i < period-1; ++i) {
        state >>= 1;
        if (ss[i] != 0) {
            state.set(ss[i]-1);
        }
        if (!seen_states.insert(state).second) {
            return false;
        }
    }
    return true;
}

int main(int argc, char const *argv[]) {
    args::ArgumentParser parser("Count prime or composite siteswaps from input.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::CompletionFlag completion(parser, {"complete"});
    args::Positional<int> period_flag(parser, "period", "The period to use", args::Options::Required);
    args::Flag verbose_flag(parser, "verbose", "Don't print updates", {'v', "verbose"});
    args::Flag print_flag(parser, "print", "Print the prime/composite siteswaps being counted", {'p', "print"});
    args::Flag comp_flag(parser, "comp", "Count the number of composites instead of primes", {'c', "comp"});
    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::Completion& e) {
        std::cout << e.what();
        return 0;
    } catch (const args::Help&) {
        std::cout << parser;
        return 0;
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
	int period = args::get(period_flag);
    bool count_comp = args::get(comp_flag);
    bool verbose = args::get(verbose_flag);
    bool print = args::get(print_flag);
    unsigned long long count = 0;
    int ss[period];
    unsigned long long c = 0;
    while (std::cin >> ss[0]) {
        for (int i = 1; i < period; ++i) {
            std::cin >> ss[i];
        }
        if (verbose && (c & 0xFFFFF) == 0) {
            std::cout << "Looked at: " << c << ", count: " << count << std::endl;
        }
        if (count_comp ^ isPrime(ss, period)) {
            if (print) {
                for (int i = 0; i < period; ++i) {
                    std::cout << ss[i] << " ";
                }
                std::cout << "\n";
            }
            count++;
        }
        ++c;
    }
    std::cout << count << "\n";
}
