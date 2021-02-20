#define VERSION "0.94.0"

#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

const char* kVersion = VERSION;

struct Configuration {
    bool displayOk = true;
    bool displayTime;
    bool evalAndExit;
    unsigned int numCores;
    std::string strToEval;
    bool usePrompt = true;
};

enum class Command {
    Proceed,
    PrintUsage,
    PrintVersion,
};

struct ParseResult {
    std::vector<std::string> args;
    Command cmd;
    Configuration config;
};

static void parseOptions(int argc, char* argv[], ParseResult& result) noexcept;

static void printUsage() noexcept;
static void printVersion() noexcept;

int main(int argc, char* argv[]) {
#if DEBUG || _DEBUG
    std::cout << "!!! THIS IS DEBUG VERSION !!!" << std::endl;
#endif
    ParseResult result;
    parseOptions(argc, argv, result);

    switch (result.cmd) {
        case Command::PrintUsage:
            printUsage();
            break;

        case Command::PrintVersion:
            printVersion();
            break;

        case Command::Proceed:
            for (size_t i = 0; i < result.args.size(); i++) {
                std::cout << result.args[i] << std::endl;
            }
    }

    return EXIT_SUCCESS;
}

/** コマンドライン引数をパースする */
static void parseOptions(int argc, char* argv[], ParseResult& result) noexcept {
    po::options_description description("options");
    // clang-format off
    description.add_options()
        ("help,h",    "print help.")
        ("version,v", "print version info.")
        ("quiet,q",   "quiet mode (equivalent to -nk).")
        ("thread",          po::value<int>(), "set maximum thread (ex: --thread 8).")
        ("eval,e",          po::value<std::string>(), "eval parameter.")
        ("eval-and-exit,E", po::value<std::string>(), "eval and exit (for one liner).")
        ("noprompt,n", "suppress prompt.")
        ("nook,k", "suppress 'ok'.")
        ("time,k", "display spent time.");
    // clang-format on

    po::variables_map vm;

    try {
        auto const parseResult =
            po::parse_command_line(argc, argv, description);
        po::store(parseResult, vm);
        po::notify(vm);
        result.args = po::collect_unrecognized(parseResult.options,
                                               po::include_positional);
    } catch (const po::error& e) {
        std::cerr << e.what() << std::endl << description << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (vm.count("help")) {
        result.cmd = Command::PrintUsage;
        return;
    }

    if (vm.count("version")) {
        result.cmd = Command::PrintVersion;
        return;
    }

    if (vm.count("time")) result.config.displayTime = true;
    if (vm.count("noprompt")) result.config.usePrompt = false;
    if (vm.count("nook")) result.config.displayOk = false;
    if (vm.count("quiet"))
        result.config.usePrompt = result.config.displayOk = false;
    if (vm.count("thread")) {
        auto n = vm["thread"].as<int>();
        if (n < 0) {
            std::cerr
                << "The value of --thread argument should be a positive integer"
                << std::endl;
            std::exit(EXIT_FAILURE);
        }
        result.config.numCores = n;
    }
    if (vm.count("eval"))
        result.config.strToEval = vm["eval"].as<std::string>();
    if (vm.count("eval-and-exit")) {
        result.config.strToEval = vm["eval-and-exit"].as<std::string>();
        result.config.evalAndExit = true;
    }
}

static void printUsage() noexcept {
    std::cout
        << "Usage: para [-ehknqtv] [--thread] [program-file] [program-code]"
        << std::endl;
}

static void printVersion() noexcept {
    std::cout << "Paraphrase " << kVersion << std::endl;
}
