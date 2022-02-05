/*************************************************************************************************************
 * FileName     [ sceneMgr.h ]
 * PackageName  [ img ]
 * Synopsis     [ Define member functions of SceneMgr ]
 * Author       [ You-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/

#include "sceneMgr.h"
#include <iostream>
#include <typeinfo>
#include "backpack.h"

bool SceneMgr::setSceneList(SavMgr* s) {
    try {
        for (auto& item : (**s)["Scenes"].items()) {
            std::string str = item.key();
            if (_scenes.find(str) != _scenes.end()) {
                printf(
                    "Repeated scene \"%s\" found. Please refer to "
                    ".save/attributes.json.\n",
                    str.c_str());
                return false;
            }
            auto it = item.value().find("Type");
            if (it == item.value().end())
                _scenes[str] = new Scene;
            else {
                if (it->get<std::string>() == "Backpack") {
                    _scenes[str] = new Backpack;
                }
            }
            subscribe(_scenes[str]);
        }

    } catch (json::type_error& e) {
        std::cerr
            << "Type error in getting scene list. "
            << "Please check the tag \"scenes\" in \".save/attributes.json\". "
            << std::endl;
        return false;
    }
    return true;
}

bool SceneMgr::refer(const std::string& name) {
    std::unordered_map<std::string, Scene*>::iterator it = _scenes.find(name);
    if (it == _scenes.end()) {
        std::cerr
            << "The scene \"" << name << "\" does not exists in the list. "
            << std::endl
            << "Please check the tag \"scenes\" in \".save/attributes.json\". "
            << std::endl;
        return false;
    }
    // scene is not loaded yet
    std::list<std::string>::iterator cit = std::find(_cache.begin(), _cache.end(), name);
    if (!it->second->loaded()) {
        if (!loadScene(name)) {
            std::cerr << "Fail to load scene \"" << name << "\". " << std::endl
                      << "Please check the tag \"scenes\" in "
                         "\".save/attributes.json\". "
                      << std::endl;
            return false;
        }
        if (cit == _cache.end()) {
            if (_cache.size() >= _cacheSize && _cache.back() != "LittleTool") {
                std::string last = _cache.back();
                _cache.pop_back();
                _scenes[last]->clear();
                std::cout << "[TEST] removing  scene " << last << std::endl;
            }
            std::cout << "[TEST] inserting scene " << name << std::endl;
        }
    } else {
        _cache.erase(cit);
        std::cout << "[TEST] renewing scene " << name << std::endl;
    }

    _cache.push_front(name);

    return true;
}

bool SceneMgr::loadScene(const std::string& name) {
    Scene*& s = _scenes[name];
    printf("**********************************\n");
    printf("*[%s].......\n", name.c_str());
    printf("**********************************\n");
    json& attr = (**attrMgr)["Scenes"][name];
    json& sav = (**savMgr)["Scenes"][name];
    if (attr.is_null()) {
        std::cerr << "Missing attributes for the scene \"" << name << "\"!"
                  << std::endl;
    }

    if (sav.is_null()) {
        std::cerr << "Save file missing content for the scene \"" << name
                  << "\"!" << std::endl;
    }
    if (!s->load(attr, sav)) {
        return false;
    }
    return true;
}
void SceneMgr::subscribe(Subject* o) { o->add(this); }

void SceneMgr::unsubscribe(Subject* o) { o->remove(this); }
void SceneMgr::update(EventMsg* e) {
    auto type = e->type();
    if (type == SCENE_CHANGE_MSG) {
        auto ptr = dynamic_cast<SceneChangeMsg*>(e);
        if (ptr != NULL) {
            if (!refer(e->msg)) {
                std::cerr << "Invalid Scene Change Destination! " << std::endl;
                return;
            }
            setCurrentScene(e->msg, (size_t)e->ptr);
        }
    } else if (type == OPEN_SCENE_MSG) {
        auto ptr = dynamic_cast<OpenSceneMsg*>(e);
        if (ptr != NULL) {
            refer(e->msg);
            addLayer(e->msg, (bool)e->ptr);
        }
    } else if (type == CLOSE_SCENE_MSG) {
        auto ptr = dynamic_cast<CloseSceneMsg*>(e);
        if (ptr != NULL) {
            removeTopLayer(e->ptr);
        }
    } else if (type == REQUEST_SCENE_NAME_MSG) {
        auto ptr = dynamic_cast<RequestSceneNameMsg*>(e);
        if (ptr != NULL) {
            notify(new ReplySceneNameMsg(_currentScenes[0]->name()));
        }
    } else if (type == LET_ME_SUBSCRIBE_MSG) {
        auto ptr = dynamic_cast<LetMeSubscribeMsg*>(e);
        if (ptr != NULL) {
            Subject::add((Save_Button*)(e->ptr));
        }
    }
}