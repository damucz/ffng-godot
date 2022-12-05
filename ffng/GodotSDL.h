#pragma once

#include "core/rid.h"
#include "scene/2d/node_2d.h"
#include "scene/resources/dynamic_font.h"
#include "core/list.h"
#include <string.h>      // NULL

extern Node2D* SDL_node;
extern int SDL_window_width;
extern int SDL_window_height;

typedef unsigned long long int Uint64;
typedef unsigned long int Uint32;
typedef unsigned short int Uint16;
typedef unsigned char Uint8;
typedef signed long long int Sint64;
typedef signed long int Sint32;
typedef signed short int Sint16;
typedef signed char Sint8;

class SDL_Rect {
public:
    SDL_Rect() : x(0), y(0), w(0), h(0) {}
    int x, y;
    int w, h;
};

class SDL_Color {
public:
    Uint8 r, g, b, unused;
};

class SDL_Surface {
public:
    enum Type {
        NONE,
        EMPTY,
        TEXTURE,
        VIEWPORT,
    };

    Type type = Type::NONE;
    int w, h;
    //int pitch;
    //SDL_PixelFormat* format;
    //Uint8* pixels;
    //Uint32 flags;

    // cache up to next frame
    List<Ref<Texture>> _textures;
    List<Ref<DynamicFont>> _fonts;

    // texture
    RID texture;

    // viewport, screen
    RID viewport;
    RID canvas;
    List<RID> canvas_items;

    RID _last_material;
};

extern List<SDL_Surface*> SDL_to_be_cleared;
extern List<SDL_Surface*> SDL_to_be_removed;
extern Dictionary SDL_shaders;

#define KMOD_NONE 0
#define KMOD_LALT 1
#define KMOD_RALT 2
#define KMOD_ALT (KMOD_LALT | KMOD_RALT)
#define KMOD_LSHIFT 4
#define KMOD_RSHIFT 8
#define KMOD_SHIFT (KMOD_LSHIFT | KMOD_RSHIFT)
#define KMOD_LCTRL 16
#define KMOD_RCTRL 32
#define KMOD_CTRL (KMOD_LCTRL | KMOD_RCTRL)

#define SDLK_UP 0               // KeyControl: current fish up
#define SDLK_LEFT 1             // KeyControl: current fish left
#define SDLK_DOWN 2             // KeyControl: current fish down
#define SDLK_RIGHT 3            // KeyControl: current fish right
#define SDLK_i 4                // ModelFactory: small fish up
#define SDLK_j 5                // ModelFactory: small fish left
#define SDLK_k 6                // ModelFactory: small fish down
#define SDLK_l 7                // ModelFactory: small fish right
#define SDLK_w 8                // ModelFactory: big fish up
#define SDLK_a 9                // ModelFactory: big fish left
#define SDLK_s 10               // ModelFactory: big fish down
#define SDLK_d 11               // ModelFactory: big fish right
#define SDLK_SPACE 12           // LevelInput: switch fish
#define SDLK_F1 13              // StateInput: help
#define SDLK_F2 14              // LevelInput: save
#define SDLK_F3 15              // LevelInput: load
#define SDLK_BACKSPACE 16       // LevelInput: restart
#define SDLK_F5 17              // LevelInput: show number of steps
#define SDLK_F6 18              // StateInput: subtitles
#define SDLK_F10 19             // StateInput: menu
#define SDLK_MINUS 20           // LevelInput: undo
#define SDLK_KP_MINUS 21        // LevelInput: undo
#define SDLK_PLUS 22            // LevelInput: redo
#define SDLK_KP_PLUS 23         // LevelInput: redo
#define SDLK_EQUALS 24          // LevelInput: redo
#define SDLK_TAB 25             // WorldInput: next room
#define SDLK_RETURN 26          // WorldInput: select room
#define SDLK_ESCAPE 27          // StateInput: quit
#define SDLK_LAST 255

#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 4

#define SDL_BUTTON(x) (1 << (x-1))

typedef Uint8 SDLKey;
extern Uint8 SDL_keys[SDLK_LAST];

class SDL_keysym
{
public:
    SDLKey sym;
    int mod;
    Uint16 unicode;
};

class SDL_KeyEvent
{
public:
    SDL_keysym keysym;
};

class SDL_MouseButtonEvent
{
public:
    int x, y;
    Uint8 button;
};

#define SDL_QUIT 0
#define SDL_KEYDOWN 1
#define SDL_KEYUP 2
#define SDL_MOUSEBUTTONDOWN 3
#define SDL_MOUSEBUTTONUP 4         // not used in ffng

class SDL_Event
{
public:
    Uint8 type;
    SDL_KeyEvent key;
    SDL_MouseButtonEvent button;
};


//int SDL_Init(int system);
void SDL_Quit();

SDL_Surface* SDL_SetVideoMode(int width, int height, Uint8 bpp, Uint32 videoflags);
void SDL_Flip(SDL_Surface* surface);
void SDL_Purge();
void SDL_FitScreen(SDL_Surface* screen = nullptr);

SDL_Surface* SDL_CreateRGBSurface(Uint32 flags, int width, int height, Uint8 bpp, Uint8 Rmask, Uint8 Gmask, Uint8 Bmask, Uint8 Amask);
void SDL_BlitSurface(SDL_Surface* surface, SDL_Rect* src_rect, SDL_Surface* screen, SDL_Rect* dest_rect, RID material = RID());
void SDL_FillRect(SDL_Surface* screen, SDL_Rect* rect, SDL_Color color);   // rect can be null
void SDL_FreeSurface(SDL_Surface* surface);

void SDL_ResetKeyStates();
Uint8* SDL_GetKeyState(void*);
void SDL_SetKeyPressed(SDLKey key, bool pressed);
Uint8 SDL_GetMouseState(int* x, int* y);
void SDL_SetMouseState(int x, int y, Uint8 pressed);

extern List<SDL_Event> SDL_events;
void SDL_PushEvent(const SDL_Event& event);
bool SDL_PollEvent(SDL_Event& event);

Uint32 SDL_GetTicks();
void SDL_Delay(Uint32 delay);

const char* SDL_GetError();

int Godot_mkdir(const char* name, int permissions);


void _SDL_CreateViewport(SDL_Surface* surface, int width, int height, bool screen = false);
void _SDL_CreateCanvas(SDL_Surface* surface);
void _SDL_CreateCanvasItem(SDL_Surface* surface);
void _SDL_CheckCanvasAndMaterial(SDL_Surface* surface, RID material = RID());