#include <iostream>
#include <chrono>
#include <Windows.h>
#include <vector>
#include <algorithm>

using namespace std;
int windowWidth = 120;
int windowHeight = 40;

int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

float player_x = 8.0f;
float player_y = 8.0f;
float player_A = 0.0f;

int Map_Width = 16;
int Map_Height = 16;

float FOV = 3.14159 / 4.0;		//field od view;
float depth = 16.0f;

float rotationSpeed = 0.75f;		//speed of rotation;

int main()
{
	// 创建控制台屏幕缓冲区
	wchar_t* screen = new wchar_t[windowWidth * windowHeight];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD dwBytesWritten = 0;

	// 设置控制台窗口的屏幕缓冲区大小
	COORD bufferSize = { static_cast<SHORT>(windowWidth), static_cast<SHORT>(windowHeight) };
	SetConsoleScreenBufferSize(console, bufferSize);

	//wchar_t* screen = new wchar_t[windowWidth * windowHeight];
	//HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	//SetConsoleActiveScreenBuffer(console);
	//DWORD dwBytesWritten = 0;

	wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	POINT currentMousePos;
	GetCursorPos(&currentMousePos);
	POINT lastMousePos = currentMousePos;

	bool isRunning = true;
	bool isMouseFree = true;
	//Game Loop
	while (isRunning)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float>elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float ElapsedTime = elapsedTime.count();
		GetCursorPos(&currentMousePos);
		float deltaX = currentMousePos.x - lastMousePos.x;
		float deltaY = currentMousePos.y - lastMousePos.y;
		ShowCursor(false);

		if (GetAsyncKeyState(VK_MENU) & 0x8000)
		{
			isMouseFree = true;
			ShowCursor(true);
		}
		else
		{
			isMouseFree = false;
			currentMousePos = lastMousePos;
		}
		if (!isMouseFree && (deltaX != 0 || deltaY != 0))
		{
			ShowCursor(false);
			player_A += static_cast<float>(deltaX) * rotationSpeed * ElapsedTime;

			// 限制视角在一定范围内（例如360度循环）
			if (player_A > 360.0f)
				player_A -= 360.0f;
			else if (player_A < 0.0f)
				player_A += 360.0f;

			if (currentMousePos.x <= 0) {
				SetCursorPos(windowWidth - 1, currentMousePos.y);
			}
			else if (currentMousePos.x >= windowWidth - 1) {
				SetCursorPos(0, currentMousePos.y);
			}

			if (currentMousePos.y <= 0) {
				SetCursorPos(currentMousePos.x, windowHeight - 1);
			}
			else if (currentMousePos.y >= windowHeight - 1) {
				SetCursorPos(currentMousePos.x, 0);
			}

		}
		
		lastMousePos = currentMousePos;

		Sleep(16);
		
		//Controls
		//Handle CCW Rotation

		/*if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			player_A -= (0.8f) * ElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			player_A += (0.8f) * ElapsedTime;*/

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		{
			// 'A'键向左移动
			player_x -= cosf(player_A) * 5.0f * ElapsedTime;
			player_y += sinf(player_A) * 5.0f * ElapsedTime;

			if (map[(int)player_y * Map_Width + (int)player_x] == '#')
			{
				player_x += cosf(player_A) * 5.0f * ElapsedTime;
				player_y -= sinf(player_A) * 5.0f * ElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			// 'D'键向右移动
			player_x += cosf(player_A) * 5.0f * ElapsedTime;
			player_y -= sinf(player_A) * 5.0f * ElapsedTime;

			if (map[(int)player_y * Map_Width + (int)player_x] == '#')
			{
				player_x -= cosf(player_A) * 5.0f * ElapsedTime;
				player_y += sinf(player_A) * 5.0f * ElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			player_x += sinf(player_A) * 5.0f * ElapsedTime;
			player_y += cosf(player_A) * 5.0f * ElapsedTime;

			if (map[(int)player_y * Map_Width + (int)player_x] == '#')
			{
				player_x -= sinf(player_A) * 5.0f * ElapsedTime;
				player_y -= cosf(player_A) * 5.0f * ElapsedTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			player_x -= sinf(player_A) * 5.0f * ElapsedTime;
			player_y -= cosf(player_A) * 5.0f * ElapsedTime;

			if (map[(int)player_y * Map_Width + (int)player_x] == '#')
			{
				player_x += sinf(player_A) * 5.0f * ElapsedTime;
				player_y += cosf(player_A) * 5.0f * ElapsedTime;
			}
		}

		// drawing the wall and the field
		for (int x = 0; x < windowWidth; x++)
		{
			//for each colum, calculate the projected ray angie into world space
			float rayAngle = (player_A - FOV / 2.0f) + ((float)x / (float)windowWidth) * FOV;

			float DistanceToWall = 0;
			bool bHitWall = false;
			bool Boundary = false;

			float eye_x = sinf(rayAngle);
			float eye_y = cosf(rayAngle);

			while (!bHitWall && DistanceToWall < depth)
			{
				DistanceToWall += 0.1f;

				int Test_X = (int)(player_x + eye_x * DistanceToWall);
				int Test_Y = (int)(player_y + eye_y * DistanceToWall);

				//Test if ray is out of bounds
				if (Test_X < 0 || Test_X >= Map_Width || Test_Y < 0 || Test_Y >= Map_Height)
				{
					bHitWall = true;		//just set distance to maxium depth
					DistanceToWall = depth;
				}
				else
				{
					//Ray is inbounds so test to see if the ray cell is a wall block
					if (map[Test_Y * Map_Width + Test_X] == '#')
					{
						bHitWall = true;

						vector<pair<float, float>> p;		//distamce,dot

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)Test_Y + ty - player_y;
								float vx = (float)Test_X + tx - player_x;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (eye_x * vx / d) + (eye_y * vy / d);

								p.push_back(make_pair(d, dot));
							}
						//sort pairs from closest to farthest
						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });

						float Bound = 0.01;
						if (acos(p.at(0).second) < Bound) Boundary = true;
						if (acos(p.at(1).second) < Bound) Boundary = true;
						if (acos(p.at(2).second) < Bound) Boundary = true;
					}
				}
			}

			//Calculate distance to celling and floor
			int celling = (float)(windowHeight / 2.0) - windowHeight / ((float)DistanceToWall);
			int floor = windowHeight - celling;

			short shade = '-';

			if (DistanceToWall <= depth / 4.0f)			shade = 0x2588;		//very close
			else if (DistanceToWall < depth / 3.0f)		shade = 0x2593;
			else if (DistanceToWall < depth / 2.0f)		shade = 0x2592;
			else if (DistanceToWall < depth)				shade = 0x2591;
			else											shade = ' ';		//too far away

			if (Boundary)		shade = 'I';

			for (int y = 0; y < windowHeight; y++)
			{
				if (y < celling)
					screen[y * windowWidth + x] = ' ';
				else if (y > celling && y <= floor)
					screen[y * windowWidth + x] = shade;
				else
				{
					//shade floor based on distance
					float b = 1.0f - (((float)y - windowHeight / 2.0f) / ((float)windowHeight / 2.0f));
					if (b < 0.25)			shade = '#';
					else if (b < 0.5)		shade = 'x';
					else if (b < 0.75)		shade = '.';
					else if (b < 0.9)		shade = '-';
					else					shade = '|';
					screen[y * windowWidth + x] = shade;
				}

			}
		}

		screen[windowWidth * windowHeight - 1] = '\0';
		WriteConsoleOutputCharacter(console, screen, windowWidth * windowHeight, { 0,0 }, &dwBytesWritten);

		//update the last position of mouse
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			isRunning = false;
		}
	}

	return 0;
}