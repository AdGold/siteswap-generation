#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <stack>

#include "args.hxx"

void doSiteswap(int *siteswap, int period, int place) {
    int place2 = (place)? place-1 : period-1;
    int c = siteswap[place];
    siteswap[place] = siteswap[place2] - 1;
    siteswap[place2] = c + 1;
}

int getRepeatLength(int *siteswap, int period) {
    for (int i = 1; i <= period/2; ++i) {
        if (period%i == 0) {
            bool good = true;
            for (int j = 0; j < period/i; ++j) {
                for (int k = 0; k < i; ++k) {
                    if (siteswap[k] != siteswap[j*i + k]) {
                        good = false;
                        j = period; // break out of other loop
                        break;
                    }
                }
            }
            if (good) return i;
        }
    }
    return period;
}

// The offset for the maximum rotation where the first two items are not the same
bool maxRotSpecial(int *siteswap, int period, int test) {
    test = test ? test-1 : period-1;
    for (int i = 0; i < period; ++i) {
        if (siteswap[i] != siteswap[(i+1)%period] && i != test) {
            for (int j = 0; j < period; ++j) {
                int a = siteswap[(j+i)%period];
                int b = siteswap[(j+test)%period];
                if (b > a) break;
                else if (a > b) return false;
            }
        }
    }
    return true;
}

std::stack<std::pair<int,bool> > st;

int main(int argc, char const *argv[]) {
    args::ArgumentParser parser("Generate siteswaps using stack notation.", "");
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
    unsigned long long count = 0;
    int balls = args::get(balls_flag);
    int period = args::get(period_flag);
    int siteswap[period];
    for (int i = 0; i < period; i++) siteswap[i] = balls;
    if (period == 1) {
        for (int i = 0; i < period; i++) std::cout<<siteswap[i]<<" ";
        std::cout<<"\n";
        return 0;
    }
    bool first = true;
    while (!st.empty() || first) {
        if (!first) {
            const auto current = st.top();
            st.pop();
            doSiteswap(siteswap, period, current.first);
            if (current.second) continue;
            st.emplace(current.first, true);
        }
        else first = false;
        for (int i = 0; i < period; i++) std::cout << siteswap[i] << " ";
        std::cout << "\n";
        int repLen = getRepeatLength(siteswap, period);
        int max = 0;
        for (int i = 0; i < repLen; i++) {
            if (siteswap[i] > max) max = siteswap[i];
        }
        int prev = period-1;
        for (int i = 0; i < repLen; i++) {
            if (siteswap[i] >= max-1 && siteswap[prev] > 0) {
                doSiteswap(siteswap, period, i);
                if (maxRotSpecial(siteswap, period, i)) st.emplace(i, false);
                doSiteswap(siteswap, period, i);
            }
            prev = i;
        }
    }
}

