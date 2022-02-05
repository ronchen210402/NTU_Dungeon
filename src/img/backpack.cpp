#include "backpack.h"

bool Backpack::loadButtons(const json& attr, const json& sav){
    printf("Loading Buttons...\n");
    if (attr.find("Buttons") != attr.end()) {
        for (auto& type : attr["Buttons"].items()) {
            std::cout << "  Loading " << type.key() << " buttons..." << std::endl;

            for (auto& bu : attr["Buttons"][type.key()].items()) {
                std::cout << "    Loading " << bu.key() << std::endl;

                Button* tmp = nullptr;
                if (type.key() == "SceneChange")
                    tmp = new SceneChange_Button();
                else if (type.key() == "Open")
                    tmp = new Open_Button();
                else if (type.key() == "Close")
                    tmp = new Close_Button();
                else if (type.key() == "PageTurn")
                    tmp = new PageTurn_Button();
                else if (type.key() == "Save") {
                    tmp = new Save_Button();
                    notify(new LetMeSubscribeMsg((Observer*)tmp));
                }
                else if (type.key() == "Load")
                    tmp = new Load_Button();

                if (tmp != NULL) _buttons[bu.key()] = tmp;
                *tmp = bu.value();
                subscribe(tmp);
            }

            std::cout << "  Loaded " << type.key() << " buttons! " << std::endl;
        }
    }
    if (sav.find("Buttons") != sav.end()) {
        for (auto& type : sav["Buttons"].items()) {
            std::cout << "  Loading " << type.key() << " buttons..." << std::endl;

            for (auto& bu : sav["Buttons"][type.key()].items()) {
                std::cout << "    Loading " << bu.key() << std::endl;

                Button* tmp = nullptr;
                if (type.key() == "Dialog"){
                    tmp = new Dialog_Button();
                    if (tmp != NULL) _buttons[bu.key()] = tmp;
                }
                else if (type.key() == "Item"){
                    tmp = new Item_Button();
                    if (tmp != NULL) _itemButtonList[bu.key()] = dynamic_cast<Item_Button*>(tmp);
                }

                *tmp = bu.value();
                subscribe(tmp);
            }

            std::cout << "  Loaded " << type.key() << " buttons! " << std::endl;
        }
    }
    std::cout << "Loaded Buttons! " << std::endl << std::endl;
    return true;
}

// observer function
void Backpack::update(EventMsg* e) {
    Scene::update(e);
    auto type = e->type();
    if (type == PAGE_TURN_MSG) {
        pageTurn((long)e->ptr);
    }
    else if (type == MESSAGE_ACTIVE_MSG) {
        if((bool)(e->ptr) == false)
            _messageActive = false;
        else
            _messageActive = true;
    }
}
