#ifndef BUTTON_H
#define BUTTON_H

#include <iostream>
#include <string>
#include "DialogBox.h"
#include "json.hpp"
#include "savMgr.h"

const int ITEM_INFO_X = 751;
const int ITEM_INFO_Y = 483;
const int ITEM_INFO_TEXT_WIDTH = 10;

extern SavMgr* savMgr;

class Scene;

class Button : public Image, public Subject {
public:
    enum BottonStatus {
        MOUSE_OUT,
        MOUSE_HOVER,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_TOTAL
    };

    Button() {
        _buttonRect = {0, 0, 0, 0};
        _active = true;
    }
    Button(int x, int y, int w, int h) {
        Image::set_imageRect(x, y, w, h);
        set_buttonRect(x, y, w, h);
        _active = true;
    }
    virtual ~Button() {}

    BottonStatus checkButtonStatus(SDL_Event* e);
    void handleEvent(SDL_Event* e);
    virtual void outExecution();
    virtual void hoverExecution();
    virtual void downExecution();
    virtual void upExecution();

    void set_Rect(int x, int y, int w, int h) {
        set_buttonRect(x, y, w, h);
        set_imageRect(x, y, w, h);
    }
    void set_buttonRect(int x, int y, int w, int h) {
        _buttonRect.x = x;
        _buttonRect.y = y;
        _buttonRect.w = w;
        _buttonRect.h = h;
    }
    int buttonWidth() { return _buttonRect.w; }
    int buttonHeight() { return _buttonRect.h; }
    int buttonX() { return _buttonRect.x; }
    int buttonY() { return _buttonRect.y; }

    virtual const std::string type() = 0;

    virtual Button& operator=(const json& j) {
        loadImage(j["ImagePath"].get<std::string>());
        Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                             j["rect"][2].get<int>(), j["rect"][3].get<int>());
        set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                       j["rect"][2].get<int>(), j["rect"][3].get<int>());
        return *this;
    }
    bool active() { return _active; }
    void activate() { _active = true; }
    void deactivate() { _active = false; }

protected:
    SDL_Rect _buttonRect;
    bool _active;
};

//------------------------------------------------------------
//      SCENE CHANGE BUTTON
//------------------------------------------------------------

class SceneChange_Button : public Button {
public:
    SceneChange_Button() : Button(), _destName("") {}
    SceneChange_Button(int x, int y, int w, int h, std::string d)
        : Button(), _destName(d) {
        set_Rect(x, y, w, h);
    }

    ~SceneChange_Button() {}

    void setDestination(std::string s) { _destName = s; }
    // void setCurrent(Scene** s) { _currentScene = s; }

    void upExecution();
    SceneChange_Button& operator=(const json&);

    const std::string type() { return "SceneChange"; }

private:
    std::string _destName;
};

//------------------------------------------------------------
//      DIALOG BUTTON
//------------------------------------------------------------

class Dialog_Button : public Button, public Observer {
public:
    Dialog_Button() : Button(), _dialogBox(NULL) {}
    Dialog_Button(int x, int y, int w, int h, DialogBox* d) : Button() {
        set_Rect(x, y, w, h);
        _dialogBox = NULL;
        setDialog(d);
    }
    ~Dialog_Button() {}

    void upExecution() {
        _dialogBox->activate();
        setColor(255, 255, 255);
    }

    void setDialog(DialogBox* newDialog) {
        // TO DO? change dialog
        if (_dialogBox != NULL) unsubscribe(_dialogBox);
        _dialogBox = newDialog;
        subscribe(_dialogBox);
    }

    Dialog_Button& operator=(const json&);

    const std::string type() { return "Dialog"; }

    //-------------------------------------------------
    // inherit obsever function
    //-------------------------------------------------

    void subscribe(Subject* o);
    void unsubscribe(Subject* o);
    void update(EventMsg* e);

private:
    std::string _dialogBoxName;
    DialogBox* _dialogBox;
};

//------------------------------------------------------------
//      OPEN BUTTON
//------------------------------------------------------------

class Open_Button : public Button {
public:
    Open_Button() : Button(), _deactivateBelow(true) {}
    ~Open_Button() {}

    void upExecution();
    Open_Button& operator=(const json&);
    const std::string type() { return "Open"; }

private:
    std::string _openScene;
    bool _deactivateBelow;
};

//------------------------------------------------------------
//      CLOSE BUTTON
//------------------------------------------------------------

class Close_Button : public Button {
public:
    Close_Button() : Button(), _activateAll(true) {}
    ~Close_Button() {}

    void upExecution();
    Close_Button& operator=(const json&);
    const std::string type() { return "Close"; }

private:
    bool _activateAll;
};

//------------------------------------------------------------
//      PAGETURN BUTTONS
//------------------------------------------------------------

class PageTurn_Button : public Button {
public:
    PageTurn_Button() : Button() {}
    ~PageTurn_Button() {}

    void upExecution();
    PageTurn_Button& operator=(const json&);
    const std::string type() { return "PageTurn"; }
private:
    int _jump;
};

//------------------------------------------------------------
//      ITEM BUTTONS
//------------------------------------------------------------

class Item_Button : public Button {
public:
    Item_Button() : Button(),_itemActivated(true) , _infoActivated(false) {}
    ~Item_Button() {
        if(_infoBackground != NULL) delete _infoBackground;
    }
    void render(double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void setItemActivate(bool a) { _itemActivated = a; }
    static void setInPuzzle(bool b) {_inPuzzle = b; }
    bool is_Activated() { return _itemActivated; }
    void hoverExecution();
    void outExecution();
    void upExecution();
    bool loadInfoImage();
    Item_Button& operator=(const json&);
    const std::string type() { return "Item"; }
private:
    static bool _inPuzzle;
    bool _itemActivated;
    bool _infoActivated;
    Image* _infoBackground;
    MessageImage _infoImage;
};

//------------------------------------------------------------
//      SAVE BUTTON
//------------------------------------------------------------

class Save_Button : public Button, public Observer {
public:
    Save_Button() : Button() {}
    ~Save_Button() {}
    void upExecution();
    Save_Button& operator=(const json&);
    const std::string type() { return "Save"; }

    // observer function
    void subscribe(Subject* o) {
        o->add(this);
    }
    void unsubscribe(Subject* o) {
        o->remove(this);
    }
    void update(EventMsg* e);
private:
    std::string _path;
};

//------------------------------------------------------------
//      LOAD BUTTON
//------------------------------------------------------------

class Load_Button : public Button {
public:
    Load_Button() : Button() {}
    ~Load_Button() {}
    void upExecution();
    Load_Button& operator=(const json&);
    const std::string type() { return "Load"; }
private:
    std::string _path;
};





#endif  // BUTTON_H