#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream> 

using namespace std;
using namespace sf;

#define SPEED -5 // Speed of pipes and grounds
#define Sover -10 // Transition for game over
#define GAP 165 // gap between pipes
#define DIS 1330 // distances between pipes
#define distbet 100
int oo = 0;

enum GameState
{
    MainMenu,
    eTTP,
    eGame,
    eDifficulty,
    Credits,
    eThemes,
    BirdThemes,
    GameOver
};

GameState currentGameState = GameState::MainMenu;

RenderWindow window(VideoMode(1700, 800), "Flappy Bird", Style::Default);
Event event;
const float SCREEN_W = window.getSize().x;
const float SCREEN_H = window.getSize().y;

short int diff = 0, BirdTheme = 0, ChooseTheme = 0;
bool ttp = 0, Reseting = 0;
float Velocity, Gravity, PipeSpeedUpNDown = 0.75;

Font MainFont;
Font FontTheme;
Font fontrate;

void setFonts()
{
    FontTheme.loadFromFile("arial.TTF");
    fontrate.loadFromFile("pixelmix_bold.ttf");
    MainFont.loadFromFile("pixelmix.ttf");
}

void setAssets();
void transition();
void draw();
void animation();


struct Flash
{
    RectangleShape TheFlash;
    Clock Flash_dt;
    bool FlashOn, RestartTime;

    void Constructor()
    {
        TheFlash.setSize(Vector2f(window.getSize().x, window.getSize().y));
        FlashOn = 0, RestartTime = 0;
    }

    void Draw()
    {
        if (!RestartTime)
        {
            Flash_dt.restart();
            RestartTime = 1;
        }

        if (FlashOn && Flash_dt.getElapsedTime().asSeconds() < 0.15)
            TheFlash.setFillColor(Color(0, 0, 0, 150));

        else
            TheFlash.setFillColor(Color(0, 0, 0, 0));

        window.draw(TheFlash);
    }
} Flash;

struct Pipes
{
    Sprite PiUp[5];
    Sprite PiDown[5];
    Texture Tex[2];

    int Rand;
    bool x, y, a, b;

    void Constructor()
    {
        Tex[0].loadFromFile("pipeup2.png");
        Tex[1].loadFromFile("pipedown2.png");

        for (int i = 0; i < 5; i++)
        {
            GenRan();
            PiUp[i].setTexture(Tex[0]);
            PiUp[i].setPosition(1700, Rand);

            PiDown[i].setTexture(Tex[1]);
            PiDown[i].setPosition(1700, (PiUp[i].getPosition().y - GAP * 5) + 10);
        }
        x = 0, y = 0, a = 0, b = 0;
    }

    void GenRan()
    {
        Rand = 260 + rand() % 236;
    }

    void MovePipesOne()
    {
        PiUp[0].move(SPEED, 0);
        PiDown[0].move(SPEED, 0);
    }

    void MovePipesTwo()
    {
        if (PiUp[0].getPosition().x <= DIS)
            x = 1;

        if (x == 1)
        {
            PiUp[1].move(SPEED, 0);
            PiDown[1].move(SPEED, 0);
        }
    }

    void MovePipesThree()
    {
        if (PiUp[1].getPosition().x <= DIS)
            y = 1;

        if (y == 1)
        {
            PiUp[2].move(SPEED, 0);
            PiDown[2].move(SPEED, 0);
        }
    }

    void MovePipesFour()
    {
        if (PiUp[2].getPosition().x <= DIS)
            a = 1;

        if (a == 1)
        {
            PiUp[3].move(SPEED, 0);
            PiDown[3].move(SPEED, 0);
        }
    }

    void MovePipesFive()
    {
        if (PiUp[3].getPosition().x <= DIS)
            b = 1;

        if (b == 1)
        {
            PiUp[4].move(SPEED, 0);
            PiDown[4].move(SPEED, 0);
        }
    }

    void MovePipes()
    {
        MovePipesOne();
        MovePipesTwo();
        MovePipesThree();
        MovePipesFour();
        MovePipesFive();
    }

    void ResetPipesPosition()
    {
        for (int i = 0; i < 5; i++)
        {
            GenRan();
            if (PiUp[i].getPosition().x <= -112)
            {
                PiUp[i].setPosition(1700, Rand);
                PiDown[i].setPosition(1700, (PiUp[i].getPosition().y - GAP * 5) + 10);
            }
        }
    }

    void Draw()
    {
        for (int i = 0; i < 5; i++)
        {
            window.draw(PiUp[i]);
            window.draw(PiDown[i]);
        }
    }
} Pipes;

struct Ground
{
    Sprite TwoGrounds;
    Texture Grounds;
    void Constructor(int PosX, int PosY)
    {
        Grounds.loadFromFile("Land.PNG");
        TwoGrounds.setTexture(Grounds);
        TwoGrounds.setPosition(PosX, PosY);
        TwoGrounds.setScale(2.18, 1);
    }

    void MoveGrounds()
    {
        TwoGrounds.move(SPEED, 0);

        if (TwoGrounds.getPosition().x <= -1700)
            TwoGrounds.setPosition(1700, 700);
    }

    void Draw()
    {
        window.draw(TwoGrounds);
    }

} Lone, Ltwo;

struct Bird
{
    Texture TxOne[4], TxTwo[4], TxThree[4];
    Sprite Bird;
    Clock dt, UpAndDown;
    SoundBuffer H, J, W;
    Sound Hit, Jump, Whoop;
    int Rotation, Iterator;
    bool Input, Splayed, ToMove;

    void Constructor(float Scale, float PosX, float PosY)
    {
        TxOne[0].loadFromFile("birddown.png");
        TxOne[1].loadFromFile("birdmid.png");
        TxOne[2].loadFromFile("birdup.png");
        TxOne[3].loadFromFile("birdmid.png");

        TxTwo[0].loadFromFile("birddown2.png");
        TxTwo[1].loadFromFile("birdmid2.png");
        TxTwo[2].loadFromFile("birdup2.png");
        TxTwo[3].loadFromFile("birdmid2.png");

        TxThree[0].loadFromFile("birddown3.png");
        TxThree[1].loadFromFile("birdmid3.png");
        TxThree[2].loadFromFile("birdup3.png");
        TxThree[3].loadFromFile("birdmid3.png");


        Bird.setPosition(PosX, PosY);
        Bird.setOrigin(Bird.getGlobalBounds().width / 2, Bird.getGlobalBounds().height / 2);
        Bird.setScale(Scale, Scale);
        Bird.setRotation(0);

        J.loadFromFile("Jump.wav");
        Jump.setBuffer(J);

        H.loadFromFile("Hit.wav");
        Hit.setBuffer(H);

        W.loadFromFile("Whoop.wav");
        Whoop.setBuffer(W);

        Splayed = 0, Velocity = 0, ToMove = 0, Rotation = 0, Iterator = 0, Input = 0, Gravity = 0.35;
    }

    void Animate()
    {
        if (dt.getElapsedTime().asSeconds() > 0.06)
        {
            if (Iterator < 3)
                Iterator++;

            else
                Iterator = 0;

            if (BirdTheme == 0)
                Bird.setTexture(TxOne[Iterator]);

            if (BirdTheme == 1)
                Bird.setTexture(TxTwo[Iterator]);

            if (BirdTheme == 2)
                Bird.setTexture(TxThree[Iterator]);

            dt.restart();
        }
    }

    void UpNDown()
    {
        if (UpAndDown.getElapsedTime().asSeconds() >= 0.1 && !ToMove)
        {
            Bird.move(0, -1);
            ToMove = 1;
        }
        if (UpAndDown.getElapsedTime().asSeconds() >= 0.2 && ToMove)
        {
            Bird.move(0, 1);
            ToMove = 0;
            UpAndDown.restart();
        }
    }

    void RotateDown()
    {
        if (Rotation <= 35)
            Rotation += 3;

        Bird.setRotation(Rotation);
    }

    void RotateUp()
    {
        if (Rotation >= -35)
            Rotation = -35;

        Bird.setRotation(Rotation);
    }

    void GUI()
    {
        Bird.move(0, Velocity);
        if (!Input && event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Space)
            {
                Jump.play();
                Input = 1;

                if (diff == 1)
                    Velocity = 5;

                else
                    Velocity = -5;

                Bird.move(0, Velocity * 9 / 5);
            }
        }

        if (event.type == Event::KeyReleased)
        {
            if (event.key.code == Keyboard::Space)
                Input = 0;
        }
    }

    void MoveDTCol()
    {
        if (!(Bird.getGlobalBounds().intersects(Lone.TwoGrounds.getGlobalBounds()) || Bird.getGlobalBounds().intersects(Ltwo.TwoGrounds.getGlobalBounds())))
        {
            Bird.setRotation(75);
            Bird.move(0, 7);
        }
    }

    void Draw()
    {
        window.draw(Bird);
    }
} Bird;

struct Themes
{
    Sprite themes;
    Texture Textures[4];
    void Constructor()
    {
        Textures[0].loadFromFile("Classic.png");
        Textures[1].loadFromFile("Shanghai.png");
        Textures[2].loadFromFile("Tokyo.png");
        Textures[3].loadFromFile("London.png");

        themes.setTexture(Textures[0]);
        themes.setScale(2.85, 1.5);
        themes.setPosition(0, -100);
    }

    void ChoosingThemeGUI()
    {
        if (currentGameState == GameState::eThemes)
        {
            if (Mouse::isButtonPressed(Mouse::Left) && Mouse::getPosition(window).x > 840 && Mouse::getPosition(window).x < 1258 && Mouse::getPosition(window).y > 41 && Mouse::getPosition(window).y < 327)
            {
                ChooseTheme = 3;
                currentGameState = GameState::MainMenu;
            }

            if (Mouse::isButtonPressed(Mouse::Left) && Mouse::getPosition(window).x > 840 && Mouse::getPosition(window).x < 1258 && Mouse::getPosition(window).y > 330 && Mouse::getPosition(window).y < 690)
            {
                ChooseTheme = 2;
                currentGameState = GameState::MainMenu;
            }

            if (Mouse::isButtonPressed(Mouse::Left) && Mouse::getPosition(window).x > 438 && Mouse::getPosition(window).x < 827 && Mouse::getPosition(window).y > 41 && Mouse::getPosition(window).y < 327)
            {
                ChooseTheme = 0;
                currentGameState = GameState::MainMenu;
            }

            if (Mouse::isButtonPressed(Mouse::Left) && Mouse::getPosition(window).x > 438 && Mouse::getPosition(window).x < 827 && Mouse::getPosition(window).y > 333 && Mouse::getPosition(window).y < 688)
            {
                ChooseTheme = 1;
                currentGameState = GameState::MainMenu;
            }
        }
    }

    void Draw()
    {
        themes.setTexture(Textures[ChooseTheme]);
        window.draw(themes);
    }
} Themes;

struct Scoring
{
    Font ScoreFont;
    Text Score, HighScore;
    SoundBuffer ScoInc;
    Sound ScoreSound;
    int Sinc, EasyHigh = 0, MediumHigh = 0, HardHigh = 0;
    bool Inc, MoveScoreUp;

    void Constructor(int x, int y, int size)
    {
        ScoreFont.loadFromFile("JANDAMANATEESOLID.ttf");
        Score.setFont(ScoreFont);
        Score.setCharacterSize(size);
        Score.setPosition(x, y); // 280, 100
        Score.setString(to_string(Sinc));

        HighScore.setFont(ScoreFont);

        HighScore.setPosition(880, 1175);
        HighScore.setCharacterSize(20);

        ScoInc.loadFromFile("Score.wav");
        ScoreSound.setBuffer(ScoInc);

        Inc = 0, MoveScoreUp = 0, Sinc = 0;
    }

    void hsSetup()
    {
        ifstream EasyHighScore("EasyHighScore.txt");
        ifstream MediumHighScore("MediumHighScore.txt");
        ifstream HardHighScore("HardHighScore.txt");

        if (diff == 0)
        {
            HighScore.setString(to_string(EasyHigh));

            if (EasyHighScore.is_open())
            {
                EasyHighScore >> EasyHigh;
                EasyHighScore.close();
            }

            if (Sinc > EasyHigh)
                EasyHigh = Sinc;

            ofstream highScoreFile("EasyHighScore.txt");
            if (highScoreFile.is_open())
            {
                highScoreFile << EasyHigh;
                highScoreFile.close();
            }
        }

        if (diff == 1)
        {
            HighScore.setString(to_string(MediumHigh));

            if (MediumHighScore.is_open())
            {
                MediumHighScore >> MediumHigh;
                MediumHighScore.close();
            }

            if (Sinc > MediumHigh)
                MediumHigh = Sinc;

            ofstream highScoreFile("MediumHighScore.txt");
            if (highScoreFile.is_open())
            {
                highScoreFile << MediumHigh;
                highScoreFile.close();
            }
        }

        if (diff == 2)
        {
            HighScore.setString(to_string(HardHigh));

            if (HardHighScore.is_open())
            {
                HardHighScore >> HardHigh;
                HardHighScore.close();
            }

            if (Sinc > HardHigh)
                HardHigh = Sinc;

            ofstream highScoreFile("HardHighScore.txt");
            if (highScoreFile.is_open())
            {
                highScoreFile << HardHigh;
                highScoreFile.close();
            }
        }



    }

    void IncScore()
    {
        if ((Bird.Bird.getPosition().x == Pipes.PiUp[0].getPosition().x || Bird.Bird.getPosition().x == Pipes.PiUp[1].getPosition().x || Bird.Bird.getPosition().x == Pipes.PiUp[2].getPosition().x) || Bird.Bird.getPosition().x == Pipes.PiUp[3].getPosition().x || Bird.Bird.getPosition().x == Pipes.PiUp[4].getPosition().x)
        {
            ScoreSound.play();
            Sinc++;
            Score.setString(to_string(Sinc));
        }

    }

    void ForGameOver()
    {

        if (!MoveScoreUp)
        {
            Constructor(880, 1115, 20);
            MoveScoreUp = 1;
        }

        if (HighScore.getPosition().y > 345)
            HighScore.move(0, Sover);

        if (Score.getPosition().y > 290)
            Score.move(0, Sover);
    }

    void DrawScore()
    {
        window.draw(Score);
    }

    void DrawHighScore()
    {
        window.draw(HighScore);
    }
} Score;

struct Collision
{
    void CollisionWPipes(Sprite pipes[])
    {
        if (Bird.Bird.getGlobalBounds().intersects(pipes[0].getGlobalBounds()) || Bird.Bird.getGlobalBounds().intersects(pipes[1].getGlobalBounds()) || Bird.Bird.getGlobalBounds().intersects(pipes[2].getGlobalBounds()) || Bird.Bird.getGlobalBounds().intersects(pipes[3].getGlobalBounds()) || Bird.Bird.getGlobalBounds().intersects(pipes[4].getGlobalBounds()))
        {
            if (!Bird.Splayed)
            {
                Bird.Whoop.play();
                Bird.Hit.play();
                Bird.Splayed = 1;
            }
            Bird.Bird.setRotation(75);
            Flash.FlashOn = 1;
            currentGameState = GameState::GameOver;
        }
    }

    void CollisionWGround()
    {
        if (Bird.Bird.getPosition().y <= 0)
            Bird.Bird.setPosition(Bird.Bird.getPosition().x, 0);

        if (Bird.Bird.getGlobalBounds().intersects(Lone.TwoGrounds.getGlobalBounds()) || Bird.Bird.getGlobalBounds().intersects(Ltwo.TwoGrounds.getGlobalBounds()))
        {
            if (!Bird.Splayed)
            {
                Bird.Hit.play();
                Bird.Splayed = 1;
            }
            currentGameState = GameState::GameOver;
        }
    }
}Collide;

struct GameOverMenu
{
    Texture gameOver[4];
    Sprite over[4];
    Sprite medal;
    Texture medalTx[5];


    void Constuctor()
    {
        gameOver[0].loadFromFile("gameover.png");
        gameOver[1].loadFromFile("score.png");
        gameOver[2].loadFromFile("Play.png");
        gameOver[3].loadFromFile("Themes.png");

        medalTx[0].loadFromFile("Silver-Medal.png");
        medalTx[1].loadFromFile("Bronze-Medal.png");
        medalTx[2].loadFromFile("Gold-Medal.png");
        medalTx[3].loadFromFile("Platinum-Medal.png");

        medal.setScale(0.5, 0.5);

        for (int i = 0; i < 4; i++)
        {
            over[i].setTexture(gameOver[i]);  // over 0 = gameover, over 1 = score

            if (i < 2)
                over[i].setScale(1.2, 1.2);
        }
        over[0].setScale(1.5, 1.5);

        over[0].setPosition(650, 1000);
        over[1].setPosition(650, 1075);

        over[2].setPosition(675, 1225);
        over[3].setPosition(800, 1225);
        medal.setPosition(685, 1119);
    }

    void Transition()
    {

        if (over[0].getPosition().y > 175)
            over[0].move(0, Sover);

        if (over[1].getPosition().y > 250)
            over[1].move(0, Sover);

        if (over[2].getPosition().y > 400)
            over[2].move(0, Sover);

        if (over[3].getPosition().y > 400)
            over[3].move(0, Sover);

        if (medal.getPosition().y > 295)
            medal.move(0, Sover);

    }
    void medall()
    {

        if (Score.Sinc > 9)
            medal.setTexture(medalTx[0]);

        if (Score.Sinc > 19)
            medal.setTexture(medalTx[1]);

        if (Score.Sinc > 29)
            medal.setTexture(medalTx[2]);

        if (Score.Sinc > 39)
            medal.setTexture(medalTx[3]);

    }

    void Draw()
    {
        for (int i = 0; i < 4; i++)
            window.draw(over[i]);
        window.draw(medal);
    }
} Gover;

struct Difficulty
{
    Clock PipeTimer;
    bool ForPipeTimer;

    void DifficultySettings()
    {
        if (diff == 0 || diff == 2)
        {
            Velocity += Gravity;

            if (Velocity >= 0)
                Bird.RotateDown();

            else
                Bird.RotateUp();

            if (diff == 2)
            {
                for (int i = 0; i < 5; i++)
                {
                    if (i % 2 == 0)
                    {
                        Pipes.PiUp[i].move(0, -PipeSpeedUpNDown);
                        Pipes.PiDown[i].move(0, -PipeSpeedUpNDown);
                    }
                    else
                    {
                        Pipes.PiUp[i].move(0, PipeSpeedUpNDown);
                        Pipes.PiDown[i].move(0, PipeSpeedUpNDown);
                    }


                    if ((int)PipeTimer.getElapsedTime().asSeconds() >= 2)
                    {
                        PipeSpeedUpNDown *= -1;
                        PipeTimer.restart();
                    }
                }
            }
        }

        if (diff == 1)
        {
            Velocity -= Gravity;
            if (Velocity <= 0)
                Bird.RotateDown();

            else
                Bird.RotateUp();
        }
    }
}Difficulty;

struct Menu
{
    RectangleShape darkWindow;
    Texture Rate;
    Sprite SpriteRate;

    Texture Bc1;
    Sprite MainBackGrounds[2];
    RectangleShape RectangleRate;


    Text textrate;

    Texture MainMenu;
    RectangleShape SpriteMainMenu;
    Texture mainmenubgdark;
    Sprite bgdark;
    Vector2u windowSize = window.getSize();

    Texture Title;
    Sprite SpriteTitle;

    Sprite btnplay;
    Texture Theme;
    Sprite SpriteTheme;
    //flexibility
    Texture returnback;
    Sprite SpriteReturnBack;


    RectangleShape rectangle;
    //triangle
    CircleShape triangle;


    //themes for bird

    Texture birdthemes;
    Sprite SpriteBIRDthemes;


    RectangleShape rectanglebirdthemes;

    Sprite BirdForButton; // Bird On Button

    
    Texture birdtheme;
    Sprite Birdtheme;

    void set()
    {
        birdtheme.loadFromFile("birdmid3.png");
        BirdForButton.setTexture(birdtheme);
        BirdForButton.setScale(1.5, 1.5);
        BirdForButton.setPosition(1155, 570);
        RectangleShape dar(Vector2f(SCREEN_W, SCREEN_H));
        darkWindow = dar;
        darkWindow.setFillColor(Color(0, 0, 0, 200));

        RectangleShape rectanglebirdthemes1(Vector2f(200.f, 100.f));
        rectanglebirdthemes = rectanglebirdthemes1;
        birdthemes.loadFromFile("btn-play.png");
        SpriteBIRDthemes.setTexture(birdthemes);
        SpriteBIRDthemes.setPosition(Vector2f(1100, 550));
        SpriteBIRDthemes.setScale(1.5f, 1.5f);
        rectanglebirdthemes.setPosition(Vector2f(1120, 560));
        rectanglebirdthemes.setFillColor(Color::White);
        rectanglebirdthemes.setScale(0.5f, 0.65f);


        Bc1.loadFromFile("land.png");

        RectangleShape RectangleRate111(Vector2f(95.0f, 35.0f));
        RectangleShape rectangle111(Vector2f(70.0f, 25.0f));
        rectangle = rectangle111;
        RectangleRate = RectangleRate111;
        textrate.setFont(fontrate);
        textrate.setCharacterSize(25);
        textrate.setString("Credits");
        textrate.setFillColor((Color(240, 240, 0, 255)));
        textrate.setPosition(713, 580);
        Rate.loadFromFile("btn-rate.png");
        SpriteRate.setTexture(Rate);
        SpriteRate.setPosition(Vector2f(700, 550));
        SpriteRate.setScale(2.5f, 2.5f);
        RectangleRate.setPosition(Vector2f(725, 570));
        RectangleRate.setOutlineColor(Color::White);
        RectangleRate.setFillColor(Color::White);


        triangle.setPosition(Vector2f(33.f, 48.f));
        triangle.setRadius(17.f);
        triangle.setPointCount(3);
        triangle.setFillColor(Color::Green);
        triangle.setOutlineThickness(5.f);
        triangle.setRotation(270.f);

        rectangle.setPosition(Vector2f(20.f, 14.f));
        rectangle.setOutlineColor(Color::White);
        rectangle.setFillColor(Color::White);


        btnplay.setTexture(birdthemes);
        btnplay.setPosition(Vector2f(500, 550));
        btnplay.setScale(1.5f, 1.5f);
        SpriteReturnBack.setTexture(Rate);
        SpriteReturnBack.setPosition(Vector2f(5, 5));
        SpriteReturnBack.setScale(1.5f, 1.5f);

        Theme.loadFromFile("Theme.png");
        SpriteTheme.setTexture(Theme);
        SpriteTheme.setPosition(Vector2f(900, 550));

        SpriteTheme.setScale(1.5f, 1.5f);

        Title.loadFromFile("game_title.png");
        SpriteTitle.setTexture(Title);
        SpriteTitle.setPosition(Vector2f(655, 100));
        SpriteTitle.setScale(2.0f, 2.0f);

        mainmenubgdark.loadFromFile("Dark bg.jpg");
        MainMenu.loadFromFile("Classic.png");
        bgdark.setTexture(mainmenubgdark);
        RectangleShape SpriteMainMenu1(Vector2f(SCREEN_W, SCREEN_H));
        SpriteMainMenu = SpriteMainMenu1;
        SpriteMainMenu.setTexture(&MainMenu);
    }
    void draw()
    {
        Bird.UpNDown();
        Bird.Animate();
        Lone.Draw();
        Ltwo.Draw();
        Bird.Draw();
        window.draw(menu.SpriteTitle);
        window.draw(menu.btnplay);
        window.draw(menu.SpriteTheme);
        window.draw(menu.SpriteRate);
        window.draw(menu.RectangleRate);
        window.draw(menu.textrate);
        window.draw(menu.SpriteBIRDthemes);
        window.draw(menu.rectanglebirdthemes);
        window.draw(BirdForButton);
    }
}menu;

struct credits
{
    Font font;
    Text text[7];
    Clock wingTimer;
    int currBird = 0, Colour = 20;
    RectangleShape black;
    Sprite birds[7];

    void set()
    {
        font.loadFromFile("JANDAMANATEESOLID.ttf");
        RectangleShape _b(Vector2f(SCREEN_W, SCREEN_H));
        _b.setFillColor(Color(0, 0, 0, 150));
        black = _b;

        for (int i = 0; i < 7; i++)
        {
            text[i].setFont(font);
            text[i].setFillColor(Color(50 + (Colour * i), 50 + (Colour * i), 200 - (Colour * i)));
            text[i].setOrigin(text[i].getGlobalBounds().width / 2, text[i].getGlobalBounds().height / 2);
            text[i].setScale(1.5, 1.5);
        }

        for (int i = 0; i < 7; i++)
        {
            birds[i] = Bird.Bird;
            birds[i].setTexture(Bird.TxOne[0]);
            birds[i].setOrigin(birds[0].getGlobalBounds().width / 2, text[i].getGlobalBounds().height / 2);
            birds[i].setScale(1.3, 1.3);
        }
    }

    void ResetCredits()
    {
        text[0].setString("Youssef Fayek");
        text[1].setString("Maria Akram");
        text[2].setString("Karen Bahaa");
        text[3].setString("Maria Ramy");
        text[4].setString("Karen Ashraf");
        text[5].setString("Jennifer Hany");
        text[6].setString("Omar Zamel");

        birds[0].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 3, 910 - 100);
        birds[1].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 20, 980 - 100);
        birds[2].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 20, 1050 - 100);
        birds[3].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 20, 1120 - 100);
        birds[4].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 20, 1190 - 100);
        birds[5].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 20, 1260 - 100);
        birds[6].setPosition(SCREEN_W / 2 - text[0].getGlobalBounds().width / 2 - 20, 1330 - 100);
        text[0].setPosition(SCREEN_W / 2 - 150, 800);
        text[1].setPosition(SCREEN_W / 2 - 150, 870);
        text[2].setPosition(SCREEN_W / 2 - 150, 940);
        text[3].setPosition(SCREEN_W / 2 - 150, 1010);
        text[4].setPosition(SCREEN_W / 2 - 150, 1080);
        text[5].setPosition(SCREEN_W / 2 - 150, 1150);
        text[6].setPosition(SCREEN_W / 2 - 150, 1220);
    }

    void draw()
    {
        for (int i = 0; i < 7; i++)
        {
            window.draw(text[i]);
            window.draw(birds[i]);
        }
        window.draw(menu.SpriteReturnBack);
        window.draw(menu.rectangle);
        window.draw(menu.triangle);
    }

    void move()
    {
        if (text[0].getPosition().y >= SCREEN_H / 2 - 240)
        {
            for (int i = 0; i < 7; i++)
            {
                text[i].move(0, -1);
                birds[i].move(0, -1);
            }

        }
    }

    void wingMove()
    {
        if (wingTimer.getElapsedTime().asMilliseconds() >= 150)
        {
            currBird = (currBird + 1) % 3;
            for (int i = 0; i < 7; i++)
                birds[i].setTexture(Bird.TxOne[currBird]);

            wingTimer.restart();
        }
    }
}credits;

struct themes
{
    Texture theme1;

    Sprite SpriteTheme1;

    Texture theme2;

    Sprite SpriteTheme2;


    Texture theme3;

    Sprite SpriteTheme3;


    RectangleShape titleBar;
    Texture theme4;
    Text titleText;
    Sprite SpriteTheme4;

    void set()
    {
        Text titleText1("Choose a Theme", FontTheme, 20);
        titleText = titleText1;

        theme1.loadFromFile("Classic.png");
        SpriteTheme1.setTexture(theme1);
        SpriteTheme1.setPosition(Vector2f(440, 40));
        SpriteTheme1.setScale(0.7, 0.6f);

        theme2.loadFromFile("London.png");
        SpriteTheme2.setTexture(theme2);
        SpriteTheme2.setPosition(Vector2f(840, 40));
        SpriteTheme2.setScale(0.7, 0.6f);

        theme3.loadFromFile("Shanghai.png");
        SpriteTheme3.setTexture(theme3);
        SpriteTheme3.setPosition(Vector2f(440, 330));
        SpriteTheme3.setScale(0.7, 0.6f);

        theme4.loadFromFile("Tokyo.png");
        SpriteTheme4.setTexture(theme4);
        SpriteTheme4.setPosition(Vector2f(840, 330));
        SpriteTheme4.setScale(0.7, 0.6);

        titleBar.setSize(Vector2f(822, 40));
        titleBar.setFillColor(Color::Black);
        titleBar.setOrigin(titleBar.getGlobalBounds().width / 2, titleBar.getOrigin().y);
        titleBar.setPosition(window.getSize().x / 2, 0);

        titleText.setFillColor(Color::White);
        titleText.setPosition(800, 10); // Adjusted for vertical centering
    }
    void draw()
    {
        Lone.Draw();
        Ltwo.Draw();
        window.draw(menu.darkWindow);
        window.draw(menu.SpriteReturnBack);
        window.draw(menu.rectangle);
        window.draw(menu.triangle);
        window.draw(theme.SpriteTheme1);
        window.draw(theme.SpriteTheme2);
        window.draw(theme.SpriteTheme3);
        window.draw(theme.SpriteTheme4);
        window.draw(theme.titleBar);
        window.draw(theme.titleText);
    }
}theme;

struct levels
{
    RectangleShape Rectangle;
    Texture play;
    Sprite btnplay1;
    RectangleShape rectangle2;
    Texture play2;
    Sprite btnplay2;
    RectangleShape rectangle3;
    Texture play3;
    Sprite btnplay3;

    Text text;
    Text text2;
    Text text3;

    void set()
    {
        RectangleShape _Rectangle(Vector2f(200, 100));
        RectangleShape _rectangle2(Vector2f(200.f, 100.f));
        RectangleShape _rectangle3(Vector2f(200.f, 100.f));
        Rectangle = _Rectangle;
        rectangle3 = _rectangle3;
        rectangle2 = _rectangle2;


        Rectangle.setPosition(Vector2f(790, 180));
        Rectangle.setFillColor(Color::White);
        Rectangle.setScale(0.7f, 0.7f);

        play.loadFromFile("btn-play.png");
        btnplay1.setTexture(play);
        btnplay1.setPosition(Vector2f(735, 150));
        btnplay1.setScale(2.0f, 2.0f);

        rectangle2.setPosition(Vector2f(790, 330));
        rectangle2.setFillColor(Color::White);
        rectangle2.setScale(0.7f, 0.7f);

        play2.loadFromFile("btn-play.png");
        btnplay2.setTexture(play2);
        btnplay2.setPosition(Vector2f(735, 320));
        btnplay2.setScale(2.0f, 2.0f);

        rectangle3.setPosition(Vector2f(790, 520));
        rectangle3.setFillColor(Color::White);
        rectangle3.setScale(0.7f, 0.7f);

        play3.loadFromFile("btn-play.png");
        btnplay3.setTexture(play3);
        btnplay3.setPosition(Vector2f(735, 490));
        btnplay3.setScale(2.0f, 2.0f);

        text.setFont(MainFont);
        text.setString("Easy");
        text.setCharacterSize(40);
        text.setFillColor(Color::Green);
        text.setPosition(780, 185);

        text2.setFont(MainFont);
        text2.setString("Medium");
        text2.setCharacterSize(40);
        text2.setFillColor(Color(240, 240, 0, 255));

        text2.setPosition(760, 355);

        text3.setFont(MainFont);
        text3.setString("Hard");
        text3.setCharacterSize(40);
        text3.setFillColor(Color::Red);
        text3.setPosition(780, 525);
    }

    void draw()
    {
        window.draw(menu.SpriteReturnBack); // return back button
        Lone.Draw();
        Ltwo.Draw();
        window.draw(menu.rectangle); // 
        window.draw(menu.triangle);
        window.draw(level.btnplay1);//
        window.draw(level.btnplay2); //
        window.draw(level.btnplay3); // play
        window.draw(level.Rectangle);
        window.draw(level.rectangle2);
        window.draw(level.rectangle3);
        window.draw(level.text); // easy
        window.draw(level.text2); // medium
        window.draw(level.text3); // hard
    }
}level;

struct TapToPlayMenu
{
    Texture Textures[2]; // Textures[0] = ttp, Textures[1] = get ready
    Sprite Stuff[2]; // Stuff[0] = ttp, Stuff[1] = get ready
    void Constructor()
    {
        Textures[0].loadFromFile("ttplay.png");
        Stuff[0].setTexture(Textures[0]);
        Stuff[0].setPosition(815, 220);

        Textures[1].loadFromFile("getready.png");
        Stuff[1].setTexture(Textures[1]);
        Stuff[1].setScale(1.1, 1.1);
        Stuff[1].setPosition(740, 100);
        Stuff[1].setScale(1.3, 1.3);
    }
    void Draw()
    {
        window.draw(Stuff[0]);
        window.draw(Stuff[1]);
        window.draw(menu.SpriteReturnBack);
        window.draw(menu.rectangle);
        window.draw(menu.triangle);
    }
} Tmenu;

struct BirdThemes
{


    Sprite ForBirdTheme[3]; // 3 Rate Buttons
    RectangleShape ForBirds[3]; // 3 White Rectangles
    Sprite ThreeBirds[3]; // The 3 Birds

    Clock BirdTimer;
    int mm;
void Constructor()
{
    for (int i = 0; i < 3; i++)
    {
        ForBirdTheme[i].setTexture(menu.Rate);
        ForBirdTheme[i].setOrigin(ForBirdTheme[i].getGlobalBounds().width / 2, ForBirdTheme[i].getOrigin().y);
        ForBirdTheme[i].setScale(2, 2);

        ForBirds[i].setSize(Vector2f(80, 30));
        ForBirds[i].setOrigin(ForBirds[i].getGlobalBounds().width / 2, ForBirds[i].getOrigin().y);

        ThreeBirds[i].setScale(1.5, 1.5);
    }

    ForBirdTheme[0].setPosition(window.getSize().x / (3) - 100, 400);
    ForBirdTheme[1].setPosition(window.getSize().x / (3) + 300, 400);
    ForBirdTheme[2].setPosition(window.getSize().x / (3) + 700, 400);

    ForBirds[0].setPosition(window.getSize().x / (3) - 100, 420);
    ForBirds[1].setPosition(window.getSize().x / (3) + 300, 420);
    ForBirds[2].setPosition(window.getSize().x / (3) + 700, 420);

    ThreeBirds[0].setTexture(Bird.TxOne[0]);
    ThreeBirds[1].setTexture(Bird.TxTwo[0]);
    ThreeBirds[2].setTexture(Bird.TxThree[0]);

    ThreeBirds[0].setPosition(window.getSize().x / (3) - 120, 415);
    ThreeBirds[1].setPosition(window.getSize().x / (3) + 275, 415);
    ThreeBirds[2].setPosition(window.getSize().x / (3) + 675, 415);
}

void setNanimate()
{
    if (BirdTimer.getElapsedTime().asSeconds() > 0.06)
    {
        if (mm < 3)
            mm++;

        else
            mm = 0;

        ThreeBirds[0].setTexture(Bird.TxOne[mm]);
        ThreeBirds[1].setTexture(Bird.TxTwo[mm]);
        ThreeBirds[2].setTexture(Bird.TxThree[mm]);
        BirdTimer.restart();
    }
}
void drawBirdThemes()
{
    Lone.Draw();
    Ltwo.Draw();
    Birds.setNanimate();
    //window.draw(menu.darkWindow);
    window.draw(menu.SpriteReturnBack);
    window.draw(menu.rectangle);
    window.draw(menu.triangle);

    for (int i = 0; i < 3; i++)
    {
        window.draw(ForBirdTheme[i]);
        window.draw(ForBirds[i]);
        window.draw(ThreeBirds[i]);
    }
}
}Birds;

struct ForModeControl
{
    void ControlModeOne()
    {
        if (currentGameState == GameState::eTTP && oo == 1 && Keyboard::isKeyPressed(Keyboard::Space))
            currentGameState = GameState::eGame;
        if (currentGameState != GameState::MainMenu)
            Reseting = 0;
    }

    void Reset()
    {
        if (currentGameState == GameState::MainMenu && !Reseting)
        {
            Bird.Constructor(1.5, 300, 250);

            Pipes.Constructor();

            Lone.Constructor(0, 700);
            Ltwo.Constructor(1700, 700);

            Themes.Constructor();

            Tmenu.Constructor();

            Gover.Constuctor();

            Score.Constructor(800, 100, 50);

            Flash.Constructor();

            Reseting = 1;
        }
    }
} Control;

struct Gamemodes
{
    void MainMenu()
    {
        if (currentGameState != GameState::eGame)
        {
            Lone.MoveGrounds();
            Ltwo.MoveGrounds();

            //window.clear();

            Themes.Draw();
            if (currentGameState == GameState::Credits)
            {
                //credits.draw();
                credits.move();
                credits.wingMove();
            }
        }
    }
    void TTPlay()
    {
        Bird.Animate();
        Bird.UpNDown();



        Themes.Draw();

        Tmenu.Draw();

        Bird.Draw();

        Lone.Draw();
        Ltwo.Draw();

    }
    void Playing()
    {
        Collide.CollisionWGround();
        Collide.CollisionWGround();
        Collide.CollisionWPipes(Pipes.PiUp);
        Collide.CollisionWPipes(Pipes.PiDown);

        Bird.GUI();

        Bird.Animate();
        Pipes.MovePipes();
        Pipes.ResetPipesPosition();


        Score.IncScore();

        Bird.Draw();

        Pipes.Draw();

        Lone.Draw();
        Ltwo.Draw();

        Score.DrawScore();
    }
    void Collided()
    {
        Bird.MoveDTCol();
        Gover.medall();

        Gover.Transition();

        Score.ForGameOver();


        Themes.Draw();

        Pipes.Draw();

        Bird.Draw();

        Lone.Draw();
        Ltwo.Draw();

        Gover.Draw();

        Score.DrawScore();
        Score.DrawHighScore();
        Flash.Draw();

        window.draw(menu.SpriteReturnBack);
        window.draw(menu.rectangle);
        window.draw(menu.triangle);
    }
} Mode;

struct Game
{
    void Flappy_Bird() // Call Me
    {
        switch (currentGameState)
        {
        case MainMenu:
            menu.draw();
            break;
        case eTTP:
            Mode.TTPlay();
            break;
        case eGame:
            Mode.Playing();
            break;
        case eDifficulty:
            level.draw();
            break;
        case Credits:
            credits.draw();
            break;
        case eThemes:
            theme.draw();
            break;
        case BirdThemes:
            Birds.drawBirdThemes();
            break;
        case GameOver:
            Mode.Collided();
            break;
        default:
            break;
        }
    }
} Game;


int main()
{
    srand(time(NULL));
    window.setPosition(Vector2i(100, 50));
    window.setFramerateLimit(60);
    setAssets();
    Birds.Constructor();
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            transition();
            if (currentGameState == GameState::eGame || currentGameState == GameState::eTTP)
                Bird.GUI();
        }
        if (currentGameState == GameState::eGame)
            Difficulty.DifficultySettings();

        Control.ControlModeOne();
        Control.Reset();
        Score.hsSetup();
        draw();
        //cout << Mouse::getPosition(window).x << "         " << Mouse::getPosition(window).y << endl;
    }
    return 0;
}

void setAssets()
{
    menu.set();
    Bird.Constructor(2, 500, 350);
    level.set();
    theme.set();
    setFonts();
    credits.set();
}

void draw()
{
    if (currentGameState != GameState::GameOver)
    {
        Lone.MoveGrounds();
        Ltwo.MoveGrounds();
    }

    window.clear();
    Themes.Draw();
    Game.Flappy_Bird();
    animation();
    window.display();
}

void transition()
{
    Themes.ChoosingThemeGUI();

    //return
    if (Mouse::isButtonPressed(Mouse::Left) && currentGameState != GameState::eGame)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        if (mousePos.x > 6 && mousePos.x < 96 && mousePos.y > 7 && mousePos.y < 57)
            currentGameState = GameState::MainMenu;
    }
    //button press

    if (Mouse::isButtonPressed(Mouse::Left) && currentGameState == GameState::MainMenu)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        if (mousePos.x > 500 && mousePos.x < 654 && mousePos.y > 550 && mousePos.y < 634)
        {
            currentGameState = GameState::eDifficulty;
        }
    }
    //themes
    if (Mouse::isButtonPressed(Mouse::Left) && currentGameState == GameState::MainMenu)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        if (mousePos.x > 900 && mousePos.x < 1053 && mousePos.y > 550 && mousePos.y < 634)
        {
            currentGameState = GameState::eThemes;
        }
    }
    //credits
    if (Mouse::isButtonPressed(Mouse::Left) && currentGameState == GameState::MainMenu)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        if (mousePos.x > 700 && mousePos.x < 852 && mousePos.y > 550 && mousePos.y < 635)
        {
            currentGameState = GameState::Credits;
        }
    }

    // Check for mouse click events
    /*
    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        // Check if the return back sprite is clicked
    }
    */
    // levels
    if (Mouse::isButtonPressed(Mouse::Left) && currentGameState == GameState::eDifficulty)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        if (mousePos.x > 736 && mousePos.x < 939 && mousePos.y > 150 && mousePos.y < 262)
        {
            diff = 0;
            currentGameState = GameState::eTTP;
            oo = 1;
        }

        if (mousePos.x > 736 && mousePos.x < 939 && mousePos.y > 321 && mousePos.y < 434)
        {
            diff = 1;
            currentGameState = GameState::eTTP;
            oo = 1;
        }
        if (mousePos.x > 736 && mousePos.x < 939 && mousePos.y > 492 && mousePos.y < 602)
        {
            diff = 2;
            currentGameState = GameState::eTTP;
            oo = 1;
        }
    }
    // Bird Themes
    if (Mouse::isButtonPressed(Mouse::Left) && currentGameState == GameState::MainMenu)
    {
        Vector2i mousePos = Mouse::getPosition(window);
        if (mousePos.x > 1100 && mousePos.x < 1250 && mousePos.y>550 && mousePos.y < 790)
        {
            currentGameState = GameState::BirdThemes;
        }
    }
}

void animation() // for credits
{

    if (currentGameState == GameState::Credits)
    {
        Bird.Animate();
        credits.move();
        credits.wingMove();
    }

    else
        credits.ResetCredits();
}