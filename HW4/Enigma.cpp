#include "Enigma.h"
using namespace mtm::escaperoom;
Enigma::Enigma(const std::string& name, const Difficulty& difficulty,
       const unsigned int& numOfElements,const set<string>& elements) : name(name),difficulty(difficulty) {
    if(elements.size()!=numOfElements){
        throw EnigmaIllegalSizeParamException();
    }
    items_amount=numOfElements;
    items_names=set<string>(elements);
}
/*
 * test this function set initialization.
 */
Enigma::Enigma(const std::string& name, const Difficulty& difficulty) : name(name),difficulty(difficulty),
                                                                        items_amount(0),items_names(set<string>()){
}



bool Enigma::operator==(const Enigma& enigma) const{
    return (name==enigma.name && difficulty == enigma.difficulty);
}

bool Enigma::operator!=(const Enigma& enigma) const{
    return (!(operator==(enigma)));
}

bool Enigma::operator<(const Enigma& enigma) const{
    return difficulty<enigma.difficulty;
}

bool Enigma::operator>(const Enigma& enigma) const{
    return difficulty>enigma.difficulty;
}


bool Enigma::areEqualyComplex(const Enigma& enigma) const{
    return (difficulty==enigma.difficulty &&
            items_amount == enigma.items_amount);
}

Difficulty Enigma::getDifficulty() const{
    return difficulty;
}

string Enigma::getName() const{
    string name_copy(name);
    return name_copy;
}

std::ostream& mtm::escaperoom::operator<<(std::ostream& output,
                                          const Enigma& enigma){
    output << enigma.name << " (" << enigma.difficulty << ") "
           << enigma.items_amount;
    return output;
}

void Enigma::addElement(const string& element){
    items_names.insert(element);
    items_amount++;
}

void Enigma::removeElement(const string& element){
    if(items_names.size()<1){
        throw EnigmaNoElementsException();
    }
    if(items_names.erase(element)<1){
        throw EnigmaElementNotFoundException();
    }
    items_amount--;
}
Enigma::Enigma(const Enigma& enigma){
    difficulty=enigma.difficulty;
    name=enigma.name;
    items_names=set<string>(enigma.items_names);
    items_amount=enigma.items_amount;

}
Enigma& Enigma::operator=(const Enigma& enigma){
    difficulty=enigma.difficulty;
    items_names=set<string>(enigma.items_names);
    items_amount=enigma.items_amount;
    name=enigma.name;
    return *this;

}