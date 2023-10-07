#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include "TaskbarBeGone.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
	TaskbarBeGone* app = new TaskbarBeGone();
	app->Run();
	delete app;
}

#endif /* ENTRY_POINT_H */
