#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "FSMKeyValuePairFileProcessor.h"

TEST(FSMKeyValuePairFileProcessorTests, MixString) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output[R"(no:me)"] = "neymar";
    expected_output["age"] = "30";
    expected_output["height"] = "175";
    expected_output["escola"] = "lupe picasso";
    expected_output["team"] = "psg";

    auto result = processor.process(R"(9 ads =nm,  no\:me: neymar, age: 30, daojmskdpoa and a  height:   175, escola: lupe picasso, team: psg,)");

    EXPECT_EQ(result, expected_output);
}


TEST(FSMKeyValuePairFileProcessorTests, ValuesCanBeEmptyStrings) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output["age"] = "";

    auto result = processor.process("age:");

    EXPECT_EQ(result, expected_output);
}

TEST(FSMKeyValuePairFileProcessorTests, DoNotAllowUnescapedSpecialCharactersOnKey) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output["age"] = "30";

    auto result = processor.process("na,me,: neymar, age:30");

    EXPECT_EQ(result, expected_output);
}

TEST(FSMKeyValuePairFileProcessorTests, QuotedBackSlashKV) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', '\"');

    std::map<std::string, std::string> expected_output;

    expected_output["name"] = "neymar";
    expected_output["favorite_quote"] = "Premature optimization is the root of all evil";
    expected_output["age"] = "30";

    auto result = processor.process(R"(name: neymar, "favorite_quote": "Premature optimization is the root of all evil", age:30)");

    EXPECT_EQ(result, expected_output);
}

TEST(FSMKeyValuePairFileProcessorTests, MixString2) {
    std::string long_string = R"(2582511992885811767 Qfic=QELUWB, BciBbb=9, IeyClzKmrs {Pvkrq=86093, Glhbmwg=5, FtuzyjOsf=66, YssMbiUswk=0, AxuDcgzkpx=1, KacsIbu=277, MgmjkhKh=9q60pqp43s246u2745, KucFjl=61350852, SdfbJshj=5462619308589345326}, UzfFmg {IsaRubn=3, PqgPmah=954, JagRddm=31692}, KnxKigysBjo {Mnfmybl=7941, Vnvpht=xghk, BgqvTj=9843, UlnrrCr=72940085719217638, YawhzZkqv=260753, QyxbSqamh=627.4022, Ulejhjgk=-0, DjyecbleCqazeZdyc=2229740235971220001, YocxsggkXvxwWzgd=2320663994026035210, NhntkbcQzghj=6}, Rdodi {OSERVWEI_MIVZAPPDLCR_JT="068G14THM", KFQMUR_HXEVFA="XZSQ", DVCRIJ_ZOUMQ_DD="P0889542750958729205", XNFHGSSF_RHRUZHVBS_KWBT="F", DIOQZO_PVNNTGISHEH_VB="bmhncvchhrepqnxb", BHMYYKME_VCPWN_QR=754501869725462264, SYZIWCHJK=2z1791877467804982}, Zfv {AyvbOrexdlUdzj="GESF", FescvXwmfFlgijw="RPUGI|RULHKBESCT", RvnzbdkCwjt="3087"}, UetXuhy {R_YU_FVBD_TCGUTEDZU_BBVAK=0422461531412992163, G_OV_WBLZ_WMMJKMHQZ=5216958198816473961, B_REU_XUQXBYR_LVOIMULK_MMSN=IcmniychTgxn{unsaurbrLnaqQc=1, fbqiUtcjYul=20911}}, AuorsGvyImnh {P_EWJN="JFVL"})";

    std::map<std::string, std::string> expected_output;

    FSMKeyValuePairFileProcessor processor(',', '=', '\\', '\"');

    auto result = processor.process(long_string);

    for (const auto & pair : result)
        std::cout<<pair.first<<": "<<pair.second<<", ";

//    EXPECT_EQ(result, expected_output);
}