#include <graphics.h>
#include <string>

int static idx_current_anim = 0;

const int PLAYER_ANIM_NUM = 5;

long const play_speed = 10;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

POINT player_position = {500, 500};

inline void putimage_alpha(int const x, int const y, const IMAGE* img)
{
	const int w = img->getwidth();   
	const int h = img->getheight();

	#pragma comment(lib, "MSIMG32.LIB")

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, bf);
}

void LoadAnimation()
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; ++i)
	{
		std::wstring path_left = L"img/player_left_" + std::to_wstring(i) + L".png";
		std::wstring path_right = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path_left.c_str());
		loadimage(&img_player_right[i], path_right.c_str());

	}
}

int main()
{
	initgraph(1280, 720);
	LoadAnimation();

	bool running = true;

	ExMessage msg;
	IMAGE img_background;

	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD const start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
				switch (msg.vkcode)
				{
				case VK_UP:
					player_position.y -= play_speed;
					break;
				case VK_DOWN:
					player_position.y += play_speed;
					break;
				case VK_RIGHT:
					player_position.x += play_speed;
					break;
				case VK_LEFT:
					player_position.x -= play_speed;
					break;
				default:
					break;
				}
		}

		int static counter = 0;
		if (++counter % 5 == 0)
		{
			idx_current_anim++;
		}

		idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;

		cleardevice();

		putimage(0, 0, &img_background);
		putimage_alpha(player_position.x, player_position.y, &img_player_left[idx_current_anim]);

		FlushBatchDraw();

		DWORD const end_time = GetTickCount();
		DWORD const delta_time = end_time - start_time;

		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}

	}

	EndBatchDraw();

	return 0;
}