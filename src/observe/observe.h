/*************************************************************************************************************
 * FileName     [ observe.h ]
 * PackageName  [ observe ]
 * Synopsis     [ Define abstract interface for observer pattern ]
 * Author       [ You-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/

#ifndef OBSERVE_H
#define OBSERVE_H

#include <unordered_set>
#include <algorithm>
#include <string>
#include <typeinfo>
#include <iostream>
class Observer;
class Subject;
struct EventMsg;
class Dialog_Button;
//------------------------------------------------------------
//      ABSTRACT MESSAGE TYPE
//------------------------------------------------------------

enum EventMsgType {
    CHANGE_DIALOG_MSG,
    REQUEST_DIALOG_MSG,
    SCENE_CHANGE_MSG,
    OPEN_SCENE_MSG,
    CLOSE_SCENE_MSG,
    PAGE_TURN_MSG,
    MESSAGE_ACTIVE_MSG,
    REQUEST_SCENE_NAME_MSG,
    REPLY_SCENE_NAME_MSG,
    LET_ME_SUBSCRIBE_MSG,
    GIVE_ITEM_MSG,
    TOTAL_MSG
};

struct EventMsg {
    EventMsg() {}
    EventMsg(std::string _msg, void* _ptr = NULL): msg(_msg), ptr(_ptr) {}
    virtual ~EventMsg() {}
    virtual EventMsgType type() = 0;

    std::string msg;
    void* ptr;
};

//------------------------------------------------------------
//      ENTITY MESSAGE TYPE
//------------------------------------------------------------

struct ChangeDialogMsg : public EventMsg {
    ChangeDialogMsg (std::string next): EventMsg(next) {}
    ~ChangeDialogMsg() {}
    EventMsgType type() { return CHANGE_DIALOG_MSG; }
};

struct RequestDialogMsg : public EventMsg {
    RequestDialogMsg(std::string what, Dialog_Button* d)
        : EventMsg(what, (void*)d) {}
    ~RequestDialogMsg() {}
    EventMsgType type() { return REQUEST_DIALOG_MSG; }
};

struct SceneChangeMsg : public EventMsg {
    SceneChangeMsg(std::string what, size_t layer = 0)
        : EventMsg(what, (void*)layer) {}
    ~SceneChangeMsg() {}
    EventMsgType type() { return SCENE_CHANGE_MSG; }
};

struct OpenSceneMsg : public EventMsg {
    OpenSceneMsg(std::string msg, bool deactivate)
        : EventMsg(msg, (void*)deactivate) {}
    ~OpenSceneMsg() {}
    EventMsgType type() { return OPEN_SCENE_MSG; }
};

struct CloseSceneMsg : public EventMsg {
    CloseSceneMsg(bool activateAll) : EventMsg("", (void*)activateAll) {}
    ~CloseSceneMsg() {}
    EventMsgType type() { return CLOSE_SCENE_MSG; }
};

struct PageTurnMsg : public EventMsg {
    PageTurnMsg(long jump) : EventMsg("", (void*)jump) {}
    ~PageTurnMsg() {}
    EventMsgType type() { return PAGE_TURN_MSG; }
};

struct MessageActiveMsg : public EventMsg {
    MessageActiveMsg(bool active) : EventMsg("", (void*)active) {}
    ~MessageActiveMsg() {}
    EventMsgType type() { return MESSAGE_ACTIVE_MSG; }
};

struct RequestSceneNameMsg : public EventMsg {
    RequestSceneNameMsg() : EventMsg("", NULL) {}
    ~RequestSceneNameMsg() {}
    EventMsgType type() { return REQUEST_SCENE_NAME_MSG; }
};

struct ReplySceneNameMsg : public EventMsg {
    ReplySceneNameMsg(std::string name) : EventMsg(name, NULL) {}
    ~ReplySceneNameMsg() {}
    EventMsgType type() { return REPLY_SCENE_NAME_MSG; }
};

struct LetMeSubscribeMsg : public EventMsg {
    LetMeSubscribeMsg(Observer* o) : EventMsg("", (void*)o) {}
    ~LetMeSubscribeMsg() {}
    EventMsgType type() { return LET_ME_SUBSCRIBE_MSG; }
};

struct GiveItemMsg : public EventMsg {
    GiveItemMsg(std::string itemName) : EventMsg(itemName, NULL) {}
    ~GiveItemMsg() {}
    EventMsgType type() { return GIVE_ITEM_MSG; }
};

//------------------------------------------------------------
//      OBSERVER
//------------------------------------------------------------

class Observer {
    public:
    Observer() {}
    virtual ~Observer() {}

    virtual void subscribe(Subject* o) = 0;

    virtual void unsubscribe(Subject* o) = 0;
    virtual void update(EventMsg* e) = 0;

    protected:

    // std::unordered_set<Observant*> subsribedList;

};

//------------------------------------------------------------
//      SUBJECT
//------------------------------------------------------------

class Subject {
    public:
    Subject() {}
    virtual ~Subject() {}

    virtual void add(Observer* o) {
        notifyList.insert(o);
    }

    virtual void remove(Observer* o) {
        auto it = std::find(notifyList.begin(), notifyList.end(), o);
        if (it != notifyList.end()) {
            notifyList.erase(it);
        }
    }

    virtual void notify(EventMsg* e) {
        for (auto& o: notifyList) {
            std::cout << typeid(*o).name() << std::endl;
            o->update(e);
        }
    }

    protected:
    std::unordered_set<Observer*> notifyList;
};



#endif // OBSERVE_H