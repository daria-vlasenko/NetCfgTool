#include <QtTest>
#include "validator.h"

class TValidator : public QObject {
    Q_OBJECT
private slots:
    void ipv4_ok()    { QVERIFY( Validator::isValidIpv4("192.168.1.1") ); }
    void ipv4_bad()   { QVERIFY(!Validator::isValidIpv4("999.1.1.1"));   }
    void cidr_ok()    { QVERIFY( Validator::isValidCidr("10.0.0.5/24")); }
    void cidr_bad()   { QVERIFY(!Validator::isValidCidr("10.0.0.5/40")); }
    void dns_ok()     { QVERIFY( Validator::isValidDnsList({"1.1.1.1","8.8.8.8"}) ); }
    void dns_bad()    { QVERIFY(!Validator::isValidDnsList({"1.1.1.999"}) ); }
};
QTEST_APPLESS_MAIN(TValidator)
#include "test_validator.moc"
