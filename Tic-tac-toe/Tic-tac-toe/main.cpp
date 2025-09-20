#include <graphics.h>

#include <array>

class Game
{
public:

	Game() = default;

	bool check()
	{
		check_state();
		if (game_result_ == gameResult::playing) return true;

		switch (game_result_) {
		case gameResult::x_win:
			MessageBox(GetHWnd(), _T("X 赢了"), _T("游戏结束"), MB_OK);
			break;
		case gameResult::o_win:
			MessageBox(GetHWnd(), _T("O 赢了"), _T("游戏结束"), MB_OK);
			break;
		case gameResult::draw:
			MessageBox(GetHWnd(), _T("平局"), _T("游戏结束"), MB_OK);
			break;
		default: break;
		}
		return false;
	}

	void DrawBoard() const
	{
		line(0, 200, 600, 200);
		line(0, 400, 600, 400);
		line(200, 0, 200, 600);
		line(400, 0, 400, 600);
	}

	void DrawPiece () const 
	{
		for (int i = 0; i < 3; ++i) 
		{
			for (int j = 0; j < 3; ++j)
			{
				switch (board_[i][j])
				{
				case cell::o:
					circle(200 * j + 100, 200 * i + 100, 100);
					break;
				case cell::x:
					line(200 * j, 200 * i, 200 * (j + 1), 200 * (i + 1));
					line(200 * (j + 1), 200 * i, 200 * j, 200 * (i + 1));
					break;
				case cell::empty:
					break;
				}
			}
		}
	}

	void DrawTipText() const
	{
		static TCHAR str[64];
		const TCHAR who = (player_ == cell::x) ? _T('X') : _T('O');
		_stprintf_s(str, _T("当前棋子类型: %c"), who);
		settextcolor(WHITE);
		outtextxy(10, 10, str);
	}

	void DownPiece(const int& x, const int& y)
	{
		if (x < 0 || x >= 3 || y < 0 || y >= 3) return;       
		if (board_[y][x] != cell::empty) return;

		board_[y][x] = player_;

		const int v = (player_ == cell::x) ? +1 : -1;
		rows_[y] += v;
		cols_[x] += v;
		if (y == x) diag_ += v;
		if (y + x == 2) anti_ += v;
		++moves_;

		player_ = (player_ == cell::x) ? cell::o : cell::x;
	}


private:
	enum class cell { empty, x, o };
	using board = std::array<std::array<cell, 3>, 3>;
	enum class gameResult {playing, x_win, o_win, draw};

	void check_state() {
		auto won = [](int const v) { return v == 3 || v == -3; };

		for (int i = 0; i < 3; ++i) {
			if (won(rows_[i])) { game_result_ = (rows_[i] == 3) ? gameResult::x_win : gameResult::o_win; return; }
			if (won(cols_[i])) { game_result_ = (cols_[i] == 3) ? gameResult::x_win : gameResult::o_win; return; }
		}
		if (won(diag_)) { game_result_ = (diag_ == 3) ? gameResult::x_win : gameResult::o_win; return; }
		if (won(anti_)) { game_result_ = (anti_ == 3) ? gameResult::x_win : gameResult::o_win; return; }

		if (moves_ == 9) game_result_ = gameResult::draw;
		else             game_result_ = gameResult::playing;
	}

private:
	cell player_ = cell::x;
	board board_{};
	gameResult game_result_ = gameResult::playing;

	int rows_[3]{};                  
	int cols_[3]{};             
	int diag_ = 0, anti_ = 0;       
	int moves_ = 0;                
};



int main()
{
	initgraph(600, 600);

	Game my_game;
	bool running = true;
	ExMessage msg;

	BeginBatchDraw();

	while (running)
	{
		while (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{

				int const x = msg.x;
				int const y = msg.y;

				int index_x = x / 200;
				int index_y = y / 200;

				my_game.DownPiece(index_x, index_y);
			}
		}

		cleardevice();
		my_game.DrawBoard();
		my_game.DrawPiece();
		my_game.DrawTipText();

		FlushBatchDraw();
		running = my_game.check();
	}

	EndBatchDraw();
	closegraph();
	return 0;
}