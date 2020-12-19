#pragma once
#include <switch.h>
#include <algorithm>
#include <stdio.h>

#define MAX_POINTERS_IN_LIST 10
#define MAX_PARTS_PER_MONSTER 8

//Parts info
#pragma pack(1)
typedef struct
{
  u16 unknown;      //0x0
  u32 fixed;        //0x2: F8 99 D0 00
  u8 stagger_count; //0x6
  u8 break_count;   //0x7
  u16 stagger_hp;   //0x8
  s16 break_hp;     //0xA  
} Part;
#pragma pack()

#pragma pack(1)
typedef struct
{
    u8 pad_0x0[0xD];
    u8 location_flag;     //0xD: 4C = in current map, 44 = not in current map
    u8 pad_0xE[0x15DC];
    u8 identifier2;       //0x15EA: 00 or 80 for small monsters, 20 for ~drome type, 60 for Great Maccao, 08 for Kirin, otherwise ...
    //        04 or 44 for regular monsters, 0C or 4C for hyper monsters or special monsters (eg. Alatreon)
    //        Note: deviant/special-form monsters can have either, depending on the species, so not useful for differentiating those
    u8 pad_0x15EB[0x1C5];
    u32 hp;               //0x17B0
    u32 max_hp;           //0x17B4
    u8 pad_0x17B8[0x3E];  //0x17B8
    Part parts[8];        //0x17F6
    u8 pad_0x1856[0x5DD2];      //0x1856
    u16 identifier1;           //0x7628
} Monster;
#pragma pack()

typedef struct
{
  u32 m[MAX_POINTERS_IN_LIST];   //0x10  monster pointer 32bit
  u8 count;         //0x38: number of pointers
} MonsterPointerList;

//#pragma pack(pop)

typedef struct
{
  u16 max_stagger_hp;
  s16 max_break_hp;
} PartCache;

typedef struct
{
  u32 mptr;
  u32 hp;
  u32 max_hp;//set to 0 to deactivate
  PartCache p[8]; //only sum displayable parts
  char* name;
} MonsterCache;

typedef struct {
  u32 id;           //identifier1 concat with identifier2, which makes unique id for each monster type
  char name[11];
  char cn_name[32];
  u8 is_hyper;
  float base_size;
  float s_size;
  float l_size;
  float xl_size;
} MonsterInfo;

// monster database
static MonsterInfo unknown = {0x0, "UNKNOWN", "未知", 0};
static MonsterInfo database[] = {
  {0x452060, "Gt. Maccao", "跳狗龙王", 0, 797.3, 718, 917, 981},
  {0x27A020, "Velocidrom", "蓝速龙王", 0, 868, 781, 998, 1068},
  {0x2F2004, "Bulldrome", "野猪王", 0, 566, 509, 651, 696},
  {0x40A004, "Seltas", "穿甲虫", 0, 616, 554, 708, 758},
  {0x3F2044, "Sel. Queen", "重甲虫", 0, 1445, 1300, 1662, 1777},
  {0x3F204C, "Sel. Queen", "狞猛重甲虫", 1, 1445, 1300, 1662, 1777},
  {0x39A044, "Arzuros", "青熊兽", 0, 563, 506, 647, 692},
  {0x3A2044, "Redhelm", "红盔", 0, 822, 789, 863, 896},
  {0x5F2020, "Giadrome", "白速龙王", 0, 867, 780, 997, 1066},
  {0x282020, "Gendrome", "黄速龙王", 0, 864.5, 778, 994, 1063},
  {0x292044, "Cephadrome", "砂龙王", 0, 1678, 1510, 1929, 2063},
  {0x25A044, "Yian K.K.", "怪鸟", 0, 899, 809, 1033, 1105},
  {0x25A04C, "Yian K.K.", "狞猛怪鸟", 1, 899, 809, 1033, 1105},
  {0x28A020, "Iodrome", "红速龙王", 0, 912, 821, 1048, 1121},
  {0x3CA044, "K. Wacha", "奇猿狐", 0, 886, 797, 966, 1010},
  {0x3CA04C, "K. Wacha", "狞猛奇猿狐", 1, 886, 797, 966, 1010},
  {0x3AA004, "Lagombi", "白兔兽", 0, 527, 475, 606, 649},
  {0x3B2004, "Snowbaron", "大雪主", 0, 639, 620, 748, 787},
  {0x262044, "Gypceros", "毒怪鸟", 0, 991, 892, 1140, 1219},
  {0x26204C, "Gypceros", "狞猛毒怪鸟", 1, 991, 892, 1140, 1219},
  {0x3D2044, "Tetsucabra", "鬼娃", 0, 1266, 1139, 1456, 1557},
  {0x3D204C, "Tetsucabra", "狞猛鬼娃", 1, 1266, 1139, 1456, 1557},
  {0x3DA044, "Drilltusk", "岩穿", 0, 1513, 1452, 1589, 1649},
  {0x2AA044, "Daimyo H.", "大名盾蟹", 0, 495, 446, 569, 609},
  {0x2AA04C, "Daimyo H.", "狞猛大名盾蟹", 1, 495, 446, 569, 609},
  {0x2B2044, "Stonefist", "矛碎", 0, 1495, 1450, 1704, 1779},
  {0x3BA044, "Volvidon", "赤甲兽", 0, 697, 627, 801, 857},
  {0x5EA044, "Congalala", "桃毛兽王", 0, 984, 886, 1132, 1210},
  {0x5EA04C, "Congalala", "狞猛桃毛兽王", 1, 984, 886, 1132, 1210},
  {0x35A044, "R. Ludroth", "水兽", 0, 1545, 1391, 1777, 1901},
  {0x35A04C, "R. Ludroth", "狞猛水兽", 1, 1545, 1391, 1777, 1901},
  {0x602044, "Barroth", "土砂龙", 0, 1406, 1266, 1617, 1730},
  {0x60204C, "Barroth", "狞猛土砂龙", 1, 1406, 1266, 1617, 1730},
  {0x5A2044, "Basarios", "岩龙", 0, 1426, 1284, 1640, 1755},
  {0x5A204C, "Basarios", "狞猛岩龙", 1, 1426, 1284, 1640, 1755},
  {0x41A044, "Malfestio", "夜鸟", 0, 612, 550, 703, 752},
  {0x41A04C, "Malfestio", "狞猛夜鸟", 1, 612, 550, 703, 752},
  {0x622044, "Hazewing", "胧隐", 0, 733, 711, 858, 902},
  {0x3E2044, "Zamtrios", "变形冰鲨", 0, 1520, 1368, 1748, 1869},
  {0x3E204C, "Zamtrios", "狞猛变形冰鲨", 1, 1520, 1368, 1748, 1869},
  {0x252044, "Khezu", "白电", 0, 872, 785, 950, 994},
  {0x25204C, "Khezu", "狞猛白电", 1, 872, 785, 950, 994},
  {0x612004, "Nerscylla", "影蜘蛛", 0, 999, 899, 1119, 1179},
  {0x61200C, "Nerscylla", "狞猛影蜘蛛", 1, 999, 899, 1119, 1179},
  {0x222044, "Rathian", "雌火龙", 0, 1654, 1489, 1902, 2035},
  {0x22204C, "Rathian", "雌火龙", 1, 1654, 1489, 1902, 2035},
  {0x22A044, "Gld. Rath.", "金火龙", 0, 1654, 1605, 1936, 2035},
  {0x22A04C, "Gld. Rath.", "狞猛金火龙", 1, 1654, 1605, 1936, 2035},
  {0x232044, "Dreadqueen", "紫毒姬", 0, 2056, 1973, 2158, 2241},
  {0x23A044, "Rathalos", "火龙", 0, 1711, 1540, 1968, 2105},
  {0x23A04C, "Rathalos", "狞猛火龙", 1, 1711, 1540, 1968, 2105},
  {0x242044, "Slv. Rath.", "银火龙", 0, 1711, 1660, 2002, 2105},
  {0x24204C, "Slv. Rath.", "狞猛银火龙", 1, 1711, 1660, 2002, 2105},
  {0x24A044, "Dreadking", "黑炎王", 0, 2058, 1975, 2161, 2243},
  {0x37A004, "Nibelsnarf", "潜口龙", 0, 2040, 1836, 2347, 2510},
  {0x37A00C, "Nibelsnarf", "狞猛潜口龙", 1, 2040, 1836, 2347, 2510},
  {0x26A044, "Plesioth", "水龙", 0, 2625, 2362, 3019, 3229},
  {0x26A04C, "Plesioth", "狞猛水龙", 1, 2625, 2362, 3019, 3229},
  {0x2C2044, "Blangonga", "雪狮子王", 0, 860, 774, 989, 1058},
  {0x2C204C, "Blangonga", "狞猛雪狮子王", 1, 860, 774, 989, 1058},
  {0x312044, "Lavasioth", "熔岩龙", 0, 2078, 1870, 2389, 2556},
  {0x31204C, "Lavasioth", "狞猛熔岩龙", 1, 2078, 1870, 2389, 2556},
  {0x2BA044, "Shogun C.", "将军镰蟹", 0, 598, 538, 688, 736},
  {0x2BA04C, "Shogun C.", "狞猛将军镰蟹", 1, 598, 538, 688, 736},
  {0x5E2044, "Shredclaw", "铠裂", 0, 688, 661, 757, 785},
  {0x3EA004, "Najarala", "绞蛇龙", 0, 4055, 3650, 4663, 4988},
  {0x3EA00C, "Najarala", "狞猛绞蛇龙", 1, 4055, 3650, 4663, 4988},
  {0x31A044, "Nargacuga", "迅龙", 0, 1602, 1442, 1843, 1971},
  {0x31A04C, "Nargacuga", "狞猛迅龙", 1, 1602, 1442, 1843, 1971},
  {0x322044, "Silverwind", "白疾风", 0, 1702, 1651, 1992, 2094},
  {0x29A044, "Y. Garuga", "黑狼鸟", 0, 1396, 1256, 1605, 1717},
  {0x29A04C, "Y. Garuga", "狞猛黑狼鸟", 1, 1396, 1256, 1605, 1717},
  {0x2A2044, "Deadeye", "独眼", 0, 1186, 1151, 1388, 1459},
  {0x342044, "Uragaan", "爆锤龙", 0, 2085, 1877, 2398, 2565},
  {0x34204C, "Uragaan", "狞猛爆锤龙", 1, 2085, 1877, 2398, 2565},
  {0x34A044, "Crystalbrd", "宝缠", 0, 2655, 2549, 2787, 2894},
  {0x352044, "Lagiacrus", "海龙", 0, 2649, 2384, 3046, 3258},
  {0x35204C, "Lagiacrus", "狞猛海龙", 1, 2649, 2384, 3046, 3258},
  {0x382044, "Zinogre", "雷狼龙", 0, 1455, 1310, 1674, 1790},
  {0x38204C, "Zinogre", "狞猛雷狼龙", 1, 1455, 1310, 1674, 1790},
  {0x38A04C, "Thunderlrd", "金雷公", 0, 1534, 1488, 1795, 1887},
  {0x5FA044, "Barioth", "冰牙龙", 0, 1822, 1640, 2095, 2241},
  {0x5FA04C, "Barioth", "狞猛冰牙龙", 1, 1822, 1640, 2095, 2241},
  {0x43A044, "Mizutsune", "泡狐龙", 0, 1885, 1697, 2168, 2319},
  {0x43A04C, "Mizutsune", "狞猛泡狐龙", 1, 1885, 1697, 2168, 2319},
  {0x632044, "Divinsight", "天眼", 0, 2428, 2356, 2671, 2768},
  {0x432044, "Astalos", "电龙", 0, 1755, 1580, 2019, 2159},
  {0x43204C, "Astalos", "狞猛电龙", 1, 1755, 1580, 2019, 2159},
  {0x62A044, "Azurebolt", "青电主", 0, 2219, 2131, 2330, 2419},
  {0x442004, "Gammoth", "巨兽", 0, 2276, 2048, 2481, 2595},
  {0x44200C, "Gammoth", "狞猛巨兽", 1, 2276, 2048, 2481, 2595},
  {0x63A004, "Frostpeak", "银峰", 0, 2276, 2208, 2504, 2595},
  {0x422044, "Glavenus", "斩龙", 0, 2539, 2286, 2920, 3124},
  {0x42204C, "Glavenus", "狞猛斩龙", 1, 2539, 2286, 2920, 3124},
  {0x42A04C, "Hellblade", "烬灭刃", 0, 2841, 2756, 3239, 3381},
  {0x362044, "Agnaktor", "炎戈龙", 0, 2737, 2464, 3148, 3367},
  {0x36204C, "Agnaktor", "狞猛炎戈龙", 1, 2737, 2464, 3148, 3367},
  {0x3FA044, "G. Magala", "黑蚀龙", 0, 1761, 1585, 2025, 2166},
  {0x3FA04C, "G. Magala", "狞猛黑蚀龙", 1, 1761, 1585, 2025, 2166},
  {0x61A044, "Chaotic GM", "混沌黑蚀龙", 0, 1795, 1615, 2064, 2207},
  {0x412044, "Seregios", "千刃龙", 0, 1728, 1555, 1884, 1970},
  {0x41204C, "Seregios", "狞猛千刃龙", 1, 1728, 1555, 1884, 1970},
  {0x372044, "Duramboros", "尾锤龙", 0, 2084, 1876, 2397, 2564},
  {0x37204C, "Duramboros", "狞猛尾锤龙", 1, 2084, 1876, 2397, 2564},
  {0x2FA044, "Tigrex", "轰龙", 0, 1735, 1562, 1996, 2135},
  {0x2FA04C, "Tigrex", "狞猛轰龙", 1, 1735, 1562, 1996, 2135},
  {0x302044, "Grimclaw", "荒钩爪", 0, 2121, 2036, 2227, 2312},
  {0x5AA044, "Gravios", "铠龙", 0, 2099, 1889, 2414, 2581},
  {0x5AA04C, "Gravios", "狞猛铠龙", 1, 2099, 1889, 2414, 2581},
  {0x5B2044, "Diablos", "角龙", 0, 1993, 1794, 2292, 2451},
  {0x5B204C, "Diablos", "狞猛角龙", 1, 1993, 1794, 2292, 2451},
  {0x5BA044, "Bloodlust", "鏖魔", 0, 2409, 2313, 2650, 2747},
  {0x272008, "Kirin", "麒麟", 0, 550, 495, 643, 676},
  {0x3C2044, "Brachydios", "碎龙", 0, 1522, 1370, 1750, 1872},
  {0x3C204C, "Brachydios", "狞猛碎龙", 1, 1522, 1370, 1750, 1872},
  {0x60A044, "Rg. Brachy", "猛爆碎龙", 0, 1522, 1370, 1750, 1872},
  {0x40204C, "S. Magala", "天廻龙", 0, 1778, 1600, 2080, 2187},
  {0x64204C, "Valfalk", "天彗龙", 0, 2267, 2041, 2539, 2675},
  {0x2CA044, "Rajang", "金狮子", 0, 966, 869, 1130, 1188},
  {0x2CA04C, "Rajang", "狞猛金狮子", 1, 966, 869, 1130, 1188},
  {0x2D204C, "Fr. Rajang", "激昂金狮子", 0, 966, 869, 1130, 1188},
  {0x332044, "Deviljho", "恐暴龙", 0, 2047, 1842, 2456, 2620},
  {0x33204C, "Deviljho", "狞猛恐暴龙", 1, 2047, 1842, 2456, 2620},
  {0x33A044, "Savage D.", "惶怒恐暴龙", 0, 2047, 1842, 2456, 2620},
  {0x2DA044, "K. Daora", "钢龙", 0, 1801, 1621, 2108, 2216},
  {0x2E204C, "Chameleos", "霞龙", 0, 1993, 1794, 2332, 2451},
  {0x2EA04C, "Teostra", "炎王龙", 0, 1740, 1566, 2036, 2140},
  {0x5C200C, "LaoShan L.", "老山龙", 0, 0, 0, 0, 0},
  {0x30A04C, "Akantor", "霸龙", 0, 0, 0, 0, 0},
  {0x32A04C, "Ukanlos", "崩龙", 0, 0, 0, 0, 0},
  {0x39200C, "Amatsu", "岚龙", 0, 0, 0, 0, 0},
  {0x44A00C, "Nakarkos", "骸龙", 0, 0, 0, 0, 0},
  {0x00000C, "Tentacle", "触手", 0, 0, 0, 0, 0},
  {0x64A00C, "Altal Thrn", "阁螳螂·机甲", 0, 0, 0, 0, 0},
  {0x652004, "Atlal Ka", "阁螳螂", 0, 0, 0, 0, 0},
  {0x36A04C, "Alatreon", "煌黑龙", 0, 0, 0, 0, 0},
  {0x5CA00C, "Fatalis", "黑龙", 0, 0, 0, 0, 0},
  {0x5D200C, "Crimson F.", "红龙", 0, 0, 0, 0, 0},
  {0x5DA00C, "White F.", "祖龙", 0, 0, 0, 0, 0},
};

//static vars
static MonsterCache m_cache[2]; //assume only 2 big monsters are active at a time

inline u8 isSmallMonster(Monster* m)
{
  return m->identifier2 == 0 || m->identifier2 == 0x80;
}

static int compareMonsterInfo(const void* p1, const void* p2);

void initMonsterInfoDB();

MonsterInfo* getMonsterInfoFromDB(Monster* m);
