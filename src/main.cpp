#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>
using namespace std;

int main() {
    // Map (1 - wall, 0 - none)
    vector<string> mapGrid = {
        "1111111111111111",
        "1..............1",
        "1..111...11....1",
        "1..1.......1...1",
        "1..1..11...1...1",
        "1....1......1..1",
        "1....1......1..1",
        "1....11111111..1",
        "1..............1",
        "1..11......11..1",
        "1..1........1..1",
        "1..1...11...1..1",
        "1..............1",
        "1..............1",
        "1..............1",
        "1111111111111111"
    };
    const int mapW = mapGrid[0].size();
    const int mapH = mapGrid.size();

    const int screenW = 120;
    const int screenH = 40;

    double px = 8.0, py = 8.0;
    double dirX = -1.0, dirY = 0.0;
    double planeX = 0.0, planeY = 0.66;

    string screen(screenW * screenH, ' ');

    auto render = [&]() {
        fill(screen.begin(), screen.end(), ' ');
        for (int x = 0; x < screenW; ++x) {
            double cameraX = 2.0 * x / double(screenW) - 1.0;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = int(px);
            int mapY = int(py);

            double sideDistX, sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1.0 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1.0 / rayDirY);
            double perpWallDist;

            int stepX, stepY;
            int hit = 0;
            int side = 0;

            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (px - mapX) * deltaDistX;
            } else {
                stepX = 1;
                sideDistX = (mapX + 1.0 - px) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (py - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0 - py) * deltaDistY;
            }

            while (!hit) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                if (mapX < 0 || mapX >= mapW || mapY < 0 || mapY >= mapH) {
                    hit = 1;
                    perpWallDist = 1e30;
                    break;
                }
                if (mapGrid[mapY][mapX] == '1') hit = 1;
            }

            if (side == 0)
                perpWallDist = (mapX - px + (1 - stepX) / 2.0) / rayDirX;
            else
                perpWallDist = (mapY - py + (1 - stepY) / 2.0) / rayDirY;

            int lineHeight = (perpWallDist > 0) ? int(screenH / perpWallDist) : screenH;

            int drawStart = -lineHeight / 2 + screenH / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + screenH / 2;
            if (drawEnd >= screenH) drawEnd = screenH - 1;

            char shade;
            if (perpWallDist <= 1) shade = '#';
            else if (perpWallDist < 2) shade = 'X';
            else if (perpWallDist < 3) shade = 'x';
            else if (perpWallDist < 5) shade = '.';
            else shade = ' ';

            if (side == 1) {
                if (shade == '#') shade = 'M';
                else if (shade == 'X') shade = 'x';
                else if (shade == 'x') shade = '-';
            }

            for (int y = drawStart; y <= drawEnd; ++y) {
                screen[y * screenW + x] = shade;
            }

            for (int y = drawEnd + 1; y < screenH; ++y) {
                screen[y * screenW + x] = (y > screenH * 0.75) ? ',' : ' ';
            }
        }

        stringstream ss;
        ss << "X=" << fixed << setprecision(2) << px << " Y=" << py
           << "  Dir=(" << dirX << "," << dirY << ")  WASD to move, Q to quit\n";
        string hud = ss.str();
        for (size_t i = 0; i < hud.size() && i < (size_t)screenW; ++i)
            screen[i] = hud[i];

        cout << "\x1B[2J\x1B[H";
        for (int y = 0; y < screenH; ++y) {
            cout << screen.substr(y * screenW, screenW) << '\n';
        }
    };

    while (true) {
        render();
        cout << "Command (w/a/s/d - move, q - quit): ";
        char c;
        if (!(cin >> c)) break;
        if (c == 'q' || c == 'Q') break;

        const double moveSpeed = 0.25;
        const double rotSpeed = 0.15;

        if (c == 'w') {
            double nx = px + dirX * moveSpeed;
            double ny = py + dirY * moveSpeed;
            if (mapGrid[int(ny)][int(nx)] == '.') { px = nx; py = ny; }
        } else if (c == 's') {
            double nx = px - dirX * moveSpeed;
            double ny = py - dirY * moveSpeed;
            if (mapGrid[int(ny)][int(nx)] == '.') { px = nx; py = ny; }
        } else if (c == 'a') {
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        } else if (c == 'd') {
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
    }
    
    return 0;
}
