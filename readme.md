# This project is a game engine base on SDL
## To Build
```shell
make
```

## To Run
```shell
./NTUDungeon
```

- 簡介：「台大地下城」是一款密室逃脫遊戲的開發引擎，可以根據不同的謎題設計，
創造不同全新的解迷遊戲
- 功能設計：
    - 場景：基本的畫面架構，畫面中會有許多可觸發物件
    - 物品：透過謎題或是在場景中找到，是觸發一些事件的依據
    - 背包：可以查看已獲得物品的內容
    - 地圖：傳送到不同的地方
    - 存檔：可以存取遊戲的進度
- 美工界面：
    - 根據台大不同的地方，手繪成合適的場景
    - 為了統一風格，手繪背包界面、各個物品
- 工作分配：
    - 畫面設計：謝菩家
    - 故事、程式設計：陳祐融、劉慕德、謝菩家

## 第二部分：
- `class Button`：畫面上每一個有作用的物件，都是一個按鈕，不同按鈕有不同功能（程式內容在 src/img/Button.h）
主要功能：
1. 判斷滑鼠在哪個位置、執行什麼動作，呼叫相對應的執行程式
2. 告知 Object 要做什麼動作（觀察者模式，詳見 B06507 027 劉慕德的報
告）
3. 具體要執行什麼，在子類別定義
```c++
class Button : public Image, public Subject {
public: 
void handleEvent(SDL_Event* e);
virtual void outExecution();
virtual void hoverExecution();
virtual void downExecution();
virtual void upExecution();
};
```

- `Class SceneChange_Button`: 繼承 Button。告訴告訴 Scene 這個 Object 要換頁了
```c++
class SceneChange_Button : public Button {
public: 
void setDestination(std: : string s) { _destName = s; }
void upExecution();
SceneChange_Button& operator= (const json&);
const std: : string type() { return "SceneChange"; }
};
```
- `class Dialog_Button`: 繼承 Button。告訴告訴 DialogBox 這個 Object 該執行了
```c++
class Dialog_Button : public Button, public Observer {
public: 
void upExecution();
void setDialog(DialogBox* newDialog);
Dialog_Button& operator= (const json&);
const std: : string type() { return "Dialog"; }
//-------------------------------------------------
// inherit obsever function 
//-------------------------------------------------
void subscribe(Subject* o);
void unsubscribe(Subject* o);
void update(EventMsg* e);
};
```
- `class Open_Button`: 繼承 Button。告訴告訴 SceneMgr 這個 Object 要增加某個圖層（Scene）
```c++
class Open_Button : public Button {
public: 
void upExecution();
Open_Button& operator= (const json&);
const std: : string type() { return "Open"; }
};
```
- `class Close_Button`: 繼承 Button。告訴告訴 SceneMgr 這個 Object 要刪掉某個圖層（Scene）
```c++
class Close_Button : public Button {
public: 
void upExecution();
Close_Button& operator= (const json&);
const std: : string type() { return "Close"; }
};
```
- `class PageTurn_Button`: 繼承 Button。告訴告訴 Backpack 這個 Object 要換頁了
```c++
class PageTurn_Button : public Button {
public: 
void upExecution();
PageTurn_Button& operator= (const json&);
const std: : string type() { return "PageTurn"; }
};
```

- `class Item_Button`: 繼承 Button。告訴
有兩種模式：
1. 在背包裡：滑鼠停在 Item 上面會顯示基本資訊
2. 在謎題中會通知謎題這個 Item 被選取了
```c++
class Item_Button : public Button {
public: 
void render(double angle = 0.0, SDL_Point* center = NULL, 
SDL_RendererFlip flip = SDL_FLIP_NONE);
void setItemActivate(bool a) { _itemActivated = a; }
static void setInPuzzle(bool b) { _inPuzzle = b; }
bool is_Activated() { return _itemActivated; }
void hoverExecution();
void outExecution();
void upExecution();
bool loadInfoImage();
Item_Button& operator= (const json&);
const std: : string type() { return "Item"; }
};
```
- `class Save_Button`: 繼承 Button。告訴把目前的遊戲狀態寫進 save1.json 裡面
```c++
class Save_Button : public Button, public Observer {
public: 
Save_Button& operator= (const json&);
const std: : string type() { return "Save"; }
// observer function 
void subscribe(Subject* o) { o->add(this); }
void unsubscribe(Subject* o) { o->remove(this); }
void update(EventMsg* e);
};
```
- `class Load_Button`: 繼承 Button。告訴從 save1.json 裡面讀去遊戲進度
```c++
class Load_Button : public Button {
public: 
void upExecution();
Load_Button& operator= (const json&);
const std: : string type() { return "Load"; }
};
```

