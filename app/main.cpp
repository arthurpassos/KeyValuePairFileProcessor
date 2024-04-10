#include <iostream>
#include <KeyValuePairExtractorBuilder.h>
#include <argparse/argparse.hpp>

struct Arguments
{
    // if input is too big, this is a problem
    // a stream would be a better approach
    // might be empty in case input_path is set
    std::optional<std::string> input;

    std::optional<std::string> input_path;

    std::optional<std::string> output_path;

    std::optional<char> key_value_delimiter;

    std::optional<std::vector<char>> item_delimiters;

    std::optional<char> quoting_character;

    std::optional<uint32_t> max_number_of_pairs;

    bool escape = false;

    bool verbose = false;
};

auto parse_arguments(int argc, char * argv[])
{
    Arguments arguments;

    argparse::ArgumentParser program("key_value_pair_extractor");

    program.add_argument("-i", "--input").help("Raw ASCII string");
    program.add_argument("-f", "--file").help("Path to file containing raw ASCII string");
    program.add_argument("-o", "--output").help("Path to output file");
    program.add_argument("-kvd", "--key-value-delimiter").help("Key-value delimiter, sits between key and value");
    program.add_argument("-itd", "--item-delimiters").nargs(0, 99).help("Item delimiters, separates pairs from each other. Multiple values are allowed");
    program.add_argument("-q", "--quoting-character").help("Character used for quoting");
    program.add_argument("-e", "--escape").flag().help("Enable escape sequences");
    program.add_argument("-mnp", "--max-number-of-pairs")
    .scan<'u', uint32_t>()
    .help("Maximum number of key-value pairs to extract. Helpful to avoid memory exhaustion in case of a corrupted input file");
    program.add_argument("-v", "--verbose").default_value(false).implicit_value(true).help("Verbose mode");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    if (program.present("input"))
    {
        arguments.input = program.get("input");
    }

    if (program.present("file"))
    {
        arguments.input_path = program.get("file");
    }

    // assign all fields if present in arguments
    if (program.present("output"))
    {
        arguments.output_path = program.get("output");
    }

    if (program.present("key-value-delimiter"))
    {
        arguments.key_value_delimiter = program.get<std::string>("key-value-delimiter")[0];
    }

    if (program.present("item-delimiters"))
    {
        auto string_array = program.get<std::vector<std::string>>("item-delimiters");
        arguments.item_delimiters = std::vector<char>();
        for (const auto & item : string_array)
        {
            arguments.item_delimiters.value().push_back(item[0]);
        }
    }

    if (program.present("quoting-character"))
    {
        arguments.quoting_character = program.get<std::string>("quoting-character")[0];
    }

    if (program.present<uint32_t>("max-number-of-pairs"))
    {
        arguments.max_number_of_pairs = program.get<uint32_t>("max-number-of-pairs");
    }

    arguments.escape = program.get<bool>("escape");

    arguments.verbose = program.get<bool>("verbose");

    return arguments;
}

auto make_extractor(const Arguments & program_arguments)
{
    auto builder = KeyValuePairExtractorBuilder();

    if (program_arguments.key_value_delimiter.has_value())
    {
        builder.withKeyValueDelimiter(program_arguments.key_value_delimiter.value());
    }

    if (program_arguments.item_delimiters.has_value())
    {
        builder.withItemDelimiters(program_arguments.item_delimiters.value());
    }

    if (program_arguments.quoting_character.has_value())
    {
        builder.withQuotingCharacter(program_arguments.quoting_character.value());
    }

    if (program_arguments.max_number_of_pairs.has_value())
    {
        builder.withMaxNumberOfPairs(program_arguments.max_number_of_pairs.value());
    }

    if (program_arguments.escape)
    {
        builder.withEscaping();
    }

    return builder.build();
}

void print_program_arguments(const auto & arguments)
{
    std::cout<<"Program arguments: \n";
    std::cout<<"Input: "<<arguments.input.value_or("not set")<<"\n";
    std::cout<<"Input path: "<<arguments.input_path.value_or("not set")<<"\n";
    std::cout<<"Output path: "<<arguments.output_path.value_or("not set")<<"\n";
}

void print_extractor_configuration(const auto & configuration)
{
    std::cout<<"Extractor configuration:\n";
    std::cout<<"Key-value delimiter: "<<configuration.key_value_delimiter<<"\n";
    std::cout<<"Item delimiters: ";
    for (const auto & item : configuration.pair_delimiters)
    {
        std::cout<<item<<" ";
    }
    std::cout<<"\n";
    std::cout<<"Quoting character: "<<configuration.quoting_character<<"\n";
}

int main(int argc, char * argv[])
{
    auto program_arguments = parse_arguments(argc, argv);

    auto extractor = make_extractor(program_arguments);

    // todo add a proper logger that takes logger level and compares against global verbosity level
    if (program_arguments.verbose)
    {
        print_program_arguments(program_arguments);
        print_extractor_configuration(extractor->getConfiguration());

        std::cout << "--------------------------------\n";

        std::cout << "Starting extraction...\n";

        std::cout << "--------------------------------\n";
    }

    auto map = extractor->extract(program_arguments.input.value());

    for (const auto & [key, value] : map)
    {
        std::cout<<key << ":" << value <<"\n";
    }

    return 1;
}
