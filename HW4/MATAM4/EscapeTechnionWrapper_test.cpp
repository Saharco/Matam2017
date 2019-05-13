#include "EscapeRoomWrapper.h"
#include "mtmtest.h"
#include "Exceptions.h"
#include "Enigma.h"
#include "ScaryRoom.h"
#include "KidsRoom.h"
using namespace mtm::escaperoom;

//full constructor
bool fullConstructorTest() {
    ASSERT_NO_THROW(EscapeRoomWrapper((char*)"Best Room", 50, 6, 5));
    ASSERT_NO_THROW(EscapeRoomWrapper((char*)"B", 90, 10, 10));
    return true;
}

//pseudo-full constructor
bool pseudoFullConstructorTest() {
    ASSERT_NO_THROW(EscapeRoomWrapper((char*)"Best Room", 1));
    return true;
}

//copy constructor
bool copyConstructorTest() {
    EscapeRoomWrapper wrapper((char*)"Best Room", 1);
    ASSERT_NO_THROW(EscapeRoomWrapper(wrapper));
    return true;
}

//functions: getName, getMaxTime, getMaxParticipants, level
bool gettersTest() {
    EscapeRoomWrapper wrapper((char*)"Best Room", 36, 8, 10);
    ASSERT_TRUE(wrapper.level() == 8);
    ASSERT_TRUE(wrapper.getMaxParticipants() == 10);
    ASSERT_TRUE(wrapper.getMaxTime() == 36);
    ASSERT_TRUE(wrapper.getName() == "Best Room");
    return true;
}

//functions: getRate, rate
bool rateTest() {
    EscapeRoomWrapper wrapper((char*)"Best Room", 36, 8, 10);
    ASSERT_TRUE(wrapper.getRate() == 0);
    ASSERT_THROWS(EscapeRoomIllegalRateException, wrapper.rate(43));
    ASSERT_NO_THROW(wrapper.rate(4));
    ASSERT_TRUE(wrapper.getRate() == 4);
    ASSERT_NO_THROW(wrapper.rate(2));
    ASSERT_TRUE(wrapper.getRate() == 3);
    return true;
}

//operator: =
bool operatorAssignmentTest() {
    EscapeRoomWrapper wrapper1((char*)"Best Room", 1);
    EscapeRoomWrapper wrapper2 = wrapper1;
    ASSERT_TRUE(wrapper1 == wrapper2);
    return true;
}

//operator: ==, !=
bool operatorEqualToTest() {
    EscapeRoomWrapper wrapper1((char*)"Best Room", 36, 10, 10);
    EscapeRoomWrapper wrapper2((char*)"Best Room", 36, 10, 10);
    ASSERT_TRUE(wrapper1 == wrapper2);
    EscapeRoomWrapper wrapper3((char*)"Worst Room", 30, 10, 10);
    ASSERT_TRUE(wrapper1 != wrapper3);
    return true;
}

//operator: <, >
bool operatorBiggerSmallerThanTest() {
    EscapeRoomWrapper wrapper1((char*)"Best Room", 36, 10, 10);
    EscapeRoomWrapper wrapper2((char*)"Best-est Room!!", 90, 1, 1);
    ASSERT_TRUE(wrapper2 > wrapper1);
    ASSERT_TRUE(wrapper1 < wrapper2);
    return true;
}

//operator: >>
bool printTest() {
    EscapeRoomWrapper wrapper1((char*)"Best Room", 36, 8, 10);
    ASSERT_PRINT("Best Room (36/8/10)", wrapper1);
    EscapeRoomWrapper wrapper2((char*)"Biblical Cyborg Yoga Room", 2);
    ASSERT_PRINT("Biblical Cyborg Yoga Room (60/2/6)", wrapper2);
    EscapeRoomWrapper wrapper3((char*)"Excruciating Forklift on the Road", 6);
    ASSERT_PRINT("Excruciating Forklift on the Road (60/6/6)", wrapper3);
    return true;
}

bool EnigmaTest(){
    std::set<string> elements;
    elements.insert("1");
    elements.insert("2");
    elements.insert("3");
    elements.insert("4");
    elements.insert("5");
    elements.insert("6");
    elements.insert("7");
    elements.insert("8");
    Enigma enigma1("cool enigma",MEDIUM_ENIGMA,8,elements);
    elements.insert("9");
    elements.insert("10");
    Enigma enigma2("long enigma",HARD_ENIGMA,10,elements);
    Enigma enigma3("stupid enigma",EASY_ENIGMA,10,elements);
    EscapeRoomWrapper room((char *)"cool room",90,7,10);
    room.addEnigma(enigma1);
    room.addEnigma(enigma2);
    room.addEnigma(enigma3);
    ASSERT_TRUE(room.getHardestEnigma()==enigma2);
    std::vector<Enigma>& all_enigmas=room.getAllEnigmas();
    ASSERT_TRUE(all_enigmas.size()==3);
    std::vector<Enigma>::iterator iterator;
    iterator=all_enigmas.begin();
    ASSERT_PRINT("cool enigma (1) 8",*iterator);
    iterator++;
    ASSERT_PRINT("long enigma (2) 10",*iterator);
    iterator++;
    ASSERT_PRINT("stupid enigma (0) 10",*iterator);
    room.removeEnigma(enigma2);
    room.removeEnigma(enigma1);
    ASSERT_TRUE(all_enigmas.size()==1);
    return true;
}
bool getType(){
    scaryroom::ScaryRoom scary_room((char *)"scary stuff",90,7,3,20,10);
    kidsroom::KidsRoom kids_room((char *)"plush toys",60,9,4,7);
    EscapeRoomWrapper room((char *)"normal room",50,10,8);
    ASSERT_TRUE(scary_room.getType()=="Scary Room");
    ASSERT_TRUE(kids_room.getType()=="Kids Room");
    ASSERT_TRUE(room.getType()=="no type");
    return true;
}
int main() {
    RUN_TEST(fullConstructorTest);
    RUN_TEST(pseudoFullConstructorTest);
    RUN_TEST(copyConstructorTest);
    RUN_TEST(gettersTest);
    RUN_TEST(rateTest);
    RUN_TEST(operatorAssignmentTest);
    RUN_TEST(operatorEqualToTest);
    RUN_TEST(operatorBiggerSmallerThanTest);
    RUN_TEST(printTest);
    RUN_TEST(EnigmaTest);
    RUN_TEST(getType);
}