// Лаб1 Вариант 4: Кардиоида
// https://programmercave0.github.io/blog/2019/10/10/How-to-Install-graphics.h-C-C++-library-on-Ubuntu
// Тема: Изучение стандартных средств отображения графической информации.
// Цель: Используя стандартные средства вывода графической информации, построить графики
// кривых высших порядков и обеспечить масштабирование по размерам формы приложения
#include <math.h>
#include <graphics.h>

const int WIDTH = 625, HEIGHT = 600;

using namespace std;

void drawLine(int moveToX, int moveToY, int drawX, int drawY, int color, int textX, int textY, char name)
{
    moveto(moveToX, moveToY);
    setcolor(color);
    lineto(drawX, drawY);
    outtextxy(textX, textY, &name);
}

int main()
{
    int gd = DETECT, gmode;
    initgraph(&gd, &gmode, NULL);
    double x, y;

    moveto(210, 0); // start position
    // y = 3x^2
    for (x = -1; x < 1; x += 0.1)
    {
        y = 3 * pow(x, 2.0);
        lineto(WIDTH / 2 + (x * 100), HEIGHT / 2 - (y * 100));
    }

    moveto(0, 0);
    // y = 1/(x^2 + 2x + 1)
    for (x = -5; x < -1.2; x += 0.1)
    {
        y = 1 / (pow(x, 2.0) + 2 * x + 1);
        lineto(WIDTH / 2 + (x * 100), HEIGHT / 2 - (y * 100));
    }

    moveto(210, 0);
    for (x = -0.5; x < 4; x += 0.1)
    {
        y = 1 / (pow(x, 2.0) + 2 * x + 1);
        lineto(WIDTH / 2 + (x * 100), HEIGHT / 2 - (y * 100));
    }

    drawLine(0, HEIGHT / 2, WIDTH + 30, HEIGHT / 2, GREEN, WIDTH - 20, HEIGHT / 2 + 10, 'X');
    drawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GREEN, WIDTH / 2 + 10, 0, 'Y');

    delay(10000);
    closegraph();
    return 0;
}