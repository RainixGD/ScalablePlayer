#include "./includes.h"

using namespace cocos2d;
using namespace gd;

float scale = 0.4;
CCLabelBMFont* label;
Slider* slider;

void(__thiscall* playlayerUpdate)(gd::PlayLayer* self, float dt);
void __fastcall playlayerUpdateH(gd::PlayLayer* self, void*, float dt) {
    self->m_player1->setRScale(scale * (MBO(bool, self->m_player1, 0x644) ? 0.6 : 1));
    self->m_player1->setRScaleY(scale * (MBO(bool, self->m_player1, 0x63E) ? -1 : 1) * (MBO(bool, self->m_player1, 0x644) ? 0.6 : 1));
    self->m_player1->m_objectSize = ccp(30 * scale, 30 * scale);
    if (MBO(bool, self->m_player1, 0x63B)) self->m_player1->m_objectSize = ccp(10 * scale, 10 * scale);
    MBO(float, self->m_player1, 0x698) = 30 * scale;

    self->m_player2->setRScale(scale * (MBO(bool, self->m_player2, 0x644) ? 0.6 : 1));
    self->m_player2->setRScaleY(scale * (MBO(bool, self->m_player2, 0x63E) ? -1 : 1) * (MBO(bool, self->m_player2, 0x644) ? 0.6 : 1));
    self->m_player2->m_objectSize = ccp(30 * scale, 30 * scale);
    MBO(float, self->m_player2, 0x698) = 30 * scale;
    if (MBO(bool, self->m_player2, 0x63B)) self->m_player2->m_objectSize = ccp(10 * scale, 10 * scale);
    playlayerUpdate(self, dt);
}

std::string formatFloat(float value, int precision) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

class abobus {
public:
    void onClick(CCObject*) {
        scale = slider->getValue();
        label->setString(formatFloat(scale, 2).c_str());
    }
};

bool(__thiscall* PauseLayerInit)(CCLayer* self);
bool __fastcall PauseLayerInitH(CCLayer* self, void*) {
    auto size = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setPosition({ size.width - 70, size.height - 50 });
    self->addChild(menu);

    label = CCLabelBMFont::create(formatFloat(scale, 2).c_str(), "bigFont.fnt");
    menu->addChild(label);

    slider = Slider::create(self, menu_selector(abobus::onClick), 0.5);
    slider->setValue(scale);
    slider->updateBar();
    slider->setPositionY(-20);
    menu->addChild(slider);


    PauseLayerInit(self);
    return 1;
}


DWORD WINAPI thread_func(void* hModule) {
    MH_Initialize();
    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x1E4620),
        reinterpret_cast<void*>(&PauseLayerInitH),
        reinterpret_cast<void**>(&PauseLayerInit)
    );

    MH_CreateHook(
        reinterpret_cast<void*>(base + 0x2029C0),
        reinterpret_cast<void*>(&playlayerUpdateH),
        reinterpret_cast<void**>(&playlayerUpdate)
    );
    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        auto h = CreateThread(0, 0, thread_func, handle, 0, 0);
        if (h)
            CloseHandle(h);
        else
            return FALSE;
    }
    return TRUE;
}