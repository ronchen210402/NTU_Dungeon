#include "Image.h"

//---------------------------------------------------------
//      Image
//---------------------------------------------------------
bool Image::loadImage(std::string path) {
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture* newTexture = NULL;
	
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
               IMG_GetError());
    } else {
        // Color key image
        // SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB(
        // loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

        // Create texoture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n",
                   path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    // Return success
    mTexture = newTexture;
    
	if (path == "img/General/transparent.png") {
		printf("[Note] transparent\n");
		setAlpha(0);
	}
	else setAlpha(255);
	
    if (_imageRect.x == 0 && _imageRect.y == 0 && _imageRect.w == 0 &&
        _imageRect.h == 0)
        set_imageRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    return mTexture != NULL;
}

void Image::render(double angle, SDL_Point* center, SDL_RendererFlip flip) {
    // Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, NULL, &_imageRect, angle, center,
                     flip);
}

//---------------------------------------------------------
//      MessageImage
//---------------------------------------------------------
bool MessageImage::loadImage(std::string message) {
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture* newTexture = NULL;
	
    // Load image at specified path
    SDL_Surface* tempSurface =
            TTF_RenderUTF8_Solid(gFont, message.c_str(), _textColor);
    if (tempSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n",
                TTF_GetError());
        return false;
    }
    else {
        // Color key image
        // SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB(
        // loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

        // Create texoture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
        if (newTexture == NULL) {
            printf(
                "Unable to create texture from rendered text! SDL Error: %s\n",
                SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(tempSurface);
    }

    // Return success
    mTexture = newTexture;
	
    set_imageRect(DIALOG_BOX_POS.x + 100, SCREEN_HEIGHT - LINE_HEIGHT*2,\
                  tempSurface->w, tempSurface->h);

    return mTexture != NULL;
}