#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <unordered_map>
#include <vector>

#include "args.hxx"

#define MAX_PERIOD 25
#define MAX_HEIGHT 300
#define INVALID -1

using StateRep = std::bitset<MAX_HEIGHT>;
struct State {
    StateRep state;
    int length = 0;
};

std::ostream& operator<<(std::ostream& os, const State& s) {
    for (int i = 0; i < s.length; ++i) {
        os << s.state[i];
    }
    return os;
}

// Returns the indices which minimum length composite patterns start at and the minumum length
// Also returns if the siteswap has a composite section starting from a finite state
// Early exits if it's infinite but incl_infinite is false.
std::tuple<std::vector<int>, int, bool> getMinStarts(
        State state, int ss[], int period, bool incl_finite, bool incl_infinite) {
    std::unordered_map<StateRep, std::vector<int>> last_seen;
    last_seen[state.state].push_back(0);
    int min_diff = period;
    bool is_finite = incl_finite;
    std::vector<int> mins;
    for (int i = 0; i < period*2; ++i) {
        state.state >>= 1;
        if (ss[i%period] != 0) {
            state.state.set(ss[i%period]-1);
        }
        const auto it = last_seen.find(state.state);
        if (it != last_seen.end()) {
            for (int start : it->second) {
                if (start >= period) continue;
                int diff = i+1 - start;
                if (std::gcd(diff, period) == 1) {
                    if (!incl_finite) {
                        continue;
                    }
                } else if (incl_finite && diff != period && diff != period*2) {
                    // has a composite subpattern with gcd(subperiod, period) > 1
                    is_finite = false;
                    if (!incl_infinite) {
                        // Early exit because it's an inf pattern and we don't want them
                        return {mins, INVALID, false};
                    }
                }
                if (diff < min_diff) {
                    min_diff = diff;
                    mins.clear();
                    mins.push_back(start);
                } else if (diff == min_diff) {
                    mins.push_back(start);
                }
            }
        }
        last_seen[state.state].push_back(i+1);
    }
    return {mins, min_diff, is_finite};
}

bool higher(int ss[], int a, int b, int period) {
    for (int i = 0; i < period; ++i) {
        int x = ss[(a+i)%period] - ss[(b+i)%period];
        if (x < 0) return false;
        if (x > 0) return true;
    }
    return false;
}

// subperiod should be the length of the repeating pattern at the start which
// was used to generate this ss.
std::pair<bool, bool> isCanonical(const State& state, int ss[], int period, int subperiod, bool incl_finite, bool incl_infinite) {
    const auto [mins, min_diff, is_finite] = getMinStarts(state, ss, period, incl_finite, incl_infinite);
    if (min_diff == INVALID) {
        // We early exited as we knew early on that we didn't want this one.
        return {false, false};
    }
    assert(mins.size() > 0); // Should have no prime patterns
    if (min_diff != subperiod) {
        // There was a smaller repeating part
        return {false, false};
    }
    // If it is *an* option for this length it should be there.
    assert(mins[0] == 0);
    // Do we have the max rot version?
    for (int i = 1; i < mins.size(); ++i) {
        if (higher(ss, mins[i], 0, period)) {
            return {false, false};
        }
    }
    return {true, is_finite};
}

bool stateValidForPeriod(const State& state, int period) {
    for (int i = 0; i < state.length; ++i) {
        if (i + period < state.length && state.state[i+period] && !state.state[i]) {
            return false;
        }
    }
    return true;
}

void forEachPartition(std::vector<int> partition, int at, int n, int k, std::function<void(const std::vector<int>&)> processPartition) {
    if (k == at) {
        if (n == 0) processPartition(partition);
        return;
    }
    for (int i = 0; i <= n; ++i) {
        partition[at] = i;
        forEachPartition(partition, at+1, n-i, k, processPartition);
    }
}

void forEachPerfectMatching(std::vector<int>& land_times, int ss[], int at, int period, std::function<void(int[], int)> processMatching) {
    if (at == -1) {
        processMatching(ss, period);
        return;
    }
    for (int i = land_times.size()-1; i >= 0; --i) {
        int t = land_times[i];
        if (t != -1 && t >= at) {
            int th = t - at;
            land_times[i] = -1;
            ss[at] = th;
            forEachPerfectMatching(land_times, ss, at-1, period, processMatching);
            land_times[i] = t;
        } else if (t >= at) {
            break;
        }
    }
}

void forEachSiteswap(const State& state, int period, std::function<void(int[], int)> processSiteswap) {
    std::vector<int> land_times;
    for (int i = 0; i < period; ++i) {
        // A 0 in the first state before the second starts.
        if (i >= state.length || !state.state[i]) {
            land_times.push_back(i);
        }
    }
    for (int i = 0; i < state.length; ++i) {
        // A 0 in the first state and a 1 in the secon.
        if ((i + period >= state.length || !state.state[i + period]) && state.state[i]) {
            land_times.push_back(i + period);
        }
    }
    assert(land_times.size() == period);
    int ss[MAX_PERIOD];
    forEachPerfectMatching(land_times, ss, period-1, period, processSiteswap);
}

State getStateFromPartition(const std::vector<int>& partition) {
    const auto m = std::max_element(partition.begin(), partition.end());
    assert(m != partition.end());
    State state;
    for (int b = 0; b < *m; ++b) {
        for (int p : partition) {
            state.state[state.length++] = (p > b);
        }
    }
    assert(state.length < MAX_HEIGHT);
    return state;
}

int tmp[MAX_PERIOD];
int state[MAX_HEIGHT];
int tmp_ss[MAX_PERIOD];
unsigned long long c = 0;
unsigned long long count = 0;
unsigned long long finite_count = 0;
int main(int argc, char const *argv[]) {
    args::ArgumentParser parser("Count composite siteswaps.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::CompletionFlag completion(parser, {"complete"});
    args::Positional<int> balls_flag(parser, "balls", "The number of balls to use", args::Options::Required);
    args::Positional<int> period_flag(parser, "period", "The period to use", args::Options::Required);
    args::Flag finite_flag(parser, "finite", "Include composites from finite states", {'f', "finite"});
    args::Flag infinite_flag(parser, "infinite", "Include composites from infinite states", {'i', "infinite"});
    args::Flag verbose_flag(parser, "verbose", "Don't print updates", {'v', "verbose"});
    args::Flag zero_flag(parser, "zero", "Only count patterns with zeros", {'z', "zero"});
    args::Flag print_flag(parser, "print", "Print out all the siteswaps rather than just counting", {'p', "print"});
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
    assert(period < MAX_PERIOD);
    bool incl_finite = args::get(finite_flag);
    bool incl_infinite = args::get(infinite_flag);
    bool verbose = args::get(verbose_flag);
    bool zero = args::get(zero_flag);
    bool print = args::get(print_flag);
    for (int subperiod = 1; subperiod <= period / 2; ++subperiod) {
        int gcd = std::gcd(period, subperiod);
        if ((gcd == 1 && !incl_finite) || (gcd != 1 && !incl_infinite)) continue;
        if (verbose) {
            std::cout << "period, subperiod: " << period << " " << subperiod << "\n";
        }
        std::vector<int> tmp(subperiod, 0);
        forEachPartition(tmp, 0, balls, subperiod, [&](const std::vector<int>& partition) {
            const auto state = getStateFromPartition(partition);
            if (verbose) {
                std::cout << "partition: ";
                for (int p : partition) std::cout << p << " ";
                std::cout << "state: " << state << "\n";
            }
            if (partition[0] == 0 && balls > 0) {
                // Siteswap must start with a 0 so cannot be canonical as there
                // will be a non-zero somewhere, unless it's a zero ball
                // pattern.
                return;
            }
            if (!stateValidForPeriod(state, period-subperiod)) {
                return;
            }
            forEachSiteswap(state, subperiod, [&](int* left, int left_period) {
                bool left_has_0 = false;
                for (int i = 0; i < left_period; ++i) {
                    tmp_ss[i] = left[i];
                    left_has_0 = left_has_0 || (left[i] == 0);
                }
                forEachSiteswap(state, period-subperiod, [&](int *right, int right_period) {
                    bool has_0 = left_has_0;
                    for (int i = 0; i < right_period; ++i) {
                        tmp_ss[i+left_period] = right[i];
                        has_0 = has_0 || (right[i] == 0);
                    }
                    if (zero && !has_0) return;
                    const auto [is_canonical, is_finite] = isCanonical(state, tmp_ss, period, subperiod, incl_finite, incl_infinite);
                    if (is_canonical) {
                        if (is_finite) {
                            ++finite_count;
                        }
                        if (print) {
                            for (int i = 0; i < period; ++i) {
                                std::cout << tmp_ss[i] << " ";
                            }
                            std::cout << "\n";
                        }
                        ++count;
                    }
                    if (verbose && (c++ & 0x3FFFFF) == 0) {
                        std::cout << "Examined " << c << " and found " << count << " composites so far" << std::endl;
                    }
                });
            });
        });
    }
    if (incl_infinite && incl_finite) {
        std::cout << count << "\n";
        std::cout << count - finite_count << "\n";
        std::cout << finite_count << "\n";
    } else if (incl_finite) {
        std::cout << finite_count << "\n";
    } else if (incl_infinite) {
        std::cout << count - finite_count << "\n";
    }
}
