/*************************************************************************************************************
 * FileName     [ Scene.h ]
 * PackageName  [ img ]
 * Synopsis     [ define Scene ]
 * Author       [ Yu-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/
#ifndef SCENE_H
#define SCENE_H

#include "observe.h"

#include "Button.h"
#include "DialogBox.h"
#include "json.hpp"

#include "unordered_map"

extern SavMgr* savMgr;
extern SavMgr* attrMgr;

using json = nlohmann::json;

class Scene : public Image, public Observer, public Subject {
    friend class SceneMgr;

public:
    Scene() : _loaded(false), _active(true) {}
    virtual ~Scene() { clear(); }

    bool dialogActive() {
        for (auto box : _dialogBoxes) {
            if (box.second->active()) return true;
        }

        return false;
    }
    virtual void handleEvent(SDL_Event* e) {
        if (!_active) return;
        if (!dialogActive()) {
            for (auto button : _buttons) 
                button.second->handleEvent(e);
        }
        for (auto box : _dialogBoxes) box.second->handleEvent(e);
    }

    virtual void render(double angle = 0.0, SDL_Point* center = NULL,
                SDL_RendererFlip flip = SDL_FLIP_NONE) {
        Image::render(angle, center, flip);
        for (auto button : _buttons) button.second->render(angle, center, flip);
        for (auto box : _dialogBoxes) box.second->render(angle, center, flip);
    }

    void clear() {
        for (auto button : _buttons) {
            if (button.second != NULL) delete button.second;
        }
        for (auto box : _dialogBoxes) {
            if (box.second != NULL) delete box.second;
        }
        _buttons.clear();
        _dialogBoxes.clear();
        _loaded = false;
    }

    virtual bool loaded() { return _loaded; }

    DialogBox* getDialog(const std::string& s);
    //-------------------------------------------------
    // load functions
    //-------------------------------------------------
    virtual bool load(const json&, const json&);
    virtual bool loadButtons(const json&, const json&);
    virtual bool loadDialogBoxes(const json&);
    //-------------------------------------------------
    // inherit obsever function
    //-------------------------------------------------
    virtual void subscribe(Subject* o);
    virtual void unsubscribe(Subject* o);
    virtual void update(EventMsg* e);

    virtual void activate() {
        _active = true;
        for (auto button: _buttons) {
            button.second->activate();
        }
        setColor(255, 255, 255);
    }
    void deactivate() {
        _active = false;
        setColor(128, 128, 128);
    }
    bool active() {
        return _active;
    }
    const std::string& name() const {
        return _name;
    }

protected:
    bool _loaded;
    bool _active;
    std::string _name;
    std::unordered_map<std::string, Button*> _buttons;
    std::unordered_map<std::string, DialogBox*> _dialogBoxes;

    // helper functions
    bool loadDialogButtons(const json&);
    bool loadSceneChangeButtons(const json&);
    bool loadOpenButtons(const json&);
    bool loadCloseButtons(const json&);
};


#endif  // SCENE_H