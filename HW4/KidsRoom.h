#ifndef MATAM4_KIDSROOM_H
#define MATAM4_KIDSROOM_H

#include "EscapeRoomWrapper.h"

namespace mtm {
    namespace escaperoom {
        namespace kidsroom {
            /**
             * a class for kids room. inherits EscapeRoomWrapper.
             */
            class KidsRoom : public EscapeRoomWrapper {
                int age_limit;

            public:
                /**
                 * constructor for KidsRoom.
                 * @param name - the name of the KidsRoom.
                 * @param escapeTime - the escapeTime of the KidsRoom.
                 * @param level - the level of the KidsRoom.
                 * @param maxParticipants - the maximum participants for the
                 * KidsRoom.
                 * @param age_limit - the age limit of the KidsRoom.
                 */
                KidsRoom(char *name, const int &escapeTime, const int &level,
                         const int &maxParticipants,
                         const int &age_limit);
                /**
                 * a setter for the age limit.
                 * @param limit - the new age limit.
                 */
                void setNewAgeLimit(const int &limit);
                /**
                 * a getter for the age limit.
                 * @return the age limit of the KidsRoom.
                 */
                int getAgeLimit() const;
                //alowing outer function to get the private variables
                // of the class.
                friend std::ostream &
                operator<<(std::ostream &output, const KidsRoom &room);
            };
            /**
             * an operator<< overload function.
             * @param output - the output stream to use.
             * @param room - the KidsRoom to print the data on.
             * @return - the output stream reference.
             */
            std::ostream &operator<<(std::ostream &output, const KidsRoom &room);
        }
    }
}


#endif //MATAM4_KIDSROOM_H
