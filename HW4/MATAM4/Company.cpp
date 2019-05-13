#include "Company.h"
#include "ScaryRoom.h"
#include "KidsRoom.h"

using namespace mtm::company;
using namespace mtm::escaperoom;
using namespace mtm::escaperoom::scaryroom;
using namespace mtm::escaperoom::kidsroom;

Company::Company(const string &name, const string &phone_number) : name(name),
                                                                   phone_number(
                                                                           phone_number) {}

void Company::createRoom(char *name, const int &escapeTime, const int &level,
                         const int &maxParticipants) {
    try {
        EscapeRoomWrapper *escape_room = new EscapeRoomWrapper(name, escapeTime,
                                                               level,
                                                               maxParticipants);
        rooms.push_back(escape_room);
    } catch (EscapeRoomMemoryProblemException e) {
        throw CompanyMemoryProblemException();
    }
}

Company::~Company() {
    for (EscapeRoomWrapper *room_remover:rooms) {
        delete (room_remover);
    }
}

void
Company::createScaryRoom(char *name, const int &escapeTime, const int &level,
                         const int &maxParticipants,
                         const int &ageLimit, const int &numOfScaryEnigmas) {
    try {
        EscapeRoomWrapper *scary_room = new ScaryRoom(name, escapeTime, level,
                                                      maxParticipants, ageLimit,
                                                      numOfScaryEnigmas);
        rooms.push_back(scary_room);
    } catch (EscapeRoomMemoryProblemException e) {
        throw CompanyMemoryProblemException();
    }
}

void
Company::createKidsRoom(char *name, const int &escapeTime, const int &level,
                        const int &maxParticipants,
                        const int &age_limit) {
    try {
        EscapeRoomWrapper *kids_room = new KidsRoom(name, escapeTime, level,
                                                    maxParticipants, age_limit);
        rooms.push_back(kids_room);
    } catch (EscapeRoomMemoryProblemException e) {
        throw CompanyMemoryProblemException();
    }
}

std::vector<EscapeRoomWrapper *> Company::getAllRooms() const {
    return std::vector<EscapeRoomWrapper *>(rooms);
}

static bool
roomsEqual(const EscapeRoomWrapper &room1, const EscapeRoomWrapper &room2) {
    return room1.getName() == room2.getName();
}

static std::vector<EscapeRoomWrapper *>::iterator
findRoom(std::vector<EscapeRoomWrapper *> &rooms,
         const EscapeRoomWrapper &room_to_find) {
    for (std::vector<EscapeRoomWrapper *>::iterator iterator = rooms.begin();
         iterator != rooms.end(); iterator++) {
        if (roomsEqual(**iterator, room_to_find)) {
            return iterator;
        }
    }
    throw CompanyRoomNotFoundException();
}

void Company::removeRoom(const EscapeRoomWrapper &room) {
    std::vector<EscapeRoomWrapper *>::iterator room_to_delete = findRoom(rooms,
                                                                         room);
    delete (*room_to_delete);
    rooms.erase(room_to_delete);
}

void Company::addEnigma(const Enigma &enigma, const EscapeRoomWrapper &room) {
    EscapeRoomWrapper *room_to_add_enigma = *findRoom(rooms, room);
    (*room_to_add_enigma).addEnigma(enigma);
}

void Company::removeEnigma(EscapeRoomWrapper &room, const Enigma &enigma) {
    EscapeRoomWrapper *room_to_remove_engima = *findRoom(rooms, room);
    try {
        room_to_remove_engima->removeEnigma(enigma);
    } catch (EscapeRoomNoEnigmasException e) {
        throw CompanyRoomHasNoEnigmasException();
    }
    catch (EscapeRoomEnigmaNotFoundException e) {
        throw CompanyRoomEnigmaNotFoundException();
    }
}

static Enigma &
findEnigma(const EscapeRoomWrapper &room, const Enigma &enigma_to_find) {
    for (Enigma &enigma_iterator : room.getAllEnigmas()) {
        if (enigma_iterator == enigma_to_find) {
            return enigma_iterator;
        }
    }
    throw CompanyRoomEnigmaNotFoundException();
}

void
Company::addItem(EscapeRoomWrapper &room, Enigma &enigma, string item_name) {
    EscapeRoomWrapper *room_add_item = *findRoom(rooms, room);
    Enigma &enigma_add_item = findEnigma(*room_add_item, enigma);
    try {
        enigma_add_item.addElement(item_name);
    } catch (EnigmaNoElementsException e) {
        throw CompanyRoomEnigmaHasNoElementsException();
    }
    catch (EnigmaElementNotFoundException e) {
        throw CompanyRoomEnigmaElementNotFoundException();
    }
}

void
Company::removeItem(EscapeRoomWrapper &room, Enigma &enigma, string item_name) {
    EscapeRoomWrapper *room_remove_item = *findRoom(rooms, room);
    Enigma &enigma_remove_item = findEnigma(*room_remove_item, enigma);
    try {
        enigma_remove_item.removeElement(item_name);
    } catch (EnigmaNoElementsException e) {
        throw CompanyRoomEnigmaHasNoElementsException();
    }
    catch (EnigmaElementNotFoundException e) {
        throw CompanyRoomEnigmaElementNotFoundException();
    }
}



EscapeRoomWrapper *Company::getRoomByName(const string &room_name) const {
    for (EscapeRoomWrapper *room:rooms) {
        if ((*room).getName() == room_name) {
            return room;
        }
    }
    throw CompanyRoomNotFoundException();
}

std::ostream &
mtm::company::operator<<(std::ostream &output, const Company &company) {
    output << company.name << ":" << company.phone_number << std::endl;
    for (EscapeRoomWrapper *room : company.rooms) {
        ScaryRoom *s = dynamic_cast<ScaryRoom *>(room);
        KidsRoom *k = dynamic_cast<KidsRoom *>(room);
        if(s!=NULL) {
            output << *s << std::endl;
        }
        else if(k!=NULL){
           output<<*k<<std::endl;
        }
        else {
            output << *room << std::endl;
        }
    }

    return output;
}