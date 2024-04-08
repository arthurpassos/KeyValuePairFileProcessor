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

    std::optional<char> escaping_character;

    std::optional<uint32_t> max_number_of_pairs;
};

auto parse_arguments(int argc, char * argv[])
{
    Arguments arguments;

    argparse::ArgumentParser program("key_value_pair_extractor");

    program.add_argument("-i", "--input").default_value("").help("Raw ASCII string");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    arguments.input = program.get("input");

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

    if (program_arguments.escaping_character.has_value())
    {
        builder.withQuotingCharacter(program_arguments.escaping_character.value());
    }

    if (program_arguments.max_number_of_pairs.has_value())
    {
        builder.withMaxNumberOfPairs(program_arguments.max_number_of_pairs.value());
    }

    return builder.build();
}

int main(int argc, char * argv[])
{
    auto program_arguments = parse_arguments(argc, argv);

    auto extractor = make_extractor(program_arguments);
    auto map = extractor->extract(program_arguments.input.value());

    for (const auto & [key, value] : map)
    {
        std::cout<<key << ":" << value <<"\n";
    }

    return map.size();
}
