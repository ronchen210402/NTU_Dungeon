/*************************************************************************************************************
 * FileName     [ Scene.h ]
 * PackageName  [ img ]
 * Synopsis     [ define Scene ]
 * Author       [ Yu-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/

#include "Scene.h"

DialogBox* Scene::getDialog(const std::string& s) { return _dialogBoxes[s]; }

//-------------------------------------------------
// load functions
//-------------------------------------------------
//-------------------------------------------------
// load Buttons
//-------------------------------------------------

bool Scene::load(const json& attr, const json& sav) {
    if (!keysExist({"Name", "ImagePath", "Buttons"}, attr)) exit(-1);

    if (!(loadDialogBoxes(attr) && loadButtons(attr, sav))) return false;
    if (!loadImage(attr["ImagePath"].get<std::string>())) return false;
    _name = attr["Name"].get<std::string>();
    _loaded = true;
    return true;
}

bool Scene::loadButtons(const json& attr, const json& sav) {
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
                subscribe(tmp);
                *tmp = bu.value();
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
                if (type.key() == "Dialog")
                    tmp = new Dialog_Button();
                else if (type.key() == "Item")
                    tmp = new Item_Button();

                if (tmp != NULL) _buttons[bu.key()] = tmp;
                subscribe(tmp);
                *tmp = bu.value();
            }

            std::cout << "  Loaded " << type.key() << " buttons! " << std::endl;
        }
    }
    std::cout << "Loaded Buttons! " << std::endl << std::endl;
    return true;
}

//-------------------------------------------------
// load Dialogs
//-------------------------------------------------

bool Scene::loadDialogBoxes(const json& j) {
    std::cout << "Loading dialog boxes... " << std::endl;

    if (j.find("DialogBoxes") == j.end()) {
        std::cout << "[NOTE] No dialog box... " << std::endl << std::endl;
        return true;
    }

    for (auto& box : j["DialogBoxes"].items()) {
        std::cout << "  Loading " << box.key() << std::endl;
        DialogBox* tmp = new DialogBox(box.value());
        _dialogBoxes[box.key()] = tmp;
    }

    std::cout << "Loaded dialog boxes! " << std::endl
              << "Total No.: " << _dialogBoxes.size() << std::endl;

    return true;
}

//-------------------------------------------------
// inherit obsever function
//-------------------------------------------------
void Scene::subscribe(Subject* o) {
    std::cout << "[SCENE] subscribed!" << std::endl;
    o->add(this);
}
void Scene::unsubscribe(Subject* o) { o->remove(this); }
void Scene::update(EventMsg* e) {
    // std::cout << "GENERAL SCENES" << std::endl;
    auto type = e->type();
    if (type == REQUEST_DIALOG_MSG) {
        auto p = dynamic_cast<RequestDialogMsg*>(e);
        if (p != NULL) {
            Dialog_Button* d = (Dialog_Button*)p->ptr;
            auto it = _dialogBoxes.find(p->msg);
            if (it == _dialogBoxes.end()) {
                std::cerr << "Error: Cannot find the requested dialog!" << std::endl;
            }
            d->setDialog(_dialogBoxes[p->msg]);
        }
    } else if (type == SCENE_CHANGE_MSG || type == OPEN_SCENE_MSG || 
               type == CLOSE_SCENE_MSG  || type == REQUEST_SCENE_NAME_MSG) {
        notify(e);  // notify SceneMgr
    }
}
