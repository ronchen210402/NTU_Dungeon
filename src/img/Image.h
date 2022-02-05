#ifndef IMAGE_H
#define IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 750;
const int FONT_SIZE = 35;
const int TEXT_PER_WIDTH = 18;
const int LINE_HEIGHT = 45;
const SDL_Rect DIALOG_BOX_POS = { 200, SCREEN_HEIGHT - 50 - SCREEN_HEIGHT / 2,\
                                  SCREEN_WIDTH - 400, SCREEN_HEIGHT / 2 };

//The window renderer
extern TTF_Font* gFont;
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
class MessageImage;

class Image{
    friend class MessageImage;
public:
    Image(){
        mTexture = NULL;
        _imageRect = {0, 0, 0, 0};
    }
    Image(int x, int y, int w, int h){
        mTexture = NULL;
        _imageRect = {x, y, w, h};
    }
    virtual ~Image() { free(); }

    virtual bool loadImage( std::string path );

    void free(){
        if( mTexture != NULL ){
            SDL_DestroyTexture( mTexture );
            mTexture = NULL;
            _imageRect = {0, 0, 0, 0};
        }
    }

    void setColor( Uint8 red, Uint8 green, Uint8 blue ){
	    SDL_SetTextureColorMod( mTexture, red, green, blue );
    }
    void setBlendMode( SDL_BlendMode blending ){
	    SDL_SetTextureBlendMode( mTexture, blending );
    }
    void setAlpha( Uint8 alpha ){
	    SDL_SetTextureAlphaMod( mTexture, alpha );
    }

    void set_imageRect(int x, int y, int w, int h){
        _imageRect.x = x; _imageRect.y = y;
        _imageRect.w = w; _imageRect.h = h;
    }
    void set_imageRect(int x, int y){
        _imageRect.x = x; _imageRect.y = y;
    }
    void set_imageRect(const SDL_Rect& rect){
        _imageRect = rect;
    }
    virtual void render( double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

    bool image_is_Loaded() { return mTexture != NULL; }
    int textureWidth() { return _imageRect.w; }
    int textureHeight() { return _imageRect.h; }

private:
    // The actual hardware texture
    SDL_Texture* mTexture;
    SDL_Rect _imageRect;
};

class MessageImage: public Image {
public:
    MessageImage(): Image() {
        _textColor = {0, 0, 0, 255};
    }
    virtual ~MessageImage() { free(); }

    virtual bool loadImage( std::string path );
private:
    SDL_Color _textColor;
};

#endif //IMAGE_H