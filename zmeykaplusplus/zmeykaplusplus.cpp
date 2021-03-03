#define _WIN32_WINNT 0x0A00

#include <iostream>
#include <Windows.h>
#include <vector>
#include <thread>
#include <string>



struct timeCheck {
	unsigned int clockStart;
	void Restart() {
		clockStart = clock();
	}
	int getTime() {
		return clock() - clockStart ;
	}
};
struct tail {
	tail* next;
	int x;
	int y;
} head = {nullptr, 1,1};
enum class WhereMove {
	UP,
	DOWN,
	LEFT,
	RIGHT
}; 


// Global Vars:
	WhereMove toMove = WhereMove::LEFT;
	HDC dc = nullptr;
	constexpr short width = 20;
	constexpr short height = 20;
	std::vector< tail* > tails;
	timeCheck timer;
	std::pair<int, int> apple(0,0);
	wchar_t points[] = L"Points:    ";
// End global vars

void setup() {
	tails.push_back(&head);

	// Setup drow
	dc = GetDC(GetConsoleWindow()); // - Console
	//dc = GetDC(GetDesktopWindow());
	SelectObject(dc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(dc, RGB(255, 255, 255));
}

void toNext() {
	for (auto i = tails.rbegin(); i != tails.rend(); ++i) {
		if ((*i)->next != nullptr) {
			
			(*i)->x = (*i)->next->x;
			(*i)->y = (*i)->next->y;
		}
	}
}


void drow() {
	//Rectangle(dc, 10, 10, 200, 200);
	for (int y = 0; y < 20; ++y) {
		for (int x = 0; x < 20; ++x) {

			for (int i = 1; i < tails.size(); ++i) {
				if (tails[i]->x == x && tails[i]->y == y) {
					SetDCBrushColor(dc, RGB(0, 255, 0));
					Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
					goto exit;
				}
			}

			// Head drow
			if (head.x == x && head.y == y) {
				SetDCBrushColor(dc, RGB(0, 0, 255));
				Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
			} 

			// Apple drow
			else if (apple.first == x && apple.second == y) {
				SetDCBrushColor(dc, RGB(255, 0, 0));
				Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
			}
			
			// Pole drow
			else {
				SetDCBrushColor(dc, RGB(255, 255, 255));
				Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
			}

		exit:{}
			
		}
	}
}
void drowDesktop() {
	SetDCBrushColor(dc, RGB(255, 255, 255));
	Rectangle(dc, 0, 0, width * width, height * height);
	for (int y = 0; y < 20; ++y) {
		for (int x = 0; x < 20; ++x) {

			for (int i = 1; i < tails.size(); ++i) {
				if (tails[i]->x == x && tails[i]->y == y) {
					SetDCBrushColor(dc, RGB(0, 255, 0));
					Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
					goto exit;
				}
			}

			// Head drow
			if (head.x == x && head.y == y) {
				SetDCBrushColor(dc, RGB(0, 0, 255));
				Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
			}

			// Apple drow
			else if (apple.first == x && apple.second == y) {
				SetDCBrushColor(dc, RGB(255, 0, 0));
				Rectangle(dc, width * x, height * y, width * x + width, height * y + height);
			}

		exit: {}
		}
	}
}

void move() {
	while (1) {
		if (GetKeyState('D') < 0)
			toMove = WhereMove::LEFT;
		else if (GetKeyState('A') < 0)
			toMove = WhereMove::RIGHT;
		else if (GetKeyState('S') < 0)
			toMove = WhereMove::DOWN;
		else if (GetKeyState('W') < 0)
			toMove = WhereMove::UP;
	}
}
void moveKD() {
	if (toMove == WhereMove::LEFT)					head.x++;
	else if (toMove == WhereMove::RIGHT)			head.x--;
	else if (toMove == WhereMove::UP)				head.y--;
	else if (toMove == WhereMove::DOWN)				head.y++;
}
void appleSpawn() {
	srand(time(0));
	apple.first = rand() % width;		// X
	apple.second = rand() % height;		// Y
}
void onOverrite() {
	for (auto i : tails) {
		if (i->next != nullptr) {
			if (i->x == head.x && i->y == head.y) exit(0);
		}
	}

	if (apple.first == head.x && apple.second == head.y) {
		appleSpawn();
		tails.push_back(new tail{ tails.back(),-1,-1 });
	}
}
void onWall() {
	if (head.x > width - 1)			head.x = 0;
	else if (head.x < 0)			head.x = width - 1;
	if (head.y > height - 1)		head.y = 0;
	else if (head.y < 0)			head.y = height - 1;
}
void outText() {
	if (tails.size() < 10)					points[8] = tails.size() - 1 + '0';
	else if (tails.size() < 100)			points[8] = std::to_string(tails.size() - 1)[0], points[9] = std::to_string(tails.size() - 1)[1];
	else if (tails.size() < 1000)			points[8] = std::to_string(tails.size() - 1)[0], points[9] = std::to_string(tails.size() - 1)[1], points[10] = std::to_string(tails.size() - 1)[2];

	SetDCBrushColor(dc, RGB(0,0,0));	
	TextOut(dc, width*width, 0, points, sizeof(points) / sizeof(wchar_t));
}

int update() {
	std::thread moveth(move);
	moveth.detach();

	timer.Restart();
	appleSpawn();
	while (1)
	{
		// Exit
		if (GetKeyState(VK_ESCAPE) < 0) return 0;

		// Logik:
			if (timer.getTime() > 300) {
				toNext();
				moveKD();
				timer.Restart();

				// Check on tail
				onOverrite();
			}
			onWall();
		// end Logik

		// Drow
			drow();		// Pole out (Console)
			//drowDesktop();	// Pole out (Desktop)
			outText();	// Text 
	}
	return 1;
}

int main() {
	setup();
	return update();
}