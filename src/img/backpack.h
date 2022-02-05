#ifndef BACKPACK_H
#define BACKPACK_H

#include <string>
#include <vector>
#include "Scene.h"
#include "savMgr.h"

const size_t ITEM_PER_WIDTH  = 3;
const size_t ITEM_PER_HEIGHT = 3;
const int    PAGE_X          = 182;
const int    PAGE_Y          = 150;
const int    PAGE_WIDTH      = 450;
const int    PAGE_HEIGHT     = 450;
const size_t ITEM_PER_PAGE   = ITEM_PER_WIDTH * ITEM_PER_HEIGHT;
const int    ITEM_WIDTH      = PAGE_WIDTH / ITEM_PER_WIDTH;
const int    ITEM_HEIGHT     = PAGE_HEIGHT / ITEM_PER_HEIGHT;

extern SavMgr* savMgr;

class Backpack: public Scene {
public:
    Backpack(size_t it = ITEM_PER_PAGE): Scene(), _messageActive(false), _currPage(0), _itemsPerPage(it) {
        _itemNameList.reserve(10);
    }
    ~Backpack() {}
    void pageTurn(int jump) {
        std::cout << "[PAGETURN] " << _currPage << "-->";
        _currPage += jump;
        if (_currPage >= maxPage()) _currPage = maxPage() - 1;
        if (_currPage < 0) _currPage = 0;
        std::cout << _currPage << std::endl;
    }

    virtual void activate() {
        _active = true;
        getItemList();
        Item_Button::setInPuzzle(false);
    }

    virtual void update(EventMsg*);
    /* virtual bool loaded() {
        if (!_loaded) getItemList();
        return _loaded;
    } */

    virtual bool load(const json& attr, const json& sav) {
        if (!Scene::load(attr, sav)) return false;
        // if(!loadMessageBackground(attr)) return false;

        return true;
    }
    virtual bool loadButtons(const json&, const json&);

    bool loadMessageBackground(const json& attr){
        if (!keysExist({"DialogImage"}, attr))
            exit(-1);

        if (!_messageBackground.loadImage(attr["DialogImage"].get<std::string>()))
            return false;

        return true;
    }

    virtual void handleEvent(SDL_Event* e) {
        if (!_active) return;
        if (!dialogActive()) {
            size_t itemIdx = 0;
            for (auto button : _itemButtonList){
                if(ITEM_PER_PAGE*_currPage <= itemIdx && itemIdx < ITEM_PER_PAGE*(_currPage + 1))
                    if(button.second != NULL && button.second->is_Activated())
                        button.second->handleEvent(e);
                ++itemIdx;
            }
            for(auto button : _buttons){
                    button.second->handleEvent(e);
            }
        }
        for (auto box : _dialogBoxes) box.second->handleEvent(e);
    }
    virtual void render(double angle = 0.0, SDL_Point* center = NULL,
                SDL_RendererFlip flip = SDL_FLIP_NONE) {
        Image::render(angle, center, flip);
        /* if(_messageActive)
            _messageBackground.render(); */

        size_t itemIdx = 0;
        for (auto button : _itemButtonList){
            if(ITEM_PER_PAGE*_currPage <= itemIdx && itemIdx < ITEM_PER_PAGE*(_currPage + 1))
                if(button.second != NULL && button.second->is_Activated())
                    button.second->render(angle, center, flip);
            ++itemIdx;
        }
        for(auto button : _buttons){
                button.second->render(angle, center, flip);
        }
        for (auto box : _dialogBoxes) box.second->render(angle, center, flip);
    }

protected:
    // data members
    bool _active;
    bool _messageActive;
    long _currPage;
    long _itemsPerPage;
    Image _messageBackground;
    std::vector<std::string> _itemNameList;
    std::unordered_map<std::string, Item_Button*> _itemButtonList;

    // helper functions
    long maxPage() { return (_itemNameList.size() - 1)/_itemsPerPage + 1; }

    void getItemList() {
        if (!aKeyExists("Scenes", (**savMgr))) exit(-1);
        if (!aKeyExists("Backpack", (**savMgr)["Scenes"])) exit(-1);
        if (!aKeyExists("Buttons", (**savMgr)["Scenes"]["Backpack"])) exit(-1);
        if (!aKeyExists("Item", (**savMgr)["Scenes"]["Backpack"]["Buttons"])) exit(-1);

        _itemNameList.clear();
        json itemList = (**savMgr)["Scenes"]["Backpack"]["Buttons"]["Item"];
        for (auto& item: itemList.items()) {
            if (item.value()["taken"].get<bool>() == true) {
                // std::cout << item.key() << std::endl;
                _itemNameList.push_back(item.key());
            }
        }

        // set Item's position
        size_t x = 0, y = 0;
        for(auto iter : _itemButtonList){
            if(itemList[iter.first]["taken"].get<bool>() == false)
                iter.second->setItemActivate(false);
            else{
                iter.second->setItemActivate(true);
                iter.second->set_Rect(PAGE_X + x*ITEM_WIDTH, \
                                    PAGE_Y + y*ITEM_HEIGHT, \
                                    ITEM_WIDTH, ITEM_HEIGHT);
                ++x;
                if(x >= ITEM_PER_WIDTH) x = 0, ++y;
                if(y >= ITEM_PER_HEIGHT) y = 0;
            }
        }
    }
};

#endif  // BACKPACK H