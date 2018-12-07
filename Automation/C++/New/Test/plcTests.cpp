#include <limits.h>
#include "gtest/gtest.h"

#include "plcBase.h"
#include "plcMQTT.h"
#include <iostream>

#include <mosquitto.h>


plcMQTT *tstPlc= new plcMQTT();

class plcTest : public ::testing::Test {
    public:


    protected:
        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST(plcTest, setAddress) {
    EXPECT_EQ(false, tstPlc->setHost("192.168.0.65"));
}


TEST(plcTest, setPort) {
    ASSERT_EQ(false, tstPlc->setPort(1883));
}

TEST(plcTest, Connect) {
    tstPlc->plcDump();
    ASSERT_EQ(false, tstPlc->initPlc());
}

TEST(plcTest, addIOPoint) {
    ASSERT_EQ(false, tstPlc->addIOPoint("TEST"));
}

TEST(plcTest, setValue) {
    ASSERT_EQ( false, tstPlc->setValue("TEST", "ON"));
}

TEST(plcTest, getValue) {
    ASSERT_EQ( "ON", tstPlc->getValue("TEST"));
}

TEST(plcTest,Load ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->Ld("TEST");
    ASSERT_EQ( false, tstPlc->getTOS());
    ASSERT_EQ(1,tstPlc->stackSize());
    tstPlc->fromStack();
}

TEST(plcTest,LoadInvert ) {
    tstPlc->Ldn("TEST");
    ASSERT_EQ( true, tstPlc->getTOS());
    ASSERT_EQ(1, tstPlc->stackSize());
    tstPlc->fromStack();
}

TEST(plcTest,Or ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Or("TEST");

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->Or("TEST");

    ASSERT_EQ(1,tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(false);

    tstPlc->Or("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(true);

    tstPlc->Or("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
}

TEST(plcTest,Orn ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Orn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->Orn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(false);

    tstPlc->Orn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(true);

    tstPlc->Orn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
}

TEST(plcTest,And ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->And("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->And("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(false);

    tstPlc->And("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(true);

    tstPlc->And("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
}

TEST(plcTest,Andn ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Andn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->Andn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( true, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(false);

    tstPlc->Andn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
    /* ===== */

    tstPlc->setValue("TEST", "ON");
    tstPlc->toStack(true);

    tstPlc->Andn("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());

    ASSERT_EQ( false, tstPlc->fromStack());
}

TEST(plcTest,Out ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Out("TEST");
    ASSERT_EQ( false, tstPlc->getBoolValue("TEST"));

    tstPlc->toStack(true);

    tstPlc->Out("TEST");
    ASSERT_EQ( true, tstPlc->getBoolValue("TEST"));
    ASSERT_EQ(0, tstPlc->stackSize());
}

TEST(plcTest,Outn ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Outn("TEST");
    ASSERT_EQ( true, tstPlc->getBoolValue("TEST"));

    tstPlc->toStack(true);

    tstPlc->Outn("TEST");
    ASSERT_EQ( false, tstPlc->getBoolValue("TEST"));
    ASSERT_EQ(0, tstPlc->stackSize());
}






