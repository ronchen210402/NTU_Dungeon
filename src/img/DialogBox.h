#ifndef DIALOGBOX_CPP
#define DIALOGBOX_CPP

#include <cstring>
#include <json.hpp>
#include <string>
#include <vector>
#include "Image.h"
#include "observe.h"
#include "savMgr.h"
using json = nlohmann::json; 
extern SavMgr* savMgr;

class DialogBox : public Image, public Subject {
public:
    DialogBox() : _sentenceIdx(-1), _active(false) {
        _textColor = {0, 0, 0, 255};
        _textRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        set_imageRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    DialogBox(const json& j)
        : _sentenceIdx(-1), _active(false) {
        _textColor = {0, 0, 0, 255};
        set_imageRect(DIALOG_BOX_POS);
        set_textRect(DIALOG_BOX_POS.x + 100, DIALOG_BOX_POS.y + 100, 0, 0);

        if(!keysExist({"SpokenBy", "Sentences", "Result", "ImagePath"}, j)) exit (-1);
        if(!keysExist({"Next", "Give"/* , "Puzzle" */}, j["Result"])) exit (-1);
        _spokenBy = j["SpokenBy"].get<std::string>();
        _sentences = j["Sentences"].get<std::vector<std::string>>();
        _next = j["Result"]["Next"].get<std::string>();
        _give = j["Result"]["Give"].get<std::string>();
        // _puzzle = j["Result"]["Puzzle"].get<std::string>();
        loadImage(j["ImagePath"].get<std::string>());
    }

    virtual ~DialogBox() {}

    virtual void handleEvent(SDL_Event* e) {
        if (!_active) return;

        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_RETURN ||
                e->key.keysym.sym == SDLK_SPACE)
                ++_sentenceIdx;
        } else if (e->type == SDL_MOUSEBUTTONUP)
            ++_sentenceIdx;

        if (_sentenceIdx == _sentences.size()) {
            _sentenceIdx = -1;
            _active = false;
            // TODO: Based on JSON file, execute one of the following event:
            // 1. Choose an item (activate puzzle)
            // 2. Change dialog
            if (_next.size() > 0) {
                auto tmp = new ChangeDialogMsg(_next); 
                notify(tmp); // Observed by: Dialog_Button
            }
            if (_give.size() > 0) {
                (**savMgr)["Scenes"]["Backpack"]["Buttons"]["Item"][_give]["taken"] = true;
            }
            // 3. Default: do nothing
        }
    }
    void render(double angle = 0.0, SDL_Point* center = NULL,
                SDL_RendererFlip flip = SDL_FLIP_NONE) {
        if (!_active) return;

        Image::render();

        if (_sentenceIdx >= _sentences.size()) return;

        SDL_Rect tempRect = {_textRect.x, 0, 0, 0};
        int nTexts = strlen(_sentences[_sentenceIdx].c_str());
        int textIdx = 0;
        int lineIdx = 0;
        std::string temp;

        while (nTexts - textIdx > TEXT_PER_WIDTH * 3) {
            temp =
                _sentences[_sentenceIdx].substr(textIdx, TEXT_PER_WIDTH * 3);
            renderline(temp, lineIdx, tempRect, angle, center, flip);

            ++lineIdx;
            textIdx += TEXT_PER_WIDTH * 3;
        }
        temp = _sentences[_sentenceIdx].substr(textIdx, nTexts - textIdx);
        renderline(temp, lineIdx, tempRect, angle, center, flip);
    }
    void renderline(std::string& temp, int lineIdx, SDL_Rect& tempRect,
                    double angle, SDL_Point* center, SDL_RendererFlip flip) {
        SDL_Surface* tempSurface =
            TTF_RenderUTF8_Solid(gFont, temp.c_str(), _textColor);
        if (tempSurface == NULL) {
            printf("Unable to render text surface! SDL_ttf Error: %s\n",
                   TTF_GetError());
            return;
        }

        tempRect.y = lineIdx * LINE_HEIGHT + _textRect.y;
        tempRect.w = tempSurface->w;
        tempRect.h = tempSurface->h;

        SDL_Texture* textTexture =
            SDL_CreateTextureFromSurface(gRenderer, tempSurface);
        if (textTexture == NULL) {
            printf(
                "Unable to create texture from rendered text! SDL Error: %s\n",
                SDL_GetError());
            SDL_FreeSurface(tempSurface);
            return;
        }
        SDL_RenderCopyEx(gRenderer, textTexture, NULL, &tempRect, angle, center,
                         flip);
        SDL_FreeSurface(tempSurface);
        SDL_DestroyTexture(textTexture);
    }

    void set_textRect(int x, int y, int w, int h) {
        _textRect.x = x;
        _textRect.y = y;
        _textRect.w = w;
        _textRect.h = h;
    }
    void set_textColor(int r, int g, int b, int a) {
        _textColor.r = r;
        _textColor.g = g;
        _textColor.b = b;
        _textColor.a = a;
    }
    void activate() { _active = true; }
    bool active() { return _active; }

private:
    // data member
    std::string _name;
    std::string _spokenBy;
    std::string _result;
    std::string _next;
    std::string _give;
    std::string _puzzle;
    std::vector<std::string> _sentences;

    size_t _sentenceIdx;
    SDL_Color _textColor;
    SDL_Rect _textRect;
    bool _active;
};



#endif  // DIALOGBOX_CPP