#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

// WARNING: Very little validation is performed on this struct
// The main validation occurs when writing it back to a file
// Take care when setting it using code

struct UserSettings
{
    int ui_scale_index = 0;
};

#endif