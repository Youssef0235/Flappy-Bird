#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream> 

#define SPEED - 4 // Speed of pipes and grounds
#define Sover -10 // Transition for game over
#define GAP 105 // gap between pipes
#define DIS 470 // distances between pipes

int modes = 0;


using namespace std;
using namespace sf;

RenderWindow window(VideoMode(600, 600), "Flappy Bird");

struct Pipes
{
    Sprite PiUp[3];
    Sprite PiDown[3];
    Texture Tex[2];

    int Rand;
    bool x, y;

    // Generate Random Positions

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
            PiUp[0].setPosition(600, Rand);
            PiDown[0].setPosition(600, Rand - GAP * 5);
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
            PiUp[1].setPosition(600, Rand);
            PiDown[1].setPosition(600, Rand - GAP * 5);
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
            PiUp[2].setPosition(600, Rand);
            PiDown[2].setPosition(600, Rand - GAP * 5);
        }
    }

    void MovePipes()
    {
        MovePipesOne();
        MovePipesTwo();
        MovePipesThree();
    }

    void Constructor(Texture Tx[])
    {
        for (int i = 0; i < 2; i++)
        {
            Tex[i] = Tx[i];
        }

        for (int i = 0; i < 3; i++)
        {
            GenRan();
            PiUp[i].setTexture(Tex[0]);
            PiUp[i].setPosition(700, Rand);

            PiDown[i].setTexture(Tex[1]);
            PiDown[i].setPosition(700, Rand - GAP * 5);
        }

        Rand = 0;
        x = 0, y = 0;
    }


    void DisplayPipes(RenderWindow& window)
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


    void Constructor(int PosX, int PosY, Texture& Ground)
    {
        TwoGrounds.setTexture(Ground);
        TwoGrounds.setPosition(PosX, PosY);
    }

    void MoveGrounds()
    {
        TwoGrounds.move(SPEED, 0);

        if (TwoGrounds.getPosition().x <= -780)
            TwoGrounds.setPosition(780, 500);
    }

    void Draw(RenderWindow& window)
    {
        window.draw(TwoGrounds);
    }

} Lone, Ltwo;

struct Bird
{
    Texture Tx[4];
    int Rotation = 0, Iterator = 0;
    Clock dt;
    Sprite Bird;
    float Velocity;
    bool Input = 0, Splayed;
    SoundBuffer J, H, W;
    Sound Hit, Jump, Whoop;


    void Constructor(Texture Tex[], float Scale, float PosX, float PosY)
    {
        for (int i = 0; i < 4; i++)
            Tx[i] = Tex[i];

        Bird.setTexture(Tex[Iterator]);
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

        Splayed = 0;
        Velocity = 0;
    }

    void Animate()
    {
        if (dt.getElapsedTime().asSeconds() > 0.05)
        {
            if (Iterator < 3)
                Iterator++;

            else
                Iterator = 0;

            Bird.setTexture(Tx[Iterator]);
            dt.restart();
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
        if (Bird.getPosition().y < 0)
            Bird.setPosition(Bird.getPosition().x, 10);

        if (Velocity > 0)
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

            modes = 2;
        }
    }

    void CollisionWGround(Sprite x)
    {
        if (Bird.getGlobalBounds().intersects(x.getGlobalBounds()))
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

    void Draw(RenderWindow& window)
    {
        window.draw(Bird);
    }
} Bird;

struct Themes
{
    Sprite themes;
    Texture Textures[4];

    void Constructor(Texture Tex[4])
    {
        for (int i = 0; i < 4; i++)
            Textures[i] = Tex[i];

        themes.setTexture(Textures[0]);
    }

    void Draw(RenderWindow& window, int x)
    {
        if (x < 0 || x > 3)
            themes.setTexture(Textures[0]);

        else
            themes.setTexture(Textures[x]);

        window.draw(themes);
    }
} Themes;

struct Scoring
{
    int Sinc = 0, highScore = 0;
    Font ScoreFont;
    Text Score, HighScore;
    SoundBuffer ScoInc;
    Sound S;


    void Constructor(int x, int y, int size)
    {
        ScoreFont.loadFromFile("JANDAMANATEESOLID.ttf");
        Score.setFont(ScoreFont);
        Score.setCharacterSize(size);
        Score.setPosition(x, y); // 280, 100
        Score.setString("0");
        Score.setString(to_string(Sinc));

        HighScore.setFont(ScoreFont);
        HighScore.setString(to_string(highScore));
        HighScore.setPosition(390, 345);
        HighScore.setCharacterSize(20);

        ScoInc.loadFromFile("Score.wav");
        S.setBuffer(ScoInc);

        ifstream highScoreFile("highscore.txt");

        if (highScoreFile.is_open())
        {
            highScoreFile >> highScore;
            highScoreFile.close();
        }
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
        if (Bird.Bird.getPosition().x == Pipes.PiUp[0].getPosition().x || Bird.Bird.getPosition().x == Pipes.PiUp[1].getPosition().x || Bird.Bird.getPosition().x == Pipes.PiUp[2].getPosition().x)
        {
            S.play();
            Sinc++;
            Score.setString(to_string(Sinc));
        }
    }

    void DisplayScore(RenderWindow& window)
    {
        window.draw(Score);
    }

    void DisplayHighScore(RenderWindow& window)
    {
        window.draw(HighScore);
    }
}Score;

struct GameOverMenu
{
    Sprite over[4];

    void Constuctor(Texture gOver[])
    {
        for (int i = 0; i < 2; i++)
        {
            over[i].setTexture(gOver[i]);  // over 0 = gameover, over 1 = score
            over[i].setScale(1.2, 1.2);
        }

        for (int i = 2; i < 4; i++)
            over[i].setTexture(gOver[i]);

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

    void Draw(RenderWindow& window)
    {
        window.draw(over[0]);
        window.draw(over[1]);
        window.draw(over[2]);
        window.draw(over[3]);
    }
} Gover;

struct Gamemodes
{
    Sprite tt;
    Sprite rr;

    Texture t;
    Texture r;

    void TTPlay(RenderWindow& window)
    {
        t.loadFromFile("ttplay.png");
        tt.setTexture(t);
        tt.setScale(0.8, 0.8);
        tt.setPosition(250, 200);
        
        r.loadFromFile("getready.png");
        rr.setTexture(r);
        rr.setScale(1, 1);
        rr.setPosition(200, 100);

        window.clear();

        Themes.Draw(window, 0);
        Bird.Draw(window);
        Bird.Animate();
        Lone.MoveGrounds();
        Ltwo.MoveGrounds();
        Lone.Draw(window);
        Ltwo.Draw(window);
        //window.draw(tt);
        window.draw(rr);
        window.display();
    }
    void Playing(RenderWindow& window, Event event)
    {
        Bird.CollisionWGround(Lone.TwoGrounds);
        Bird.CollisionWGround(Ltwo.TwoGrounds);

        Bird.CollisionWPipes(Pipes.PiUp);
        Bird.CollisionWPipes(Pipes.PiDown);

        Score.IncScore();

        window.clear();

        Lone.MoveGrounds();
        Ltwo.MoveGrounds();

        Pipes.MovePipes();


        Bird.Animate();
        Bird.gravNvelo(event);

        Themes.Draw(window, 0);

        Pipes.DisplayPipes(window);

        Bird.Draw(window);
        Score.DisplayScore(window);
        Lone.Draw(window);
        Ltwo.Draw(window);


        window.display();
    }

    void Collided(RenderWindow& window)
    {
        window.clear();

        Bird.MoveDTCol();

        Themes.Draw(window, 0);

        Pipes.DisplayPipes(window);

        Bird.Draw(window);
        Score.DisplayScore(window);

        Lone.Draw(window);
        Ltwo.Draw(window);

        Gover.Draw(window);
        Gover.Transition();

        Score.Constructor(390, 290, 20);
        Score.DisplayScore(window);


        Score.DisplayHighScore(window);

        window.display();
    }
}M;




int main()
{
    window.setPosition(Vector2i(400, 50));
    window.setFramerateLimit(60);

    // Textures

    Texture backgrounds[2], pitex[2], bird[4], th[4], gameOver[4];

    bird[0].loadFromFile("birddown.png");
    bird[1].loadFromFile("birdmid.png");
    bird[2].loadFromFile("birdup.png");
    bird[3].loadFromFile("birdmid.png");

    backgrounds[0].loadFromFile("Land.PNG");
    backgrounds[1].loadFromFile("ttplay.png");

    th[0].loadFromFile("Classic.png");
    th[1].loadFromFile("Shanghai.png");
    th[2].loadFromFile("Tokyo.png");
    th[3].loadFromFile("London.png");

    pitex[0].loadFromFile("pipeup.png");
    pitex[1].loadFromFile("pipedown.png");

    gameOver[0].loadFromFile("gameover.png");
    gameOver[1].loadFromFile("score.png");
    gameOver[2].loadFromFile("Play.png");
    gameOver[3].loadFromFile("Themes.png");


    // Variables

    int IMPOSSIBLE = 0;
    bool ttp = 0;

    // Objects


    Lone.Constructor(0, 500, backgrounds[0]);
    Ltwo.Constructor(780, 500, backgrounds[0]);

    Pipes.Constructor(pitex);

    Bird.Constructor(bird, 1.2, 200, 300);

    Themes.Constructor(th);

    Score.Constructor(280, 100, 50);

    Gover.Constuctor(gameOver);



    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (Mouse::isButtonPressed(Mouse::Left) && modes == 0 && !ttp)
            modes = 1;


        if (Mouse::isButtonPressed(Mouse::Left) && modes == 2 && Mouse::getPosition().x <= 683 && Mouse::getPosition().x >= 587 && Mouse::getPosition().y <= 530 && Mouse::getPosition().y >= 480 && Gover.over[2].getPosition().y <= 480)
        {
            do
            {
                ttp = 1;
                modes = 0;
                Lone.Constructor(0, 500, backgrounds[0]);
                Ltwo.Constructor(780, 500, backgrounds[0]);

                Pipes.Constructor(pitex);

                Bird.Constructor(bird, 1.3, 200, 300);

                Themes.Constructor(th);

                Score.Constructor(280, 100, 50);

                Gover.Constuctor(gameOver);

                Score.Sinc = 0;
                Score.Score.setString(to_string(Score.Sinc));
            }
            while (IMPOSSIBLE > 0);
        }

        if (event.type == Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == Mouse::Left)
                ttp = 0;
        }

        Score.hsSetup();

        if (modes == 0)
            M.TTPlay(window);

        if (modes == 1)
            M.Playing(window, event);

        if (modes == 2)
            M.Collided(window);
    }
    return 0;
}