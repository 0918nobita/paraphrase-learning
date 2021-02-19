#define VERSION "0.94.0"

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

const char *kVersion = VERSION;

struct Configuration {
    bool displayOk = true;
    bool displayTime;
    bool evalAndExit;
    unsigned int numCores;
    std::string strToEval;
    bool usePrompt = true;
};

/** CLI が何をすればいいのか */
enum class Command {
    PrintUsage,
    PrintVersion,
    /** 続けてプログラムを実行する */
    Proceed,
    /** コマンドの特定に失敗 */
    Failure,
};

static Command parseOptions(int argc, char* argv[], std::vector<std::string>& args, Configuration& config);

static void printUsage();
static void printVersion();

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    Configuration config;
    auto command = parseOptions(argc, argv, args, config);

    switch (command) {
        case Command::Failure:
            return EXIT_FAILURE;

        case Command::PrintUsage:
            printUsage();
            break;

        case Command::PrintVersion:
            printVersion();
            break;

        case Command::Proceed:
            for (size_t i = 0; i < args.size(); i++) {
                std::cout << args[i] << std::endl;
            }
    }

    return EXIT_SUCCESS;
}

/** コマンドライン引数をパースする */
static Command parseOptions(int argc, char* argv[], std::vector<std::string>& args, Configuration& config) {
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

    po::variables_map vm;

    try {
        auto const parseResult = po::parse_command_line(argc, argv, description);
        po::store(parseResult, vm);
        po::notify(vm);

        for (auto const& str : po::collect_unrecognized(parseResult.options, po::include_positional)) {
            args.push_back(str);
        }
    } catch(const po::error& e) {
        std::cerr << e.what() << std::endl << description << std::endl;
        return Command::Failure;
    }

    if (vm.count("help")) return Command::PrintUsage;
    if (vm.count("version")) return Command::PrintVersion;

    if (vm.count("time")) config.displayTime = true;
    if (vm.count("noprompt")) config.usePrompt = false;
    if (vm.count("nook")) config.displayOk = false;
    if (vm.count("quiet")) config.usePrompt = config.displayOk = false;
    if (vm.count("thread")) {
        auto n = vm["thread"].as<int>();
        if (n < 0) {
            std::cerr << "The value of --thread argument should be a positive integer" << std::endl;
            return Command::Failure;
        }
        config.numCores = n;
    }
    if (vm.count("eval")) config.strToEval = vm["eval"].as<std::string>();
    if (vm.count("eval-and-exit")) {
        config.strToEval = vm["eval-and-exit"].as<std::string>();
        config.evalAndExit = true;
    }

    return Command::Proceed;
}

static void printUsage() {
    std::cout << "Usage: para [-ehknqtv] [--thread] [program-file] [program-code]" << std::endl;
}

static void printVersion() {
    std::cout << "Paraphrase " << kVersion << std::endl;
}
