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

TEST(plcTest, getOldValue) {
    ASSERT_EQ( "OFF", tstPlc->getOldValue("TEST"));
}

TEST(plcTest,Load ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->Ld("TEST");
    ASSERT_EQ( false, tstPlc->getTOS());
    ASSERT_EQ(1,tstPlc->stackSize());
    tstPlc->fromStack();
}

TEST(plcTest,LoadRisingEdge ) {
    //
    // value = OFF old = OFF
    //
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Ldr("TEST");
    ASSERT_EQ( false, tstPlc->getTOS());
    tstPlc->fromStack();
    //
    // value = ON old = OFF
    //
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->Ldr("TEST");

    ASSERT_EQ( true, tstPlc->getTOS());
    tstPlc->fromStack();
    //
    // value = OFF old = ON
    //
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "ON");
    tstPlc->Ldr("TEST");

    ASSERT_EQ( false, tstPlc->getTOS());
    tstPlc->fromStack();
    //
    // value = ON old = OFF
    //
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");
    tstPlc->Ldr("TEST");

    ASSERT_EQ( false, tstPlc->getTOS());
    tstPlc->fromStack();

    tstPlc->setValue("TEST", "OFF");
    tstPlc->plcEnd(0);
}

TEST(plcTest,LoadFallingEdge ) {
    //
    // value = OFF old = OFF
    //
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Ldf("TEST");
    
    ASSERT_EQ( false, tstPlc->getTOS());
    tstPlc->fromStack();
    //
    // value = ON old = OFF
    //
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->Ldf("TEST");

    ASSERT_EQ( false, tstPlc->getTOS());
    tstPlc->fromStack();
    //
    // value = OFF old = ON
    //
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "ON");
    tstPlc->Ldf("TEST");

    ASSERT_EQ( true, tstPlc->getTOS());
    tstPlc->fromStack();
    //
    // value = ON old = OFF
    //
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");
    tstPlc->Ldf("TEST");

    ASSERT_EQ( false, tstPlc->getTOS());
    tstPlc->fromStack();

    tstPlc->setValue("TEST", "OFF");
    tstPlc->plcEnd(0);
}

TEST(plcTest,LoadInvert ) {
    tstPlc->setValue("TEST", "OFF");
    tstPlc->Ldn("TEST");
    ASSERT_EQ( true, tstPlc->getTOS());
    ASSERT_EQ(1, tstPlc->stackSize());
    
    tstPlc->fromStack();
}

TEST(plcTest,AddMinutes ) {
    string adj = tstPlc->addMinutes("11:00", 51);
    ASSERT_EQ("11:51", adj);

    adj = tstPlc->addMinutes("11:00", -9);
    ASSERT_EQ("10:51", adj);
}

TEST(plcTest,TimeBetween ) {
    string testTime = tstPlc->getTime();

    string startTime = tstPlc->addMinutes(testTime, -10);
    string endTime = tstPlc->addMinutes(testTime, 10);

    ASSERT_EQ(true, tstPlc->timeBetween( startTime, endTime));

    startTime = tstPlc->addMinutes(testTime, 5);
    ASSERT_EQ(false, tstPlc->timeBetween( startTime, endTime));
}

TEST(plcTest,TimLoad ) {
    string now = tstPlc->getTime();

    tstPlc->TimLd(now);
    ASSERT_EQ( true, tstPlc->getTOS());
    ASSERT_EQ(1,tstPlc->stackSize());
    tstPlc->fromStack();

    tstPlc->TimLd("00:00");
    ASSERT_EQ( false, tstPlc->getTOS());
    ASSERT_EQ(1,tstPlc->stackSize());
    tstPlc->emptyStack();

}

TEST(plcTest,TimLoadNot ) {
    string now = tstPlc->getTime();

    tstPlc->TimLdn(now);
    ASSERT_EQ( false, tstPlc->getTOS());
    ASSERT_EQ(1,tstPlc->stackSize());
    tstPlc->fromStack();

    tstPlc->TimLdn("00:00");
    ASSERT_EQ( true, tstPlc->fromStack());
    ASSERT_EQ(0,tstPlc->stackSize());

}

TEST(plcTest,TimAnd) {
    string now = tstPlc->getTime();
    string older;
    string newer;
    
    tstPlc->toStack(false);
    tstPlc->TimAnd(now);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimAnd(now);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    tstPlc->TimAnd("00:00");
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimAnd("00:00");
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimAnd(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimAnd(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimAnd(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimAnd(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
}

TEST(plcTest,TimAndn) {
    string now = tstPlc->getTime();
    string older;
    string newer;
    
    tstPlc->toStack(false);
    tstPlc->TimAndn(now);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimAndn(now);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    tstPlc->TimAndn("00:00");
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimAndn("00:00");
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimAndn(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimAndn(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimAndn(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimAndn(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
}

TEST(plcTest,TimOr) {
    string now = tstPlc->getTime();
    string older;
    string newer;
    
    tstPlc->toStack(false);
    tstPlc->TimOr(now);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimOr(now);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    tstPlc->TimOr("00:00");
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimOr("00:00");
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimOr(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimOr(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimOr(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimOr(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
}

TEST(plcTest,TimOrn) {
    string now = tstPlc->getTime();
    string older;
    string newer;
    
    tstPlc->toStack(false);
    tstPlc->TimOrn(now);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimOrn(now);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    tstPlc->TimOrn("00:00");
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    tstPlc->TimOrn("00:00");
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimOrn(older,newer);
    ASSERT_EQ(false,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -10 );
    newer = tstPlc->addMinutes( now, 10 );
    tstPlc->TimOrn(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(false);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimOrn(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
    
    tstPlc->toStack(true);
    now = tstPlc->getTime();
    older = tstPlc->addMinutes( now, -20 );
    newer = tstPlc->addMinutes( now, -10 );
    tstPlc->TimOrn(older,newer);
    ASSERT_EQ(true,tstPlc->fromStack());
    tstPlc->emptyStack();
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

TEST(plcTest,Orf ) {
    // 
    // Stack: false
    // New: OFF
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: true
    // New: OFF
    // Old: OFF
    // 
    // Expected outcome: true
    // 
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
    // 
    // Stack: false
    // New: ON
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: false
    // New: ON
    // Old: ON
    //
    // Expected outcome: false
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: false
    // New: OFF
    // Old: ON
    //
    // Expected outcome: true
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
    // 
    // Stack: true
    // New: OFF
    // Old: ON
    //
    // Expected outcome: true
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
    // 
    // Stack: true
    // New: ON
    // Old: ON
    //
    // Expected outcome: false
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Orf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
}

TEST(plcTest,Andr ) {
    // 
    // Stack: false
    // New: OFF
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Andr("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: true
    // New: OFF
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->Andr("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: false
    // New: ON
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Andr("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: false
    // New: ON
    // Old: ON
    //
    // Expected outcome: false
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Andr("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: true
    // New: ON
    // Old: OFF
    //
    // Expected outcome: true
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Andr("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
    // 
    // Stack: true
    // New: ON
    // Old: ON
    //
    // Expected outcome: false
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Andr("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
}

TEST(plcTest,Andf ) {
    // 
    // Stack: false
    // New: OFF
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->toStack(false);

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: true
    // New: OFF
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "OFF");
    tstPlc->toStack(true);

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: false
    // New: ON
    // Old: OFF
    // 
    // Expected outcome: false
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: false
    // New: ON
    // Old: ON
    //
    // Expected outcome: false
    // 
    tstPlc->toStack(false);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: true
    // New: ON
    // Old: OFF
    //
    // Expected outcome: true
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "OFF");

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
    // 
    // Stack: true
    // New: OFF
    // Old: ON
    //
    // Expected outcome: true
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "OFF");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( true, tstPlc->fromStack());
    // 
    // Stack: true
    // New: OFF
    // Old: OFF
    //
    // Expected outcome: false
    // 
    tstPlc->toStack(true);
    tstPlc->setValue("TEST", "ON");
    tstPlc->setOldValue("TEST", "ON");

    tstPlc->Andf("TEST");
    ASSERT_EQ(1, tstPlc->stackSize());
    ASSERT_EQ( false, tstPlc->fromStack());
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

TEST(plcTest,plcEnd ) {
    tstPlc->setValue("TEST","OFF");
    tstPlc->setOldValue("TEST","ON");
    tstPlc->toStack(true);

    ASSERT_EQ(false, tstPlc->plcEnd(0));
    ASSERT_EQ(tstPlc->getValue("TEST"), tstPlc->getOldValue("TEST"));

    ASSERT_EQ(0, tstPlc->stackSize());
}

TEST(plcTest,sqlCount) {
    tstPlc->setValue("TEST","OFF");
    tstPlc->setOldValue("TEST","OFF");

    ASSERT_LE(0,tstPlc->sqlCount("short_name='TEST' and value='ON' and oldvalue='OFF'"));

    tstPlc->setValue("TEST","ON");
    ASSERT_EQ(1,tstPlc->sqlCount("short_name='TEST' and value='ON' and oldvalue='OFF'"));

    tstPlc->setOldValue("TEST","ON");
    ASSERT_EQ(0,tstPlc->sqlCount("short_name='TEST' and value='ON' and oldvalue='OFF'"));

    tstPlc->setValue("TEST","OFF");
    ASSERT_EQ(0,tstPlc->sqlCount("short_name='TEST' and value='ON' and oldvalue='OFF'"));

    tstPlc->setOldValue("TEST","OFF");
    ASSERT_EQ(0,tstPlc->sqlCount("short_name='TEST' and value='ON' and oldvalue='OFF'"));
}



