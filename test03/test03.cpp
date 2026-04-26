#include <graphics.h>
#include <vector>
#include <ctime>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")  

using namespace std;

const int PLAYER_SIZE = 80;
const int ENEMY_SIZE = 50;
const int BULLET_SIZE = 10;
const int HP_BAG_SIZE = 30;
const int STAR_SIZE = 50;

const int P_SPEED = 10;
const int BULLET_SPEED = 15;
const int HP_BAG_SPEED = 3;
const int STAR_SPEED = 3;
const int MAX_HP = 100;
const int HURT = 10;
const int HEAL = 10;

const int NEED_STAR = 3;
const int ATTACK_DURATION = 10;

int WIN_SCORE = 0;

struct Diff
{
    int e_speed;
    int b_speed;
    int e_cd;
    int b_cd;
};

Diff diff;
const Diff EASY = { 3, 5, 600, 450 };
const Diff NORMAL = { 5, 7, 350, 250 };
const Diff HARD = { 7, 10, 200, 150 };

IMAGE img_bg;
IMAGE img_main;
IMAGE img_intro;
IMAGE img_p1, img_p2, img_p3, img_p4, img_p5;
IMAGE img_enemy;
IMAGE img_star;
IMAGE img_win;

void showMainUI(int w, int h);

struct Player
{
    int x, y, hp;
    Player(int w, int h)
        : x(w / 2 - PLAYER_SIZE / 2),
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

struct Star
{
    int x, y;
    void move(int s) { y += s; }
    RECT rect() const { return { x, y, x + STAR_SIZE, y + STAR_SIZE }; }
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

void showIntro(int w, int h)
{
    setbkmode(TRANSPARENT);
    while (true)
    {
        cleardevice();
        putimage(0, 0, &img_intro);

        settextcolor(RED);
        settextstyle(36, 0, _T("微软雅黑"));
        const TCHAR* title = _T("圆哈镇历险记");
        int title_w = textwidth(title);
        outtextxy((w - title_w) / 2, h / 2 - 90, title);

        settextstyle(24, 0, _T("微软雅黑"));
        const TCHAR* line1 = _T("圆哈镇爆发终极对决，耄耋与大狗嚼展开激烈攻防。");
        const TCHAR* line2 = _T("操控耄耋战机，在趣味关卡中对决，");
        const TCHAR* line3 = _T("吸收大黄获取攻击能力，同时躲避大狗嚼的攻击，");
        const TCHAR* line4 = _T("争夺小镇霸主，体验爽快乱斗！");

        outtextxy((w - textwidth(line1)) / 2, h / 2 - 40, line1);
        outtextxy((w - textwidth(line2)) / 2, h / 2 - 10, line2);
        outtextxy((w - textwidth(line3)) / 2, h / 2 + 20, line3);
        outtextxy((w - textwidth(line4)) / 2, h / 2 + 50, line4);

        settextstyle(24, 0, _T("微软雅黑"));
        outtextxy(w / 2 - 120, h - 60, _T("点击任意位置开始游戏"));

        FlushBatchDraw();

        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
                return;
        }
        Sleep(20);
    }
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
        const TCHAR* line2 = _T("绿色十字：恢复血量");
        const TCHAR* line3 = _T("收集3个大黄猫：解锁10秒攻击");
        const TCHAR* line4 = _T("攻击期间角色变为哈气形态");
        const TCHAR* line5 = _T("空格发射子弹，胜利或失败后点击界面返回主界面");
        const TCHAR* line6 = _T("简单10000分/普通7000分/困难5000分获胜");

        outtextxy((w - textwidth(line1)) / 2, h / 2 - 100, line1);
        outtextxy((w - textwidth(line2)) / 2, h / 2 - 60, line2);
        outtextxy((w - textwidth(line3)) / 2, h / 2 - 20, line3);
        outtextxy((w - textwidth(line4)) / 2, h / 2 + 20, line4);
        outtextxy((w - textwidth(line5)) / 2, h / 2 + 60, line5);
        outtextxy((w - textwidth(line6)) / 2, h / 2 + 100, line6);

        settextstyle(20, 0, _T("微软雅黑"));
        const TCHAR* tip = _T("点击任意处返回");
        outtextxy((w - textwidth(tip)) / 2, h / 2 + 160, tip);

        FlushBatchDraw();

        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
                return;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            return;

        Sleep(20);
    }
}

// 胜利界面（不再停止bgm）
void showWin(int w, int h)
{
    setbkmode(TRANSPARENT);
    while (true)
    {
        cleardevice();

        int img_w = 400;
        int img_h = 320;
        img_win.Resize(img_w, img_h);

        int img_x = (w - img_w) / 2;
        int img_y = (h - img_h) / 2 - 50;
        putimage(img_x, img_y, &img_win);

        settextcolor(WHITE);
        settextstyle(42, 0, _T("微软雅黑"));
        const TCHAR* winText = _T("恭喜你，在大狗嚼的进攻下成功守护圆哈镇");
        int text_w = textwidth(winText);
        int text_x = (w - text_w) / 2;
        int text_y = img_y + img_h + 30;
        outtextxy(text_x, text_y, winText);

        settextstyle(24, 0, _T("微软雅黑"));
        const TCHAR* tip = _T("点击任意位置返回主界面");
        int tip_w = textwidth(tip);
        int tip_x = (w - tip_w) / 2;
        int tip_y = h - 80;
        outtextxy(tip_x, tip_y, tip);

        FlushBatchDraw();

        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
            {
                showMainUI(w, h);
                return;
            }
        }
        Sleep(20);
    }
}

void showMainUI(int w, int h)
{
    int bw = 240, bh = 60;
    int x = w / 2 - bw / 2;
    int y1 = h / 2 - 150;
    int y2 = h / 2 - 50;
    int y3 = h / 2 + 50;
    int y4 = h / 2 + 150;

    DWORD lastKeyTime = 0;

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
        const TCHAR* title = _T("圆哈镇历险记");
        int title_w = textwidth(title);
        outtextxy((w - title_w) / 2, 120, title);

        settextstyle(40, 0, _T("微软雅黑"));
        const TCHAR* subtitle = _T("年轻的耄耋渴望建立功勋");
        int subtitle_w = textwidth(subtitle);
        outtextxy((w - subtitle_w) / 2, 190, subtitle);

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
            int mx = m.x, my = m.y;
            if (m.uMsg == WM_LBUTTONDOWN)
            {
                if (mx >= x && mx <= x + bw && my >= y1 && my <= y1 + bh)
                {
                    diff = EASY;
                    WIN_SCORE = 10000;
                    return;
                }
                if (mx >= x && mx <= x + bw && my >= y2 && my <= y2 + bh)
                {
                    diff = NORMAL;
                    WIN_SCORE = 7000;
                    return;
                }
                if (mx >= x && mx <= x + bw && my >= y3 && my <= y3 + bh)
                {
                    diff = HARD;
                    WIN_SCORE = 5000;
                    return;
                }
                if (mx >= x && mx <= x + bw && my >= y4 && my <= y4 + bh)
                {
                    showRules(w, h);
                }
            }
        }

        DWORD now = GetTickCount();
        if (now - lastKeyTime > 200)
        {
            if (GetAsyncKeyState('1') & 0x8000)
            {
                diff = EASY; WIN_SCORE = 10000; lastKeyTime = now; return;
            }
            if (GetAsyncKeyState('2') & 0x8000)
            {
                diff = NORMAL; WIN_SCORE = 7000; lastKeyTime = now; return;
            }
            if (GetAsyncKeyState('3') & 0x8000)
            {
                diff = HARD; WIN_SCORE = 5000; lastKeyTime = now; return;
            }
            if (GetAsyncKeyState('4') & 0x8000)
            {
                showRules(w, h); lastKeyTime = now;
            }
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            closegraph();
            exit(0);
        }

        Sleep(20);
    }
}

int main()
{
    // 全程循环播放，只在最后退出时关闭
    mciSendString(L"open bgm.mp3 alias testmusic", NULL, 0, NULL);
    mciSendString(L"play testmusic repeat", NULL, 0, NULL);

    srand((unsigned int)time(NULL));
    int win_w = GetSystemMetrics(SM_CXSCREEN);
    int win_h = GetSystemMetrics(SM_CYSCREEN);

    initgraph(win_w, win_h, 0);
    HWND hwnd = GetHWnd();
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, win_w, win_h, SWP_NOZORDER);
    BeginBatchDraw();

    loadimage(&img_intro, _T("intro.jpg"));
    loadimage(&img_bg, _T("background.jpg"));
    loadimage(&img_main, _T("zhujiemian.jpg"));
    loadimage(&img_p1, _T("maodie1.jpg"));
    loadimage(&img_p2, _T("maodie2.jpg"));
    loadimage(&img_p3, _T("maodie3.jpg"));
    loadimage(&img_p4, _T("maodie4.jpg"));
    loadimage(&img_p5, _T("maodie5.jpg"));
    loadimage(&img_enemy, _T("dagoujiao.jpg"));
    loadimage(&img_star, _T("dahuang.jpg"));
    loadimage(&img_win, _T("win.jpg"));

    img_intro.Resize(win_w, win_h);
    img_win.Resize(win_w, win_h);
    img_p1.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p2.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p3.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p4.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p5.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_enemy.Resize(ENEMY_SIZE, ENEMY_SIZE);
    img_star.Resize(STAR_SIZE, STAR_SIZE);

    showIntro(win_w, win_h);
    showMainUI(win_w, win_h);

    Player p(win_w, win_h);
    vector<Enemy> es;
    vector<Bullet> bs_enemy;
    vector<Bullet> bs_player;
    vector<HpBag> hp_bags;
    vector<Star> stars;

    DWORD t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    bool over = false;
    int score = 0;
    TCHAR txt[50];

    int collected_star = 0;
    bool attack_unlocked = false;
    int attack_remain = ATTACK_DURATION;
    DWORD attack_timer = 0;

    while (true)
    {
        cleardevice();
        setfillcolor(RGB(30, 30, 50));
        bar(0, 0, win_w, win_h);
        int bg_x = (win_w - img_bg.getwidth()) / 2;
        int bg_y = (win_h - img_bg.getheight()) / 2;
        putimage(bg_x, bg_y, &img_bg);

        if (score >= WIN_SCORE)
        {
            showWin(win_w, win_h);
            p.reset(win_w, win_h);
            es.clear(); bs_enemy.clear(); bs_player.clear();
            hp_bags.clear(); stars.clear();
            collected_star = 0; attack_unlocked = false;
            attack_remain = ATTACK_DURATION; attack_timer = 0;
            score = 0; over = false;
        }

        if (!over)
        {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000)
                p.move(-P_SPEED, 0, win_w, win_h);
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
                p.move(P_SPEED, 0, win_w, win_h);
            if (GetAsyncKeyState(VK_UP) & 0x8000)
                p.move(0, -P_SPEED, win_w, win_h);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000)
                p.move(0, P_SPEED, win_w, win_h);

            if (attack_unlocked && (GetAsyncKeyState(VK_SPACE) & 0x8000))
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
            es.clear(); bs_enemy.clear(); bs_player.clear();
            hp_bags.clear(); stars.clear();
            collected_star = 0;
            attack_unlocked = false;
            attack_remain = ATTACK_DURATION;
            attack_timer = 0;
            score = 0;
            over = false;
            showMainUI(win_w, win_h);
            continue;
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            break;
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
                int idx = rand() % es.size();
                bs_enemy.push_back({ es[idx].x + ENEMY_SIZE / 2, es[idx].y + ENEMY_SIZE });
                t2 = GetTickCount();
            }
            if (GetTickCount() - t3 > 5000)
            {
                hp_bags.push_back({ rand() % (win_w - HP_BAG_SIZE), 0 });
                t3 = GetTickCount();
            }
            if (GetTickCount() - t4 > 6000)
            {
                stars.push_back({ rand() % (win_w - STAR_SIZE), 0 });
                t4 = GetTickCount();
            }

            if (attack_unlocked && GetTickCount() - attack_timer > 1000)
            {
                attack_remain--;
                attack_timer = GetTickCount();
                if (attack_remain <= 0)
                {
                    attack_unlocked = false;
                    collected_star = 0;
                }
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
            for (auto it = stars.begin(); it != stars.end();)
            {
                it->move(STAR_SPEED);
                if (it->y > win_h) it = stars.erase(it); else ++it;
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
                if (hit)
                {
                    p.hp = max(0, p.hp - HURT);
                    it = es.erase(it);
                }
                else ++it;
            }
            for (auto it = bs_enemy.begin(); it != bs_enemy.end();)
            {
                RECT br = it->rect();
                bool hit = !(pr.right <= br.left || pr.left >= br.right || pr.bottom <= br.top || pr.top >= br.bottom);
                if (hit)
                {
                    p.hp = max(0, p.hp - HURT);
                    it = bs_enemy.erase(it);
                }
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
            for (auto it = stars.begin(); it != stars.end();)
            {
                RECT sr = it->rect();
                bool hit = !(pr.right <= sr.left || pr.left >= sr.right || pr.bottom <= sr.top || pr.top >= sr.bottom);
                if (hit)
                {
                    collected_star++;
                    it = stars.erase(it);
                    if (collected_star >= NEED_STAR)
                    {
                        attack_unlocked = true;
                        attack_remain = ATTACK_DURATION;
                        attack_timer = GetTickCount();
                        collected_star = 0;
                    }
                }
                else ++it;
            }

            if (p.hp <= 0) over = true;
            score++;
        }

        for (auto& e : es) putimage(e.x, e.y, &img_enemy);

        setfillcolor(WHITE);
        for (auto& b : bs_enemy)
            solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);

        setfillcolor(YELLOW);
        for (auto& b : bs_player)
            solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);

        setfillcolor(GREEN);
        for (auto& hp : hp_bags)
        {
            int cx = hp.x + HP_BAG_SIZE / 2;
            int cy = hp.y + HP_BAG_SIZE / 2;
            solidrectangle(hp.x, cy - 2, hp.x + HP_BAG_SIZE, cy + 2);
            solidrectangle(cx - 2, hp.y, cx + 2, hp.y + HP_BAG_SIZE);
        }

        for (auto& s : stars)
            putimage(s.x, s.y, &img_star);

        setbkmode(TRANSPARENT);
        if (attack_unlocked)
            putimage(p.x, p.y, &img_p5);
        else
        {
            if (!over)
            {
                if (p.hp > 50)      putimage(p.x, p.y, &img_p1);
                else if (p.hp > 20) putimage(p.x, p.y, &img_p2);
                else                putimage(p.x, p.y, &img_p3);
            }
            else
                putimage(p.x, p.y, &img_p4);
        }

        drawHP(p.hp);
        setcolor(WHITE);
        settextstyle(24, 0, _T("微软雅黑"));
        _stprintf_s(txt, _T("得分: %d / %d"), score, WIN_SCORE);
        outtextxy(20, 70, txt);
        _stprintf_s(txt, _T("dahuang: %d/%d"), collected_star, NEED_STAR);
        outtextxy(20, 100, txt);

        if (attack_unlocked)
        {
            _stprintf_s(txt, _T("攻击剩余: %d秒"), attack_remain);
            outtextxy(20, 130, txt);
        }

        if (over)
        {
            settextcolor(RED);
            settextstyle(36, 0, _T("微软雅黑"));
            const TCHAR* overTxt = _T("已经...不用再哈气了");
            outtextxy((win_w - textwidth(overTxt)) / 2, win_h / 2, overTxt);

            settextstyle(24, 0, _T("微软雅黑"));
            const TCHAR* tip = _T("点击任意位置返回主界面");
            outtextxy((win_w - textwidth(tip)) / 2, win_h / 2 + 50, tip);

            FlushBatchDraw();

            if (MouseHit())
            {
                MOUSEMSG m = GetMouseMsg();
                if (m.uMsg == WM_LBUTTONDOWN)
                {
                    p.reset(win_w, win_h);
                    es.clear(); bs_enemy.clear(); bs_player.clear();
                    hp_bags.clear(); stars.clear();
                    collected_star = 0; attack_unlocked = false;
                    attack_remain = ATTACK_DURATION; attack_timer = 0;
                    score = 0; over = false;

                    showMainUI(win_w, win_h);
                }
            }
        }

        FlushBatchDraw();
        Sleep(10);
    }

    // 只在真正退出程序时才停止并关闭
    mciSendString(L"stop testmusic", NULL, 0, NULL);
    mciSendString(L"close testmusic", NULL, 0, NULL);
    closegraph();
    return 0;
}