#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

// Размер плитки
int titleSize = 54;
// Первая плитка (точка)
Vector2i offset(158, 27);


// Плитки
struct Piece
{
    int x, y, column, row, kind, match, alpha;
    Piece() { match = 0; alpha = 255;}
}   grid[10][10];

// Замена двух плиток
void swap(Piece firstPiece, Piece secondPiece)
{
    // Меняем координату Y (столлбец)
    std::swap(firstPiece.column, secondPiece.column);
    // Меняем координату X (строку)
    std::swap(firstPiece.row, secondPiece.row);


    // Задаем плитку в сетке
    grid[firstPiece.row][firstPiece.column] = firstPiece;
    grid[secondPiece.row][secondPiece.column] = secondPiece;
}


int main()
{
    // Задаем стартовое значение для генератора
    srand(time(0));

    // Окно для приложения
    RenderWindow window(VideoMode(740, 480), "Candy crush");
    // Устанавливаем лимит кадров в секунду
    window.setFramerateLimit(60);

    // Объявляем текстуры
    Texture backgroundTexture, gemsTexture;

    // Объявляем и загружаем шрифт
    Font font;
    font.loadFromFile("../fonts/gardensC.ttf");

    // Объявляем переменную для текста
    Text text("", font, 20);
    text.setColor(Color::Black);

    // Загружаем текстуры
    backgroundTexture.loadFromFile("../images/background.png");
    gemsTexture.loadFromFile("../images/gems.png");

    // Создаем спрайты из текстур
    Sprite background(backgroundTexture), gems(gemsTexture);

    // Кол-во ходов
    int countMoves = 0;

    for (int i = 1; i <= 8; i++)
        for (int j = 1; j <= 8; j++)
        {
            // Задаем тип плитки
            grid[i][j].kind = rand() % 7;
            // Складываем ее в колонку j
            grid[i][j].column = j;
            // Складываем ее в строку i
            grid[i][j].row = i;
            // Задаем размер плитки
            grid[i][j].x = j * titleSize;
            grid[i][j].y = i * titleSize;
        }

    int x0, y0, x, y, click = 0;
    Vector2i position;
    bool isSwap = false, isMoving = false;

    // Основной цикл приложния
    while (window.isOpen())
    {
        // Очередь событий
        Event event;
        while (window.pollEvent(event))
        {
            // Условие для закрытия приложения
            if (event.type == Event::Closed)
                window.close();

            // Проверка на нажатие мыши
            if (event.type == Event::MouseButtonPressed)
                // Если нажата левая кнопка
                if (event.key.code == Mouse::Left)
                {
                    // Если плитка не перемещенна и не сдвинута, то увеличить переменнную клик на один
                    if (!isSwap && !isMoving) click++;
                    // Позиция мыши минус смещение - текущая позиция мыши
                    position = Mouse::getPosition(window) - offset;
                }
        }

        // Первый клик мыши
        /// ВСТАВИТЬ КАРТИНКУ ВЫБОРА
        if (click == 1)
        {
            x0 = position.x / titleSize + 1;
            y0 = position.y / titleSize + 1;
        }
        // Второй клик
        if (click == 2)
        {
            x = position.x / titleSize + 1;
            y = position.y / titleSize + 1;

            // Если плитки находятся рядом
            if (abs(x - x0) + abs(y - y0) == 1)
            {
                // Меняем плитки
                swap(grid[y0][x0], grid[y][x]);
                isSwap = true;
                click = 0;
                //countMoves++;
            } else click = 1;
        }

        // Нахождение совподения
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
            {
                if (grid[i][j].kind == grid[i + 1][j].kind)
                    if (grid[i][j].kind == grid[i - 1][j].kind)
                        for (int n = -1; n <= 1; n++) grid[i + n][j].match++;

                if (grid[i][j].kind == grid[i][j + 1].kind)
                    if (grid[i][j].kind == grid[i][j - 1].kind)
                        for (int n = -1; n <= 1; n++) grid[i][j + n].match++;
            }

        // Анимация передвижения
        isMoving = false;
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
            {
                Piece &piece = grid[i][j];
                int dx, dy;

                // 4 - скорость
                for (int n = 0; n < 4; n++)
                {
                    dx = piece.x - piece.column * titleSize;
                    dy = piece.y - piece.row * titleSize;

                    if (dx) piece.x -= dx / abs(dx);
                    if (dy) piece.y -= dy / abs(dy);
                    if (dx || dy) isMoving = 1;
                }
            }

        // Анимация удаления
        if (!isMoving)
            for (int i = 1; i <= 8; i++)
                for (int j = 1; j <= 8; j ++)
                    if (grid[i][j].match)
                        if (grid[i][j].alpha > 10)
                        {
                            grid[i][j].alpha -= 10;
                            isMoving = true;
                        }


        // Счет
        int score = 0;
        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
                score += grid[i][j].match;

        // Вторая замена
        if (isSwap && !isMoving)
        {
            if (!score) swap(grid[y0][x0], grid[y][x]);
            isSwap = false;
            countMoves++;
        }

        // Обнавление сетки
        if (!isMoving)
        {
            for (int i = 8; i > 0; i--)
                for (int j = 1; j <= 8; j++)
                    if (grid[i][j].match)
                        for (int n = i; n > 0; n--)
                            if (!grid[n][j].match)
                            {
                                swap(grid[n][j], grid[i][j]);
                                break;
                            }
            for (int j = 1; j <= 8; j++)
                for (int i = 8, n = 0; i > 0; i--)
                    if (grid[i][j].match)
                    {
                        grid[i][j].kind = rand() % 7;
                        grid[i][j].y = -titleSize * n++;
                        grid[i][j].match = 0;
                        grid[i][j].alpha = 255;
                    }
        }

        text.setString(std::to_string(countMoves));
        text.setPosition(0, 0);

        ///Draw///
        window.draw(background);

        for (int i = 1; i <= 8; i++)
            for (int j = 1; j <= 8; j++)
            {
                Piece piece = grid[i][j];
                gems.setTextureRect(IntRect(piece.kind * 49, 0, 49, 49));
                gems.setColor(Color(255, 255, 255, piece.alpha));
                gems.setPosition(piece.x, piece.y);
                gems.move(offset.x - titleSize, offset.y - titleSize);
                window.draw(text);
                window.draw(gems);
            }

        window.display();
    }

    return 0;
}