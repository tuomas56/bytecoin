#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BYTC");
    BOOST_CHECK(GetBoolArg("-BYTC"));
    BOOST_CHECK(GetBoolArg("-BYTC", false));
    BOOST_CHECK(GetBoolArg("-BYTC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BYTCo"));
    BOOST_CHECK(!GetBoolArg("-BYTCo", false));
    BOOST_CHECK(GetBoolArg("-BYTCo", true));

    ResetArgs("-BYTC=0");
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));

    ResetArgs("-BYTC=1");
    BOOST_CHECK(GetBoolArg("-BYTC"));
    BOOST_CHECK(GetBoolArg("-BYTC", false));
    BOOST_CHECK(GetBoolArg("-BYTC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBYTC");
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));

    ResetArgs("-noBYTC=1");
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));

    ResetArgs("-BYTC -noBYTC");  // -BYTC should win
    BOOST_CHECK(GetBoolArg("-BYTC"));
    BOOST_CHECK(GetBoolArg("-BYTC", false));
    BOOST_CHECK(GetBoolArg("-BYTC", true));

    ResetArgs("-BYTC=1 -noBYTC=1");  // -BYTC should win
    BOOST_CHECK(GetBoolArg("-BYTC"));
    BOOST_CHECK(GetBoolArg("-BYTC", false));
    BOOST_CHECK(GetBoolArg("-BYTC", true));

    ResetArgs("-BYTC=0 -noBYTC=0");  // -BYTC should win
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BYTC=1");
    BOOST_CHECK(GetBoolArg("-BYTC"));
    BOOST_CHECK(GetBoolArg("-BYTC", false));
    BOOST_CHECK(GetBoolArg("-BYTC", true));

    ResetArgs("--noBYTC=1");
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", "eleven"), "eleven");

    ResetArgs("-BYTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", "eleven"), "");

    ResetArgs("-BYTC=");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", "eleven"), "");

    ResetArgs("-BYTC=11");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", "eleven"), "11");

    ResetArgs("-BYTC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BYTC", 0), 0);

    ResetArgs("-BYTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BYTC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BYTC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BYTC");
    BOOST_CHECK_EQUAL(GetBoolArg("-BYTC"), true);

    ResetArgs("--BYTC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BYTC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBYTC");
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));

    ResetArgs("-noBYTC=1");
    BOOST_CHECK(!GetBoolArg("-BYTC"));
    BOOST_CHECK(!GetBoolArg("-BYTC", true));
    BOOST_CHECK(!GetBoolArg("-BYTC", false));

    ResetArgs("-noBYTC=0");
    BOOST_CHECK(GetBoolArg("-BYTC"));
    BOOST_CHECK(GetBoolArg("-BYTC", true));
    BOOST_CHECK(GetBoolArg("-BYTC", false));

    ResetArgs("-BYTC --noBYTC");
    BOOST_CHECK(GetBoolArg("-BYTC"));

    ResetArgs("-noBYTC -BYTC"); // BYTC always wins:
    BOOST_CHECK(GetBoolArg("-BYTC"));
}

BOOST_AUTO_TEST_SUITE_END()
