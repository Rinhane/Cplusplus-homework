
 
/*#include <graphics.h>
#include <vector>
#include <ctime>
#include <windows.h>
using namespace std;

const int PLAYER_SIZE = 80;
const int ENEMY_SIZE = 50;
const int BULLET_SIZE = 10;

const int P_SPEED = 10;
const int MAX_HP = 100;
const int HURT = 10;

struct Diff
{
    int e_speed;
    int b_speed;
    int e_cd;
    int b_cd;
} diff;

const Diff EASY = { 2, 4, 600, 500 };
const Diff NORMAL = { 3, 6, 400, 300 };
const Diff HARD = { 5, 8, 250, 200 };

IMAGE img_bg;
IMAGE img_main;
IMAGE img_p1, img_p2, img_p3, img_p4;
IMAGE img_enemy;

struct Player
{
    int x, y, hp;
    Player(int w, int h) : x(w / 2 - PLAYER_SIZE / 2),
        y(h / 2 + 150), hp(MAX_HP) {}

    void move(int dx, int dy, int w, int h)
    {
        x = max(0, min(w - PLAYER_SIZE, x + dx));
        y = max(0, min(h - PLAYER_SIZE, y + dy));
    }

    RECT rect() const { return { x, y, x + PLAYER_SIZE, y + PLAYER_SIZE }; }

    void reset(int w, int h)
    {
        x = w / 2 - PLAYER_SIZE / 2;
        y = h / 2 + 150;
        hp = MAX_HP;
    }
};

struct Enemy
{
    int x, y;
    void move(int s) { y += s; }
    RECT rect() const { return { x, y, x + ENEMY_SIZE, y + ENEMY_SIZE }; }
};

struct Bullet
{
    int x, y;
    void move(int s) { y += s; }
    RECT rect() const { return { x, y, x + BULLET_SIZE, y + BULLET_SIZE }; }
};

void drawHP(int hp)
{
    setfillcolor(LIGHTGRAY);
    bar(20, 20, 260, 50);
    if (hp > 50) setfillcolor(GREEN);
    else if (hp > 20) setfillcolor(YELLOW);
    else setfillcolor(RED);
    bar(20, 20, 20 + (int)(2.4 * hp), 50);
    setcolor(WHITE);
    rectangle(20, 20, 260, 50);
}

// 🔑 真正居中对齐的规则弹窗（自动计算文字宽度）
void showRules(int w, int h)
{
    setbkmode(TRANSPARENT); // 提前设置透明背景

    while (true)
    {
        cleardevice();
        // 1. 全屏黑色背景
        setfillcolor(RGB(0, 0, 0));
        bar(0, 0, w, h);

        settextcolor(WHITE);

        // 2. 标题居中
        settextstyle(36, 0, _T("微软雅黑"));
        const TCHAR* title = _T("游戏规则");
        int title_w = textwidth(title); // 获取标题宽度
        outtextxy((w - title_w) / 2, h / 2 - 150, title);

        // 3. 规则内容居中
        settextstyle(22, 0, _T("微软雅黑"));
        const TCHAR* line1 = _T("方向键（右下角箭头）控制角色移动");
        const TCHAR* line2 = _T("躲避敌机和子弹，防止血量耗尽");
        const TCHAR* line3 = _T("按R返回主界面，ESC退出游戏");

        int line1_w = textwidth(line1);
        int line2_w = textwidth(line2);
        int line3_w = textwidth(line3);

        outtextxy((w - line1_w) / 2, h / 2 - 80, line1);
        outtextxy((w - line2_w) / 2, h / 2 - 30, line2);
        outtextxy((w - line3_w) / 2, h / 2 + 20, line3);

        // 4. 底部提示居中
        settextstyle(20, 0, _T("微软雅黑"));
        const TCHAR* tip = _T("点击任意处返回");
        int tip_w = textwidth(tip);
        outtextxy((w - tip_w) / 2, h / 2 + 120, tip);

        FlushBatchDraw();

        // 5. 退出逻辑
        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN) return;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return;

        Sleep(10);
    }
}

// 主界面：新增“游戏规则”按钮
void showMainUI(int w, int h)
{
    int bw = 240;
    int bh = 60;
    int x = w / 2 - bw / 2;
    int y1 = h / 2 - 150;
    int y2 = h / 2 - 50;
    int y3 = h / 2 + 50;
    int y4 = h / 2 + 150; // 新增：规则按钮位置

    while (true)
    {
        cleardevice();
        setfillcolor(RGB(220, 235, 250)); 
        bar(0, 0, w, h);
        int img_x = (w - img_main.getwidth()) / 2;
        int img_y = (h - img_main.getheight()) / 2;
        putimage(img_x, img_y, &img_main);

        setbkmode(TRANSPARENT);
        settextcolor(RED);
        settextstyle(70, 0, _T("微软雅黑"));
        outtextxy(w / 2 - 200, 120, _T("圆哈镇历险记"));
        settextcolor(RED);
        settextstyle(40, 0, _T("微软雅黑"));
        outtextxy(w / 2 - 260, 190, _T("年轻的耄耋渴望建立功勋"));

        // 四个按钮：简单/普通/困难/规则
        setcolor(WHITE);
        rectangle(x, y1, x + bw, y1 + bh);
        rectangle(x, y2, x + bw, y2 + bh);
        rectangle(x, y3, x + bw, y3 + bh);
        rectangle(x, y4, x + bw, y4 + bh); // 规则按钮框

        settextstyle(32, 0, _T("微软雅黑"));
        outtextxy(x + 50, y1 + 10, _T("1 - 简单模式"));
        outtextxy(x + 50, y2 + 10, _T("2 - 普通模式"));
        outtextxy(x + 50, y3 + 10, _T("3 - 困难模式"));
        outtextxy(x + 60, y4 + 10, _T("游戏规则")); // 规则按钮文字

        FlushBatchDraw();

        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
            {
                int mx = m.x;
                int my = m.y;
                if (mx >= x && mx <= x + bw && my >= y1 && my <= y1 + bh) { diff = EASY; return; }
                if (mx >= x && mx <= x + bw && my >= y2 && my <= y2 + bh) { diff = NORMAL; return; }
                if (mx >= x && mx <= x + bw && my >= y3 && my <= y3 + bh) { diff = HARD; return; }
                // 点击规则按钮，进入规则弹窗
                if (mx >= x && mx <= x + bw && my >= y4 && my <= y4 + bh) { showRules(w, h); }
            }
        }
        if (GetAsyncKeyState('1') & 0x8000) { diff = EASY; Sleep(200); return; }
        if (GetAsyncKeyState('2') & 0x8000) { diff = NORMAL; Sleep(200); return; }
        if (GetAsyncKeyState('3') & 0x8000) { diff = HARD; Sleep(200); return; }
        if (GetAsyncKeyState('4') & 0x8000) { showRules(w, h); } // 键盘4也能打开规则
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { closegraph(); exit(0); }
        Sleep(10);
    }
}

int main()
{
    srand((unsigned int)time(NULL));
    int win_w = GetSystemMetrics(SM_CXSCREEN);
    int win_h = GetSystemMetrics(SM_CYSCREEN);

    initgraph(win_w, win_h, 0);
    HWND hwnd = GetHWnd();
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, win_w, win_h, SWP_NOZORDER);
    BeginBatchDraw();

    loadimage(&img_bg, _T("background.jpg"));
    loadimage(&img_main, _T("zhujiemian.jpg"));
    loadimage(&img_p1, _T("maodie1.jpg"));
    loadimage(&img_p2, _T("maodie2.jpg"));
    loadimage(&img_p3, _T("maodie3.jpg"));
    loadimage(&img_p4, _T("maodie4.jpg"));
    loadimage(&img_enemy, _T("dahuang.jpg"));

    img_p1.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p2.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p3.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p4.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_enemy.Resize(ENEMY_SIZE, ENEMY_SIZE);

    showMainUI(win_w, win_h);

    Player p(win_w, win_h);
    vector<Enemy> es;
    vector<Bullet> bs;
    DWORD t1 = 0, t2 = 0;
    bool over = false;
    int score = 0;
    TCHAR txt[50];

    while (true)
    {
        cleardevice();
        setfillcolor(RGB(30, 30, 50));
        bar(0, 0, win_w, win_h);
        int bg_x = (win_w - img_bg.getwidth()) / 2;
        int bg_y = (win_h - img_bg.getheight()) / 2;
        putimage(bg_x, bg_y, &img_bg);

        if (!over)
        {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) p.move(-P_SPEED, 0, win_w, win_h);
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) p.move(P_SPEED, 0, win_w, win_h);
            if (GetAsyncKeyState(VK_UP) & 0x8000) p.move(0, -P_SPEED, win_w, win_h);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) p.move(0, P_SPEED, win_w, win_h);
        }

        if (GetAsyncKeyState('R') & 0x8000) { p.reset(win_w, win_h); es.clear(); bs.clear(); score = 0; over = false; showMainUI(win_w, win_h); continue; }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { closegraph(); return 0; }

        if (!over)
        {
            if (GetTickCount() - t1 > diff.e_cd) { es.push_back({ rand() % (win_w - ENEMY_SIZE), 0 }); t1 = GetTickCount(); }
            if (GetTickCount() - t2 > diff.b_cd && !es.empty()) { int idx = rand() % (int)es.size(); bs.push_back({ es[idx].x + ENEMY_SIZE / 2, es[idx].y + ENEMY_SIZE }); t2 = GetTickCount(); }
            for (auto it = es.begin(); it != es.end();) { it->move(diff.e_speed); if (it->y > win_h) it = es.erase(it); else ++it; }
            for (auto it = bs.begin(); it != bs.end();) { it->move(diff.b_speed); if (it->y > win_h) it = bs.erase(it); else ++it; }

            RECT pr = p.rect();
            for (auto it = es.begin(); it != es.end();) { RECT er = it->rect(); bool hit = !(pr.right <= er.left || pr.left >= er.right || pr.bottom <= er.top || pr.top >= er.bottom); if (hit) { p.hp = max(0, p.hp - HURT); it = es.erase(it); } else ++it; }
            for (auto it = bs.begin(); it != bs.end();) { RECT br = it->rect(); bool hit = !(pr.right <= br.left || pr.left >= br.right || pr.bottom <= br.top || pr.top >= br.bottom); if (hit) { p.hp = max(0, p.hp - HURT); it = bs.erase(it); } else ++it; }
            if (p.hp <= 0) over = true;
            score++;
        }

        for (auto& e : es) putimage(e.x, e.y, &img_enemy);
        setfillcolor(WHITE); for (auto& b : bs) solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);
        setbkmode(TRANSPARENT);
        if (!over) { if (p.hp > 50) putimage(p.x, p.y, &img_p1); else if (p.hp > 20) putimage(p.x, p.y, &img_p2); else putimage(p.x, p.y, &img_p3); }
        else putimage(p.x, p.y, &img_p4);

        drawHP(p.hp);
        setcolor(WHITE); settextstyle(24, 0, _T("微软雅黑")); _stprintf_s(txt, _T("得分: %d"), score); outtextxy(20, 60, txt);
        if (over) { settextcolor(RED); settextstyle(36, 0, _T("微软雅黑")); outtextxy(win_w / 2 - 90, win_h / 2, _T("游戏结束")); outtextxy(win_w / 2 - 180, win_h / 2 + 50, _T("按 R 返回主界面，ESC 退出")); }

        FlushBatchDraw();
        Sleep(10);
    }
    closegraph();
    return 0;
}*/
/*#include <graphics.h>
#include <vector>
#include <ctime>
#include <windows.h>
using namespace std;

const int PLAYER_SIZE = 80;
const int ENEMY_SIZE = 50;
const int BULLET_SIZE = 10;
const int HP_BAG_SIZE = 30;

const int P_SPEED = 10;
const int BULLET_SPEED = 15;
const int HP_BAG_SPEED = 3;
const int MAX_HP = 100;
const int HURT = 10;
const int HEAL = 20;

struct Diff
{
    int e_speed;
    int b_speed;
    int e_cd;
    int b_cd;
} diff;

const Diff EASY = { 2, 4, 600, 500 };
const Diff NORMAL = { 3, 6, 400, 300 };
const Diff HARD = { 5, 8, 250, 200 };

IMAGE img_bg;
IMAGE img_main;
IMAGE img_p1, img_p2, img_p3, img_p4;
IMAGE img_enemy;

struct Player
{
    int x, y, hp;
    Player(int w, int h) : x(w / 2 - PLAYER_SIZE / 2),
        y(h / 2 + 150), hp(MAX_HP) {
    }

    void move(int dx, int dy, int w, int h)
    {
        x = max(0, min(w - PLAYER_SIZE, x + dx));
        y = max(0, min(h - PLAYER_SIZE, y + dy));
    }

    RECT rect() const { return { x, y, x + PLAYER_SIZE, y + PLAYER_SIZE }; }

    void reset(int w, int h)
    {
        x = w / 2 - PLAYER_SIZE / 2;
        y = h / 2 + 150;
        hp = MAX_HP;
    }
};

struct Enemy
{
    int x, y;
    void move(int s) { y += s; }
    RECT rect() const { return { x, y, x + ENEMY_SIZE, y + ENEMY_SIZE }; }
};

struct Bullet
{
    int x, y;
    void move(int s) { y += s; }
    RECT rect() const { return { x, y, x + BULLET_SIZE, y + BULLET_SIZE }; }
};

struct HpBag
{
    int x, y;
    void move(int s) { y += s; }
    RECT rect() const { return { x, y, x + HP_BAG_SIZE, y + HP_BAG_SIZE }; }
};

void drawHP(int hp)
{
    setfillcolor(LIGHTGRAY);
    bar(20, 20, 260, 50);
    if (hp > 50) setfillcolor(GREEN);
    else if (hp > 20) setfillcolor(YELLOW);
    else setfillcolor(RED);
    bar(20, 20, 20 + (int)(2.4 * hp), 50);
    setcolor(WHITE);
    rectangle(20, 20, 260, 50);
}

void showRules(int w, int h)
{
    setbkmode(TRANSPARENT);
    while (true)
    {
        cleardevice();
        setfillcolor(RGB(0, 0, 0));
        bar(0, 0, w, h);
        settextcolor(WHITE);

        settextstyle(36, 0, _T("微软雅黑"));
        const TCHAR* title = _T("游戏规则");
        int title_w = textwidth(title);
        outtextxy((w - title_w) / 2, h / 2 - 150, title);

        settextstyle(22, 0, _T("微软雅黑"));
        const TCHAR* line1 = _T("方向键控制角色移动");
        const TCHAR* line2 = _T("按空格键发射子弹攻击敌机");
        const TCHAR* line3 = _T("躲避敌机和子弹，收集血包恢复血量");
        const TCHAR* line4 = _T("按R返回主界面，ESC退出游戏");

        int line1_w = textwidth(line1);
        int line2_w = textwidth(line2);
        int line3_w = textwidth(line3);
        int line4_w = textwidth(line4);

        outtextxy((w - line1_w) / 2, h / 2 - 80, line1);
        outtextxy((w - line2_w) / 2, h / 2 - 40, line2);
        outtextxy((w - line3_w) / 2, h / 2, line3);
        outtextxy((w - line4_w) / 2, h / 2 + 40, line4);

        settextstyle(20, 0, _T("微软雅黑"));
        const TCHAR* tip = _T("点击任意处返回");
        int tip_w = textwidth(tip);
        outtextxy((w - tip_w) / 2, h / 2 + 120, tip);

        FlushBatchDraw();
        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN) return;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return;
        Sleep(10);
    }
}

void showMainUI(int w, int h)
{
    int bw = 240;
    int bh = 60;
    int x = w / 2 - bw / 2;
    int y1 = h / 2 - 150;
    int y2 = h / 2 - 50;
    int y3 = h / 2 + 50;
    int y4 = h / 2 + 150;

    while (true)
    {
        cleardevice();
        setfillcolor(RGB(220, 235, 250));
        bar(0, 0, w, h);
        int img_x = (w - img_main.getwidth()) / 2;
        int img_y = (h - img_main.getheight()) / 2;
        putimage(img_x, img_y, &img_main);

        setbkmode(TRANSPARENT);
        settextcolor(RED);
        settextstyle(70, 0, _T("微软雅黑"));
        outtextxy(w / 2 - 200, 120, _T("圆哈镇历险记"));
        settextcolor(RED);
        settextstyle(40, 0, _T("微软雅黑"));
        outtextxy(w / 2 - 260, 190, _T("年轻的耄耋渴望建立功勋"));

        setcolor(WHITE);
        rectangle(x, y1, x + bw, y1 + bh);
        rectangle(x, y2, x + bw, y2 + bh);
        rectangle(x, y3, x + bw, y3 + bh);
        rectangle(x, y4, x + bw, y4 + bh);

        settextstyle(32, 0, _T("微软雅黑"));
        outtextxy(x + 50, y1 + 10, _T("1 - 简单模式"));
        outtextxy(x + 50, y2 + 10, _T("2 - 普通模式"));
        outtextxy(x + 50, y3 + 10, _T("3 - 困难模式"));
        outtextxy(x + 60, y4 + 10, _T("游戏规则"));

        FlushBatchDraw();
        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
            {
                int mx = m.x;
                int my = m.y;
                if (mx >= x && mx <= x + bw && my >= y1 && my <= y1 + bh) { diff = EASY; return; }
                if (mx >= x && mx <= x + bw && my >= y2 && my <= y2 + bh) { diff = NORMAL; return; }
                if (mx >= x && mx <= x + bw && my >= y3 && my <= y3 + bh) { diff = HARD; return; }
                if (mx >= x && mx <= x + bw && my >= y4 && my <= y4 + bh) { showRules(w, h); }
            }
        }
        if (GetAsyncKeyState('1') & 0x8000) { diff = EASY; Sleep(200); return; }
        if (GetAsyncKeyState('2') & 0x8000) { diff = NORMAL; Sleep(200); return; }
        if (GetAsyncKeyState('3') & 0x8000) { diff = HARD; Sleep(200); return; }
        if (GetAsyncKeyState('4') & 0x8000) { showRules(w, h); }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { closegraph(); exit(0); }
        Sleep(10);
    }
}

int main()
{
    srand((unsigned int)time(NULL));
    int win_w = GetSystemMetrics(SM_CXSCREEN);
    int win_h = GetSystemMetrics(SM_CYSCREEN);

    initgraph(win_w, win_h, 0);
    HWND hwnd = GetHWnd();
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, win_w, win_h, SWP_NOZORDER);
    BeginBatchDraw();

    loadimage(&img_bg, _T("background.jpg"));
    loadimage(&img_main, _T("zhujiemian.jpg"));
    loadimage(&img_p1, _T("maodie1.jpg"));
    loadimage(&img_p2, _T("maodie2.jpg"));
    loadimage(&img_p3, _T("maodie3.jpg"));
    loadimage(&img_p4, _T("maodie4.jpg"));
    loadimage(&img_enemy, _T("dahuang.jpg"));

    img_p1.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p2.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p3.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p4.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_enemy.Resize(ENEMY_SIZE, ENEMY_SIZE);

    showMainUI(win_w, win_h);

    Player p(win_w, win_h);
    vector<Enemy> es;
    vector<Bullet> bs_enemy;
    vector<Bullet> bs_player;
    vector<HpBag> hp_bags;
    DWORD t1 = 0, t2 = 0, t3 = 0;
    bool over = false;
    int score = 0;
    TCHAR txt[50];

    while (true)
    {
        cleardevice();
        setfillcolor(RGB(30, 30, 50));
        bar(0, 0, win_w, win_h);
        int bg_x = (win_w - img_bg.getwidth()) / 2;
        int bg_y = (win_h - img_bg.getheight()) / 2;
        putimage(bg_x, bg_y, &img_bg);

        if (!over)
        {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) p.move(-P_SPEED, 0, win_w, win_h);
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) p.move(P_SPEED, 0, win_w, win_h);
            if (GetAsyncKeyState(VK_UP) & 0x8000) p.move(0, -P_SPEED, win_w, win_h);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) p.move(0, P_SPEED, win_w, win_h);

            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                static DWORD last_shot = 0;
                if (GetTickCount() - last_shot > 200)
                {
                    bs_player.push_back({ p.x + PLAYER_SIZE / 2 - BULLET_SIZE / 2, p.y });
                    last_shot = GetTickCount();
                }
            }
        }

        if (GetAsyncKeyState('R') & 0x8000)
        {
            p.reset(win_w, win_h);
            es.clear();
            bs_enemy.clear();
            bs_player.clear();
            hp_bags.clear();
            score = 0;
            over = false;
            showMainUI(win_w, win_h);
            continue;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            closegraph();
            return 0;
        }

        if (!over)
        {
            if (GetTickCount() - t1 > diff.e_cd)
            {
                es.push_back({ rand() % (win_w - ENEMY_SIZE), 0 });
                t1 = GetTickCount();
            }
            if (GetTickCount() - t2 > diff.b_cd && !es.empty())
            {
                int idx = rand() % (int)es.size();
                bs_enemy.push_back({ es[idx].x + ENEMY_SIZE / 2, es[idx].y + ENEMY_SIZE });
                t2 = GetTickCount();
            }
            if (GetTickCount() - t3 > 5000)
            {
                hp_bags.push_back({ rand() % (win_w - HP_BAG_SIZE), 0 });
                t3 = GetTickCount();
            }

            for (auto it = es.begin(); it != es.end();)
            {
                it->move(diff.e_speed);
                if (it->y > win_h) it = es.erase(it); else ++it;
            }
            for (auto it = bs_enemy.begin(); it != bs_enemy.end();)
            {
                it->move(diff.b_speed);
                if (it->y > win_h) it = bs_enemy.erase(it); else ++it;
            }
            for (auto it = bs_player.begin(); it != bs_player.end();)
            {
                it->move(-BULLET_SPEED);
                if (it->y < 0) it = bs_player.erase(it); else ++it;
            }
            for (auto it = hp_bags.begin(); it != hp_bags.end();)
            {
                it->move(HP_BAG_SPEED);
                if (it->y > win_h) it = hp_bags.erase(it); else ++it;
            }

            for (auto it_p = bs_player.begin(); it_p != bs_player.end();)
            {
                bool hit = false;
                for (auto it_e = es.begin(); it_e != es.end();)
                {
                    RECT pr = { it_p->x, it_p->y, it_p->x + BULLET_SIZE, it_p->y + BULLET_SIZE };
                    RECT er = it_e->rect();
                    if (!(pr.right <= er.left || pr.left >= er.right || pr.bottom <= er.top || pr.top >= er.bottom))
                    {
                        hit = true;
                        it_e = es.erase(it_e);
                        score += 10;
                        break;
                    }
                    else ++it_e;
                }
                if (hit) it_p = bs_player.erase(it_p); else ++it_p;
            }

            RECT pr = p.rect();
            for (auto it = es.begin(); it != es.end();)
            {
                RECT er = it->rect();
                bool hit = !(pr.right <= er.left || pr.left >= er.right || pr.bottom <= er.top || pr.top >= er.bottom);
                if (hit) { p.hp = max(0, p.hp - HURT); it = es.erase(it); }
                else ++it;
            }
            for (auto it = bs_enemy.begin(); it != bs_enemy.end();)
            {
                RECT br = it->rect();
                bool hit = !(pr.right <= br.left || pr.left >= br.right || pr.bottom <= br.top || pr.top >= br.bottom);
                if (hit) { p.hp = max(0, p.hp - HURT); it = bs_enemy.erase(it); }
                else ++it;
            }
            for (auto it = hp_bags.begin(); it != hp_bags.end();)
            {
                RECT hr = it->rect();
                bool hit = !(pr.right <= hr.left || pr.left >= hr.right || pr.bottom <= hr.top || pr.top >= hr.bottom);
                if (hit)
                {
                    p.hp = min(MAX_HP, p.hp + HEAL);
                    it = hp_bags.erase(it);
                }
                else ++it;
            }

            if (p.hp <= 0) over = true;
            score++;
        }

        for (auto& e : es) putimage(e.x, e.y, &img_enemy);

        setfillcolor(WHITE);
        for (auto& b : bs_enemy) solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);

        setfillcolor(YELLOW);
        for (auto& b : bs_player) solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);

        // 绿色十字血包（纯代码绘制）
        setfillcolor(GREEN);
        for (auto& hp : hp_bags)
        {
            int cx = hp.x + HP_BAG_SIZE / 2;
            int cy = hp.y + HP_BAG_SIZE / 2;
            solidrectangle(hp.x, cy - 2, hp.x + HP_BAG_SIZE, cy + 2);
            solidrectangle(cx - 2, hp.y, cx + 2, hp.y + HP_BAG_SIZE);
        }

        setbkmode(TRANSPARENT);
        if (!over)
        {
            if (p.hp > 50)      putimage(p.x, p.y, &img_p1);
            else if (p.hp > 20) putimage(p.x, p.y, &img_p2);
            else                putimage(p.x, p.y, &img_p3);
        }
        else
        {
            putimage(p.x, p.y, &img_p4);
        }

        drawHP(p.hp);
        setcolor(WHITE);
        settextstyle(24, 0, _T("微软雅黑"));
        _stprintf_s(txt, _T("得分: %d"), score);
        outtextxy(20, 60, txt);

        if (over)
        {
            settextcolor(RED);
            settextstyle(36, 0, _T("微软雅黑"));
            outtextxy(win_w / 2 - 90, win_w / 2, _T("游戏结束"));
            outtextxy(win_w / 2 - 180, win_w / 2 + 50, _T("按 R 返回主界面，ESC 退出"));
        }

        FlushBatchDraw();
        Sleep(10);
    }

    closegraph();
    return 0;
}*/