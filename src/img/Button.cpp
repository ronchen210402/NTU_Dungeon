#include "Button.h"

Button::BottonStatus Button::checkButtonStatus(SDL_Event* e) {
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN ||
        e->type == SDL_MOUSEBUTTONUP) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        bool inside = true;

        if (x < _buttonRect.x)
            inside = false;
        else if (x > _buttonRect.x + _buttonRect.w)
            inside = false;
        else if (y < _buttonRect.y)
            inside = false;
        else if (y > _buttonRect.y + _buttonRect.h)
            inside = false;

        if (!inside) return MOUSE_OUT;

        // Set mouse over sprite
        switch (e->type) {
            case SDL_MOUSEMOTION:
                return MOUSE_HOVER;
            case SDL_MOUSEBUTTONDOWN:
                return MOUSE_DOWN;
            case SDL_MOUSEBUTTONUP:
                return MOUSE_UP;
            default:
                return MOUSE_TOTAL;
        }
    }

    return MOUSE_TOTAL;
}

void Button::handleEvent(SDL_Event* e) {
    if (!_active) return;
    BottonStatus status = checkButtonStatus(e);
    switch (status) {
        case MOUSE_UP:
            upExecution();
            break;
        case MOUSE_DOWN:
            downExecution();
            break;
        case MOUSE_HOVER:
            hoverExecution();
            break;
        case MOUSE_OUT:
            outExecution();
            break;
        default:
            break;
    }
}

void Button::outExecution() { setColor(255, 255, 255); }

void Button::hoverExecution() { setColor(128, 128, 128); }

void Button::upExecution() { setColor(255, 255, 255); }

void Button::downExecution() { setColor(64, 64, 64); }

//------------------------------------------------------------
//      SCENE CHANGE BUTTON
//------------------------------------------------------------
void SceneChange_Button::upExecution() {
    printf("Switching scene!\n  Destination: %s\n", _destName.c_str());
    // call the SceneMgr change the scene
    notify(new SceneChangeMsg(_destName));  // notify Scene
    setColor(255, 255, 255);
}

SceneChange_Button& SceneChange_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "dest"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                         j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                   j["rect"][2].get<int>(), j["rect"][3].get<int>());

    _destName = j["dest"].get<std::string>();

    //notify(new EstablishSceneLinkMsg(dest, this));

    return *this;
}

//------------------------------------------------------------
//      DIALOG BUTTON
//------------------------------------------------------------
void Dialog_Button::subscribe(Subject* o) { o->add(this); }
void Dialog_Button::unsubscribe(Subject* o) { o->remove(this); }
void Dialog_Button::update(EventMsg* e) {
    auto ptr = dynamic_cast<ChangeDialogMsg*>(e);
    if (ptr != NULL) {
        notify(new RequestDialogMsg(e->msg, this));
    }
}

Dialog_Button& Dialog_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "DialogBox"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                         j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                   j["rect"][2].get<int>(), j["rect"][3].get<int>());
    std::string s = j["DialogBox"].get<std::string>();
    notify(new RequestDialogMsg(s, this));
    return *this;
}

//------------------------------------------------------------
//      OPEN BUTTON
//------------------------------------------------------------
Open_Button& Open_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "OpenScene", "DeactivateBelow"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                         j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                   j["rect"][2].get<int>(), j["rect"][3].get<int>());
    _openScene = j["OpenScene"].get<std::string>();
    _deactivateBelow = j["DeactivateBelow"].get<bool>();
    return *this;
}

void Open_Button::upExecution() {
    std::cout << "[OPEN BUTTON] Notify!" << std::endl;
    notify(new OpenSceneMsg(_openScene, _deactivateBelow));
    setColor(255, 255, 255);
}

//------------------------------------------------------------
//      CLOSE BUTTON
//------------------------------------------------------------
Close_Button& Close_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "ActivateAll"}, j)) 
        exit(-1);
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                         j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                   j["rect"][2].get<int>(), j["rect"][3].get<int>());
    _activateAll = j["ActivateAll"].get<bool>();
    return *this;
}

void Close_Button::upExecution() {
    notify(new CloseSceneMsg(_activateAll));
    setColor(255, 255, 255);
}

//------------------------------------------------------------
//      PAGETURN BUTTON
//------------------------------------------------------------
PageTurn_Button& PageTurn_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "Jump"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                            j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                    j["rect"][2].get<int>(), j["rect"][3].get<int>());
    _jump = j["Jump"].get<int>();
    
    return *this;
}

void PageTurn_Button::upExecution() {
    notify(new PageTurnMsg(_jump));
    setColor(255, 255, 255);
}

//------------------------------------------------------------
//      ITEM BUTTON
//------------------------------------------------------------
bool Item_Button::_inPuzzle = false;
Item_Button& Item_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());

    _infoImage.loadImage(j["Info"].get<std::string>());
    _infoImage.set_imageRect(ITEM_INFO_X, ITEM_INFO_Y);
    /* std::string info = j["Info"].get<std::string>();
    int nTexts = strlen(info.c_str());
    if(nTexts == 0) return *this;
    int textIdx = 0;
    int lineIdx = 0;
    std::string temp;

    _infoImage.resize((nTexts - 1) / (ITEM_INFO_TEXT_WIDTH * 3) + 1);
    while (nTexts - textIdx > ITEM_INFO_TEXT_WIDTH * 3) {
        temp = info.substr(textIdx, ITEM_INFO_TEXT_WIDTH * 3);
        std::cout << temp << std::endl;
        _infoImage[lineIdx].loadImage(temp);
        _infoImage[lineIdx].set_imageRect(ITEM_INFO_X, ITEM_INFO_Y + LINE_HEIGHT*lineIdx);

        ++lineIdx;
        textIdx += ITEM_INFO_TEXT_WIDTH * 3;
    }
    temp = info.substr(textIdx, nTexts - textIdx);
    std::cout << temp << std::endl;
    _infoImage[lineIdx].loadImage(temp); */

    return *this;
}

void Item_Button::render(double angle, SDL_Point* center, SDL_RendererFlip flip){
    Image::render(angle, center, flip);
    if(_infoActivated){
        _infoImage.render();
        /* for(auto iter : _infoImage){
            std::cout << iter.textureWidth() << std::endl;
            iter.render(); 
        } */
    }
}

void Item_Button::hoverExecution() {
    _infoActivated = true;
    setColor(128, 128, 128);
    // notify(new MessageActiveMsg(true));
}

void Item_Button::upExecution() {
    // notify(new PageTurnMsg(_jump));
    //TODO: check answer
    setColor(255, 255, 255);
}

void Item_Button::outExecution() {
    _infoActivated = false;
    setColor(255, 255, 255);
    // notify(new MessageActiveMsg(false));
}
//------------------------------------------------------------
//      SAVE BUTTON
//------------------------------------------------------------

Save_Button& Save_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "FilePath"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                            j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                    j["rect"][2].get<int>(), j["rect"][3].get<int>());
    _path = j["FilePath"].get<std::string>();
    // notify(new LetMeSubscribeMsg(this));
    return *this;
}

void Save_Button::upExecution() {
    // TODO: 
    // 1. overwrite last scene to the current scene
    // 2. write the _file to json file
    if (!keysExist({"LastScene", "Scenes"}, **savMgr))
        exit(-1);
    notify(new RequestSceneNameMsg());
    setColor(255, 255, 255);
}

void Save_Button::update(EventMsg* e) {
    auto ptr = dynamic_cast<ReplySceneNameMsg*>(e);
    if (ptr != NULL) {
        std::cout << (**savMgr)["LastScene"] << std::endl;
        (**savMgr)["LastScene"] = e->msg;
        std::cout << (**savMgr)["LastScene"] << std::endl;
        savMgr->saveFile(_path);
    }
}

//------------------------------------------------------------
//      LOAD BUTTON
//------------------------------------------------------------

Load_Button& Load_Button::operator=(const json& j) {
    if (!keysExist({"ImagePath", "rect", "FilePath"}, j)) {
        exit(-1);
    }
    loadImage(j["ImagePath"].get<std::string>());
    Image::set_imageRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                            j["rect"][2].get<int>(), j["rect"][3].get<int>());
    set_buttonRect(j["rect"][0].get<int>(), j["rect"][1].get<int>(),
                    j["rect"][2].get<int>(), j["rect"][3].get<int>());
    _path = j["FilePath"].get<std::string>();

    return *this;
}

void Load_Button::upExecution() {
    // TODO: 
    // 1. let savMgr read from save file
    // 2. assert there are "LastScene", "Scenes".
    // 3. Scene change to "last scene"
    setColor(255, 255, 255);
    savMgr->loadFile(_path);
    if (!keysExist({"LastScene", "Scenes"}, **savMgr))
        exit(-1);
    notify(new SceneChangeMsg((**savMgr)["LastScene"].get<std::string>()));
    notify(new OpenSceneMsg("LittleTool", false));
    
}