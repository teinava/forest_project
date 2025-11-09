#include <glfw3.h>
#include <math.h>
#define M_PI 3.141592653
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <chrono>
#include <string>
using namespace std::chrono;

const int windowWidth = 844, windowHeight = 652;
const int texCount = 60;

GLuint textures[texCount];

float angle = 0;
int frameCount = 1;
int playerScore = 0;
bool gameOver = false;
milliseconds currentTime;

class gameObject
{
protected:
    float vertex[12];
    float texCoord[8];
    float x, y;
    float angle;
public:
    gameObject(int imageWidth, int imageHeight)
    {
        this->angle = ::angle;

        texCoord[0] = 0, texCoord[1] = 1;
        texCoord[2] = 1, texCoord[3] = 1;
        texCoord[4] = 1, texCoord[5] = 0;
        texCoord[6] = 0, texCoord[7] = 0;

        this->x = (imageWidth * 2.0f) / windowWidth;
        this->y = (imageHeight * 2.0f) / windowHeight;

        vertex[0] = -x, vertex[1] = -y, vertex[2] = 0;
        vertex[3] = x, vertex[4] = -y, vertex[5] = 0;
        vertex[6] = x, vertex[7] = y, vertex[8] = 0;
        vertex[9] = -x, vertex[10] = y, vertex[11] = 0;
    }

    void rotation()
    {
        angle = ::angle;

        if (this->angle >= 360 || this->angle <= -360) angle = 0;
        angle = angle * M_PI / 180;
        float tempX = vertex[0];
        float tempY = vertex[1];
        vertex[0] = tempX * cos(angle) - tempY * sin(angle);
        vertex[1] = tempX * sin(angle) + tempY * cos(angle);

        vertex[3] = vertex[0] + x * 2;
        vertex[4] = vertex[1];

        vertex[6] = vertex[0] + x * 2;
        vertex[7] = vertex[1] + y * 2;

        vertex[9] = vertex[0];
        vertex[10] = vertex[1] + y * 2;
    }

    void moveObject(float x, float y)
    {
        for (int i = 0; i < 12; i += 3)
        {
            vertex[i] += x;
        }
        for (int i = 1; i < 12; i += 3)
        {
            vertex[i] += y;
        }
    }

    void drawTexture(int texID)
    {
        glBindTexture(GL_TEXTURE_2D, textures[texID]);
        glVertexPointer(3, GL_FLOAT, 0, vertex);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    void bindTexture(float coordX, float coordY)
    {
        vertex[0] = coordX;
        vertex[1] = coordY;

        vertex[3] = vertex[0] + x * 2;
        vertex[4] = vertex[1];

        vertex[6] = vertex[0] + x * 2;
        vertex[7] = vertex[1] + y * 2;

        vertex[9] = vertex[0];
        vertex[10] = vertex[1] + y * 2;
    }

    void getTexCoords(float& coordX, float& coordY)
    {
        float offset = 0.15;
        coordX = vertex[0] + offset;
        coordY = vertex[1] + offset;
    }
};

class player : public gameObject
{
public:
    enum positions
    {
        left = 1,
        right,
        up,
        down
    };

    enum states
    {
        calm = 1,
        attack
    };

protected:
    positions position;
    states state;
    int health;
    float posX, posY;
    bool updateStartTime;
    long long startTime;

public:
    player(int imageWidth, int imageHeight) : gameObject(imageWidth, imageHeight)
    {
        this->health = 100;
        this->state = calm;
        this->position = up;
        this->posX = 0;
        this->posY = 0;
        this->updateStartTime = true;
    }

    void playerMove(float moveX, float moveY)
    {
        this->posX += moveX;
        this->posY += moveY;
    }

    void takeDamage(int damage)
    {
        long long duration = 2000;
        auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

        if (updateStartTime)
        {
            startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            updateStartTime = false;
        }

        long long endTime = startTime + duration;
        
        if (now <= endTime) {}
        else
        {
            this->health -= damage;
            this->updateStartTime = true;
        }
    }

    void updateState(states newState)
    {
        this->state = newState;
    }

    void updatePosition(positions newPosition)
    {
        this->position = newPosition;
    }

    void changeDogPosition()
    {
        int maxCount = 20;

        switch (state)
        {
        case calm:
        {
            switch (position)
            {
            case left:
                if (frameCount == 0) drawTexture(10);
                if ((frameCount >= 1) && (frameCount < (maxCount / 2))) drawTexture(11);
                if ((frameCount >= (maxCount / 2)) && (frameCount <= maxCount)) drawTexture(12);
                break;
            case right:
                if (frameCount == 0) drawTexture(13);
                if ((frameCount >= 1) && (frameCount < (maxCount / 2))) drawTexture(14);
                if ((frameCount >= (maxCount / 2)) && (frameCount <= maxCount)) drawTexture(15);
                break;
            case up:
                if (frameCount == 0) drawTexture(17);
                if ((frameCount >= 1) && (frameCount < (maxCount / 2))) drawTexture(18);
                if ((frameCount >= (maxCount / 2)) && (frameCount <= maxCount)) drawTexture(19);
                break;
            case down:
                if (frameCount == 0) drawTexture(7);
                if ((frameCount >= 1) && (frameCount < (maxCount / 2))) drawTexture(8);
                if ((frameCount >= (maxCount / 2)) && (frameCount <= maxCount)) drawTexture(9);
                break;
            }
        }
        break;
        case attack:
        {
            switch (position)
            {
            case left:
            {
                drawTexture(4);
                break;
            }
            case right:
            {
                drawTexture(5);
                break;
            }
            case up:
            {
                drawTexture(6);
                break;
            }
            case down:
            {
                drawTexture(3);
                break;
            }
            }
        }
        break;
        }
    }

    int getHealth()
    {
        return health;
    }

    float getPosX()
    {
        return posX;
    }

    float getPosY()
    {
        return posY;
    }
};

class tree : public gameObject
{
public:
    tree() : gameObject(30.0f, 54.0f) { }
};

class healthBar : public gameObject
{
public:
    healthBar(int imageWidth, int imageHeight) : gameObject(imageWidth, imageHeight) { }

    void updateHealthBar(int health)
    {
        if ((health <= 100) && (health > 83)) drawTexture(28);
        if ((health <= 83) && (health > 66)) drawTexture(27);
        if ((health <= 66) && (health > 49)) drawTexture(26);
        if ((health <= 49) && (health > 32)) drawTexture(25);
        if ((health <= 32) && (health > 15)) drawTexture(24);
        if ((health <= 15) && (health > 0)) drawTexture(23);
        if (health <= 0) drawTexture(22);
    }
};

class scoreBar : public gameObject
{
public:
    scoreBar(int imageWidth, int imageHeight) : gameObject(imageWidth, imageHeight) { }

    void drawText(float posX, float posY, float scaleX, float scaleY, std::string text)
    {
        const float charSize = 1 / 16.0f;
        glBindTexture(GL_TEXTURE_2D, textures[20]);
        glVertexPointer(3, GL_FLOAT, 0, vertex);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
        glTranslatef(posX, posY, 0);
        glScalef(scaleX, scaleY, 1);
        for (int i = 0; i < text.length(); i++)
        {
            int c = text[i];
            int charNum = c - 32;
            int numCols = 16;
            int y = charNum / numCols;
            int x = charNum % numCols;
            struct rct
            {
                float left;
                float right;
                float top;
                float bottom;
            };
            rct rct;
            rct.left = x * charSize;
            rct.right = rct.left + charSize;
            rct.top = y * charSize;
            rct.bottom = rct.top + charSize;
            texCoord[0] = texCoord[6] = rct.left;
            texCoord[2] = texCoord[4] = rct.right;
            texCoord[1] = texCoord[3] = rct.bottom;
            texCoord[5] = texCoord[7] = rct.top;
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glTranslatef(1, 0.0, 0.0);
        }
    }
};

class balloon : public gameObject
{
public:
    balloon(int imageWidth, int imageHeight) : gameObject(imageWidth, imageHeight) { }

    void updateBalloon(milliseconds mss)
    {
        milliseconds endTime = currentTime + mss;
        if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) > endTime) drawTexture(2);
        else drawTexture(0);
    }
};

class enemy : public gameObject
{
public:
    enum states
    {
        inPlace,
        move,
        attack,
        death,
        revealing
    };

private:
    enum enemyDistance
    {
        enemyIsNearby,
        enemyIsClose,
        enemyIsFarAway,
        enemyIsBlowing
    };
    states state;
    int health;
    float distance1;

public:
    bool isEnemyResurrect;
    long long startTime;
    bool updateStartTime;
    float distance2;
    bool isEnemyRespawn;
    bool increaseScore;

    enemy() : gameObject (63.0f, 45.0f)
    {
        this->isEnemyResurrect = false;
        this->isEnemyRespawn = false;
        this->updateStartTime = true;
        this->increaseScore = false;

        this->state = inPlace;
        this->health = 100;
        this->distance1 = 0.6;
        this->distance2 = 0.1;
    }

    void stateEnemyInPlace()
    {
        long long duration = 1000, framesCount = 2;
        auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        if (updateStartTime)
        {
            startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            updateStartTime = false;
        }
        long long endTime = startTime + duration;
        long long elapsedTime = now - startTime;
        if (now <= endTime)
        {
            long long interval = duration / framesCount;

            if ((elapsedTime >= 0) && (elapsedTime <= interval)) drawTexture(38);
            if ((elapsedTime > interval) && (elapsedTime <= endTime)) drawTexture(39);
        }
        else updateStartTime = true;
    }

    void stateEnemyMove()
    {
        long long duration = 1000, framesCount = 9;
        auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        if (updateStartTime)
        {
            startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            updateStartTime = false;
        }
        long long endTime = startTime + duration;
        long long elapsedTime = now - startTime;
        if (now <= endTime)
        {
            long long interval = duration / framesCount;

            if ((elapsedTime >= 0) && (elapsedTime <= interval)) drawTexture(40);
            if ((elapsedTime > interval) && (elapsedTime <= (interval * 2))) drawTexture(41);
            if ((elapsedTime > (interval * 2)) && (elapsedTime <= (interval * 3))) drawTexture(42);
            if ((elapsedTime > (interval * 3)) && (elapsedTime <= (interval * 4))) drawTexture(43);
            if ((elapsedTime > (interval * 4)) && (elapsedTime <= (interval * 5))) drawTexture(44);
            if ((elapsedTime > (interval * 5)) && (elapsedTime <= (interval * 6))) drawTexture(45);
            if ((elapsedTime > (interval * 6)) && (elapsedTime <= (interval * 7))) drawTexture(46);
            if ((elapsedTime > (interval * 7)) && (elapsedTime <= (interval * 8))) drawTexture(47);
            if ((elapsedTime > (interval * 8)) && (elapsedTime <= endTime)) drawTexture(48);
        }
        else updateStartTime = true;
    }

    void stateEnemyAttack()
    {
        long long duration = 1000, framesCount = 5;
        auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        if (updateStartTime)
        {
            startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            updateStartTime = false;
        }
        long long endTime = startTime + duration;
        long long elapsedTime = now - startTime;
        if (now <= endTime)
        {
            long long interval = duration / framesCount;

            if ((elapsedTime >= 0) && (elapsedTime <= interval)) drawTexture(29);
            if ((elapsedTime > interval) && (elapsedTime <= (interval * 2))) drawTexture(30);
            if ((elapsedTime > (interval * 2)) && (elapsedTime <= (interval * 3))) drawTexture(31);
            if ((elapsedTime > (interval * 3)) && (elapsedTime <= (interval * 4))) drawTexture(32);
            if ((elapsedTime > (interval * 4)) && (elapsedTime <= endTime)) drawTexture(33);
        }
        else updateStartTime = true;
    }

    void stateEnemyDeath()
    {
        long long duration = 2000, framesCount = 4;
        auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        if (updateStartTime)
        {
            startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            updateStartTime = false;
        }
        long long endTime = startTime + duration;
        long long elapsedTime = now - startTime;
        if (now <= endTime)
        {
            long long interval = duration / framesCount;

            if ((elapsedTime >= 0) && (elapsedTime <= interval)) drawTexture(34);
            if ((elapsedTime > interval) && (elapsedTime <= (interval * 2))) drawTexture(35);
            if ((elapsedTime > (interval * 2)) && (elapsedTime <= (interval * 3))) drawTexture(36);
            if ((elapsedTime > (interval * 3)) && (elapsedTime <= endTime)) drawTexture(37);
        }
        else updateStartTime = true;
    }

    void stateEnemyRevealing()
    {
        long long duration = 1760, framesCount = 8;
        auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        if (updateStartTime)
        {
            startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            updateStartTime = false;
        }
        long long endTime = startTime + duration;
        long long elapsedTime = now - startTime;
        if (now <= endTime)
        {
            long long interval = duration / framesCount;

            if ((elapsedTime >= 0) && (elapsedTime <= interval)) drawTexture(49);
            if ((elapsedTime > interval) && (elapsedTime <= (interval * 2))) drawTexture(50);
            if ((elapsedTime > (interval * 2)) && (elapsedTime <= (interval * 3))) drawTexture(51);
            if ((elapsedTime > (interval * 3)) && (elapsedTime <= (interval * 4))) drawTexture(52);
            if ((elapsedTime > (interval * 4)) && (elapsedTime <= (interval * 5))) drawTexture(53);
            if ((elapsedTime > (interval * 5)) && (elapsedTime <= (interval * 6))) drawTexture(54);
            if ((elapsedTime > (interval * 6)) && (elapsedTime <= (interval * 7))) drawTexture(55);
            if ((elapsedTime > (interval * 7)) && (elapsedTime <= endTime)) drawTexture(56);
        }
        else updateStartTime = true;
    }

    int checkEnemyDistance()
    {
        float offset = 0.15;
        if (health <= 0) return enemyIsBlowing;
        else if ((abs(vertex[0] + offset) < distance2) && (abs(vertex[1] + offset) < distance2)) return enemyIsClose;
        else if ((abs(vertex[0] + offset) < distance1) && (abs(vertex[1] + offset) < distance1)) return enemyIsNearby;
        else return enemyIsFarAway;
    }

    void updateEnemyState(states state)
    {
        switch (state)
        {
        case inPlace: 
        {
            stateEnemyInPlace();
            break;
        }
        case move: 
        {
            stateEnemyMove(); 
            break;
        }
        case attack: 
        {
            stateEnemyAttack(); 
            break;
        }
        case death: 
        {
            stateEnemyDeath(); 
            break;
        }
        case revealing: 
        {
            stateEnemyRevealing(); 
            break;
        }
        default: 
        {
            stateEnemyInPlace(); 
            break;
        }
        }
    }

    void takeDamage(int damage)
    {
        health -= damage;
    }

    int getHealth()
    {
        return health;
    }

    void resurrect()
    {
        health = 100;
    }
};

float random(float density)
{
    int r = rand() % 201 - 100.0f;
    return r / density;
}

const int treeCount = 120;
const int slimeCount = 5;
tree* trees = new tree[treeCount];
enemy* slime = new enemy[slimeCount];
enemy* slimeShadow = new enemy[slimeCount];
tree* treesShadows = new tree[treeCount];

player* dog = new player(21, 18);
gameObject* dogShadow = new gameObject(18, 15);
healthBar* health = new healthBar(69, 21);
scoreBar* score = new scoreBar(512, 512);
gameObject* quitScreen = new gameObject(windowWidth / 2, windowHeight / 2);
balloon* warning = new balloon(19, 12);
player* sleeping = new player(19, 12);

void key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float playerSpeed = 0.02f, rotationSpeed = 0.1f;
    if (gameOver == true)
    {
        playerSpeed = 0.0;
        rotationSpeed = 0.0;
    }
    int distance = 25;
    angle = 0;
    if (frameCount >= 20) frameCount = 1;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) angle += rotationSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) angle -= rotationSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        dog->updateState(dog->attack);
        for (int i = 0; i < slimeCount; i++)
        {
            float x, y;
            slime[i].getTexCoords(x, y);
            if ((abs(x) < 0.2) && (abs(y) < 0.2))
            {
                slime[i].takeDamage(7);
            }
        }
    }
    else dog->updateState(dog->calm);
    
    if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS))
    {
        if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
        {
            frameCount++;
            dog->updatePosition(dog->up);
            if (dog->getPosY() >= distance) dog->playerMove(0, 0);
            else
            {
                dog->playerMove(0, playerSpeed);
                warning->moveObject(0, -playerSpeed);
                for (int i = 0; i < treeCount; i++)
                {
                    trees[i].moveObject(0, -playerSpeed);
                    
                }
                for (int i = 0; i < slimeCount; i++)
                {
                    slime[i].moveObject(0, -playerSpeed);
                }
            }
        }
        if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
        {
            frameCount++;
            dog->updatePosition(dog->down);
            if (dog->getPosY() <= -distance) dog->playerMove(0.0f, 0.0f);
            else
            {
                dog->playerMove(0, -playerSpeed);
                warning->moveObject(0, playerSpeed);
                for (int i = 0; i < treeCount; i++)
                {
                    trees[i].moveObject(0, playerSpeed);
                }
                for (int i = 0; i < slimeCount; i++)
                {
                    slime[i].moveObject(0, playerSpeed);
                }
            }
        }
        if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
        {
            frameCount++;
            dog->updatePosition(dog->left);
            if (dog->getPosX() <= -distance) dog->playerMove(0.0f, 0.0f);
            else
            {
                dog->playerMove(-playerSpeed, 0.0f);
                warning->moveObject(playerSpeed, 0);
                for (int i = 0; i < treeCount; i++)
                {
                    trees[i].moveObject(playerSpeed, 0);
                }
                for (int i = 0; i < slimeCount; i++)
                {
                    slime[i].moveObject(playerSpeed, 0);
                }
            }
        }
        if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS))
        {
            frameCount++;
            dog->updatePosition(dog->right);
            if (dog->getPosX() >= distance) dog->playerMove(0.0f, 0.0f);
            else
            {
                dog->playerMove(playerSpeed, 0.0f);
                warning->moveObject(-playerSpeed, 0);
                for (int i = 0; i < treeCount; i++)
                {
                    trees[i].moveObject(-playerSpeed, 0);
                    
                }
                for (int i = 0; i < slimeCount; i++)
                {
                    slime[i].moveObject(-playerSpeed, 0);
                }
            }
        }
    }
    else
    {
        frameCount = 0;
        currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }
}

void sleepingTimer(milliseconds mss)
{
    if (frameCount == 0)
    {
        milliseconds endTime = currentTime + mss;
        if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) > endTime) sleeping->drawTexture(1);
    }
}

void gameplay(int currentFrame, float slimeDensity)
{
    for (int i = 0; i < slimeCount; i++)
    {
        float slimeX, slimeY;
        float slimeSpeed = 0.0001f;

        slime[i].getTexCoords(slimeX, slimeY);
        slimeShadow[i].bindTexture(slimeX - 0.12, slimeY - 0.35);

        switch (slime[i].isEnemyResurrect)
        {
        case true:
        {
            long long duration = 1760;
            auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

            if (slime[i].updateStartTime)
            {
                slime[i].startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
                slime[i].updateStartTime = false;
            }

            long long endTime = slime[i].startTime + duration;
            
            if (now <= endTime)
            {
                slime[i].updateEnemyState(slime->revealing);
            }
            else
            {
                slime[i].updateStartTime = true;
                slime[i].updateEnemyState(slime->inPlace);
                slime[i].isEnemyResurrect = false;
            }
            break;
        }
        case false:
        {
            switch (slime[i].checkEnemyDistance())
            {
            case 0:
            {
                warning->updateBalloon(500ms);
                warning->drawTexture(0);
                warning->bindTexture(slimeX - 0.01, slimeY + 0.14);
                slime[i].updateEnemyState(slime->move);

                float slimeDistance = 0.1;
                if ((slimeX > slimeDistance) && (slimeY > slimeDistance))
                {
                    slime[i].moveObject(-slimeSpeed, -slimeSpeed);
                }
                if ((slimeX > slimeDistance) && (slimeY < slimeDistance))
                {
                    slime[i].moveObject(-slimeSpeed, slimeSpeed);
                }
                if ((slimeX < slimeDistance) && (slimeY > slimeDistance))
                {
                    slime[i].moveObject(slimeSpeed, -slimeSpeed);
                }
                if ((slimeX < slimeDistance) && (slimeY < slimeDistance))
                {
                    slime[i].moveObject(slimeSpeed, slimeSpeed);
                }
                break;
            }
            case 1:
            {
                slime[i].updateEnemyState(slime->attack);
                dog->takeDamage(7);
                break;
            }
            case 2:
            {
                slime[i].updateEnemyState(slime->inPlace);
                break;
            }
            case 3:
            {
                long long duration = 2000, respawnDuration = 10000;
                auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

                if (slime[i].updateStartTime)
                {
                    slime[i].startTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
                    slime[i].updateStartTime = false;
                }

                long long endTime = slime[i].startTime + duration;
                long long endTime1 = slime[i].startTime + respawnDuration;

                if (now <= endTime)
                {
                    slime[i].updateEnemyState(slime->death);
                    slime[i].increaseScore = true;

                }
                else
                {
                    if (now <= endTime1)
                    {
                        slime[i].isEnemyRespawn = true;
                        slime[i].drawTexture(37);
                        if (slime[i].increaseScore)
                        {
                            playerScore++;
                            slime[i].increaseScore = false;
                        }
                    }
                    else
                    {
                        slime[i].isEnemyRespawn = false;
                        slime[i].updateStartTime = true;
                        slime[i].isEnemyResurrect = true;
                        float tx = random(slimeDensity), ty = random(slimeDensity);
                        slime[i].moveObject(tx, ty);
                        slime[i].resurrect();
                    }
                }
                break;
            }
            }
            break;
        }
        }
    }

    health->updateHealthBar(dog->getHealth());
    dog->changeDogPosition();

    if (dog->getHealth() <= 0)
    {
        quitScreen->drawTexture(21);
        float gameOverScale = 0.2;
        score->drawText(-0.2, 0.3, gameOverScale, gameOverScale, "GAME");
        score->drawText(-4.0, -2, 1, 1, "OVER");
        gameOver = true;
    }
}

void loadTextures()
{
    for (int i = 0; i < texCount; i++)
    {
        unsigned char* data;
        int width, height, cnt;
        std::string fileName = "textures/tex (" + std::to_string(i) + ").png";
        data = stbi_load(fileName.c_str(), &width, &height, &cnt, STBI_rgb_alpha);

        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
}

int main(void)
{
    srand(time(NULL));
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "forest", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_Callback);

    float slimeDensity = 70.0f;
    float treeDensity = 30.0f;

    loadTextures();

    for (int i = 0; i < slimeCount; i++)
    {
        float tx = random(slimeDensity), ty = random(slimeDensity);
        slime[i].moveObject(tx, ty);
    }

    dogShadow->moveObject(0.025, -0.07);
    health->moveObject(-0.8, 0.9);
    sleeping->moveObject(0, 0.1);
    warning->moveObject(0.52, 0.66);

    for (int i = 0; i < treeCount; i++)
    {
        float tx = random(treeDensity), ty = random(treeDensity);
        trees[i].moveObject(tx, ty);
        treesShadows[i].moveObject(tx, ty - 0.32);
    }
    int currentFrame = 1;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        float alphatestvalue = 0.5f;
        glAlphaFunc(GL_GREATER, alphatestvalue);
        glEnable(GL_ALPHA_TEST);

        glEnable(GL_TEXTURE_2D);

        glColor4f(1, 1, 1, 1);

        glPushMatrix();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        for (int i = 0; i < treeCount; i++)
        {
            float treeX, treeY;
            trees[i].getTexCoords(treeX, treeY);
            treesShadows[i].bindTexture(treeX - 0.15, treeY - 0.47);
            treesShadows[i].drawTexture(59);
        }

        dogShadow->drawTexture(16);

        for (int i = 0; i < slimeCount; i++)
        {
            if (!slime[i].isEnemyRespawn) slimeShadow[i].drawTexture(57);
            else slimeShadow[i].drawTexture(37);
            slime[i].rotation();
        }

        sleepingTimer(300000ms);

        for (int i = 0; i < treeCount; i++)
        {
            trees[i].rotation();
            trees[i].drawTexture(58);
        }

        gameplay(currentFrame, slimeDensity);

        std::string text = "score: ";
        std::string plScore = std::to_string(playerScore);
        std::string plScoreText = text + plScore;
        float scoreScale = 0.06;
        score->drawText(0.4, 0.92, scoreScale, scoreScale, plScoreText);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glPopMatrix();

        glBindTexture(GL_TEXTURE_2D, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();

    delete[] slime;
    delete[] trees;
    delete[] slimeShadow;
    delete[] treesShadows;

    delete dog;
    delete dogShadow;
    delete health;
    delete score;
    delete quitScreen;
    delete warning;
    delete sleeping;

    return 0;
}