#include "EscapeRoomWrapper.h"
#include "Exceptions.h"
#include <stdlib.h>
using namespace mtm::escaperoom;
const int default_max_participant=6;
const int default_max_time=60;
EscapeRoomWrapper::EscapeRoomWrapper(char* name, const int& escapeTime,
                                     const int& level,
                                     const int& maxParticipants){
    escape_room=escapeRoomCreate(name,escapeTime,maxParticipants,level);
    if(escape_room==NULL) {
        throw EscapeRoomMemoryProblemException();
    }
}
EscapeRoomWrapper::EscapeRoomWrapper(char* name, const int& level){
    escape_room=escapeRoomCreate(name,default_max_time,default_max_participant,
                                 level);
    if(escape_room==NULL){
        throw EscapeRoomMemoryProblemException();
    }
}
EscapeRoomWrapper::EscapeRoomWrapper(const EscapeRoomWrapper& room){
    escape_room=escapeRoomCopy(room.escape_room);
    if(escape_room==NULL){
        throw EscapeRoomMemoryProblemException();
    }
}
EscapeRoomWrapper& EscapeRoomWrapper::operator=(const EscapeRoomWrapper& room){
    escapeRoomDestroy(this->escape_room);
    *this=EscapeRoomWrapper(room);
    return *this;
}

bool EscapeRoomWrapper::operator==(const EscapeRoomWrapper& room) const{
    return (getPower()==room.getPower() && getName()==room.getName());
}

bool EscapeRoomWrapper::operator!=(const EscapeRoomWrapper& room) const{
    return !(*this==room);
}

bool EscapeRoomWrapper::operator<(const EscapeRoomWrapper& room) const{
    return getPower()<room.getPower();
}

bool EscapeRoomWrapper::operator>(const EscapeRoomWrapper& room) const{
    return getPower()>room.getPower();
}
std::ostream& mtm::escaperoom::operator<<(std::ostream& output,
                                          const EscapeRoomWrapper& room){
    std::string name=room.getName();
    int level=room.level();
    int max_time=room.getMaxTime();
    int max_participants=room.getMaxParticipants();
    output << name << " (" << max_time << "/" << level << "/"
           << max_participants << ")";
    return output;
}


EscapeRoomWrapper::~EscapeRoomWrapper() {
    escapeRoomDestroy(escape_room);
}

void EscapeRoomWrapper::rate(const int& newRate) const{
    if(updateRate(escape_room,newRate)!=ESCAPEROOM_SUCCESS){
        throw EscapeRoomIllegalRateException();
    }
}

int EscapeRoomWrapper::level() const{
    return getLevel(escape_room);
}

std::string EscapeRoomWrapper::getName() const{
    char *name_c=roomGetName(escape_room);
    std::string name(name_c);
    free(name_c);
    return name;
}
double EscapeRoomWrapper::getRate() const{
    return roomGetRate(escape_room);
}
int EscapeRoomWrapper::getMaxTime() const{
    return roomGetMaxTime(escape_room);
}
int EscapeRoomWrapper::getMaxParticipants() const{
    return roomGetMaxParticipants(escape_room);
}
double EscapeRoomWrapper::getPower() const{
    int room_level=level();
    int max_time=getMaxTime();
    int max_participants=getMaxParticipants();
    return ((double)(room_level*max_time))/max_participants;
}
void EscapeRoomWrapper::addEnigma(const Enigma& enigma){
    enigmas.push_back(enigma);
}
void EscapeRoomWrapper::removeEnigma(const Enigma& enigma){
    if(enigmas.size()<1){
        throw EscapeRoomNoEnigmasException();
    }

    for(unsigned int i=0;i<enigmas.size();i++){
        if(enigmas[i]==enigma){
            enigmas.erase(enigmas.begin()+i);
                return;
        }
    }
    throw EscapeRoomEnigmaNotFoundException();
}

Enigma EscapeRoomWrapper::getHardestEnigma(){
    if(enigmas.size()==0){
        throw EscapeRoomNoEnigmasException();
    }
    Enigma hardest_enigma=*enigmas.begin();
    Difficulty max_diff=hardest_enigma.getDifficulty();
    for(Enigma& enigma : enigmas){
        if(enigma.getDifficulty()>max_diff){
            hardest_enigma=enigma;
            max_diff=enigma.getDifficulty();
        }
    }
    return hardest_enigma;
}

std::vector<Enigma>& EscapeRoomWrapper::getAllEnigmas() const{
    return const_cast<std::vector<Enigma>&>(enigmas);
}
