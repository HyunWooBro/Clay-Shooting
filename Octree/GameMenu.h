#pragma once

#include "main.h"

class GameMenu
{
public:
	GameMenu(void);
	~GameMenu(void);

	void show();
	bool isInRect(RECT rect, POINT pt);

	RECT menu_rect[MAX_MENU]; // 0 ~ 5 ���θ޴����� �� �޴�, 6 - option, help, credit���� �ڷΰ���
	bool is_menu_active[MAX_MENU];

	int select;
};

