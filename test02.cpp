#include <graphics.h>
#include <vector>
#include <ctime>
#include <windows.h>
using namespace std;

const int PLAYER_SIZE = 80;
const int ENEMY_SIZE = 50;
const int BULLET_SIZE = 10;

const int P_SPEED = 10;
const int E_SPEED = 3;
const int B_SPEED = 6;
const int MAX_HP = 100;
const int HURT = 10;

IMAGE img_bg;
IMAGE img_p1, img_p2, img_p3, img_p4;
IMAGE img_enemy;

struct Player {
    int x, y, hp;
    Player() : x(GetSystemMetrics(SM_CXSCREEN) / 2 - PLAYER_SIZE / 2),
        y(GetSystemMetrics(SM_CYSCREEN) / 2 + 150), hp(MAX_HP) {
    }

    void move(int dx, int dy) {
        int win_w = GetSystemMetrics(SM_CXSCREEN);
        int win_h = GetSystemMetrics(SM_CYSCREEN);
        x = max(0, min(win_w - PLAYER_SIZE, x + dx));
        y = max(0, min(win_h - PLAYER_SIZE, y + dy));
    }

    RECT rect() const {
        return { x, y, x + PLAYER_SIZE, y + PLAYER_SIZE };
    }

    void reset() {
        x = GetSystemMetrics(SM_CXSCREEN) / 2 - PLAYER_SIZE / 2;
        y = GetSystemMetrics(SM_CYSCREEN) / 2 + 150;
        hp = MAX_HP;
    }
};

struct Enemy {
    int x, y;
    void move() { y += E_SPEED; }
    RECT rect() const {
        return { x, y, x + ENEMY_SIZE, y + ENEMY_SIZE };
    }
};

struct Bullet {
    int x, y;
    void move() { y += B_SPEED; }
    RECT rect() const {
        return { x, y, x + BULLET_SIZE, y + BULLET_SIZE };
    }
};

void drawHP(int hp) {
    setfillcolor(LIGHTGRAY);
    bar(20, 20, 260, 50);
    if (hp > 50) setfillcolor(GREEN);
    else if (hp > 20) setfillcolor(YELLOW);
    else setfillcolor(RED);
    bar(20, 20, 20 + (int)(2.4 * hp), 50);
    setcolor(WHITE);
    rectangle(20, 20, 260, 50);
}

int main() {
    srand((unsigned int)time(NULL));
    int win_w = GetSystemMetrics(SM_CXSCREEN);
    int win_h = GetSystemMetrics(SM_CYSCREEN);
    initgraph(win_w, win_h);

    // 加载图片：敌机改为 dahuang.jpg
    loadimage(&img_bg, _T("background.jpg"));
    loadimage(&img_p1, _T("maodie.jpg"));
    loadimage(&img_p2, _T("maodie2.jpg"));
    loadimage(&img_p3, _T("maodie3.jpg"));
    loadimage(&img_p4, _T("maodie4.jpg"));
    loadimage(&img_enemy, _T("dahuang.jpg"));

    img_p1.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p2.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p3.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_p4.Resize(PLAYER_SIZE, PLAYER_SIZE);
    img_enemy.Resize(ENEMY_SIZE, ENEMY_SIZE);

    BeginBatchDraw();
    Player p;
    vector<Enemy> es;
    vector<Bullet> bs;
    DWORD t1 = 0, t2 = 0;
    bool over = false;
    int score = 0;
    TCHAR txt[50];

    while (1) {
        cleardevice();
        putimage(0, 0, win_w, win_h, &img_bg, 0, 0);

        if (!over) {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) p.move(-P_SPEED, 0);
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) p.move(P_SPEED, 0);
            if (GetAsyncKeyState(VK_UP) & 0x8000) p.move(0, -P_SPEED);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) p.move(0, P_SPEED);
        }

        if (GetAsyncKeyState('R') & 0x8000) {
            p.reset(); es.clear(); bs.clear(); score = 0; over = false;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            closegraph(); return 0;
        }

        if (!over) {
            if (GetTickCount() - t1 > 400) {
                es.push_back({ rand() % (win_w - ENEMY_SIZE), 0 });
                t1 = GetTickCount();
            }
            if (GetTickCount() - t2 > 300 && !es.empty()) {
                int idx = rand() % (int)es.size();
                bs.push_back({ es[idx].x + ENEMY_SIZE / 2, es[idx].y + ENEMY_SIZE });
                t2 = GetTickCount();
            }

            for (auto it = es.begin(); it != es.end();) {
                it->move();
                if (it->y > win_h) it = es.erase(it); else ++it;
            }
            for (auto it = bs.begin(); it != bs.end();) {
                it->move();
                if (it->y > win_h) it = bs.erase(it); else ++it;
            }

            RECT pr = p.rect();
            for (auto it = es.begin(); it != es.end();) {
                RECT er = it->rect();
                bool hit = !(pr.right <= er.left || pr.left >= er.right || pr.bottom <= er.top || pr.top >= er.bottom);
                if (hit) { p.hp = max(0, p.hp - HURT); it = es.erase(it); }
                else ++it;
            }
            for (auto it = bs.begin(); it != bs.end();) {
                RECT br = it->rect();
                bool hit = !(pr.right <= br.left || pr.left >= br.right || pr.bottom <= br.top || pr.top >= br.bottom);
                if (hit) { p.hp = max(0, p.hp - HURT); it = bs.erase(it); }
                else ++it;
            }

            if (p.hp <= 0) over = true;
            score++;
        }

        // 绘制敌机贴图 dahuang
        for (auto& e : es) {
            putimage(e.x, e.y, &img_enemy);
        }

        setfillcolor(WHITE);
        for (auto& b : bs)
            solidrectangle(b.x, b.y, b.x + BULLET_SIZE, b.y + BULLET_SIZE);

        // 飞机正常切换 + 失败显示maodie4
        if (!over) {
            if (p.hp > 50) putimage(p.x, p.y, &img_p1);
            else if (p.hp > 20) putimage(p.x, p.y, &img_p2);
            else putimage(p.x, p.y, &img_p3);
        }
        else {
            putimage(p.x, p.y, &img_p4);
        }

        drawHP(p.hp);
        settextstyle(24, 0, _T("微软雅黑"));
        _stprintf_s(txt, _T("得分: %d"), score);
        outtextxy(20, 60, txt);

        if (over) {
            setcolor(RED);
            settextstyle(36, 0, _T("微软雅黑"));
            outtextxy(win_w / 2 - 90, win_h / 2, _T("已经....不用再哈气了"));
            outtextxy(win_w / 2 - 110, win_h / 2 + 50, _T("按 R 重新开始，ESC 退出"));
        }

        FlushBatchDraw();
        Sleep(10);
    }
    closegraph();
    return 0;
}