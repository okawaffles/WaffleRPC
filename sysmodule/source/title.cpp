#include "title.hpp"
#include <cstring>
#include <iostream>

const char* getNameFromTitleID(u64 title_id)
{
    // hacky fix for super mario 3d all-stars
    // each of them have their own title ID for some reason
    if (title_id == 0x010049900F546001) return "Super Mario 64 (Super Mario 3D All-Stars)";
    if (title_id == 0x010049900F546002) return "Super Mario Sunshine (Super Mario 3D All-Stars)";
    if (title_id == 0x010049900F546003) return "Super Mario Galaxy (Super Mario 3D All-Stars)";

    // if it isn't one of those title IDs
    static NsApplicationControlData appControlData;
    size_t appControlDataSize;
    NacpLanguageEntry *langEntry;

    memset(&appControlData, 0, sizeof(NsApplicationControlData));

    if (R_SUCCEEDED(nsGetApplicationControlData(
        NsApplicationControlSource_Storage, 
        title_id, 
        &appControlData, 
        sizeof(NsApplicationControlData), 
        &appControlDataSize
    )) 
    && 
    R_SUCCEEDED(nacpGetLanguageEntry(&appControlData.nacp, &langEntry)))
    {
        if (langEntry != nullptr) return langEntry->name;
    }

    // if there is no name for whatever reason, return the id
    return std::to_string(title_id).c_str();
}