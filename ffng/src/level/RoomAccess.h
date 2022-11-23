#ifndef HEADER_ROOMACCESS_H
#define HEADER_ROOMACCESS_H

class FRoom;

#include "NoCopy.h"

/**
 * Interface to access changing room.
 */
class RoomAccess : public NoCopy {
    private:
        FRoom *m_room;
    private:
        void checkRoom() const;
    public:
        RoomAccess();
        ~RoomAccess();
        void takeRoom(FRoom *new_room);

        void cleanRoom();
        bool isRoom() const { return !!m_room; }
        FRoom *room();
        const FRoom *const_room() const;
};

#endif
