#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

static bool parseOptions(int argc, char** argv);

int main(int argc, char** argv) {
    if (parseOptions(argc, argv) == false) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/** コマンドライン引数をパースする */
static bool parseOptions(int argc, char** argv) {
    po::options_description description("options");
    description.add_options()
        ("help,h",    "print help.")
        ("version,v", "print version info.")
        ("quiet,q",   "quiet mode (equivalent to -nk).")
        ("thread",          po::value<int>(),         "set maximum thread (ex: --thread 8).")
        ("eval,e",          po::value<std::string>(), "eval parameter.")
        ("eval-and-exit,E", po::value<std::string>(), "eval and exit (for one liner).")
        ("noprompt,n", "suppress prompt.")
        ("nook,k",     "suppress 'ok'.")
        ("time,k",     "display spent time.");
    return true;
}
