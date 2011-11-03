//---------------------------------------------------------------------------
#include <cstdlib>
#include <cctype>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "SDL.h"
#include "TGui.h"
#include "TPage.h"
#include "sdlw.h"
#include "platform.h"

//---------------------------------------------------------------------------


bool LMB, MMB, RMB;
static PageManager *pm;

void show_me_money(void *widget)
{
    static bool visible = true;
    TText *txt = static_cast<TText*>(widget);
    txt->setVisible(visible);
    visible = !visible;
}

void *thread_update_text(void *widget)
{
    static int count = 0;
    char buf[64];
    TText *txt = static_cast<TText*>(widget);

    while (1) {
        time_t t = time(NULL);
        snprintf(buf, sizeof(buf), "%s", ctime(&t));
        txt->settext(buf);
        pm->getActive()->RedrawAll();
        pm->getActive()->RedrawAll();
        sleep(1);
    }

    return NULL;
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
	if( -1 == SDL_Init(SDL_INIT_VIDEO) ) {
		printf("Error initializing SDL\n");
		exit(-1);
	}

	atexit(SDL_Quit);

    TGui *Gui, *Gui2;
    SDL_Surface *screen, *background, *linuxback;

	// get video info
	const SDL_VideoInfo *inf;
	inf = SDL_GetVideoInfo();
	printf("hw_available: %d\n", inf->hw_available);
	printf("wm_available: %d\n", inf->wm_available);
	printf("blit_hw: %d\n", inf->blit_hw);
	printf("blit_hw_CC: %d\n", inf->blit_hw_CC);
	printf("blit_hw_A: %d\n", inf->blit_hw_A);
	printf("blit_sw: %d\n", inf->blit_sw);
	printf("blit_sw_CC: %d\n", inf->blit_sw_CC);
	printf("blit_sw_A: %d\n", inf->blit_sw_A);
	printf("blit_fill: %d\n", inf->blit_fill);
	printf("video_mem: %d\n", inf->video_mem);
	printf("BitsPerPixel: %d\n\n", (int)inf->vfmt->BitsPerPixel);

	screen = SDL_SetVideoMode(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, 16, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWPALETTE|SDL_HWACCEL|SDL_PREALLOC);
	if(screen == NULL) {
		printf("Error setting video mode!\n");
		exit(-1);
	}

	background = SDL_LoadBMP("layout.bmp");
	if(background == NULL) {
		printf("Error creating background surfce!\n");
		exit(-1);
	}

    linuxback = SDL_LoadBMP("linux.bmp");
    if (linuxback == NULL) {
        printf("Error creating background surfce!\n");
        exit(-1);
    }

	// create gui
	Gui = new TGui(screen, background);
    Gui2 = new TGui(screen, linuxback);

	// add few elements
    TButton *btn1 = new TButton(Gui, 10, 80, 80, 50, "btn1", "CLICK");
    Gui->AddElement(btn1);

	Gui->AddElement( new TWindow(Gui, 10, 10, 50, 50, "wnd1", "Hello!") );

    TText *mytext = new TText(Gui, 50, 50, 24, "text1", "Fuck ^_^");
    Gui->AddElement(mytext);

    Functor<TButton::CallbackType> cmd1(show_me_money);
    btn1->setClicked(cmd1, mytext);

    // here are page2
    TButton *btn2 = new TButton(Gui2, 300, 150, 100, 70, "btn2", "FUCK");
    Gui2->AddElement(btn2);
    TText *mytext2 = new TText(Gui2, 10, 10, 24, "text2", "Shit!");
    Gui2->AddElement(mytext2);

    // here are page3
    SDL_Surface *snoopy = SDL_LoadBMP("snoopy.bmp");
    if (snoopy == NULL) {
        printf("Error creating background surfce!\n");
        exit(-1);
    }

    TGui *Gui3 = new TGui(screen, snoopy);
    TButton *btn3 = new TButton(Gui3, 30, 200, 100, 50, "btn3", "BULLSHIT");
    TButton *btn_update = new TButton(Gui3, 30, 50, 80, 50, "btn_update", "Update!");
    TText *mytext3 = new TText(Gui3, 10, 140, 16, "text3", "I'm bull shit");
    TText *count = new TText(Gui3, 130, 70, 20, "count", "count: ");

    Gui3->AddElement(btn3);
    Gui3->AddElement(btn_update);
    Gui3->AddElement(mytext3);
    Gui3->AddElement(count);

//    Functor<void (void*)> cmd_update(update_text);
//    btn_update->setClicked(cmd_update, count);

    pm = new PageManager(Gui3);
    pm->insert(Gui, 0);
    pm->insert(Gui2, 1);
    pm->insert(Gui3, 2);
    pm->set_switch_button(btn2, 0);
    pm->set_switch_button(btn1, 2);
    pm->set_switch_button(btn3, 1);

	LMB = MMB = RMB = false;
	bool Done = false;
	SDL_Event ev;

#ifdef BUILD_FOR_ANDROID
    SDL_ShowCursor(SDL_DISABLE);
#endif

    SDL_UpdateRect(screen,0,0,0,0);

    pm->getActive()->RedrawAll();
    pm->getActive()->RedrawAll();

    pthread_t tid;
    pthread_create(&tid, NULL, thread_update_text, count);

	while( !Done  )
	{
		SDL_Delay(1);
		if( SDL_PollEvent(&ev) == 0 ) {
			continue;
		}
		switch(ev.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch(ev.button.button)
				{
					case SDL_BUTTON_LEFT:
						LMB = true;
                        pm->getActive()->OnMouseDown(ev.motion.x, ev.motion.y);
						break;
					case SDL_BUTTON_MIDDLE:
						MMB = true;
						break;
					case SDL_BUTTON_RIGHT:
						RMB = true;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch(ev.button.button)
				{
					case SDL_BUTTON_LEFT:
						LMB = false;
                        pm->getActive()->OnMouseUp(ev.motion.x, ev.motion.y);
						break;
					case SDL_BUTTON_MIDDLE:
						MMB = false;
						break;
					case SDL_BUTTON_RIGHT:
						RMB = false;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEMOTION:
			   Gui->OnMouseMove(ev.motion.x, ev.motion.y);
				if(LMB) Gui->Drag(ev.motion.xrel, ev.motion.yrel);
				break;
			case SDL_QUIT:
				Done = true;
				break;
			case SDL_KEYDOWN:
				switch(ev.key.keysym.sym)
				{
					case SDLK_SPACE:
						Done = true;
						break;
					default:
						break;
				}
			default:
				break;
		}
	}

    delete Gui;
    delete Gui2;
    SDL_FreeSurface(screen);
    SDL_FreeSurface(background);
    SDL_FreeSurface(linuxback);

	return 1;
}


