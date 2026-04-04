#include <graphics.h>
#include <vector>
#include <ctime>
#include <windows.h>
using namespace std;

// 自动获取屏幕分辨率，实现真正全屏
const int WIN_W = GetSystemMetrics(SM_CXSCREEN);
const int WIN_H = GetSystemMetrics(SM_CYSCREEN);

const int PLAYER_SIZE = 60;
const int ENEMY_SIZE = 50;
const int BULLET_SIZE = 10;

const int P_SPEED = 10;
const int E_SPEED = 3;
const int B_SPEED = 6;
const int MAX_HP = 100;
const int HURT = 10;

struct Player
{
    int x, y, hp;
    Player() { x = WIN_W / 2 - PLAYER_SIZE / 2; y = WIN_H - 150; hp = MAX_HP; }
    void move(int dx, int dy)
    {
        x = max(0, min(WIN_W - PLAYER_SIZE, x + dx));
        y = max(100, min(WIN_H - PLAYER_SIZE, y + dy));
    }
    RECT rect() { return { x, y, x + PLAYER_SIZE, y + PLAYER_SIZE }; }
    void reset() { x = WIN_W / 2 - PLAYER_SIZE / 2; y = WIN_H - 150; hp = MAX_HP; }
};

struct Enemy { int x, y; void move() { y += E_SPEED; } RECT rect() { return { x, y, x + ENEMY_SIZE, y + ENEMY_SIZE }; } };
struct Bullet { int x, y; void move() { y += B_SPEED; } RECT rect() { return { x, y, x + BULLET_SIZE, y + BULLET_SIZE }; } };

void drawHP(int hp)
{
    setfillcolor(LIGHTGRAY);
    bar(20, 20, 260, 50);
    if (hp > 70) setfillcolor(GREEN);
    else if (hp > 40) setfillcolor(YELLOW);
    else setfillcolor(RED);
    bar(20, 20, 20 + (int)(2.4 * hp), 50);
    setcolor(WHITE);
    rectangle(20, 20, 260, 50);
}

int main()
{
    srand((unsigned int)time(NULL));

    // ==================== 核心：真正的全屏初始化 ====================
    // 替换所有 FULLSCREEN 为 EWindowMode::eFullScreen
    // 手动定义宏（放在头文件之后、主函数之前）
#define FULLSCREEN 0x8000
    initgraph(WIN_W, WIN_H, FULLSCREEN);// 直接启动全屏，无窗口、无标题栏
    HWND hWnd = GetHWnd();
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    setbkcolor(RGB(30, 30, 60));
    cleardevice();
    BeginBatchDraw();

    Player p;
    vector<Enemy> es;
    vector<Bullet> bs;
    DWORD t1 = 0, t2 = 0;
    bool over = false;
    int score = 0;

    // 初始强制绘制，避免黑屏
    solidrectangle(p.x, p.y, p.x + PLAYER_SIZE, p.y + PLAYER_SIZE);
    FlushBatchDraw();
    Sleep(300);

    while (true)
    {
        cleardevice();

        // 四向方向键控制
        if (!over)
        {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) p.move(-P_SPEED, 0);
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) p.move(P_SPEED, 0);
            if (GetAsyncKeyState(VK_UP) & 0x8000) p.move(0, -P_SPEED);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) p.move(0, P_SPEED);
        }

        // R键重置
        if (GetAsyncKeyState('R') & 0x8000 || GetAsyncKeyState('r') & 0x8000)
        {
            p.reset();
            es.clear();
            bs.clear();
            score = 0;
            over = false;
            Sleep(200);
        }

        // ESC键退出全屏/游戏
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            closegraph();
            return 0;
        }

        if (!over)
        {
            // 生成敌机
            if (GetTickCount() - t1 > 400)
            {
                es.push_back({ rand() % (WIN_W - ENEMY_SIZE), 0 });
                t1 = GetTickCount();
            }

            // 敌机发射子弹
            if (GetTickCount() - t2 > 300 && !es.empty())
            {
                int idx = rand() % es.size();
                bs.push_back({ es[idx].x + ENEMY_SIZE / 2, es[idx].y + ENEMY_SIZE });
                t2 = GetTickCount();
            }

            // 更新敌机位置
            for (auto it = es.begin(); it != es.end();)
            {
                it->move();
                if (it->y > WIN_H) it = es.erase(it); else ++it;
            }

            // 更新子弹位置
            for (auto it = bs.begin(); it != bs.end();)
            {
                it->move();
                if (it->y > WIN_H) it = bs.erase(it); else ++it;
            }

            // 碰撞检测
            RECT pr = p.rect();
            for (auto it = es.begin(); it != es.end();)
            {
                RECT er = it->rect();
                bool hit = !(pr.right <= er.left || pr.left >= er.right || pr.bottom <= er.top || pr.top >= er.bottom);
                if (hit) { p.hp = max(0, p.hp - HURT); it = es.erase(it); }
                else ++it;
            }
            for (auto it = bs.begin(); it != bs.end();)
            {
                RECT br = it->rect();
                bool hit = !(pr.right <= br.left || pr.left >= br.right || pr.bottom <= br.top || pr.top >= br.bottom);
                if (hit) { p.hp = max(0, p.hp - HURT); it = bs.erase(it); }
                else ++it;
            }

            if (p.hp <= 0) over = true;
            score++;
        }

        // 绘制敌机
        setfillcolor(RED);
        for (auto& e : es) solidrectangle(e.x, e.y, e.x + ENEMY_SIZE, e.y + ENEMY_SIZE);

        // 绘制子弹
        setfillcolor(WHITE);
        for (auto& b : bs) solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);

        // 绘制玩家
        if (p.hp > 70) setfillcolor(GREEN);
        else if (p.hp > 40) setfillcolor(YELLOW);
        else setfillcolor(RED);
        solidrectangle(p.x, p.y, p.x + PLAYER_SIZE, p.y + PLAYER_SIZE);

        // 绘制UI
        drawHP(p.hp);
        setcolor(WHITE);
        settextstyle(24, 0, _T("微软雅黑"));
        TCHAR txt[50];
        _stprintf_s(txt, _T("得分: %d"), score);
        outtextxy(20, 60, txt);

        if (over)
        {
            setcolor(RED);
            settextstyle(36, 0, _T("微软雅黑"));
            outtextxy(WIN_W / 2 - 90, WIN_H / 2, _T("游戏结束"));
            outtextxy(WIN_W / 2 - 110, WIN_H / 2 + 50, _T("按 R 重新开始，ESC 退出"));
        }

        FlushBatchDraw();
        Sleep(16);
    }

    EndBatchDraw();
    closegraph();
    return 0;
}