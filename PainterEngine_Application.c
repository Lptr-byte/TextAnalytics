#include "PainterEngine_Application.h"
#include <stdio.h>
PX_Application App;

px_char filePath[PX_EXPLORER_MAX_PATH_LEN];
px_bool isChooseFile = PX_FALSE;
px_int countKeyWords = 0, totalKeyWords = 0;

typedef struct {
	px_char *keyWord;
	px_int count;
}KeyWord;

KeyWord KeyWords[KEYWORD_MAX_LEN + 1];

px_int App_ExplorerGetPathFolderCount(const px_char *path,const char *filter){
	return PX_FileGetDirectoryFileCount(path, PX_FILEENUM_TYPE_FOLDER, filter);
}
px_int App_ExplorerGetPathFileCount(const px_char *path,const char *filter){
	return PX_FileGetDirectoryFileCount(path, PX_FILEENUM_TYPE_FILE, filter);
}
px_int App_ExplorerGetPathFolderName(const char path[],px_int count,char FileName[][260],const char *filter){
	return PX_FileGetDirectoryFileName(path, count, FileName, PX_FILEENUM_TYPE_FOLDER, filter);
}
px_int App_ExplorerGetPathFileName(const char path[],px_int count,char FileName[][260],const char *filter){
	return PX_FileGetDirectoryFileName(path, count, FileName, PX_FILEENUM_TYPE_FILE, filter);
}

px_void App_OpenExplorer(PX_Object *pObject, PX_Object_Event e, px_void *ptr){
	PX_Application *pApp = (PX_Application*)ptr;
	PX_Object_ExplorerOpen(pApp->explorer);
}

px_void OK(PX_Object *pObject, PX_Object_Event e, void *ptr){

}

px_void App_EditOnConfirm(PX_Object *pObject, PX_Object_Event e, px_void *ptr){
	PX_Application *pApp = (PX_Application*)ptr;
	if(countKeyWords == KEYWORD_MAX_LEN){
		PX_Object_MessageBoxAlertOk(pApp->messageBox, "�ѳ����ܷ��������ؼ�����!", OK, PX_NULL);
		return;
	}
	px_char *text = PX_Object_EditGetText(pApp->edit);
	for(int i = 1; i <= countKeyWords; i++){
		if(PX_strequ(text, KeyWords[i].keyWord)){
			PX_Object_MessageBoxAlertOk(pApp->messageBox, "�˹ؼ����Ѵ���!", OK, PX_NULL);
			return;
		}
	}
	if(PX_strlen(text) != 0)
		KeyWords[++countKeyWords].keyWord = text;
	PX_Object_WidgetHide(pApp->addKeywordWindow);
}


px_void App_AddKeyWord(PX_Object *pObject, PX_Object_Event e, px_void *ptr){
	PX_Application *pApp = (PX_Application*)ptr;
	//"��ӹؼ���"�Ӵ���
	pApp->addKeywordWindow = PX_Object_WidgetCreate(&pApp->runtime.mp_ui, pApp->root, pApp->runtime.surface_width / 2  - 150,
													pApp->runtime.surface_height / 2 - 75, 300, 150, "��ӹؼ���", &pApp->fm_24);
	pApp->widgetRoot = PX_Object_WidgetGetRoot(pApp->addKeywordWindow);
	PX_Object_WidgetSetModel(pApp->addKeywordWindow, PX_TRUE);
	//�ı��༭��
	pApp->edit = PX_Object_EditCreate(&pApp->runtime.mp_ui, pApp->widgetRoot, 75, 30, 140, 32, &pApp->fm_24);
	//�Ӵ���ȷ����ť
	pApp->editConfirm = PX_Object_CursorButtonCreate(&pApp->runtime.mp_ui, pApp->widgetRoot, 225, 85, 50, 30, "Ok", &pApp->fm_18, PX_COLOR_WHITE);
	PX_ObjectRegisterEvent(PX_Object_CursorButtonGetPushButton(pApp->editConfirm), PX_OBJECT_EVENT_EXECUTE, App_EditOnConfirm, pApp);
	PX_Object_WidgetShow(pApp->addKeywordWindow);
}

px_void App_StartAnalytics(PX_Object *pObject, PX_Object_Event e, px_void *ptr){
	PX_Application *pApp = (PX_Application*)ptr;

	if(strlen(filePath) == 0){
		PX_Object_MessageBoxAlertOk(pApp->messageBox, "δѡ���ļ�!!!", OK, PX_NULL);
		return;
	}
	if(countKeyWords == 0){
		PX_Object_MessageBoxAlertOk(pApp->messageBox, "�޿ɴ���Ĺؼ���!", OK, PX_NULL);
		return;
	}
	//�����ļ�
	FILE *fp = fopen(filePath, "r");
	fseek(fp, 0, SEEK_END);
	px_int fileSize = ftell(fp);
	px_char *str = (px_char*)malloc(fileSize * sizeof(px_char));
	fseek(fp, 0, SEEK_SET);
	fread(str, fileSize, sizeof(px_char), fp);
	fclose(fp);

	//ͳ������
	totalKeyWords = 0;
	for(int i = 1; i <= countKeyWords; i++){
		px_char *target = KeyWords[i].keyWord;
		px_char *first = PX_strstr(str, target);
		KeyWords[i].count = 0;
		while(first != PX_NULL){
			KeyWords[i].count++;
			totalKeyWords++;
			first = PX_strstr(first + strlen(target), target);
		}
	}

	/*
	printf("%s\n", str);
	for(int i = 1; i <= countKeyWords; i++)
		printf("%d ", KeyWords[i].count);
	printf("\n");
	*/

	//��������Ӵ���
	pApp->result = PX_Object_WidgetCreate(&pApp->runtime.mp_ui, pApp->root, pApp->runtime.surface_width / 2  - 250,
													pApp->runtime.surface_height / 2 - 200, 500, 400, "�������", &pApp->fm_24);
	pApp->resultRoot = PX_Object_WidgetGetRoot(pApp->result);
	PX_Object_WidgetSetModel(pApp->result, PX_TRUE);
	//�ı���
	pApp->label = PX_Object_EditCreate(&pApp->runtime.mp_ui, pApp->resultRoot, 0, 0, 500, 400, &pApp->fm_24);
	PX_Object_EditSetStyle(pApp->label, PX_OBJECT_EDIT_STYLE_RECT);
	PX_Object_EditSetBorder(pApp->label, PX_TRUE);
	for(int i = 1; i <= countKeyWords; i++){
		px_char content[1024];
		px_float frequency = (px_float)KeyWords[i].count * 100 / totalKeyWords;
		sprintf_s(content, sizeof(content), "%s�������й�����%d��,��ռ%.2f%%\n", KeyWords[i].keyWord, KeyWords[i].count, frequency);
		PX_Object_EditAppendText(pApp->label, content);
	}

}

px_void App_ExplorerOnConfirm(PX_Object *pObject, PX_Object_Event e, px_void *ptr){
	PX_Application *pApp = (PX_Application*)ptr;
	isChooseFile = PX_TRUE;
	PX_Object_ExplorerGetPath(pApp->explorer, filePath, 0);
	if(PX_strstr(filePath, ".txt") == PX_NULL){
		memset(filePath, 0, sizeof(filePath));
		PX_Object_MessageBoxAlertOk(pApp->messageBox, "���ļ�����.txt�ļ�!!!", OK, PX_NULL);
		return;
	}
	//printf("%s\n", filePath);
}

px_void App_ExplorerOnCancel(PX_Object *pObject, PX_Object_Event e, px_void *ptr){

}

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height){
	static px_int startX = 32 * 10 + 5, startY = 32;
	static px_int rectWidth = 350, rectHeight = 400;
	px_int rectLeft = startX + 32 * 6, rectTop = startY + 32 * 2;
	px_int rectRight = rectLeft + rectWidth, rectBottom = rectTop + rectHeight;

	for(int i = 1; i <= KEYWORD_MAX_LEN; i++)
		KeyWords[i].count = 0;

	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	//��ʼ����������ģ
	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fm)) return PX_FALSE;
	if(!PX_LoadFontModuleFromFile(&pApp->fm,"./Fonts/gbk_zh-cn_18.pxf")) return PX_FALSE;

	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fm_18)) return PX_FALSE;
	if(!PX_LoadFontModuleFromFile(&pApp->fm_18,"./Fonts/gbk_zh-cn_18.pxf")) return PX_FALSE;

	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fm_24)) return PX_FALSE;
	if(!PX_LoadFontModuleFromFile(&pApp->fm_24,"./Fonts/gbk_zh-cn_24.pxf")) return PX_FALSE;

	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fm_32)) return PX_FALSE;
	if(!PX_LoadFontModuleFromFile(&pApp->fm_32,"./Fonts/gbk_zh-cn_32.pxf")) return PX_FALSE;

	//��������
	if(!PX_LoadTextureFromFile(&pApp->runtime.mp_resources, &pApp->MyTexture, "./pic/Alice.traw"))	return PX_FALSE;
	px_texture out;
	PX_TextureCreateScale(&pApp->runtime.mp_resources, &pApp->MyTexture, startX - 20,
							(startX - 20) * pApp->MyTexture.height / pApp->MyTexture.width, &out);
	PX_TextureCopy(&pApp->runtime.mp_resources, &out, &pApp->MyTexture);
	PX_TextureFree(&out);

	pApp->root = PX_ObjectCreate(&pApp->runtime.mp_ui, PX_NULL, 0, 0, 0, 0, 0, 0);
	//�����ļ������
	pApp->explorer = PX_Object_ExplorerCreate(&pApp->runtime.mp_ui, pApp->root, 0, 0, pApp->runtime.surface_width, pApp->runtime.surface_height, 
										&pApp->fm, App_ExplorerGetPathFolderCount, App_ExplorerGetPathFileCount, 
										App_ExplorerGetPathFolderName, App_ExplorerGetPathFileName, "");
	PX_Object_ExplorerSetMaxSelectCount(pApp->explorer, 1);
	PX_ObjectRegisterEvent(pApp->explorer, PX_OBJECT_EVENT_EXECUTE, App_ExplorerOnConfirm, pApp);
	PX_ObjectRegisterEvent(pApp->explorer, PX_OBJECT_EVENT_CANCEL, App_ExplorerOnCancel, pApp);
	//��ѡ���ļ�����ť
	pApp->openExplorer = PX_Object_CursorButtonCreate(&pApp->runtime.mp_ui, pApp->root, startX + 32 * 10, startY, 32 * 6, 32, "ѡ��һ���ļ�", &pApp->fm_18, PX_COLOR_WHITE);
	PX_ObjectRegisterEvent(PX_Object_CursorButtonGetPushButton(pApp->openExplorer), PX_OBJECT_EVENT_EXECUTE, App_OpenExplorer, pApp);
	//����ӹؼ��ʡ���ť
	pApp->addKeyWord = PX_Object_CursorButtonCreate(&pApp->runtime.mp_ui, pApp->root, rectLeft, 
													rectBottom - 32 * 2, 32 * 5, 32, "��ӹؼ���", &pApp->fm_18, PX_COLOR_WHITE);
	PX_ObjectRegisterEvent(PX_Object_CursorButtonGetPushButton(pApp->addKeyWord), PX_OBJECT_EVENT_EXECUTE, App_AddKeyWord, pApp);
	//����ʼ��������ť
	pApp->startAnalytics = PX_Object_CursorButtonCreate(&pApp->runtime.mp_ui, pApp->root, rectLeft + 32 * 7, rectBottom - 32 * 2,
														 32 * 4, 32, "��ʼ����", &pApp->fm_18, PX_COLOR_WHITE);
	PX_ObjectRegisterEvent(PX_Object_CursorButtonGetPushButton(pApp->startAnalytics), PX_OBJECT_EVENT_EXECUTE, App_StartAnalytics, pApp);
	//messagebox
	pApp->messageBox = PX_Object_MessageBoxCreate(&pApp->runtime.mp_ui, pApp->root, &pApp->fm_24);


	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed){

}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed){
	static px_int startX = 32 * 10 + 5, startY = 32;
	static px_int rectWidth = 350, rectHeight = 400;
	px_int rectLeft = startX + 32 * 6, rectTop = startY + 32 * 2;
	px_int rectRight = rectLeft + rectWidth, rectBottom = rectTop + rectHeight;
	px_char content[64] = "��ѡ����ļ�:";
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime, PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	//����ͼƬ
	PX_TextureRender(pRenderSurface, &pApp->MyTexture, 5, 7, PX_ALIGN_LEFTTOP, PX_NULL);
	//�����ı�
	PX_FontModuleDrawText(pRenderSurface,&pApp->fm_32, startX, startY, PX_ALIGN_LEFTTOP, "��ѡ��Ҫ�������ļ�:", PX_COLOR(255, 255, 255, 200));
	PX_FontModuleDrawText(pRenderSurface,&pApp->fm_32, startX, startY + 32 * 2, PX_ALIGN_LEFTTOP, "���йؼ���:", PX_COLOR(255, 255, 255, 200));

	//������ʾ��Ϣ
	sprintf_s(content, sizeof(content), "��ѡ����ļ�:%s", filePath);
	PX_FontModuleDrawText(pRenderSurface, &pApp->fm_18, pApp->runtime.surface_width, pApp->runtime.surface_height, 
							PX_ALIGN_RIGHTBOTTOM, content, PX_COLOR(255, 255, 255, 255));
	//���Ʊ߿�
	PX_GeoDrawSolidRoundRect(pRenderSurface, rectLeft, rectTop, rectRight, rectHeight, 5.0f, PX_COLOR(255, 104, 104, 104));
	//���ƹؼ���
	for(int i = 1; i <= countKeyWords + 1; i++){
		px_char text[25];
		if(i <= countKeyWords)
			sprintf_s(text, sizeof(text), "%d.%s\n", i, KeyWords[i].keyWord);
		else
		 	sprintf_s(text, sizeof(text), "%d. ...\n", i);
		PX_FontModuleDrawText(pRenderSurface,&pApp->fm_24, rectLeft + 32, rectTop + 16 + 32 * (i - 1),
							 PX_ALIGN_LEFTTOP, text, PX_COLOR(255, 0, 249, 26));
	}

	PX_ObjectRender(pRenderSurface, pApp->root, elapsed);

}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e){
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ObjectPostEvent(pApp->root, e);
}

