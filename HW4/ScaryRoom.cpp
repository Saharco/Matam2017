//
// Created by Yuval on 6/23/2017.
//

#include "ScaryRoom.h"
using namespace mtm::escaperoom::scaryroom;
ScaryRoom::ScaryRoom(char* name, const int& escapeTime, const int& level, const int& maxParticipants,
          const int& ageLimit, const int& numOfScaryEnigmas) : EscapeRoomWrapper(name,escapeTime,level,maxParticipants),
                                                               age_limit(ageLimit),num_scary_enigmas(numOfScaryEnigmas){}
void ScaryRoom::setNewAgeLimit(const int& limit){
    if(limit<0){
        throw ScaryRoomIllegalAgeLimit();
    }
    age_limit=limit;
}
void ScaryRoom::incNumberOfScaryEnigmas(){
    num_scary_enigmas++;
}
int ScaryRoom::getAgeLimit() const{
    return age_limit;
}

std::ostream& mtm::escaperoom::scaryroom::operator<<(std::ostream &output, const ScaryRoom &room){
    output << "Scary Room: " << room.getName()<<"(" <<room.getMaxTime()<< "/" <<room.level() << "/"
           << room.getMaxParticipants()<<"/"<<room.age_limit<<")";
    return output;
}