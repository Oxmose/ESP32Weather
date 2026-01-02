#include <WiFiValidator.h>
#include <unity.h>
#include <iostream>

void TestSwitches(void) {
    S_WiFiConfigRequest config;

    /* Not Set */
    config.isAP.second = false;
    config.isStatic.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSwitches(config), false);
    config.isAP.second = true;
    config.isStatic.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSwitches(config), false);
    config.isAP.second = false;
    config.isStatic.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSwitches(config), false);

    /* Valid */
    config.isAP.second = true;
    config.isStatic.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSwitches(config), true);
}

void TestSSID(void) {
    S_WiFiConfigRequest config;

    /* Not Set */
    config.ssid.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), false);

    /* Too long */
    config.ssid.second = false;
    config.ssid.first = "ThisIsA33LengthStringThatIsLong..";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), false);
    config.ssid.second = true;
    config.ssid.first = "ThisIsA33LengthStringThatIsLong..";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), false);

    /* Too short */
    config.ssid.second = false;
    config.ssid.first = "";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), false);
    config.ssid.second = true;
    config.ssid.first = "";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), false);

    /* Valid */
    config.ssid.second = false;
    config.ssid.first = "ThisIsA32LengthStringThatIsLong.";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), false);
    config.ssid.second = true;
    config.ssid.first = "ThisIsA32LengthStringThatIsLong.";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSSID(config), true);
}

void TestPassword(void) {
    S_WiFiConfigRequest config;

    /* Not Set */
    config.password.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePassword(config), false);

    /* Too long */
    config.password.second = false;
    config.password.first = "ThisIsA33LengthStringThatIsLong..";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePassword(config), false);
    config.password.second = true;
    config.password.first = "ThisIsA33LengthStringThatIsLong..";

    /* Too Short */
    config.password.second = false;
    config.password.first = "ThisIsA";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePassword(config), false);
    config.password.second = true;
    config.password.first = "ThisIsA";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePassword(config), false);

    /* Valid */
    config.password.second = false;
    config.password.first = "ThisIsA32LengthStringThatIsLong.";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePassword(config), false);
    config.password.second = true;
    config.password.first = "ThisIsA32LengthStringThatIsLong.";
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePassword(config), true);
}

void TestIP(void) {
    S_WiFiConfigRequest config;


    /* No Static */
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.ip.first = "";
    config.ip.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.ip.first = "";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.ip.first = "1.1.1.1";
    config.ip.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.ip.first = "1.1.1.1.";
    config.ip.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.ip.first = "1.1.1.1";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.ip.first = "1.1.1.1.";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);


    config.isStatic.first = true;
    config.isStatic.second = true;

    /* Static, too much dots */
    config.ip.first = "1.1.1.1.";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, not enough dots */
    config.ip.first = "1.1.11";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, too much dots + invalid range */
    config.ip.first = "1000.1.1.1.";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, too much dots + invalid range */
    config.ip.first = "1.1000.1.1.";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, too much dots + invalid range */
    config.ip.first = "1.1.1000.1.";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, too much dots + invalid range */
    config.ip.first = "1.1.1.1000.";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, not enough dots + invalid range */
    config.ip.first = "1.1.1100";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, not enough dots + invalid range */
    config.ip.first = "1.11100";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, not enough dots + invalid range */
    config.ip.first = "111100";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid range */
    config.ip.first = "256.1.1.1";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid range */
    config.ip.first = "1.256.1.1";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid range */
    config.ip.first = "1.1.256.1";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid range */
    config.ip.first = "1.1.1.256";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid character */
    config.ip.first = "a.1.1.256";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid character */
    config.ip.first = "1.:.1.256";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid character */
    config.ip.first = "1.1.z.256";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, invalid character */
    config.ip.first = "1.1.1. ";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, valid */
    config.ip.first = "1.1.1.1";
    config.ip.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), false);

    /* Static, valid */
    config.ip.first = "255.255.255.255";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);

    /* Static, valid */
    config.ip.first = "255.0.0.0";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);

    /* Static, valid */
    config.ip.first = "0.0.0.0";
    config.ip.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateIP(config), true);
}

void TestGateway(void) {
    S_WiFiConfigRequest config;


    /* No Static */
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.gateway.first = "";
    config.gateway.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.gateway.first = "";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.gateway.first = "1.1.1.1";
    config.gateway.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.gateway.first = "1.1.1.1.";
    config.gateway.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.gateway.first = "1.1.1.1";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.gateway.first = "1.1.1.1.";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);


    config.isStatic.first = true;
    config.isStatic.second = true;

    /* Static, too much dots */
    config.gateway.first = "1.1.1.1.";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, not enough dots */
    config.gateway.first = "1.1.11";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, too much dots + invalid range */
    config.gateway.first = "1000.1.1.1.";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, too much dots + invalid range */
    config.gateway.first = "1.1000.1.1.";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, too much dots + invalid range */
    config.gateway.first = "1.1.1000.1.";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, too much dots + invalid range */
    config.gateway.first = "1.1.1.1000.";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, not enough dots + invalid range */
    config.gateway.first = "1.1.1100";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, not enough dots + invalid range */
    config.gateway.first = "1.11100";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, not enough dots + invalid range */
    config.gateway.first = "111100";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid range */
    config.gateway.first = "256.1.1.1";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid range */
    config.gateway.first = "1.256.1.1";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid range */
    config.gateway.first = "1.1.256.1";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid range */
    config.gateway.first = "1.1.1.256";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid character */
    config.gateway.first = "a.1.1.256";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid character */
    config.gateway.first = "1.:.1.256";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid character */
    config.gateway.first = "1.1.z.256";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, invalid character */
    config.gateway.first = "1.1.1. ";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, valid */
    config.gateway.first = "1.1.1.1";
    config.gateway.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), false);

    /* Static, valid */
    config.gateway.first = "255.255.255.255";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);

    /* Static, valid */
    config.gateway.first = "255.0.0.0";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);

    /* Static, valid */
    config.gateway.first = "0.0.0.0";
    config.gateway.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateGateway(config), true);
}

void TestSubnet(void) {
    S_WiFiConfigRequest config;

    /* No Static */
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.subnet.first = "";
    config.subnet.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.subnet.first = "";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.subnet.first = "1.1.1.1";
    config.subnet.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.subnet.first = "1.1.1.1.";
    config.subnet.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.subnet.first = "1.1.1.1";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.subnet.first = "1.1.1.1.";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);


    config.isStatic.first = true;
    config.isStatic.second = true;

    /* Static, too much dots */
    config.subnet.first = "1.1.1.1.";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, not enough dots */
    config.subnet.first = "1.1.11";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, too much dots + invalid range */
    config.subnet.first = "1000.1.1.1.";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, too much dots + invalid range */
    config.subnet.first = "1.1000.1.1.";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, too much dots + invalid range */
    config.subnet.first = "1.1.1000.1.";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, too much dots + invalid range */
    config.subnet.first = "1.1.1.1000.";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, not enough dots + invalid range */
    config.subnet.first = "1.1.1100";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, not enough dots + invalid range */
    config.subnet.first = "1.11100";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, not enough dots + invalid range */
    config.subnet.first = "111100";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid range */
    config.subnet.first = "256.1.1.1";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid range */
    config.subnet.first = "1.256.1.1";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid range */
    config.subnet.first = "1.1.256.1";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid range */
    config.subnet.first = "1.1.1.256";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid character */
    config.subnet.first = "a.1.1.256";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid character */
    config.subnet.first = "1.:.1.256";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid character */
    config.subnet.first = "1.1.z.256";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, invalid character */
    config.subnet.first = "1.1.1. ";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, valid */
    config.subnet.first = "1.1.1.1";
    config.subnet.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), false);

    /* Static, valid */
    config.subnet.first = "255.255.255.255";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);

    /* Static, valid */
    config.subnet.first = "255.0.0.0";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);

    /* Static, valid */
    config.subnet.first = "0.0.0.0";
    config.subnet.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateSubnet(config), true);
}

void TestDNS(void) {
    S_WiFiConfigRequest config;

    config.secondaryDNS.first = "1.1.1.1";
    config.secondaryDNS.second = true;

    /* No Static */
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.primaryDNS.first = "";
    config.primaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.primaryDNS.first = "";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.primaryDNS.first = "1.1.1.1";
    config.primaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.primaryDNS.first = "1.1.1.1.";
    config.primaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.primaryDNS.first = "1.1.1.1";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.primaryDNS.first = "1.1.1.1.";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);


    config.isStatic.first = true;
    config.isStatic.second = true;

    /* Static, too much dots */
    config.primaryDNS.first = "1.1.1.1.";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots */
    config.primaryDNS.first = "1.1.11";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.primaryDNS.first = "1000.1.1.1.";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.primaryDNS.first = "1.1000.1.1.";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.primaryDNS.first = "1.1.1000.1.";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.primaryDNS.first = "1.1.1.1000.";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots + invalid range */
    config.primaryDNS.first = "1.1.1100";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots + invalid range */
    config.primaryDNS.first = "1.11100";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots + invalid range */
    config.primaryDNS.first = "111100";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.primaryDNS.first = "256.1.1.1";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.primaryDNS.first = "1.256.1.1";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.primaryDNS.first = "1.1.256.1";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.primaryDNS.first = "1.1.1.256";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.primaryDNS.first = "a.1.1.256";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.primaryDNS.first = "1.:.1.256";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.primaryDNS.first = "1.1.z.256";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.primaryDNS.first = "1.1.1. ";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, valid */
    config.primaryDNS.first = "1.1.1.1";
    config.primaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, valid */
    config.primaryDNS.first = "255.255.255.255";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);

    /* Static, valid */
    config.primaryDNS.first = "255.0.0.0";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);

    /* Static, valid */
    config.primaryDNS.first = "0.0.0.0";
    config.primaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);

    config.primaryDNS.first = "1.1.1.1";
    config.primaryDNS.second = true;

    /* No Static */
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.secondaryDNS.first = "";
    config.secondaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.secondaryDNS.first = "";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.secondaryDNS.first = "1.1.1.1";
    config.secondaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.secondaryDNS.first = "1.1.1.1.";
    config.secondaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.secondaryDNS.first = "1.1.1.1";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
    config.isStatic.first = false;
    config.isStatic.second = true;
    config.secondaryDNS.first = "1.1.1.1.";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);


    config.isStatic.first = true;
    config.isStatic.second = true;

    /* Static, too much dots */
    config.secondaryDNS.first = "1.1.1.1.";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots */
    config.secondaryDNS.first = "1.1.11";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.secondaryDNS.first = "1000.1.1.1.";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.secondaryDNS.first = "1.1000.1.1.";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.secondaryDNS.first = "1.1.1000.1.";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, too much dots + invalid range */
    config.secondaryDNS.first = "1.1.1.1000.";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots + invalid range */
    config.secondaryDNS.first = "1.1.1100";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots + invalid range */
    config.secondaryDNS.first = "1.11100";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, not enough dots + invalid range */
    config.secondaryDNS.first = "111100";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.secondaryDNS.first = "256.1.1.1";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.secondaryDNS.first = "1.256.1.1";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.secondaryDNS.first = "1.1.256.1";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid range */
    config.secondaryDNS.first = "1.1.1.256";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.secondaryDNS.first = "a.1.1.256";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.secondaryDNS.first = "1.:.1.256";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.secondaryDNS.first = "1.1.z.256";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, invalid character */
    config.secondaryDNS.first = "1.1.1. ";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, valid */
    config.secondaryDNS.first = "1.1.1.1";
    config.secondaryDNS.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), false);

    /* Static, valid */
    config.secondaryDNS.first = "255.255.255.255";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);

    /* Static, valid */
    config.secondaryDNS.first = "255.0.0.0";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);

    /* Static, valid */
    config.secondaryDNS.first = "0.0.0.0";
    config.secondaryDNS.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidateDNS(config), true);
}

void TestPort(void) {
    S_WiFiConfigRequest config;

    config.webPort.first = 50;
    config.apiPort.first = 51;
    config.webPort.second = false;
    config.apiPort.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePorts(config), false);

    config.webPort.first = 50;
    config.apiPort.first = 51;
    config.webPort.second = true;
    config.apiPort.second = false;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePorts(config), false);

    config.webPort.first = 50;
    config.apiPort.first = 51;
    config.webPort.second = false;
    config.apiPort.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePorts(config), false);

    config.webPort.first = 50;
    config.apiPort.first = 50;
    config.webPort.second = true;
    config.apiPort.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePorts(config), false);

    config.webPort.first = 50;
    config.apiPort.first = 51;
    config.webPort.second = true;
    config.apiPort.second = true;
    TEST_ASSERT_EQUAL(WiFiValidator::ValidatePorts(config), true);
}

void ValidatorTest(void) {
    RUN_TEST(TestSwitches);
    RUN_TEST(TestSSID);
    RUN_TEST(TestPassword);
    RUN_TEST(TestIP);
    RUN_TEST(TestGateway);
    RUN_TEST(TestSubnet);
    RUN_TEST(TestDNS);
    RUN_TEST(TestPort);
}