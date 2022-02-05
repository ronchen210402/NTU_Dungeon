/*************************************************************************************************************
 * FileName     [ sceneMgr.h ]
 * PackageName  [ img ]
 * Synopsis     [ Define manager object for scenes ]
 * Author       [ You-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/
#ifndef SCENE_MGR
#define SCENE_MGR

#include <algorithm>
#include <cstdlib>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include "DialogBox.h"
#include "Button.h"
#include "Scene.h"
#include "json.hpp"
#include "observe.h"
// #include "savMgr.h"

extern SavMgr* savMgr;
extern SavMgr* attrMgr;

using json = nlohmann::json;

class SceneMgr: public Observer, public Subject {
public:
    SceneMgr(size_t n, SavMgr* s, size_t c = 3)
        : _cacheSize(n) {
        _currentScenes.reserve(c);
        setSceneList(s);
    }

    // data access
    Scene*& operator[](std::string&& key) { 
        return _scenes[key];
    }

    Scene*& operator[](const std::string& key) {
        return _scenes[key];
    }

    std::vector<Scene*>& currentScene() { return _currentScenes; }

    void setCurrentScene(const std::string& s, size_t i = -1) { 
        if (i == (size_t)-1) {
            _currentScenes.pop_back();
            _currentScenes.push_back(_scenes[s]); 
        }
        else {
            _currentScenes[i] = _scenes[s];
        }
        _scenes[s]->activate();
    }
    void setCurrentScene(const std::vector<std::string>& layers) {
        for (auto layer: layers) {
            _currentScenes.push_back(_scenes[layer]);
            _scenes[layer]->activate();
        }
    }

    bool setSceneList(SavMgr*);
    bool refer(const std::string&);
    bool loadScene(const std::string&);

    // test functions
    void listScenes() {
        std::cout << "[SCENE LIST]" << std::endl;
        for (auto _scene : _scenes) {
            std::cout << _scene.first << " is at " << _scene.second
                      << std::endl;
        }
    }

    void activateAll() {
        for (auto layer: _currentScenes) {
            layer->activate();
        }
    }
    // bool loadUtilityButtons(const json&);
    void handleEvent(SDL_Event* e) { 
        for (auto layer: _currentScenes)
            if (layer != NULL && layer->active()) layer->handleEvent(e); 
    }

    void addLayer(const std::string& s, bool deactivate = true) {
        if (deactivate) {
            for (auto layer: _currentScenes) {
                layer->deactivate();
            }
        }
        _currentScenes.push_back(_scenes[s]);
        _currentScenes.back()->activate();
    }

    void removeTopLayer(bool activAll) {
        _currentScenes.pop_back();
        if (activAll) {
            activateAll();
        } else { // still need to make sure at least the top layer is active.s
            (_currentScenes.back())->activate(); 
        }
    }

    void render() {
        for (auto layer: _currentScenes)
            layer->render(); 
    }
    void subscribe(Subject* o);
    void unsubscribe(Subject* o);
    void update(EventMsg*);
    

private:
    const size_t _cacheSize;
    std::unordered_map<std::string, Scene*> _scenes;
    std::list<std::string> _cache;
    std::vector<Scene*> _currentScenes;
};

#endif  // SCENE_MGR

// TODO: ADD MULTILAYER SUPPORT
// 1. change _currentScene to a stack
// 2. disable all scenes except the top one (temporarily disable)
// 3. 