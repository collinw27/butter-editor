#ifndef NOTIFS_H
#define NOTIFS_H

namespace NOTIF_TIMELINE
{
    constexpr int ID = 1;
    
    enum Notif
    {
        CLIP_BOUNDS_CHANGED,
        // Args: (id_s* clip_id)

        CLIP_DELETED
        // Args: (id_s* clip_id)
    };
}

namespace NOTIF_PROJECT_INFO
{
    constexpr int ID = 2;

    enum Notif
    {
        LENGTH_CHANGED
        // Args: ()
    };
}

namespace NOTIF_MEDIA
{
    constexpr int ID = 3;

    enum Notif
    {
        MEDIA_CREATED
        // Args: (id_s* media_id)
    };
}

#endif