#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "KeyValuePairExtractorTest.h"
#include "LazyEscapingKeyValuePairExtractor.h"

struct LazyEscapingKeyValuePairExtractorTestCase : KeyValuePairExtractorTestCase {

};

struct LazyEscapingKeyValuePairExtractorTest : public ::testing::TestWithParam<LazyEscapingKeyValuePairExtractorTestCase> {

};;

TEST_P(LazyEscapingKeyValuePairExtractorTest, LazyEscapingKeyValuePairExtractor) {
    const auto & [input, expected_output, item_delimiter, key_value_delimiter, escape_character, enclosing_character] = GetParam();

    LazyEscapingKeyValuePairExtractor processor(item_delimiter, key_value_delimiter, escape_character, enclosing_character);

    auto result = processor.extract(input);

    EXPECT_EQ(result, expected_output);
}

INSTANTIATE_TEST_SUITE_P(
        ValuesCanBeEmptyString,
        LazyEscapingKeyValuePairExtractorTest,
        ::testing::ValuesIn(std::initializer_list<LazyEscapingKeyValuePairExtractorTestCase> {
            {
                    "age:",
                    {
                            {"age", ""}
                    },
            },
            {
                    "name: neymar, favorite_movie:,favorite_song:",
                    {
                            {"name", "neymar"},
                            {"favorite_movie", ""},
                            {"favorite_song", ""},
                    }
            }
        })
);

INSTANTIATE_TEST_SUITE_P(
        MixString,
        LazyEscapingKeyValuePairExtractorTest,
        ::testing::ValuesIn(std::initializer_list<LazyEscapingKeyValuePairExtractorTestCase> {
            {
                R"(9 ads =nm,  no\:me: neymar, age: 30, daojmskdpoa and a  height:   175, school: lupe\ picasso, team: psg,)",
                {
                    {R"(no:me)", "neymar"},
                    {"age", "30"},
                    {"height", "175"},
                    {"school", "lupe picasso"},
                    {"team", "psg"}
                }
            },
            {
                "XNFHGSSF_RHRUZHVBS_KWBT: F,",
                {
                    {"XNFHGSSF_RHRUZHVBS_KWBT", "F"}
                }
            },
        }
        )
);

INSTANTIATE_TEST_SUITE_P(
        Escaping,
        LazyEscapingKeyValuePairExtractorTest,
        ::testing::ValuesIn(std::initializer_list<LazyEscapingKeyValuePairExtractorTestCase> {
            {
                    "na,me,: neymar, age:30",
                    {
                            {"age", "30"}
                    }
            },
            {
                    "na$me,: neymar, age:30",
                    {
                            {"age", "30"}
                    }
            },
            {
                    R"(name: neymar, favorite_quote: Premature\ optimization\ is\ the\ r\$\$t\ of\ all\ evil, age:30)",
                    {
                            {"name", "neymar"},
                            {"favorite_quote", R"(Premature optimization is the r$$t of all evil)"},
                            {"age", "30"}
                    },
                    ',',
                    ':',
                    '\\',
                    '"'
            }
        })
);

TEST(LazyEscapingKeyValuePairExtractorTests, MixString2) {
    std::string long_string = R"(2582511992885811767 Qfic=QELUWB, BciBbb=9, IeyClzKmrs {Pvkrq=86093, Glhbmwg=5, FtuzyjOsf=66, YssMbiUswk=0, AxuDcgzkpx=1, KacsIbu=277, MgmjkhKh=9q60pqp43s246u2745, KucFjl=61350852, SdfbJshj=5462619308589345326}, UzfFmg {IsaRubn=3, PqgPmah=954, JagRddm=31692}, KnxKigysBjo {Mnfmybl=7941, Vnvpht=xghk, BgqvTj=9843, UlnrrCr=72940085719217638, YawhzZkqv=260753, QyxbSqamh=627.4022, Ulejhjgk=-0, DjyecbleCqazeZdyc=2229740235971220001, YocxsggkXvxwWzgd=2320663994026035210, NhntkbcQzghj=6}, Rdodi {OSERVWEI_MIVZAPPDLCR_JT="068G14THM", KFQMUR_HXEVFA="XZSQ", DVCRIJ_ZOUMQ_DD="P0889542750958729205", XNFHGSSF_RHRUZHVBS_KWBT="F", DIOQZO_PVNNTGISHEH_VB="bmhncvchhrepqnxb", BHMYYKME_VCPWN_QR=754501869725462264, SYZIWCHJK=2z1791877467804982}, Zfv {AyvbOrexdlUdzj="GESF", FescvXwmfFlgijw="RPUGI|RULHKBESCT", RvnzbdkCwjt="3087"}, UetXuhy {R_YU_FVBD_TCGUTEDZU_BBVAK=0422461531412992163, G_OV_WBLZ_WMMJKMHQZ=5216958198816473961, B_REU_XUQXBYR_LVOIMULK_MMSN=IcmniychTgxn{unsaurbrLnaqQc=1, fbqiUtcjYul=20911}}, AuorsGvyImnh {P_EWJN="JFVL"})";

    using json = nlohmann::json;

    std::ifstream f("/home/arthur/CLionProjects/key_value_pair_processing/tests/kvp_expected_output.json");

    std::string bla;
//    while (std::getline(f, bla)) {
//        std::cout<<"123: "<<bla<<"\n";
//    }

    json data = json::parse(f);

    auto expected_output = data.get<std::unordered_map<std::string, std::string>>();

    LazyEscapingKeyValuePairExtractor processor(',', '=', '\\', '\"');

    auto result = processor.extract(long_string);

//    for (const auto & pair : result)
//        std::cout<<pair.first<<": "<<pair.second<<", ";

    EXPECT_EQ(result, expected_output);
}