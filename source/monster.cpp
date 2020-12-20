#include "monster.hpp"

extern inline u8 isSmallMonster();

static u8 num_db_entries = 0;

static int compareMonsterInfo(const void* p1, const void* p2)
{
    MonsterInfo* entry1 = (MonsterInfo*)p1;
    MonsterInfo* entry2 = (MonsterInfo*)p2;

    if (entry1->id == entry2->id)
        return 0;
    else
        return (entry1->id > entry2->id) ? 1 : -1;
}

void initMonsterInfoDB()
{
    num_db_entries = sizeof(database)/sizeof(MonsterInfo);
    qsort(database, num_db_entries, sizeof(MonsterInfo), compareMonsterInfo);
}

MonsterInfo* getMonsterInfoFromDB(Monster* m)
{
    u32 id = m->identifier1;

    // offset different from game language
    switch (id & 0xFF) {
        // Français
        // Deutsch
        // 繁體中文
        case 0x40:
        case 0xC0:
            id = id + 0x60;
            break;
        // Español
        // Italiano
        case 0x00:
        case 0x80:
            id = id + 0x5A0;
            break;
        // English
        // 简体中文
        case 0x20:
        case 0xA0:
        default:
            break;
    }
    id <<= 8;
    id += m->identifier2;
    void* result = bsearch(&id, database, num_db_entries, sizeof(MonsterInfo), compareMonsterInfo);

    if (result)
        return (MonsterInfo*)result;

    return &unknown;
}