
#ifndef MATAM4_COMPANY_H
#define MATAM4_COMPANY_H

#include <string>
#include <set>
#include "EscapeRoomWrapper.h"

using namespace mtm::escaperoom;
namespace mtm {
    namespace company {
        class Company {
            const string name;
            const string phone_number;
            std::vector<EscapeRoomWrapper*> rooms;

        public:
            /**
             * constructor for Company.
             * @param name - the name of the Company.
             * @param phone_number - the phone number of the company.
             */
            Company(const string &name, const string &phone_number);
            /**
             * a destructor for Company.
             */
            ~Company();
            /**
             * creates an EscapeRoom with the given parameters.
             * @param name - the name of the EscapeRoom.
             * @param escapeTime - the EscapeTime of the EscapeRoom.
             * @param level - the level of the EscapeRoom.
             * @param maxParticipants - the maximum amount of participants in
             * the EscapeRoom.
             */
            void createRoom(char *name, const int &escapeTime, const int &level,
                            const int &maxParticipants);
            /**
             * creates a ScaryRoom with the given parameters.
             * @param name - the name of the ScaryRoom.
             * @param escapeTime - the EscapeTime of the ScaryRoom.
             * @param level - the level of the ScaryRoom.
             * @param maxParticipants - the maximum amount of participants in
             * the ScaryRoom.
             * @param ageLimit - the age limit of the ScaryRoom.
             * @param numOfScaryEnigmas - the number of scary enigmas in the
             * ScaryRoom.
             */
            void createScaryRoom(char *name, const int &escapeTime, const int &level,
                            const int &maxParticipants,
                            const int &ageLimit, const int &numOfScaryEnigmas);
            /**
             * creates a new KidsRoom with the given parameters.
             * @param name - the name of the KidsRoom.
             * @param escapeTime - the EscapeTime of the KidsRoom.
             * @param level - the level of the KidsRoom.
             * @param maxParticipants - the maximum amount of participants in
             * the KidsRoom.
             * @param age_limit - the age limit of the KidsRoom.
             */
            void createKidsRoom(char *name, const int &escapeTime, const int &level,
                           const int &maxParticipants,
                           const int &age_limit);
            /**
             * a getter used to get all the rooms of the Company.
             * @return -  a vector of the Escape Rooms in the Company.
             */
            std::vector<EscapeRoomWrapper *> getAllRooms() const;
            /**
             * removes a room from the company.
             * @param room - the room to remove from the company.
             */
            void removeRoom(const EscapeRoomWrapper &room);
            /**
             * adds a new enigma to a room.
             * @param enigma - the new enigma to add.
             * @param room - the room to add the enigma to.
             */
            void addEnigma(const Enigma &enigma, const EscapeRoomWrapper &room);
            /**
             * removes an enigma from a room in the company.
             * @param room - the room to remove the enigma from.
             * @param enigma - the enigma to remove.
             */
            void removeEnigma(EscapeRoomWrapper &room, const Enigma &enigma);
            /**
             * adds a new item to an enigma in a specific room.
             * @param room - the room to add the item to.
             * @param enigma - the enigma to add the item to.
             * @param item_name - the item to add.
             */
            void addItem(EscapeRoomWrapper &room, Enigma &enigma, string item_name);
            /**
             * removes an item from an enigma in a specific room.
             * @param room - the room the enigma belongs to.
             * @param enigma - the enigma the item belongs to.
             * @param item_name - the item to be removed.
             */
            void removeItem(EscapeRoomWrapper &room, Enigma &enigma,
                            string item_name);
            /**
             * a getter used to get all of the rooms matching to the type
             * specified in the template.
             * @tparam T - the type of room the function is adding to the result
             * vector.
             * @return a vector containing only rooms that have a matching type
             * with the given template.
             */
            template <class T>
            std::vector<T*> getAllRoomsByType() const{
                    std::vector<T *> vector_result;
                    for (EscapeRoomWrapper *room : rooms){
                        T* casted_room=dynamic_cast<T *>(room);
                        if(casted_room!=NULL){
                            vector_result.push_back(casted_room);
                        }
                }
                return vector_result;
            }
            /**
             * a getter.
             * @param room_name - the room name's.
             * @return - a reference to a room that has the same name as given.
             */
            EscapeRoomWrapper *getRoomByName(const string &room_name) const;
            //giving full access to the function to allow it to use private
            // variables of the class Company.
            friend std::ostream &operator<<(std::ostream &output, const Company &company);
        };
        /**
         * an operator<< overload function.
         * @param output - the output stream to use.
         * @param company - the company object.
         * @return - the output stream reference.
         */
        std::ostream &operator<<(std::ostream &output, const Company &company);
    }
}

#endif //MATAM4_COMPANY_H
