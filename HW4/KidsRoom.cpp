//
// Created by Yuval on 6/24/2017.
//

#include "KidsRoom.h"
using namespace mtm::escaperoom::kidsroom;
KidsRoom::KidsRoom(char *name, const int &escapeTime, const int &level, const int &maxParticipants,
                                    const int& age_limit) : EscapeRoomWrapper(name, escapeTime, level, maxParticipants),
                                                     age_limit(age_limit) {}

void KidsRoom::setNewAgeLimit(const int& limit){
    if(limit<0){
        throw KidsRoomIllegalAgeLimit();
    }
    age_limit=limit;
}

int KidsRoom::getAgeLimit() const{
    return age_limit;
}

std::ostream& mtm::escaperoom::kidsroom::operator<<(std::ostream &output, const KidsRoom &room){
    output << "Kids Room: "<<room.getName()<<" ("<<room.getMaxTime()<<"/"<<room.level()<<"/"<<room.getMaxParticipants()
           <<"/"<<room.age_limit<<")";
    return output;
}