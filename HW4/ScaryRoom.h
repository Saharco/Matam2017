#ifndef MATAM4_SCARYROOM_H
#define MATAM4_SCARYROOM_H

#include "EscapeRoomWrapper.h"

namespace mtm {
    namespace escaperoom {
        namespace scaryroom {
            /**
            * a class for scary room. inherits EscapeRoomWrapper.
            */
            class ScaryRoom : public EscapeRoomWrapper {
                int age_limit;
                int num_scary_enigmas;


            public:
                /**
                 * constructor for ScaryRoom.
                 * @param name - the name of the ScaryRoom.
                 * @param escapeTime - the escapeTime of the ScaryRoom.
                 * @param level - the level of the ScaryRoom.
                 * @param maxParticipants - the maximum participants for the
                 * ScaryRoom.
                 * @param ageLimit  - the age limit of the ScaryRoom.
                 * @param numOfScaryEnigmas - the number of scary enigmas in the
                 * ScaryRoom.
                 */
                ScaryRoom(char *name, const int &escapeTime, const int &level,
                          const int &maxParticipants,
                          const int &ageLimit, const int &numOfScaryEnigmas);
                /**
                 * a setter for the age limit.
                 * @param limit - the new age limit.
                 */
                void setNewAgeLimit(const int &limit);
                /**
                 * increments the number of scary enigmas in the ScaryRoom by 1.
                 */
                void incNumberOfScaryEnigmas();
                /**
                 * a getter for the age limit.
                 * @return the age limit of the ScaryRoom.
                 */
                int getAgeLimit() const;
                //alowing outer function to get the private variables
                // of the class.
                friend std::ostream &
                operator<<(std::ostream &output, const ScaryRoom &room);

            };
            /**
             * an operator<< overload function.
             * @param output - the output stream to use.
             * @param room - the ScaryRoom to print the data on.
             * @return - the output stream reference.
             */
            std::ostream &
            operator<<(std::ostream &output, const ScaryRoom &room);
        }
    }
}
#endif //MATAM4_SCARYROOM_H
