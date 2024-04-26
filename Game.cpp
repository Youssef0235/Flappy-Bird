#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream> 

using namespace std;
using namespace sf;

/* =================== EASY LEVEL =================== */ 

#define SPEED -3 // Speed of pipes and grounds
#define Sover -10 // Transition for game over
#define GAP 102 // gap between pipes
#define DIS 440 // distances between pipes
#define XPSCORE 380 // X - Position Of Score

RenderWindow window(VideoMode(600, 600), "Flappy Bird");
Event event;

int modes = 0, IMPOSSIBLE = 0;
bool ttp = 0;

struct Flash
{
    RectangleShape TheFlash;
    Clock Time;
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
            Time.restart();
            RestartTime = 1;
        }

        if (FlashOn && Time.getElapsedTime().asSeconds() < 0.15)
            TheFlash.setFillColor(Color(0, 0, 0, 150));

        else
            TheFlash.setFillColor(Color(0, 0, 0, 0));
        
        window.draw(TheFlash);
    }
} Flash;

struct Pipes
{
    Sprite PiUp[3];
    Sprite PiDown[3];
    Texture Tex[2];

    int Rand;
    bool x, y;

    void Constructor()
    {
        Tex[0].loadFromFile("pipeup.png");
        Tex[1].loadFromFile("pipedown.png");

        for (int i = 0; i < 3; i++)
        {
            GenRan();
            PiUp[i].setTexture(Tex[0]);
            PiUp[i].setOrigin(PiUp[i].getGlobalBounds().width / 2, PiUp[i].getOrigin().y);
            PiUp[i].setPosition(700, Rand);

            PiDown[i].setTexture(Tex[1]);
            PiDown[i].setOrigin(PiDown[i].getGlobalBounds().width / 2, PiDown[i].getOrigin().y);
            PiDown[i].setPosition(700, Rand - GAP * 5);
        }
        x = 0, y = 0;
    }

    void GenRan()
    {
        Rand = 175 + rand() % 296;
    }
    
    void MovePipesOne()
    {
        GenRan();
        PiUp[0].move(SPEED, 0);
        PiDown[0].move(SPEED, 0);

        if (PiUp[0].getPosition().x <= -69)
        {
            PiUp[0].setPosition(700, Rand);
            PiDown[0].setPosition(700, Rand - GAP * 5);
        }
    }

    void MovePipesTwo()
    {
        GenRan();

        if (PiUp[0].getPosition().x <= DIS)
            x = 1;

        if (x == 1)
        {
            PiUp[1].move(SPEED, 0);
            PiDown[1].move(SPEED, 0);
        }

        if (PiUp[1].getPosition().x <= -69)
        {
            PiUp[1].setPosition(700, Rand);
            PiDown[1].setPosition(700, Rand - GAP * 5);
        }
    }

    void MovePipesThree()
    {
        GenRan();

        if (PiUp[1].getPosition().x <= DIS)
            y = 1;

        if (y == 1)
        {
            PiUp[2].move(SPEED, 0);
            PiDown[2].move(SPEED, 0);
        }

        if (PiUp[2].getPosition().x <= -69)
        {
            PiUp[2].setPosition(700, Rand);
            PiDown[2].setPosition(700, Rand - (GAP * 5) + 10);
        }
    }

    void MovePipes()
    {
        MovePipesOne();
        MovePipesTwo();
        MovePipesThree();
    }
    
    void Draw()
    {
        for (int i = 0; i < 3; i++)
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
    }

    void MoveGrounds()
    {
        TwoGrounds.move(SPEED, 0);

        if (TwoGrounds.getPosition().x <= -780)
            TwoGrounds.setPosition(780, 500);
    }

    void Draw()
    {
        window.draw(TwoGrounds);
    }

} Lone, Ltwo;

struct Bird
{
    Texture Tx[4];
    Sprite Bird;
    Clock dt, UpAndDown;
    SoundBuffer H, J, W;
    Sound Hit, Jump, Whoop;
    int Rotation = 0, Iterator = 0;
    float Velocity;
    bool Input = 0, Splayed, ToMove;

    void Constructor(float Scale, float PosX, float PosY)
    {
        Tx[0].loadFromFile("birddown.png");
        Tx[1].loadFromFile("birdmid.png");
        Tx[2].loadFromFile("birdup.png");
        Tx[3].loadFromFile("birdmid.png");

        Bird.setTexture(Tx[Iterator]);
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

        Splayed = 0, Velocity = 0, ToMove = 0;
    }

    void Animate()
    {
        if (dt.getElapsedTime().asSeconds() > 0.06)
        {
            if (Iterator < 3)
                Iterator++;

            else
                Iterator = 0;

            Bird.setTexture(Tx[Iterator]);
            dt.restart();
        }
    }

    void UpNDown()
    {
        if (UpAndDown.getElapsedTime().asSeconds() >= 0.1 && !ToMove)
        {
            Bird.move(0, -0.5);
            ToMove = 1;
        }
        if (UpAndDown.getElapsedTime().asSeconds() >= 0.2 && ToMove)
        {
            Bird.move(0, 0.5);
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


    void gravNvelo(Event event)
    {
        if (Bird.getPosition().y <= 0)
            Bird.setPosition(Bird.getPosition().x, 0);

        if (Velocity >= 0)
            RotateDown();

        else 
            RotateUp();

            Bird.move(0, Velocity);
            Velocity += 0.3;
        
        if (event.type == Event::MouseButtonPressed && !Input)
        {
            if (event.mouseButton.button == Mouse::Left)
            {
                Jump.play();

                Input = 1;
                Velocity = -5;
                Bird.move(0, Velocity * 9/5);
            }
        }

        if (event.type == Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == Mouse::Left)
                Input = 0;
        }
    }

    void CollisionWPipes(Sprite pipes[])
    {
        if (Bird.getGlobalBounds().intersects(pipes[0].getGlobalBounds()) || Bird.getGlobalBounds().intersects(pipes[1].getGlobalBounds()) || Bird.getGlobalBounds().intersects(pipes[2].getGlobalBounds()))
        {
            if (!Splayed)
            {
                Whoop.play();
                Hit.play();
                Splayed = 1;
            }
            Bird.setRotation(70);
            Flash.FlashOn = 1;
            modes = 2;
        }
    }

    void CollisionWGround()
    {
        if (Bird.getGlobalBounds().intersects(Lone.TwoGrounds.getGlobalBounds()) || Bird.getGlobalBounds().intersects(Ltwo.TwoGrounds.getGlobalBounds()))
        {
            if (!Splayed)
            {
                Hit.play();
                Splayed = 1;
            }
            modes = 2;
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
    }

    void Draw(int x)
    {
        themes.setTexture(Textures[x]);
        window.draw(themes);
    }
} Themes;

struct Scoring
{
    Font ScoreFont;
    Text Score, HighScore;
    SoundBuffer ScoInc;
    Sound ScoreSound;
    int Sinc, highScore = 0;
    bool Inc, MoveScoreUp;

    void Constructor(int x, int y, int size)
    {
        ScoreFont.loadFromFile("JANDAMANATEESOLID.ttf");
        Score.setFont(ScoreFont);
        Score.setCharacterSize(size);
        Score.setPosition(x, y); // 280, 100
        Score.setString("1");
        Score.setString(to_string(Sinc));

        HighScore.setFont(ScoreFont);
        HighScore.setString(to_string(highScore));
        HighScore.setPosition(XPSCORE, 1175);
        HighScore.setCharacterSize(20);

        ScoInc.loadFromFile("Score.wav");
        ScoreSound.setBuffer(ScoInc);

        ifstream highScoreFile("highscore.txt");

        if (highScoreFile.is_open())
        {
            highScoreFile >> highScore;
            highScoreFile.close();
        }
        Inc = 0, MoveScoreUp = 0, Sinc = 0;
    }

    void hsSetup()
    {
        if (Sinc > highScore)
            highScore = Sinc;

        ofstream highScoreFile("highscore.txt");
        if (highScoreFile.is_open())
        {
            highScoreFile << highScore;
            highScoreFile.close();
        }
        // Reset Highest Score From Here
    }

    void IncScore()
    {
        if ((Bird.Bird.getPosition().x + 1 == Pipes.PiUp[0].getPosition().x || Bird.Bird.getPosition().x + 1 == Pipes.PiUp[1].getPosition().x || Bird.Bird.getPosition().x + 1 == Pipes.PiUp[2].getPosition().x))
        {
             ScoreSound.play();
             Sinc++;
             Score.setString(to_string(Sinc));
        }
    }

    void ForGover()
    {
        if (!MoveScoreUp)
        {
            Constructor(XPSCORE, 1115, 20);
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

struct TapToPlayMenu
{
    Texture Textures[2]; // Textures[0] = ttp, Textures[1] = get ready
    Sprite Stuff[2]; // Stuff[0] = ttp, Stuff[1] = get ready
    void Constructor()
    {
        Textures[0].loadFromFile("ttplay.png");
        Stuff[0].setTexture(Textures[0]);
        Stuff[0].setScale(0.8, 0.8);
        Stuff[0].setPosition(250, 200);

        Textures[1].loadFromFile("getready.png");
        Stuff[1].setTexture(Textures[1]);
        Stuff[1].setScale(1.1, 1.1);
        Stuff[1].setPosition(185, 100);
    }
    void Draw()
    {
        window.draw(Stuff[0]);
        window.draw(Stuff[1]);
    }
} Tmenu;

struct GameOverMenu
{
    Texture gameOver[4];
    Sprite over[4];

    void Constuctor()
    {
        gameOver[0].loadFromFile("gameover.png");
        gameOver[1].loadFromFile("score.png");
        gameOver[2].loadFromFile("Play.png");
        gameOver[3].loadFromFile("Themes.png");

        for (int i = 0; i < 4; i++)
        {
            over[i].setTexture(gameOver[i]);  // over 0 = gameover, over 1 = score

            if (i < 2)
                over[i].setScale(1.2, 1.2);
        }

        over[0].setPosition(175, 1000);
        over[1].setPosition(150, 1075);

        over[2].setPosition(175, 1225);
        over[3].setPosition(300, 1225);
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
    }

    void Draw()
    {
        window.draw(over[0]);
        window.draw(over[1]);
        window.draw(over[2]);
        window.draw(over[3]);
    }
} Gover;

struct ForModeControl
{
    void ControlModeOne ()
    {
        if (Mouse::isButtonPressed(Mouse::Left) && modes == 0 && !ttp)
        modes = 1;
    }

    void Reset()
    {
        if (Mouse::isButtonPressed(Mouse::Left) && modes == 2 && Mouse::getPosition().x <= 683 && Mouse::getPosition().x >= 587 && Mouse::getPosition().y <= 530 && Mouse::getPosition().y >= 480 && Gover.over[2].getPosition().y <= 400)
        {
                ttp = 1;
                modes = 0;

                Lone.Constructor(0, 500);
                Ltwo.Constructor(780, 500);

                Pipes.Constructor();

                Bird.Constructor(1.2, 150, 250);

                Themes.Constructor();

                Score.Constructor(280, 100, 50);

                Gover.Constuctor();

                Flash.Constructor();
        }

        if (event.type == Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == Mouse::Left)
                ttp = 0;
        }
    }
} Control;

struct Gamemodes
{
    void TTPlay()
    {
        Bird.Animate();
        Bird.UpNDown();

        Lone.MoveGrounds();
        Ltwo.MoveGrounds();

        window.clear();

        Themes.Draw(0);

        Tmenu.Draw();

        Bird.Draw();

        Lone.Draw();
        Ltwo.Draw();

        window.display();
    }
    void Playing()
    {
        Bird.CollisionWGround();
        Bird.CollisionWGround();
        Bird.Animate();
        Bird.gravNvelo(event);

        Bird.CollisionWPipes(Pipes.PiUp);
        Bird.CollisionWPipes(Pipes.PiDown);

        Pipes.MovePipes();

        Lone.MoveGrounds();
        Ltwo.MoveGrounds();
       
        Score.IncScore();

        window.clear();

        Themes.Draw(0);

        Bird.Draw();

        Pipes.Draw();

        Lone.Draw();
        Ltwo.Draw();

        Score.DrawScore();

        window.display();
    }

    void Collided()
    {
        Bird.MoveDTCol();

        Gover.Transition();

        Score.ForGover();

        window.clear();

        Themes.Draw(0);

        Pipes.Draw();

        Bird.Draw();

        Lone.Draw();
        Ltwo.Draw();

        Gover.Draw();

        Score.DrawScore();
        Score.DrawHighScore();

        Flash.Draw();

        window.display();
    }
} Mode;

int main()
{
    window.setPosition(Vector2i(400, 50));
    window.setFramerateLimit(60);

    // Objects

    Bird.Constructor(1.2, 150, 250);

    Pipes.Constructor();

    Lone.Constructor(0, 500);
    Ltwo.Constructor(780, 500);

    Themes.Constructor();

    Tmenu.Constructor();

    Gover.Constuctor();

    Score.Constructor(280, 100, 50);

    Flash.Constructor();

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }
 
        Control.ControlModeOne();
        Control.Reset();
        Score.hsSetup();

        if (modes == 0)
            Mode.TTPlay();

        if (modes == 1)
            Mode.Playing();

        if (modes == 2)
            Mode.Collided();
    }
    return 0;
}