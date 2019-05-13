#include <cstdlib>
#include "Enigma.h"
#include "mtmtest.h"
#include <string>
#include "Exceptions.h"
using namespace mtm::escaperoom;
bool testEnigma(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    items.insert("3");
    items.insert("4");
    items.insert("5");
    Enigma enigma1("enigma1",EASY_ENIGMA,5,items);
    ASSERT_EQUALS(enigma1.getName(),"enigma1");
    ASSERT_EQUALS(enigma1.getDifficulty(),EASY_ENIGMA);
    return true;
}

bool testOperatorEqualTo(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    items.insert("3");
    Enigma enigma1("enigma1",MEDIUM_ENIGMA,3,items);
    items.insert("4");
    items.insert("5");
    items.insert("6");
    items.insert("7");
    items.insert("8");
    Enigma enigma2("enigma2",HARD_ENIGMA,8,items);
    ASSERT_FALSE(enigma1==enigma2);
    items.erase("8");
    Enigma enigma3("enigma1",MEDIUM_ENIGMA,7,items);
    ASSERT_TRUE(enigma1==enigma3);
    ASSERT_FALSE(enigma2==enigma3);

    items.insert("8");
    Enigma enigma4("enigma2",EASY_ENIGMA,8, items);
    ASSERT_FALSE(enigma2==enigma4);

    Enigma enigma5("enigma5",HARD_ENIGMA,8,items);
    ASSERT_FALSE(enigma5==enigma2);

    items.insert("9");
    Enigma enigma6("enigma2",HARD_ENIGMA,9,items);
    ASSERT_TRUE(enigma6==enigma2);

    return true;
}
bool testOpreatorNotEqualTo(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    items.insert("3");
    Enigma enigma1("enigma1",MEDIUM_ENIGMA,3,items);
    items.insert("4");
    Enigma enigma2("enigma2",HARD_ENIGMA,4,items);
    ASSERT_TRUE(enigma1!=enigma2);

    items.erase("4");
    Enigma enigma3("enigma1",MEDIUM_ENIGMA,3,items);
    ASSERT_FALSE(enigma1!=enigma3);
    ASSERT_TRUE(enigma2!=enigma3);

    Enigma enigma4("enigma2",EASY_ENIGMA,3,items);
    ASSERT_TRUE(enigma2!=enigma4);

    Enigma enigma5("enigma5",HARD_ENIGMA,3,items);
    ASSERT_TRUE(enigma5!=enigma2);
    items.insert("4");
    Enigma enigma6("enigma2",HARD_ENIGMA,4,items);
    ASSERT_FALSE(enigma6!=enigma2);
    return true;
}

bool testSmallerThan(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    Enigma enigma1("enigma1",MEDIUM_ENIGMA,2,items);
    items.erase("2");
    Enigma enigma2("enigma2",HARD_ENIGMA,1,items);
    ASSERT_TRUE(enigma1<enigma2);
    items.insert("2");
    items.insert("3");
    Enigma enigma3("enigma3",HARD_ENIGMA,3,items);
    items.insert("4");
    Enigma enigma4("enigma4",HARD_ENIGMA,4,items);
    ASSERT_FALSE(enigma3<enigma4);
    ASSERT_FALSE(enigma4<enigma3);
    items.insert("5");
    Enigma enigma5("enigma3",EASY_ENIGMA,5,items);
    ASSERT_TRUE(enigma5<enigma3);
    ASSERT_TRUE(enigma5<enigma1);
    return true;
}

bool testBiggerThan(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    items.insert("3");
    items.insert("4");
    Enigma enigma1("enigma1",MEDIUM_ENIGMA,4,items);
    items.erase("4");
    Enigma enigma2("enigma2",HARD_ENIGMA,3,items);
    ASSERT_FALSE(enigma1>enigma2);
    ASSERT_TRUE(enigma2>enigma1);
    items.insert("4");
    items.insert("5");
    Enigma enigma3("enigma3",HARD_ENIGMA,5,items);
    items.insert("6");
    Enigma enigma4("enigma4",HARD_ENIGMA,6,items);
    ASSERT_FALSE(enigma4>enigma3);
    ASSERT_FALSE(enigma3>enigma4);
    items.insert("7");
    Enigma enigma5("enigma3",EASY_ENIGMA,7,items);
    ASSERT_FALSE(enigma5>enigma3);
    ASSERT_FALSE(enigma5>enigma1);
    ASSERT_TRUE(enigma3>enigma5);
    ASSERT_TRUE(enigma1>enigma5);
    return true;
}

bool testOperatorPrint(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    items.insert("3");
    items.insert("4");
    items.insert("5");
    Enigma cool_riddle("joker's riddle",HARD_ENIGMA,5,items);
    ASSERT_PRINT("joker's riddle (2) 5",cool_riddle);
    items.insert("6");
    Enigma ridler_ridle("penguin sucks",MEDIUM_ENIGMA,6,items);
    ASSERT_PRINT("penguin sucks (1) 6",ridler_ridle);
    return true;
}
bool testAddElement(){
    set<string> items;
    items.insert("1");
    items.insert("2");
    items.insert("3");
    items.insert("4");
    items.insert("5");
    Enigma cool_riddle("joker's riddle",HARD_ENIGMA,5,items);
    ASSERT_NO_THROW(cool_riddle.addElement("new Element!"));
    ASSERT_NO_THROW(cool_riddle.removeElement("new Element!"));
    ASSERT_THROWS(EnigmaElementNotFundException,cool_riddle.removeElement("6"));
    return true;
}
bool testOperatorAssign(){
    std::set<string> elements;
    elements.insert("1");
    elements.insert("2");
    elements.insert("3");
    elements.insert("4");
    Enigma enigma("first",EASY_ENIGMA,4,elements);
    Enigma enigma_copy=enigma;
    std::cerr<<enigma_copy;
    return true;
};
int main(){
    RUN_TEST(testEnigma);
    RUN_TEST(testOperatorEqualTo);
    RUN_TEST(testOpreatorNotEqualTo);
    RUN_TEST(testSmallerThan);
    RUN_TEST(testBiggerThan);
    RUN_TEST(testOperatorPrint);
    RUN_TEST(testAddElement);
    RUN_TEST(testOperatorAssign);
    return 0;
}

