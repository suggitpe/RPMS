//#############################
#include <testharness/si_testharness.hpp>
#include <distributor/ss_distributor.hpp>
#include <xmlutil/sc_xmlnodeconfig.hpp>

#include <vector>
TEST_BASE;

void testDistribution();
using rpms::SS_Distributor;
using rpms::SC_XMLNodeConfig;

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    TRY(execute());
        RPMS_INFO( LOGNAME, "START ***************************");
        testDistribution();
        RPMS_INFO( LOGNAME, "END   ***************************");
    CATCH;
}

//////////////////////////////////////////////////
void testDistribution()
{
    TRY(testDistribution());
        std::string xmlFile = SS_Ini::instance()->getString("directories", "config")
            + "/" + SS_Ini::instance()->getString("director", "director.xml.appconfig");
        RPMS_INFO(LOGNAME, "Creating a distribution");
        SS_Distributor::instance()->initialise();

        std::vector<std::string> d0;
        d0.push_back("pgdsx00");
        std::vector<std::string> d1;
        d1.push_back("pgdsx00");
        d1.push_back("pgdsx01");
        std::vector<std::string> d2;
        d2.push_back("pgdsx00");
        d2.push_back("pgdsx01");
        d2.push_back("pgdsx02");
        std::vector<std::string> d3;
        d3.push_back("pgdsx00");
        d3.push_back("pgdsx01");
        d3.push_back("pgdsx02");
        d3.push_back("pgdsx03");
        for( int i = 0; i < 1; ++i ) // allow us to do this many times for memory checks
        {
            // do first distribution
            RPMS_INFO(LOGNAME, "FIRST ***********************************");
            std::map<std::string, std::string> m = SS_Distributor::instance()->distribute( d0, false );
            std::map<std::string, std::string>::iterator iter = m.begin();
            while( iter != m.end() )
            {
                RPMS_INFO(LOGNAME, "Config for [" + iter->first + "]:\n" + iter->second);
                ++iter;
            }
            // now do s redistribution
            RPMS_INFO(LOGNAME, "SECOND ***********************************");
            std::map<std::string, std::string> m2 = SS_Distributor::instance()->distribute( d2, true );
            iter = m2.begin();
            while( iter != m2.end() )
            {
                RPMS_INFO(LOGNAME, "Config for [" + iter->first + "]:\n" + iter->second);
                ++iter;
            }
            
            // now do s redistribution
            RPMS_INFO(LOGNAME, "THIRD ***********************************");
            std::map<std::string, std::string> m1 = SS_Distributor::instance()->distribute( d1, true );
            iter = m1.begin();
            while( iter != m1.end() )
            {
                RPMS_INFO(LOGNAME, "Config for [" + iter->first + "]:\n" + iter->second);
                ++iter;
            }
            
            // now do s redistribution
            RPMS_INFO(LOGNAME, "FORTH ***********************************");
            std::map<std::string, std::string> m3 = SS_Distributor::instance()->distribute( d3, true );
            iter = m3.begin();
            while( iter != m3.end() )
            {
                RPMS_INFO(LOGNAME, "Config for [" + iter->first + "]:\n" + iter->second);
                ++iter;
            }
            
        }
    CATCH;
}

