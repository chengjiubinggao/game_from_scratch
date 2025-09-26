#include <graphics.h>
#include <string>
#include <vector>

int constexpr PLAYER_ANIM_NUM = 5;

int constexpr PLAYER_WIDTH = 80;
int constexpr PLAYER_HEIGHT = 80;
int constexpr SHADOW_WIDTH = 32;

int constexpr  WINDOW_WIDTH = 1280;
int constexpr  WINDOW_HEIGHT = 720;

long const PLAYER_SPEED = 5;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];
IMAGE img_shadow;

POINT player_position = {500, 500};

inline void putimage_alpha(int const x, int const y, const IMAGE* img)
{
	const int w = img->getwidth();   
	const int h = img->getheight();

	#pragma comment(lib, "MSIMG32.LIB")

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(GetImageHDC(nullptr), x, y, w, h,
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

class Animation
{
public:
	Animation(LPCTSTR const path, size_t const num, int const interval)
	{
		interval_ms_ = interval;

		TCHAR path_file[256];
		for (size_t i = 0; i < num; ++i)
		{
			_sntprintf_s(path_file, 256, _TRUNCATE, path, static_cast<int>(i));

			auto frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list_.push_back(frame);
		}
	}

	~Animation()
	{
		for (const auto frame : frame_list_)
		{
			delete frame;
		}
	}

	auto play(const int x, const int y, const int delta) -> void
	{
		if (frame_list_.empty()) return;

		timer_ += delta;
		if (timer_ >= interval_ms_)
		{
			frame_index_ = (frame_index_ + 1) % frame_list_.size();
			timer_ = 0;
		}

		putimage_alpha(x, y, frame_list_[frame_index_]);
	}

private:
	int interval_ms_ = 0;
	std::vector<IMAGE*> frame_list_;

	int timer_ = 0;
	int frame_index_ = 0;

};

Animation anim_left_player(_T("img/player_left_%d.png"), 6, 45);
Animation anim_right_player(_T("img/player_right_%d.png"), 6, 45);

auto DrawPlayer(int const delta, int const dir_x) -> void
{
	auto shadow_x = player_position.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
	auto shadow_y = player_position.y + PLAYER_HEIGHT - 8;
	putimage_alpha(shadow_x, shadow_y, &img_shadow);

	static auto facing_left = false;
	if (dir_x < 0)
		facing_left = true;
	else if (dir_x > 0)
		facing_left = false;

	if (facing_left)
		anim_left_player.play(player_position.x, player_position.y, delta);
	else
		anim_right_player.play(player_position.x, player_position.y, delta);
}

int main()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	LoadAnimation();

	bool running = true;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	ExMessage msg;
	IMAGE img_background;

	loadimage(&img_background, _T("img/background.png"));
	loadimage(&img_shadow, _T("img/shadow_player.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD const start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_RIGHT:
					is_move_right = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					break;
				default:
					break;
				}
			}
			else if (msg.message == WM_KEYUP)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				default:
					break;
				}
			}
			
		}

		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0.0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			player_position.x += static_cast<int>(normalized_x * PLAYER_SPEED);
			player_position.y += static_cast<int>(normalized_y * PLAYER_SPEED);
		}

		if (player_position.x < 0) player_position.x = 0;
		if (player_position.y < 0) player_position.y = 0;
		if (player_position.x > WINDOW_WIDTH) player_position.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (player_position.x > WINDOW_HEIGHT) player_position.x = WINDOW_HEIGHT - PLAYER_HEIGHT;
		cleardevice();

		putimage(0, 0, &img_background);
		DrawPlayer(1000 / 144, is_move_right - is_move_left);

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

