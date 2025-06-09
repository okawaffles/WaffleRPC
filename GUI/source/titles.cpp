#include "titles.h"

#define GAME_ICON_SIZE 512
#define GAME_ICON_BIT_DEPTH 8

namespace Titles
{

Result dumpGameIcon(u64 title_id)
{
    mkdir("sdmc:/WaffleRPC", 0777);
    mkdir("sdmc:/WaffleRPC/icons", 0777);

    std::stringstream file_out_name;
    
    file_out_name << "sdmc:/WaffleRPC/icons/" << 0 << std::hex << title_id << ".jpg";

    NsApplicationControlData *appControlData = new NsApplicationControlData();

    Result res;
    res = Titles::getAppControlData(title_id, appControlData);

    if (R_FAILED(res))
    {
        delete appControlData;
        return res;
    }

    // start creation of png
    FILE *fp = fopen(file_out_name.str().c_str(), "wb");
    if (!fp) return 2;

    fwrite(appControlData->icon, 1, sizeof(appControlData->icon), fp);

    fclose(fp);

    return 0;
}


Result dumpAllGameIcons()
{
    // get all games
    NsApplicationRecord *appRecord = new NsApplicationRecord[512];
    s32 realAppRecordCount = 0;
    Result rc;
    rc = nsListApplicationRecord(appRecord, sizeof(NsApplicationRecord) * 512, 0, &realAppRecordCount);
    if (R_FAILED(rc))
    {
        delete[] appRecord;
        return rc;
    }

    printf("Starting dump of %d icons.\nThis may take a while, please hold on...\n \n", realAppRecordCount);

    for (s32 i = 0; i < realAppRecordCount; i++)
    {
        printf("Dumping icon for %ld...", appRecord[i].application_id);
        consoleUpdate(NULL);
        rc = dumpGameIcon(appRecord[i].application_id);
        if (R_SUCCEEDED(rc)) printf("OK!\n");
        else printf("FAIL (code %d)\n", rc);
        consoleUpdate(NULL);
    }

    printf(" \nFinished dumping icons!\n");
    consoleUpdate(NULL);

    return 0;
}


Result getAppControlData(u64 title_id, NsApplicationControlData *appControlData)
{
    size_t appControlDataSize = 0;

    memset(appControlData, 0x00, sizeof(NsApplicationControlData));

    Result rc;
    rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, title_id, appControlData, sizeof(NsApplicationControlData), &appControlDataSize);
    if (R_FAILED(rc))
        return rc;

    return 0;
}

}