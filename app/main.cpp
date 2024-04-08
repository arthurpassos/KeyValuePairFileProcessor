#include <iostream>
#include <KeyValuePairExtractorBuilder.h>
#include <argparse/argparse.hpp>

int main(int argc, char * argv[])
{
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

    auto input_string = program.get("input");

    auto extractor = KeyValuePairExtractorBuilder().withItemDelimiters({',', ' '}).build();
    auto map = extractor->extract(input_string);

    for (const auto & [key, value] : map)
    {
        std::cout<<key << ":" << value <<"\n";
    }

    return map.size();
}
