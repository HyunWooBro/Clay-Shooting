#pragma once

#include "main.h"

class GameMenu
{
public:
	GameMenu(void);
	~GameMenu(void);

	void show();
	bool isInRect(RECT rect, POINT pt);

	RECT menu_rect[MAX_MENU]; // 0 ~ 5 메인메뉴에서 각 메뉴, 6 - option, help, credit에서 뒤로가기
	bool is_menu_active[MAX_MENU];

	int select;
};

