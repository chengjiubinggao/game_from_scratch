#include <graphics.h>

int main()
{
	initgraph(1280, 720);

	int x = 300;
	int y = 300;
	bool running = true;

	BeginBatchDraw();

	while (running)
	{
		ExMessage msg;
		while (peekmessage(&msg))
		{
			if (msg.message == WM_MOUSEMOVE)
			{
				x = msg.x;
				y = msg.y;
			}
			if (msg.message == WM_RBUTTONDOWN)
			{
				running = FALSE;
			}
		}

		cleardevice();
		solidcircle(x, y, 100);
		FlushBatchDraw();

	}
	EndBatchDraw();
	closegraph();

	return 0;
}