#include "Server.h"
#include "Client.h"

#define SFML_DEFINE_DISCRETE_GPU_PREFERENCE

/*#include <array>
class MazeMapx
{
private:
    static constexpr int size = 100;
    static constexpr int width = size;
    static constexpr int height = size;
    static constexpr int radius = 5;
    static constexpr int roomsNumber = 4;
    struct Rect
    {
        int x, y, width, height;
    };
    std::array<std::array<char, width>, height> map;
    void connect(const Rect &a, const Rect &b)
    {
        int xMin = std::min(a.x, b.x);
        int yMin = std::min(a.y, b.y);
        int xMax = std::max(a.x, b.x);
        int yMax = std::max(a.y, b.y);

        for (int i = yMin; i <= yMax; i++)
            if (i >= 0 && i < height)
                map[i][xMin] = '+';

        for (int i = xMin; i <= xMax; i++)
            if (i >= 0 && i < width)
                map[yMin][i] = '+';

        for (int i = yMin; i <= yMax; i++)
            if (i >= 0 && i < height)
                map[i][xMax] = '+';

        for (int i = xMin; i <= xMax; i++)
            if (i >= 0 && i < width)
                map[yMax][i] = '+';
    }
    bool free(int x1, int y1, int x2, int y2)
    {
        int xMin = std::min(x1, x2);
        int yMin = std::min(y1, y2);
        int xMax = std::max(x1, x2);
        int yMax = std::max(y1, y2);

        for (int i = yMin; i <= yMax; i++)
            for (int j = xMin; j <= xMax; j++)
                if (i >= 0 && i < height && j >= 0 && j < width)
                {
                    if (map[i][j] != '.')
                        return false;
                }
                else
                    return false;
        return true;
    }
    inline float transform(float x)
    {
        return x * (Borders::pos - Borders::width) * 2.f / float(size) - (Borders::pos - Borders::width);
    }

    void fill(int x1, int y1, int x2, int y2)
    {
        float xMin = std::min(x1, x2);
        float yMin = std::min(y1, y2);
        float xMax = std::max(x1, x2);
        float yMax = std::max(y1, y2);

        for (int i = yMin; i <= yMax; i++)
            for (int j = xMin; j <= xMax; j++)
                if (i >= 0 && i < height && j >= 0 && j < width)
                    map[i][j] = ' ';

        constexpr float margin = 0.1f;

        //float x = (float(xMax - xMin) * limits * 2.f - limits) / Object::worldScale;
        //float y = (float(yMax - yMin) * limits * 2.f - limits) / Object::worldScale;
        //float xPos = (float(xMax + xMin) * limits - limits) / Object::worldScale;
        //float yPos = (float(yMax + yMin) * limits - limits) / Object::worldScale;

        if (xMax - xMin >= 1.f && yMax - yMin >= 1.f)
            Rock::create({{transform(xMin) + margin, transform(yMin) + margin},
                          {transform(xMax + 1) - margin, transform(yMin) + margin},
                          {transform(xMax + 1) - margin, transform(yMax + 1) - margin},
                          {transform(xMin) + margin, transform(yMax + 1) - margin}});

        //Rock::create({{-x + 1.f, -y + 1.f}, {x - 1.f, -y + 1.f}, {x - 1.f, y - 1.f}, {-x + 1.f, y - 1.f}}, xPos, yPos);
    }
    void fill()
    {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                if (map[i][j] == '.')
                {
                    int y1 = i;
                    int x1 = j;
                    while (j < width && map[i][j] == '.')
                        j++;
                    j--;
                    int ii = i;
                    while (ii < height && free(x1, y1, j, ii))
                        ii++;
                    ii--;
                    fill(x1, y1, j, ii);
                }
    }

public:
    MazeMapx()
    {
        std::vector<Rect> rooms(roomsNumber);
        char c = 'a';
        for (auto &row : map)
            for (auto &point : row)
                point = '.';
        for (auto &room : rooms)
        {
            room.x = width * Object::rng01(Object::mt);
            room.y = height * Object::rng01(Object::mt);
            room.width = radius * Object::rng025(Object::mt);
            room.height = radius * Object::rng025(Object::mt);
            for (int i = room.y - room.height; i <= room.y + room.height; i++)
                for (int j = room.x - room.width; j <= room.x + room.width; j++)
                    if (i >= 0 && i < height && j >= 0 && j < width)
                        map[i][j] = c;

            c++;
        }
        for (int r = 1; r < rooms.size(); r++)
            connect(rooms[r - 1], rooms[r]);

        fill();

        for (auto &row : map)
        {
            for (auto &point : row)
            {
                std::cout << point;
            }
            std::cout << std::endl;
        }
    }
};*/

int main(int argc, const char *argv[])
{
    static_assert(sizeof(b2Vec2) == sizeof(Vec2f));
    static_assert(sizeof(sf::Vector2f) == sizeof(Vec2f));

    Args args(argc, argv);
    bool serverSide = args["server"].size();
    if (serverSide)
    {
        GameServer s(std::move(args));
        return s();
    }
    else
    {
        GameClient c(std::move(args));
        return c();
    }
}
