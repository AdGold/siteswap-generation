#include <iostream>
#include <cstdlib>

#include "args.hxx"

void printSiteswap(int *stack, int period) {
    int place, ss;
    for (int i = 0; i < period; ++i) {
        // Place holds the current position in the stack of balls
        ss = place = stack[i];
        for (int j = 1; place > 0; ++j) {
            if (stack[(i+j)%period] < place)ss++;
            else place--;
        }
        std::cout << ss << " ";
    }
    std::cout << "\n";
}

// Checks if 'ss' is in its maximum cyclic rotation
bool isMaxRotation(int *ss, int period) {
    for (int i = 0; i < period; ++i) {
        for (int j = 0; j < period; ++j) {
            int a = ss[(j+i)%period];
            if (ss[j] > a) break;
            else if (a > ss[j]) return false;
        }
    }
    return true;
}

void getStack(int *done, int nDone, bool seenB, bool seen0, int period, int balls) {
    if (nDone == period) {
        // Prevent duplicates from different rotations
        if (seen0 && seenB && isMaxRotation(done, period))
            printSiteswap(done, period);
        return;
    }
    for (int i = 0; i <= balls; ++i) {
        done[nDone] = i;
        getStack(done, nDone+1, seenB || (i == balls), seen0 || (i == 0), period, balls);
    }
}

int main(int argc, char const *argv[]) {
    args::ArgumentParser parser("Generate siteswaps using stack notation.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::CompletionFlag completion(parser, {"complete"});
    args::Positional<int> balls(parser, "balls", "The number of balls to use", args::Options::Required);
    args::Positional<int> period(parser, "period", "The period to use", args::Options::Required);
    args::Flag comp(parser, "comp", "Skip definitely prime patterns of the form Bxxx with x < B = #balls.", {'c', "comp"});
    args::Flag zero(parser, "zero", "Only print patterns with zeros", {'z', "zero"});
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
    int done[args::get(period)];
    done[0] = args::get(balls);
    getStack(done, 1, !args::get(comp), !args::get(zero), args::get(period), args::get(balls));
    return 0;
}
