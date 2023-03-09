#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include "TaskbarBeGone.h"

int main(int, char**) {
	TaskbarBeGone* app = new TaskbarBeGone();
	app->Run();
	delete app;
}

#endif /* ENTRY_POINT_H */
