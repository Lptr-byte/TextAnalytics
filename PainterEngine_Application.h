#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#ifdef __cplusplus
extern "C"{
#endif

#include "PainterEngine_Startup.h"
	typedef struct
	{
		PX_Runtime runtime;
		PX_FontModule fm, fm_18, fm_24, fm_32;
		PX_Object *root, *widgetRoot;
		PX_Object *result, *label, *resultRoot;
		PX_Object *explorer, *openExplorer;
		PX_Object *addKeyWord, *addKeywordWindow;
		PX_Object *startAnalytics;
		PX_Object *edit, *editConfirm;
		PX_Object *messageBox;
		px_texture MyTexture;
	}PX_Application;

	extern PX_Application App;

	px_bool PX_ApplicationInitialize(PX_Application* App, px_int screen_Width, px_int screen_Height);
	px_void PX_ApplicationUpdate(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationRender(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationPostEvent(PX_Application* App, PX_Object_Event e);
#ifdef __cplusplus
}
#endif

#endif
