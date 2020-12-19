#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "monster.hpp"
#include "dmntcht.h"

#define MONSTER_POINTER_LIST_OFFSET 0x10C820AC
#define MHGU_TITLE_ID 0x0100770008DD8000
#define ASCII_ZH 0x687A

//Common
Thread t0;
bool threadexit = false;
uint64_t refresh_interval = 1;

//MHGU
MonsterPointerList mlist;
char Monster1_Name[32];
char Monster2_Name[32];
char Monster1_Info[32];
char Monster2_Info[32];
u64 heap_base = 0;
u64 mlistptr = 0;
Monster new_m1;
Monster new_m2;
Monster m;
u8 largecount = 0;

//  Chinese 1, English 0
u8 mname_lang = 1;

u8 mhgu_running = 0;

bool Atmosphere_present = false;

//check if mhgu game is running
void checkMHGURunning()
{

    u64 pid;
    u64 title_id;
    Result rc;
    rc = pmdmntGetApplicationProcessId(&pid);
    if (R_FAILED(rc)) {
        mhgu_running = 0;
        return;
    }
    rc = pminfoGetProgramId(&title_id, pid);
    if (R_FAILED(rc)) {
        mhgu_running = 0;
        return;
    }
    if (title_id == MHGU_TITLE_ID) {
        mhgu_running = 1;
    } else {
        mhgu_running = 0;
    }
}

// update monster info
void updateMonsterCache()
{
    dmntchtReadCheatProcessMemory(mlistptr, &mlist, sizeof mlist);
    u32 new_m1_ptr = 0;
    u32 new_m2_ptr = 0;
    u8 keep_m1 = 0;
    u8 keep_m2 = 0;
    u8 count = 0;
    MonsterInfo* new_m1_info = NULL;
    MonsterInfo* new_m2_info = NULL;
    //check all monsters, excluding small ones
    for (u8 i = 0; i < MAX_POINTERS_IN_LIST; i++)
    {
        if (!mlist.m[i]) continue;
        dmntchtReadCheatProcessMemory(mlist.m[i], &m, sizeof m);
        if (isSmallMonster(&m))
            continue;

        count += 1;
        MonsterInfo* m_info = getMonsterInfoFromDB(&m);
        if (mlist.m[i] == m_cache[0].mptr)
        {
            keep_m1 = 1;
            m_cache[0].hp = m.hp;
            m_cache[0].max_hp = m.max_hp;
            m_cache[0].name = mname_lang ? m_info->cn_name:m_info->name;

            for (u8 i = 0; i < 8; i++)
            {
                m_cache[0].p[i].max_stagger_hp = m.parts[i].stagger_hp;
                m_cache[0].p[i].max_break_hp = m.parts[i].break_hp;
            }
        }
        else if (mlist.m[i] == m_cache[1].mptr)
        {
            keep_m2 = 1;
            m_cache[1].hp = m.hp;
            m_cache[1].max_hp = m.max_hp;
            m_cache[1].name = mname_lang ? m_info->cn_name:m_info->name;

            for (u8 i = 0; i < 8; i++)
            {
                m_cache[1].p[i].max_stagger_hp = m.parts[i].stagger_hp;
                m_cache[1].p[i].max_break_hp = m.parts[i].break_hp;
            }
        }
        else if (new_m1_ptr == 0)
        {
            //save new monster pointer so we can add parts info later
            new_m1 = m;
            new_m1_ptr = mlist.m[i];
            new_m1_info = m_info;
        }
        else if (new_m2_ptr == 0)
        {
            new_m2 = m;
            new_m2_ptr = mlist.m[i];
            new_m2_info = m_info;
        }
    }

    //remove expired monster parts
    if (!keep_m1)
    {
        m_cache[0].mptr = 0;
        m_cache[0].hp = 0;
        m_cache[0].max_hp = 0;
        m_cache[0].name = NULL;
        for (u8 i = 0; i < 8; i++) {
            m_cache[0].p[i].max_stagger_hp = 0;
            m_cache[0].p[i].max_break_hp = 0;
        }
    }
    if (!keep_m2)
    {
        m_cache[1].mptr = 0;
        m_cache[1].hp = 0;
        m_cache[1].max_hp = 0;
        m_cache[1].name = NULL;
        for (u8 i = 0; i < 8; i++) {
            m_cache[1].p[i].max_stagger_hp = 0;
            m_cache[1].p[i].max_break_hp = 0;
        }
    }

    //add new monster stats
    //note: assume new_m2 will never be assigned before new_m1
    //note: only display parts that have more than 2 break_hp; for non-breakable parts it is typically negative but it can be fixed to 1 if there are special critereas involved
    if (new_m1_ptr)
    {
        if (!m_cache[0].mptr)
        {
            m_cache[0].mptr = new_m1_ptr;
            m_cache[0].hp = new_m1.hp;
            m_cache[0].max_hp = new_m1.max_hp;
            m_cache[0].name = mname_lang ? new_m1_info->cn_name:new_m1_info->name;

            for (u8 i = 0; i < 8; i++)
            {
                m_cache[0].p[i].max_stagger_hp = new_m1.parts[i].stagger_hp;
                m_cache[0].p[i].max_break_hp = new_m1.parts[i].break_hp;
            }
        }
        else
        {
            m_cache[1].mptr = new_m1_ptr;
            m_cache[1].hp = new_m1.hp;
            m_cache[1].max_hp = new_m1.max_hp;
            m_cache[1].name = mname_lang ? new_m1_info->cn_name:new_m1_info->name;

            for (u8 i = 0; i < 8; i++)
            {
                m_cache[1].p[i].max_stagger_hp = new_m1.parts[i].stagger_hp;
                m_cache[1].p[i].max_break_hp = new_m1.parts[i].break_hp;
            }
        }
    }
    if (new_m2_ptr)
    {
        if (!m_cache[0].mptr)
        {
            m_cache[0].mptr = new_m2_ptr;
            m_cache[0].hp = new_m2.hp;
            m_cache[0].max_hp = new_m2.max_hp;
            m_cache[0].name = mname_lang ? new_m2_info->cn_name:new_m2_info->name;

            for (u8 i = 0; i < 8; i++)
            {
                m_cache[0].p[i].max_stagger_hp = new_m2.parts[i].stagger_hp;
                m_cache[0].p[i].max_break_hp = new_m2.parts[i].break_hp;
            }
        }
        else
        {
            m_cache[1].mptr = new_m2_ptr;
            m_cache[1].hp = new_m2.hp;
            m_cache[1].max_hp = new_m2.max_hp;
            m_cache[1].name = mname_lang ? new_m2_info->cn_name:new_m2_info->name;

            for (u8 i = 0; i < 8; i++)
            {
                m_cache[1].p[i].max_stagger_hp = new_m2.parts[i].stagger_hp;
                m_cache[1].p[i].max_break_hp = new_m2.parts[i].break_hp;
            }
        }
    }

    // update large monster count
    largecount = count;
}

bool isServiceRunning(const char *serviceName) {
    Handle handle;
    SmServiceName service_name = smEncodeName(serviceName);
    if (R_FAILED(smRegisterService(&handle, service_name, false, 1))) return true;
    else {
        svcCloseHandle(handle);
        smUnregisterService(service_name);
        return false;
    }
}

void getMonsterInfo(void*) {
    initMonsterInfoDB();
    while (threadexit == false) {
        checkMHGURunning();
        if (mhgu_running) {
            DmntCheatProcessMetadata mhguProcessMetaData;
            dmntchtGetCheatProcessMetadata(&mhguProcessMetaData);
            heap_base = mhguProcessMetaData.heap_extents.base;
            mlistptr = heap_base + MONSTER_POINTER_LIST_OFFSET;
            updateMonsterCache();
        } else {
            heap_base = 0;
            mlistptr = 0;
        }
        //interval
        svcSleepThread(1'000'000'000 * refresh_interval);
    }
}

//Start
void StartThreads() {
    threadCreate(&t0, getMonsterInfo, NULL, NULL, 0xF00, 0x3F, -2);
    threadStart(&t0);
}

//End
void CloseThreads() {
    threadexit = true;
    threadWaitForExit(&t0);
    threadClose(&t0);
    threadexit = false;
}

class InfoOverlay : public tsl::Gui {
public:
    InfoOverlay() { }

    // Called when this Gui gets loaded to create the UI
    // Allocate all elements on the heap. libtesla will make sure to clean them up when not needed anymore
    virtual tsl::elm::Element* createUI() override {
        // A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
        // If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
        auto frame = new tsl::elm::OverlayFrame("", "");

        auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            renderer->drawRect(0, 420, tsl::cfg::FramebufferWidth - 150, 720, a(0x7111));
            if (mhgu_running) {
                if (largecount > 1) {
                    renderer->drawString(Monster1_Name, false, 35, 475, 30, renderer->a(0xFFFF));
                    renderer->drawString(Monster1_Info, false, 40, 515, 20, renderer->a(0xFFFF));
                    renderer->drawString(Monster2_Name, false, 35, 615, 30, renderer->a(0xFFFF));
                    renderer->drawString(Monster2_Info, false, 40, 655, 20, renderer->a(0xFFFF));
                } else if (largecount == 1) {
                    renderer->drawString(m_cache[0].mptr ? Monster1_Name:Monster2_Name, false, 35, 475, 30, renderer->a(0xFFFF));
                    renderer->drawString(m_cache[0].mptr ? Monster1_Info:Monster1_Info, false, 40, 515, 20, renderer->a(0xFFFF));
                } else {
                    renderer->drawString(mname_lang ? "    未发现\n\n   大型怪物":"NO LARGE\n\nMONSTERS\n\nFOUND", false, 60, 530, 30, renderer->a(0xFFFF));
                }

            } else {
                renderer->drawString(mname_lang ? "       未检测\n\n       到游戏":"MHGU IS NOT\n\n    RUNNING", false, 35, 550, 30, renderer->a(0xFFFF));
            }
        });

        // Add the list to the frame for it to be drawn
        frame->setContent(Status);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override {
        if (m_cache[0].mptr) {
            snprintf(Monster1_Name, sizeof Monster1_Name, "%s", m_cache[0].name);
            snprintf(Monster1_Info, sizeof Monster1_Info, "HP: %d/%d\n\nHP %%: %.2f%%", m_cache[0].hp, m_cache[0].max_hp, (float)m_cache[0].hp / (float)m_cache[0].max_hp * 100);
        }
        if (m_cache[1].mptr) {
            snprintf(Monster2_Name, sizeof Monster2_Name, "%s", m_cache[1].name);
            snprintf(Monster2_Info, sizeof Monster2_Info, "HP: %d/%d\n\nHP %%: %.2f%%", m_cache[1].hp, m_cache[1].max_hp, (float)m_cache[1].hp / (float)m_cache[1].max_hp * 100);
        }
    }

    // Called once every frame to handle inputs not handled by other UI elements
    virtual bool handleInput(u64 keysDown, u64 keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
        if ((keysHeld & HidNpadButton_StickL) && (keysHeld & HidNpadButton_StickR)) {
            CloseThreads();
            tsl::goBack();
            return true;
        }
        return false;
    }
};

//Main Menu
class MainMenu : public tsl::Gui {
public:
    MainMenu() { }

    virtual tsl::elm::Element* createUI() override {
        checkMHGURunning();

        auto rootFrame = new tsl::elm::OverlayFrame("MHGU-Monster-Info", APP_VERSION);
        auto list = new tsl::elm::List();

        auto en_info = new tsl::elm::ListItem("English");
        en_info->setClickListener([](uint64_t keys) {
            if (keys & HidNpadButton_A) {
                StartThreads();
                TeslaFPS = 1;
                alphabackground = 0x0;
                tsl::hlp::requestForeground(false);
                FullMode = false;
                refresh_interval = 1;
                mname_lang = 0;
                tsl::changeTo<InfoOverlay>();
                return true;
            }
            return false;
        });
        list->addItem(en_info);

        auto zh_info = new tsl::elm::ListItem("简体中文");
        zh_info->setClickListener([](uint64_t keys) {
            if (keys & HidNpadButton_A) {
                StartThreads();
                TeslaFPS = 1;
                alphabackground = 0x0;
                tsl::hlp::requestForeground(false);
                FullMode = false;
                refresh_interval = 1;
                mname_lang = 1;
                tsl::changeTo<InfoOverlay>();
                return true;
            }
            return false;
        });
        list->addItem(zh_info);

        list->addItem(new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            renderer->drawString("\uE016  Hold Left Stick & Right Stick to go back here.", false, x+10, y+30, 15, renderer->a(0xFFFF));
        }), 30);

        rootFrame->setContent(list);

        return rootFrame;
    }

    virtual void update() override {
        if (TeslaFPS != 60) {
            FullMode = true;
            tsl::hlp::requestForeground(true);
            TeslaFPS = 60;
            alphabackground = 0xD;
            refresh_interval = 1;
        }
    }
    virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
        if (keysHeld & HidNpadButton_B) {
            tsl::goBack();
            return true;
        }
        return false;
    }
};

class MonitorOverlay : public tsl::Overlay {
public:
                                             // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override {
        Atmosphere_present = isServiceRunning("dmnt:cht");
        if (Atmosphere_present == true) dmntchtInitialize();
        pminfoInitialize();
        setInitialize();
    }  // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {
        dmntchtExit();
        pminfoExit();
        setExit();
    }  // Callet at the end to clean up all services previously initialized

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<MainMenu>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv) {
    return tsl::loop<MonitorOverlay>(argc, argv);
}