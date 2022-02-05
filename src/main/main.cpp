/*************************************************************************************************************
 * FileName     [ main ]
 * PackageName  [ main ]
 * Synopsis     [ Initialize and start the game ]
 * Author       [ Yu-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/
#include "Scene.h"
#include "sceneMgr.h"
#include <stdio.h>
#include "json.hpp"
#include "savMgr.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font *gFont = NULL;

extern SavMgr* savMgr;
extern SavMgr* attrMgr;
SceneMgr* sceneMgr;

bool initWindow(){
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ){
		printf( "Warning: Linear texture filtering not enabled!" );
	}

	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( gWindow == NULL ){
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if( gRenderer == NULL ){
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) ){
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return false;
	}

	if( TTF_Init() == -1 ){
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError() );
		return false;
	}

	gFont = TTF_OpenFont( "TTF/moonp.ttf", FONT_SIZE );
	if( gFont == NULL ){
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", SDL_GetError() );
		return false;
	}

	return true;
}

bool initSav() {
	savMgr = new SavMgr();
	attrMgr = new SavMgr();

	if (!attrMgr->loadFile(".save/attributes.json")) {
		delete attrMgr; attrMgr = 0;
		printf("Failed in reading attributes!\n");
		return false;
	}

	printf("****************\n");
	printf("*  attributes  *\n");
	printf("****************\n");
	attrMgr->print();

	/* std::string filePath;
	std::cout << "Please enter the path to the save file: \nload> " << std::flush;
	std::cin >> filePath; */
	if (!savMgr->loadFile(".save/save1.json")) {
		delete savMgr; savMgr = 0;
		printf("Failed in reading savefile!\n");
		return false;
	}

	printf("****************\n");
	printf("*  save file   *\n");
	printf("****************\n");
	savMgr->print();

	printf("Load success!\n");

	return true;
}

bool initSceneMgr() {
	sceneMgr = new SceneMgr(20, attrMgr);
	// test
	sceneMgr->listScenes();
	// Scene* s;
	return true;
}

bool loadMedia(){
	auto onStart = (**attrMgr)["LoadOnInit"]["OnStart"].get<std::vector<std::string>>();
	auto scenes = (**attrMgr)["LoadOnInit"]["Scenes"].get<std::vector<std::string>>();
	
	for (auto scene: onStart) {
		sceneMgr->refer(scene);
	}
	for (auto scene: scenes) {
		sceneMgr->refer(scene);
	}
	sceneMgr->setCurrentScene(onStart);
	sceneMgr->activateAll();

	return true;
}

void close(){
	TTF_CloseFont(gFont);
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	gFont = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char** argv){
	printf("**************************************\n");
	printf("*      Welcome to NTU Dungeon!!!     *\n");
	printf("*      Author: Yu-Jung Chen ,        *\n");
	printf("*              Mu-Te   Lau  ,        *\n");
	printf("*              Pu-Jia  Hsieh,        *\n");
	printf("**************************************\n");

    if ( !initSav() ) {
		printf( "Failed to init save managers!!!\n" );
		return -1;
	}
	if( !initSceneMgr() ) {
		printf( "Failed to init sceneMgr!!!\n" );
		return -1;		
	}
	if( !initWindow() ){
		printf( "Failed to initialize game screen!\n" );
		close();
        return -1;
	}
    if( !loadMedia() ){
        printf( "Failed to load media!\n" );
		close();
        return -2;
    }

    bool quit = false;
    SDL_Event e;
	
    while( !quit ){
        if( SDL_PollEvent( &e ) != 0 ){
            if( e.type == SDL_QUIT )
                quit = true;
            sceneMgr->handleEvent(&e);

			// get mouse position
			if(e.type == SDL_MOUSEBUTTONUP){
				int x, y;
				SDL_GetMouseState(&x, &y);
				std::cout << "x = " << x << ", y = " << y << std::endl;
			}
        }

        SDL_SetRenderDrawColor( gRenderer, 0, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        sceneMgr->render();

        //Update screen
        SDL_RenderPresent( gRenderer );
    }

	close();

    return 0;
}