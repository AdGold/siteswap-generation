#include <iostream>
#include <algorithm>
#include <cstdlib>

#include "args.hxx"

int intcmp(int *a, int *b, int len) {
    for (int i = 0; i < len; ++i) {
        if (a[i] != b[i]) return a[i]-b[i];
    }
    return 0;
}

/*
 * Checks if a composition/base pair should be used. Criteria:
 * composition must be in maximum rotation
 * if compostition has more than one maximum rotation (e.g. (1,0,1,0) or (1,1,1))
 * for each maximum rotation, the corresponding rotation of base must be maximal
 */
int testComp[100], testBase[100]; //permanent arrays to store double the composition or base
bool checkCompositionBase(int *composition, int *base, int period) {
    std::copy(composition, composition+period, testComp);
    std::copy(composition, composition+period, testComp+period);
    std::copy(base, base+period, testBase);
    std::copy(base, base+period, testBase+period);
    for (int i = 1; i < period; i++) {
        int tmp = intcmp(testComp, testComp+i, period);
        if (tmp < 0)return false;
        else if (tmp == 0) {
            if (intcmp(testBase, testBase+i, period) < 0) return false;
        }
    }
    return true;
}

int next_composition(int first, int *comp, int len, int sum, int &min) {
    for (int i = len-1; i > 0; --i) {
        if (comp[i] == 0) continue;
        if (comp[i-1] >= first) continue;
        if (i-1 < min) {
            min = i-1;
            comp[i-1] = 1;
            comp[len-1] = sum-1;
            for (int j = i; j < len-1; ++j) comp[j] = 0;
            return 1;
        } else {
            comp[i-1] += 1;
            int x = 0;
            for (int j = i; j < len-1; ++j) {
                x += comp[j];
                comp[j] = 0;
            }
            comp[len-1] += x - 1;
            return 1;
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    args::ArgumentParser parser("Generate siteswaps using Burkard Polster's 'anti-permutation method'", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::CompletionFlag completion(parser, {"complete"});
    args::Positional<int> balls_flag(parser, "balls", "The number of balls to use", args::Options::Required);
    args::Positional<int> period_flag(parser, "period", "The period to use", args::Options::Required);
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

    int balls = args::get(balls_flag);
    int period = args::get(period_flag);
    if (period == 1) {
        std::cout << balls << '\n';
        return 0;
    }
    int perm[period], base[period];
    for (int i = 0; i < period; ++i) perm[i] = i;
    do {
        int total = 0;
        for (int i = 0; i < period; ++i) {
            base[i] = (perm[i] + period - i) % period;
            total += base[i];
        }
        int extraBalls = balls - total/period;
        // Composition generation
        int composition[period];
        int *comp = composition + 1;
        for (int first = extraBalls;first >= 0; --first) {
            int min = period - 2, compLen = period-1;
            int sum = extraBalls - first;
            for (int j = 0; j < compLen; j++) comp[j] = 0;
            comp[compLen-1] = sum;
            for (int i = compLen-1; i > 0 && comp[i] > first; --i) {
                comp[i-1] = comp[i] - first;
                comp[i] = first;
                --min;
            }
            if (comp[0] > first) continue;
            do {
                composition[0] = first;
                if (checkCompositionBase(composition, base, period)) {
                    for (int i = 0; i < period; ++i) {
                        std::cout << base[i] + composition[i]*period << " ";
                    }
                    std::cout << "\n";
                }
            } while(next_composition(first, comp, compLen, sum, min));
        }
    } while (std::next_permutation(perm, perm+period));
}
