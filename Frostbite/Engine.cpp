#include "Engine.h"

#include <stack>

int random(int lo, int hi)
{
	assert(lo <= hi);
	return lo == hi ? lo : lo + rand() % (hi - lo + 1);
}

static int s_nLinesWritten = 0;
static std::stack<int> s_stateStack;

void vtWriteChar(int ch)
{
	if (ch == '\n') s_nLinesWritten += 1;
	putchar(ch);
}

void vtCursorPrevLine(int nLines)
{
	std::cout << "\033[" << nLines << "A" << std::flush;
	s_nLinesWritten -= nLines;
}

void vtEraseInDisplay()
{
	std::cout << "\033[0J" << std::flush;
}

void vtOpenWnd()
{
	s_stateStack.push(s_nLinesWritten);
}

void vtCloseWnd()
{
	int nLines = s_stateStack.top();
	s_stateStack.pop();
	vtCursorPrevLine(s_nLinesWritten - nLines);
	vtEraseInDisplay();

	s_nLinesWritten = nLines;
}


void writef(const char* fmt)
{
	while (*fmt) {
		if (*fmt == '{' && *(fmt + 1) == '}') {
#ifdef _DEBUG
			std::cout << "{UNDEFINED ARG}";
#endif //_DEBUG
			fmt += 2;
		}
		else {
			vtWriteChar(*fmt);
			fmt += 1;
		}
	}
	std::cout << std::flush;
}

bool readln(std::string& input)
{
	std::cin >> input;
	s_nLinesWritten += 1;
	return true;
}