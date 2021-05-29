#define AIDS_IMPLEMENTATION
#include "aids.hpp"

#ifdef _MSC_VER
#include "unistd_win.h"
#else
#include <unistd.h>
#endif

const int W = 5;
const int H = 5;

enum class Cell
{
    Dead = 0,
    Alive = 1
};

struct Board
{
    Cell cells[H][W];

    Cell get_wrapped(int y, int x) const
    {
        return cells[aids::mod(y, H)][aids::mod(x, W)];
    }

    int nbors(int y, int x) const
    {
        int result = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx != 0 || dy != 0) {
                    if (get_wrapped(y + dy, x + dx) == Cell::Alive) {
                        result += 1;
                    }
                }
            }
        }
        return result;
    }
    
    void render(FILE *stream)
    {
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                if (cells[y][x] == Cell::Alive) {
                    aids::print(stream, '#');
                } else {
                    aids::print(stream, '.');
                }
            }
            aids::println(stream);
        }
    }
};

void next_gen(const Board *prev, Board *next)
{
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            auto nbors = prev->nbors(y, x);
            if (prev->cells[y][x] == Cell::Alive) {
                next->cells[y][x] = nbors == 3 || nbors == 2 ? Cell::Alive : Cell::Dead;
            } else {
                next->cells[y][x] = nbors == 3 ? Cell::Alive : Cell::Dead;
            }
        }
    }
}

Board boards[2] = {
    {
        {
            {Cell::Dead, Cell::Alive, Cell::Dead, Cell::Dead, Cell::Dead},
            {Cell::Dead, Cell::Dead, Cell::Alive, Cell::Dead, Cell::Dead},
            {Cell::Alive, Cell::Alive, Cell::Alive, Cell::Dead, Cell::Dead},
            {Cell::Dead, Cell::Dead, Cell::Dead, Cell::Dead, Cell::Dead},
            {Cell::Dead, Cell::Dead, Cell::Dead, Cell::Dead, Cell::Dead},
        }
    },
    {}
};

int main()
{
#ifdef _MSC_VER
    // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example-of-sgr-terminal-sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return GetLastError();

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return GetLastError();

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) return GetLastError();
#endif

    int fb = 0;

    boards[fb].render(stdout);

    for (;;) {
        int bb = 1 - fb;
        next_gen(&boards[fb], &boards[bb]);
        fb = bb;
        aids::print(stdout, "\033[", H, "A");
        aids::print(stdout, "\033[", W, "D");
        boards[fb].render(stdout);
        usleep(200000);
        if (false) break;
    }

    return 0;
}
