#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int WIN_W = 1365;
const int WIN_H = 768;
const int SCENE_WIDTH = 2800;
const int INITIAL_OFFSET = 500;

int windowWidth = WIN_W;
int windowHeight = WIN_H;

float cameraX = INITIAL_OFFSET;
const float CAMERA_STEP = 50.0f;

float houseColor[3] = { 1.0f, 1.0f, 0.0f };
float treeColor[3] = { 0.0f, 0.8f, 0.0f };

int catColorIndex = 1;
float catColors[][3] = {
    {0.65f, 0.50f, 0.35f}, {0.96f, 0.80f, 0.56f},
    {0.3f, 0.3f, 0.3f}, {1.0f, 1.0f, 1.0f}, {0.2f, 0.2f, 0.2f}
};
int numCatColors = 5;
float catRotationAngle = 0.0f; // Cat rotation -- UUIAIAIAIEI

int plantColorIndex = 0;
float plantColors[][3] = {
    {0.2f,0.6f,0.3f}, {0.6f,0.2f,0.6f}, {0.6f,0.4f,0.2f}, {0.2f,0.4f,0.6f}, {0.8f,0.2f,0.2f}
};
int numPlantColors = 5;

float curtainOpenAmount = 0.0f;
float couchScale = 1.0f; // Couch size (controlled by scroll wheel)

float animationTime = 0.0f;
float plantSwayAngle = 0.0f;
float lightBlinkTimer = 0.0f;
float lightBrightness = 1.0f;
float curtainSway = 0.0f;
float clockTime = 0.0f;

struct Particle { float x, y, life, speed, size; };
vector<Particle> dustParticles;
float dustTimer = 0.0f;

struct MovingCat {
    float x, y;
    float speed;
    float direction; // 1.0 for right, -1.0 for left
    int colorIndex;
    float walkCycle;
    bool isWalking;
    float idleTimer;
    float walkTimer;
    bool isIdle;
};

vector<MovingCat> movingCats;

struct Color { GLfloat r, g, b; };

const float PI = 3.14159f;
const float TWO_PI = 6.28318f;

inline void setColor(const Color& c) {
    glColor3f(c.r, c.g, c.b);
}

void drawEllipse(float centerX, float centerY, float radiusX, float radiusY, int segments = 16) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    const float step = TWO_PI / segments;
    for (int segmentIndex = 0; segmentIndex <= segments; segmentIndex++) {
        float angle = segmentIndex * step;
        glVertex2f(centerX + cosf(angle) * radiusX, centerY + sinf(angle) * radiusY);
    }
    glEnd();
}


void drawFilledRect(float x, float y, float width, float height, const Color& fill, const Color& outline, float outlineWidth = 2.0f) {
    setColor(fill);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    glLineWidth(outlineWidth);
    setColor(outline);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircleFilled(float centerX, float centerY, float radius, int segments = 16) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    const float step = TWO_PI / segments;
    for (int segmentIndex = 0; segmentIndex <= segments; segmentIndex++) {
        float angle = segmentIndex * step;
        glVertex2f(centerX + cosf(angle) * radius, centerY + sinf(angle) * radius);
    }
    glEnd();
}

void drawTriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawCoffeeMachineStyled(float x, float y, float scale) {
    Color edge = { 0.0f, 0.0f, 0.0f };
    drawFilledRect(x, y, 115 * scale, 25 * scale, { 0.5f, 0.5f, 0.5f }, edge, 1.5f);
    drawFilledRect(x + 15 * scale, y + 25 * scale, 85 * scale, 140 * scale, { 0.5f, 0.5f, 0.5f }, edge, 1.5f);

    glLineWidth(1.5f);
    setColor(edge);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 30 * scale, y + 80 * scale);
    glVertex2f(x + 85 * scale, y + 80 * scale);
    glVertex2f(x + 85 * scale, y + 100 * scale);
    glVertex2f(x + 30 * scale, y + 100 * scale);
    glEnd();

    drawFilledRect(x, y + 100 * scale, 115 * scale, 50 * scale, { 0.4f, 0.4f, 0.4f }, edge, 1.5f);

    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 30 * scale, y + 120 * scale);
    glVertex2f(x + 85 * scale, y + 120 * scale);
    glVertex2f(x + 85 * scale, y + 130 * scale);
    glVertex2f(x + 30 * scale, y + 130 * scale);
    glEnd();

    setColor(edge);
    drawTriangleFilled(x + 40 * scale, y + 61 * scale, x + 57 * scale, y + 81 * scale, x + 40 * scale, y + 81 * scale);
    drawTriangleFilled(x + 75 * scale, y + 61 * scale, x + 59 * scale, y + 81 * scale, x + 75 * scale, y + 81 * scale);

    glPointSize(10 * scale);
    setColor(edge);
    glBegin(GL_POINTS);
    glVertex2f(x + 20 * scale, y + 125 * scale);
    glVertex2f(x + 95 * scale, y + 125 * scale);
    glEnd();
}

void drawBrownCoffeeMakerStyled(float x, float y, float scale) {
    Color edge = { 0.0f, 0.0f, 0.0f };
    Color brown = { 0.6f, 0.4f, 0.25f };
    Color darkBrown = { 0.45f, 0.28f, 0.15f };
    Color metallic = { 0.5f, 0.5f, 0.5f };

    drawFilledRect(x, y, 70 * scale, 90 * scale, brown, edge, 2.0f);

    glLineWidth(1.0f);
    setColor(darkBrown);
    glBegin(GL_LINES);
    glVertex2f(x + 10 * scale, y + 70 * scale);
    glVertex2f(x + 60 * scale, y + 70 * scale);
    glVertex2f(x + 10 * scale, y + 45 * scale);
    glVertex2f(x + 60 * scale, y + 45 * scale);
    glVertex2f(x + 10 * scale, y + 20 * scale);
    glVertex2f(x + 60 * scale, y + 20 * scale);
    glEnd();

    setColor(darkBrown);
    glBegin(GL_QUADS);
    glVertex2f(x + 5 * scale, y + 90 * scale);
    glVertex2f(x + 65 * scale, y + 90 * scale);
    glVertex2f(x + 50 * scale, y + 135 * scale);
    glVertex2f(x + 20 * scale, y + 135 * scale);
    glEnd();

    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 5 * scale, y + 90 * scale);
    glVertex2f(x + 65 * scale, y + 90 * scale);
    glVertex2f(x + 50 * scale, y + 135 * scale);
    glVertex2f(x + 20 * scale, y + 135 * scale);
    glEnd();

    drawFilledRect(x + 18 * scale, y + 135 * scale, 34 * scale, 5 * scale, brown, edge, 1.5f);

    setColor(metallic);
    drawCircleFilled(x + 70 * scale, y + 100 * scale, 8 * scale);
    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x + 50 * scale, y + 112 * scale);
    glVertex2f(x + 70 * scale, y + 100 * scale);
    glEnd();
    drawCircleFilled(x + 50 * scale, y + 112 * scale, 5 * scale);

    setColor({ 0.3f, 0.2f, 0.1f });
    drawFilledRect(x + 25 * scale, y + 30 * scale, 20 * scale, 3 * scale, { 0.3f, 0.2f, 0.1f }, edge, 1.0f);
    drawFilledRect(x + 15 * scale, y + 10 * scale, 40 * scale, 50 * scale, { 0.55f, 0.35f, 0.20f }, edge, 1.5f);
}

void drawCounterStyled(float x, float y, float width, float height) {
    Color edge = { 0.0f, 0.0f, 0.0f };
    Color wood = { 0.8f, 0.6f, 0.4f };
    drawFilledRect(x, y, width, height, wood, edge, 1.5f);

    setColor(edge);
    float lineSpacing = height * 0.25f;
    for (int lineIndex = 1; lineIndex < 4; lineIndex++) {
        glBegin(GL_LINES);
        glVertex2f(x, y + lineIndex * lineSpacing);
        glVertex2f(x + width - 20, y + lineIndex * lineSpacing);
        glEnd();
    }
}

void drawCountertopStyled(float x, float y, float width, float height) {
    Color edge = { 0.0f, 0.0f, 0.0f };
    drawFilledRect(x, y, width, height, { 0.8f, 0.6f, 0.4f }, edge, 1.5f);
}

inline Color darken(const Color& c, float factor = 0.7f) {
    return { c.r * factor, c.g * factor, c.b * factor };
}

inline Color lighten(const Color& c, float factor = 1.2f) {
    return {
        c.r * factor > 1.0f ? 1.0f : c.r * factor,
        c.g * factor > 1.0f ? 1.0f : c.g * factor,
        c.b * factor > 1.0f ? 1.0f : c.b * factor
    };
}

void drawCatPixel(float centerX, float centerY, float scale, int colorIndex, float rotationAngle = 0.0f, float walkCycle = 0.0f, bool isWalking = false) {
    Color body = { catColors[colorIndex][0], catColors[colorIndex][1], catColors[colorIndex][2] };
    Color darker = darken(body);
    Color lighter = lighten(body);

    float legOffset = 0.0f;
    float bodyBounce = 0.0f;
    if (isWalking) {
        legOffset = sinf(walkCycle * TWO_PI) * 5.0f * scale;
        bodyBounce = abs(sinf(walkCycle * TWO_PI)) * 1.0f * scale;
    }

    glPushMatrix();
    glTranslatef(centerX, centerY + bodyBounce, 0.0f);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-centerX, -centerY - bodyBounce, 0.0f);

    setColor(body);
    drawEllipse(centerX, centerY, 32 * scale, 28 * scale);
    setColor(lighter);
    drawEllipse(centerX - 5 * scale, centerY - 8 * scale, 18 * scale, 20 * scale);
    setColor(body);
    drawCircleFilled(centerX - 22 * scale, centerY + 22 * scale, 20 * scale);
    setColor(lighter);
    drawEllipse(centerX - 22 * scale, centerY + 18 * scale, 14 * scale, 12 * scale);

    setColor(body);
    if (isWalking) {
        drawTriangleFilled(centerX - 36 * scale, centerY + 34 * scale + legOffset, centerX - 28 * scale, centerY + 50 * scale + legOffset, centerX - 20 * scale, centerY + 36 * scale + legOffset);
        drawTriangleFilled(centerX - 24 * scale, centerY + 34 * scale - legOffset, centerX - 16 * scale, centerY + 50 * scale - legOffset, centerX - 8 * scale, centerY + 36 * scale - legOffset);
    }
    else {
        drawTriangleFilled(centerX - 36 * scale, centerY + 34 * scale, centerX - 28 * scale, centerY + 50 * scale, centerX - 20 * scale, centerY + 36 * scale);
        drawTriangleFilled(centerX - 24 * scale, centerY + 34 * scale, centerX - 16 * scale, centerY + 50 * scale, centerX - 8 * scale, centerY + 36 * scale);
    }

    setColor({ 0.95f, 0.7f, 0.7f });
    drawTriangleFilled(centerX - 33 * scale, centerY + 36 * scale, centerX - 28 * scale, centerY + 45 * scale, centerX - 23 * scale, centerY + 37 * scale);
    drawTriangleFilled(centerX - 21 * scale, centerY + 36 * scale, centerX - 16 * scale, centerY + 45 * scale, centerX - 11 * scale, centerY + 37 * scale);

    setColor({ 1.0f, 1.0f, 0.9f });
    drawEllipse(centerX - 28 * scale, centerY + 24 * scale, 4 * scale, 5 * scale);
    drawEllipse(centerX - 16 * scale, centerY + 24 * scale, 4 * scale, 5 * scale);

    setColor({ 0.1f, 0.1f, 0.1f });
    drawEllipse(centerX - 28 * scale, centerY + 24 * scale, 2 * scale, 4 * scale);
    drawEllipse(centerX - 16 * scale, centerY + 24 * scale, 2 * scale, 4 * scale);

    setColor({ 0.9f, 0.6f, 0.6f });
    drawTriangleFilled(centerX - 22 * scale, centerY + 18 * scale, centerX - 20 * scale, centerY + 14 * scale, centerX - 24 * scale, centerY + 14 * scale);

    glLineWidth(1.0f);
    setColor({ 0.3f, 0.3f, 0.3f });
    glBegin(GL_LINES);
    glVertex2f(centerX - 22 * scale, centerY + 16 * scale); glVertex2f(centerX - 40 * scale, centerY + 18 * scale);
    glVertex2f(centerX - 22 * scale, centerY + 16 * scale); glVertex2f(centerX - 40 * scale, centerY + 14 * scale);
    glVertex2f(centerX - 22 * scale, centerY + 16 * scale); glVertex2f(centerX - 4 * scale, centerY + 18 * scale);
    glVertex2f(centerX - 22 * scale, centerY + 16 * scale); glVertex2f(centerX - 4 * scale, centerY + 14 * scale);
    glEnd();

    setColor(body);
    drawEllipse(centerX - 12 * scale, centerY - 22 * scale, 8 * scale, 6 * scale);
    drawEllipse(centerX + 8 * scale, centerY - 22 * scale, 8 * scale, 6 * scale);
    drawCircleFilled(centerX + 24 * scale, centerY - 4 * scale, 10 * scale);
    drawCircleFilled(centerX + 32 * scale, centerY - 12 * scale, 9 * scale);
    drawCircleFilled(centerX + 38 * scale, centerY - 20 * scale, 8 * scale);
    setColor(darker);
    drawCircleFilled(centerX + 42 * scale, centerY - 26 * scale, 6 * scale);

    glPopMatrix();
}

void drawSleepingCatPixel(float x, float y, float scale) {
    Color body = { 0.6f, 0.4f, 0.3f };
    Color darker = { 0.5f, 0.3f, 0.2f };
    Color lighter = { 0.7f, 0.5f, 0.4f };

    drawFilledRect(x - 30 * scale, y - 10 * scale, 80 * scale, 40 * scale,
        { 0.85f, 0.82f, 0.75f }, { 0.7f, 0.65f, 0.6f }, 1.5f);

    setColor(body);
    drawEllipse(x, y, 28 * scale, 18 * scale);
    drawCircleFilled(x + 20 * scale, y + 4 * scale, 16 * scale);
    setColor(lighter);
    drawEllipse(x + 28 * scale, y + 6 * scale, 10 * scale, 7 * scale);

    setColor(body);
    drawTriangleFilled(x + 14 * scale, y + 16 * scale, x + 20 * scale, y + 28 * scale, x + 26 * scale, y + 16 * scale);
    drawTriangleFilled(x + 26 * scale, y + 16 * scale, x + 32 * scale, y + 28 * scale, x + 38 * scale, y + 16 * scale);

    glLineWidth(2.0f);
    setColor({ 0.2f, 0.2f, 0.2f });
    glBegin(GL_LINES);
    glVertex2f(x + 20 * scale, y + 10 * scale); glVertex2f(x + 26 * scale, y + 10 * scale);
    glVertex2f(x + 30 * scale, y + 10 * scale); glVertex2f(x + 36 * scale, y + 10 * scale);
    glEnd();

    setColor(body);
    drawCircleFilled(x - 20 * scale, y + 6 * scale, 8 * scale);
    drawCircleFilled(x - 26 * scale, y + 10 * scale, 6 * scale);
}

void drawPlantPixel(float centerX, float centerY, int colorIndex, float swayAngle = 0.0f) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color potColor = { 0.79f, 0.37f, 0.18f };
    Color potHighlight = { 0.89f, 0.47f, 0.28f };

    drawFilledRect(centerX - 40, centerY - 40, 80, 40, potColor, edge, 2.0f);
    drawFilledRect(centerX - 42, centerY - 2, 84, 8, potHighlight, edge, 1.5f);

    setColor(potHighlight);
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - 30, centerY - 35);
    glVertex2f(centerX - 20, centerY - 35);
    glVertex2f(centerX - 25, centerY - 10);
    glEnd();

    drawFilledRect(centerX - 36, centerY - 6, 72, 8, { 0.4f, 0.3f, 0.2f }, edge, 1.0f);

    Color leafColor = { plantColors[colorIndex][0], plantColors[colorIndex][1], plantColors[colorIndex][2] };
    Color leafDark = darken(leafColor);
    Color leafLight = lighten(leafColor, 1.3f);

    float swayX = sinf(swayAngle) * 8.0f;
    float swayY = abs(sinf(swayAngle * 0.5f)) * 3.0f;

    glLineWidth(3.0f);
    setColor({ 0.2f, 0.5f, 0.2f });
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY); glVertex2f(centerX + swayX, centerY + 45 + swayY);
    glVertex2f(centerX + swayX, centerY + 20 + swayY * 0.5f); glVertex2f(centerX - 35 + swayX * 1.2f, centerY + 25 + swayY);
    glVertex2f(centerX + swayX, centerY + 20 + swayY * 0.5f); glVertex2f(centerX + 35 + swayX * 1.2f, centerY + 25 + swayY);
    glEnd();

    struct Leaf { float x, y, angle, size; };
    Leaf leaves[] = { {centerX + swayX, centerY + 45 + swayY, -15 + swayAngle * 5.0f, 1.2f},
                      {centerX - 40 + swayX * 1.2f, centerY + 20 + swayY * 0.5f, 10 + swayAngle * 3.0f, 1.0f},
                      {centerX + 40 + swayX * 1.2f, centerY + 20 + swayY * 0.5f, 20 + swayAngle * 3.0f, 1.0f} };

    for (auto& leaf : leaves) {
        glPushMatrix();
        glTranslatef(leaf.x, leaf.y, 0);
        glRotatef(leaf.angle, 0, 0, 1);

        setColor(leafColor);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int segmentIndex = 0; segmentIndex <= 12; segmentIndex++) {
            float angle = (float)segmentIndex / 12.0f * TWO_PI;
            float radius = 28 * leaf.size;
            if (segmentIndex % 3 == 0) radius *= 0.7f;
            glVertex2f(cosf(angle) * radius, sinf(angle) * radius);
        }
        glEnd();

        glLineWidth(1.5f);
        setColor(leafDark);
        glBegin(GL_LINES);
        glVertex2f(0, 0); glVertex2f(0, 25 * leaf.size);
        glVertex2f(0, 15 * leaf.size); glVertex2f(-15 * leaf.size, 20 * leaf.size);
        glVertex2f(0, 15 * leaf.size); glVertex2f(15 * leaf.size, 20 * leaf.size);
        glEnd();

        setColor(leafLight);
        drawEllipse(-8 * leaf.size, 12 * leaf.size, 6 * leaf.size, 8 * leaf.size);

        glPopMatrix();
    }
}

void drawTreeThroughWindow(float x, float y, float scale) {
    Color trunk = { 0.55f, 0.4f, 0.25f };
    Color leaves = { 0.5f, 0.7f, 0.45f };
    Color edge = { 0.4f, 0.55f, 0.35f };

    drawFilledRect(x - 10 * scale, y, 20 * scale, 65 * scale, trunk, { 0.4f, 0.28f, 0.15f }, 1.5f);

    setColor(leaves);
    glBegin(GL_POLYGON);
    glVertex2f(x, y + 170 * scale);
    glVertex2f(x - 45 * scale, y + 100 * scale);
    glVertex2f(x - 40 * scale, y + 100 * scale);
    glVertex2f(x - 55 * scale, y + 65 * scale);
    glVertex2f(x + 55 * scale, y + 65 * scale);
    glVertex2f(x + 40 * scale, y + 100 * scale);
    glVertex2f(x + 45 * scale, y + 100 * scale);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y + 170 * scale);
    glVertex2f(x - 45 * scale, y + 100 * scale);
    glVertex2f(x - 40 * scale, y + 100 * scale);
    glVertex2f(x - 55 * scale, y + 65 * scale);
    glVertex2f(x + 55 * scale, y + 65 * scale);
    glVertex2f(x + 40 * scale, y + 100 * scale);
    glVertex2f(x + 45 * scale, y + 100 * scale);
    glEnd();
}

void drawWindowPixel(float x, float y, float width, float height, float openAmount) {
    Color frame = { 0.73f, 0.56f, 0.36f };
    Color glass = { 0.68f, 0.85f, 0.90f };

    drawFilledRect(x, y, width, height, frame, { 0.06f, 0.03f, 0.01f }, 2.0f);
    drawFilledRect(x + 8, y + 8, width - 16, height - 16, glass, { 0.06f, 0.03f, 0.01f }, 1.2f);

    drawTreeThroughWindow(x + width * 0.5f, y + 20, 1.2f);

    glLineWidth(3.0f);
    setColor({ 0.73f, 0.56f, 0.36f });
    glBegin(GL_LINES);
    glVertex2f(x + width * 0.5f, y + 8); glVertex2f(x + width * 0.5f, y + height - 8);
    glVertex2f(x + 8, y + height * 0.5f); glVertex2f(x + width - 8, y + height * 0.5f);
    glEnd();

    drawFilledRect(x - 10, y + height - 12, width + 20, 6, frame, { 0.06f, 0.03f, 0.01f }, 1.5f);

    for (float centerX = x + 12; centerX <= x + width - 12; centerX += 48)
        drawCircleFilled(centerX, y + height - 9, 6);

    float offset = openAmount * width * 0.35f;
    drawFilledRect(x + 10 - offset, y + 10, width * 0.5f - 14, height - 20, { 0.95f, 0.95f, 0.95f }, { 0.9f, 0.9f, 0.9f }, 1.0f);
    drawFilledRect(x + width * 0.5f + 4 + offset, y + 10, width * 0.5f - 14, height - 20, { 0.95f, 0.95f, 0.95f }, { 0.9f, 0.9f, 0.9f }, 1.0f);
}

void drawCashRegisterPixel(float x, float y, float scale) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color bodyColor = { 0.25f, 0.25f, 0.25f };
    Color displayColor = { 0.4f, 0.7f, 0.5f };
    Color keyColor = { 0.9f, 0.9f, 0.9f };

    drawFilledRect(x, y, 90 * scale, 50 * scale, bodyColor, edge, 2.0f);
    drawFilledRect(x + 10 * scale, y + 32 * scale, 70 * scale, 16 * scale, displayColor, edge, 1.5f);

    setColor({ 0.1f, 0.1f, 0.1f });
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x + 15 * scale, y + 42 * scale);
    glVertex2f(x + 50 * scale, y + 42 * scale);
    glVertex2f(x + 15 * scale, y + 37 * scale);
    glVertex2f(x + 45 * scale, y + 37 * scale);
    glEnd();

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            float keyX = x + 12 * scale + col * 9 * scale;
            float keyY = y + 6 * scale + row * 8 * scale;
            drawFilledRect(keyX, keyY, 7 * scale, 6 * scale, keyColor, edge, 0.8f);
        }
    }

    drawFilledRect(x + 42 * scale, y + 6 * scale, 20 * scale, 6 * scale, { 0.7f, 0.9f, 0.7f }, edge, 0.8f);
    drawFilledRect(x + 42 * scale, y + 14 * scale, 20 * scale, 6 * scale, { 0.9f, 0.7f, 0.7f }, edge, 0.8f);
    drawFilledRect(x + 5 * scale, y - 2 * scale, 80 * scale, 6 * scale, { 0.2f, 0.2f, 0.2f }, edge, 1.0f);

    setColor({ 0.5f, 0.5f, 0.5f });
    drawFilledRect(x + 35 * scale, y - 1 * scale, 20 * scale, 3 * scale, { 0.5f, 0.5f, 0.5f }, edge, 0.5f);
}

void drawJar(float x, float y, float width, float height, Color fill) {
    Color edge = { 0.2f, 0.2f, 0.2f };

    setColor(fill);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y);
    glVertex2f(x + width - 2, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 2, y);
    glVertex2f(x + width - 2, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    drawFilledRect(x - 2, y + height, width + 4, 4, { 0.5f, 0.45f, 0.4f }, edge, 1.0f);
    drawFilledRect(x - 1, y + height + 4, width + 2, 3, { 0.6f, 0.55f, 0.5f }, edge, 1.0f);
}

void drawCup(float x, float y, float width, float height, Color fill) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    drawFilledRect(x, y, width, height, fill, edge, 1.5f);

    float height30Percent = height * 0.3f;
    float height35Percent = height * 0.35f;
    float height45Percent = height * 0.45f;
    float height65Percent = height * 0.65f;
    float height75Percent = height * 0.75f;
    float height80Percent = height * 0.8f;

    setColor(fill);
    glBegin(GL_POLYGON);
    glVertex2f(x + width, y + height30Percent);
    glVertex2f(x + width + 8, y + height35Percent);
    glVertex2f(x + width + 8, y + height75Percent);
    glVertex2f(x + width, y + height80Percent);
    glVertex2f(x + width, y + height65Percent);
    glVertex2f(x + width + 4, y + height65Percent);
    glVertex2f(x + width + 4, y + height45Percent);
    glVertex2f(x + width, y + height45Percent);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x + width, y + height30Percent);
    glVertex2f(x + width + 8, y + height35Percent);
    glVertex2f(x + width + 8, y + height75Percent);
    glVertex2f(x + width, y + height80Percent);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(x + width, y + height45Percent);
    glVertex2f(x + width + 4, y + height45Percent);
    glVertex2f(x + width + 4, y + height65Percent);
    glVertex2f(x + width, y + height65Percent);
    glEnd();
}

void drawBowl(float x, float y, float width, float height, Color fill) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    float centerX = x + width * 0.5f;
    float centerY = y + height * 0.7f;
    float radiusWidth = width * 0.5f;
    float radiusHeight = height * 0.5f;

    setColor(fill);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, y + height * 0.5f);
    for (int segmentIndex = 0; segmentIndex <= 12; segmentIndex++) {
        float angle = 3.14159f * (1.0f + (float)segmentIndex / 12.0f);
        glVertex2f(centerX + cosf(angle) * radiusWidth, centerY + sinf(angle) * radiusHeight);
    }
    glEnd();

    drawFilledRect(x, centerY, width, height * 0.3f, fill, edge, 0);

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int segmentIndex = 0; segmentIndex <= 12; segmentIndex++) {
        float angle = 3.14159f * (1.0f + (float)segmentIndex / 12.0f);
        glVertex2f(centerX + cosf(angle) * radiusWidth, centerY + sinf(angle) * radiusHeight);
    }
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(x, centerY);
    glVertex2f(x, y + height);
    glVertex2f(x + width, centerY);
    glVertex2f(x + width, y + height);
    glEnd();
}

void drawShelvesWithItems(float x, float y) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color woodTop = { 0.79f, 0.58f, 0.36f };

    float topShelfY = y + 80;
    drawFilledRect(x, topShelfY, 280, 10, woodTop, edge, 2.0f);

    drawJar(x + 20, topShelfY + 10, 30, 35, { 0.85f, 0.8f, 0.7f });
    drawJar(x + 60, topShelfY + 10, 25, 30, { 0.75f, 0.85f, 0.8f });
    drawJar(x + 95, topShelfY + 10, 35, 40, { 0.9f, 0.85f, 0.75f });
    drawJar(x + 145, topShelfY + 10, 28, 32, { 0.7f, 0.8f, 0.75f });
    drawJar(x + 185, topShelfY + 10, 32, 38, { 0.85f, 0.82f, 0.7f });
    drawJar(x + 230, topShelfY + 10, 28, 34, { 0.9f, 0.88f, 0.75f });

    drawFilledRect(x, y, 300, 10, woodTop, edge, 2.0f);

    drawCup(x + 15, y + 10, 22, 20, { 0.95f, 0.95f, 0.9f });
    drawBowl(x + 45, y + 10, 30, 18, { 0.7f, 0.85f, 0.75f });
    drawCup(x + 85, y + 10, 24, 22, { 0.75f, 0.9f, 0.85f });
    drawBowl(x + 120, y + 10, 35, 20, { 0.85f, 0.75f, 0.65f });
    drawCup(x + 165, y + 10, 22, 20, { 0.9f, 0.85f, 0.75f });
    drawBowl(x + 195, y + 10, 32, 18, { 0.6f, 0.75f, 0.7f });
    drawCup(x + 240, y + 10, 24, 22, { 0.95f, 0.9f, 0.85f });
}

void drawCoffeeMenuSign(float x, float y) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    Color paper = { 0.98f, 0.96f, 0.9f };

    drawFilledRect(x, y, 100, 130, paper, edge, 2.0f);

    setColor({ 0.3f, 0.3f, 0.3f });
    drawCircleFilled(x + 25, y + 135, 4);
    drawCircleFilled(x + 75, y + 135, 4);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x + 20, y + 105); glVertex2f(x + 80, y + 105);
    glVertex2f(x + 25, y + 95); glVertex2f(x + 75, y + 95);
    glEnd();

    for (int lineIndex = 0; lineIndex < 4; lineIndex++) {
        float lineY = y + 70 - lineIndex * 18;
        glBegin(GL_LINES);
        glVertex2f(x + 15, lineY);
        glVertex2f(x + 60, lineY);
        glEnd();
        drawCircleFilled(x + 75, lineY, 3);
    }
}

void drawOrderHereSign(float x, float y) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    Color paper = { 0.98f, 0.96f, 0.9f };

    setColor({ 0.3f, 0.3f, 0.3f });
    drawCircleFilled(x + 40, y + 55, 5);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 40, y + 50);
    glVertex2f(x + 40, y + 45);
    glEnd();

    drawFilledRect(x, y, 80, 45, paper, edge, 2.0f);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x + 15, y + 28); glVertex2f(x + 65, y + 28);
    glVertex2f(x + 20, y + 15); glVertex2f(x + 60, y + 15);
    glEnd();
}

void drawTable(float x, float y, float width, float height) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color wood = { 0.7f, 0.55f, 0.35f };
    Color woodDark = { 0.6f, 0.45f, 0.25f };

    drawFilledRect(x, y + height - 10, width, 10, wood, edge, 2.0f);
    drawFilledRect(x, y + height - 12, width, 2, woodDark, woodDark, 0);

    float legWidth = 8;
    float legInset = 10;
    drawFilledRect(x + legInset, y, legWidth, height - 10, woodDark, edge, 1.5f);
    drawFilledRect(x + width - legInset - legWidth, y, legWidth, height - 10, woodDark, edge, 1.5f);
}

void drawStool(float x, float y, float width, float height) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color wood = { 0.7f, 0.55f, 0.35f };
    Color woodDark = { 0.6f, 0.45f, 0.25f };

    drawFilledRect(x, y + height - 8, width, 8, wood, edge, 2.0f);
    drawFilledRect(x + 2, y + height - 10, width - 4, 2, woodDark, woodDark, 0);

    float legWidth = 5;
    float legInset = 6;
    drawFilledRect(x + legInset, y, legWidth, height - 8, woodDark, edge, 1.2f);
    drawFilledRect(x + width - legInset - legWidth, y, legWidth, height - 8, woodDark, edge, 1.2f);

    glLineWidth(2.0f);
    setColor(woodDark);
    float crossbarY = y + height * 0.333f;
    glBegin(GL_LINES);
    glVertex2f(x + legInset + legWidth * 0.5f, crossbarY);
    glVertex2f(x + width - legInset - legWidth * 0.5f, crossbarY);
    glEnd();
}

void drawPendantLamp(float x, float y, float brightness = 1.0f) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    Color shade = { 0.95f * brightness, 0.85f * brightness, 0.7f * brightness };

    glLineWidth(2.0f);
    setColor({ 0.3f, 0.3f, 0.3f });
    glBegin(GL_LINES);
    glVertex2f(x, y + 50);
    glVertex2f(x, y);
    glEnd();

    setColor(shade);
    glBegin(GL_POLYGON);
    glVertex2f(x - 30, y);
    glVertex2f(x + 30, y);
    glVertex2f(x + 25, y - 35);
    glVertex2f(x - 25, y - 35);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 30, y);
    glVertex2f(x + 30, y);
    glVertex2f(x + 25, y - 35);
    glVertex2f(x - 25, y - 35);
    glEnd();

    glColor4f(1.0f * brightness, 0.95f * brightness, 0.7f * brightness, 0.3f * brightness);
    drawCircleFilled(x, y - 35, 20);
}

void drawCatPawPrint(float x, float y, float size) {
    setColor({ 0.6f, 0.5f, 0.4f });
    drawCircleFilled(x, y, size * 0.8f);
    drawCircleFilled(x - size * 0.6f, y - size * 0.4f, size * 0.4f);
    drawCircleFilled(x + size * 0.6f, y - size * 0.4f, size * 0.4f);
    drawCircleFilled(x - size * 0.3f, y - size * 0.8f, size * 0.35f);
    drawCircleFilled(x + size * 0.3f, y - size * 0.8f, size * 0.35f);
}

void drawWelcomeSign(float x, float y, float width, float height) {
    Color frame = { 0.5f, 0.4f, 0.3f };
    Color edge = { 0.2f, 0.15f, 0.1f };
    Color paper = { 0.98f, 0.96f, 0.92f };

    drawFilledRect(x, y, width, height, frame, edge, 2.0f);
    drawFilledRect(x + 5, y + 5, width - 10, height - 10, paper, edge, 1.0f);

    glLineWidth(2.0f);
    setColor({ 0.4f, 0.3f, 0.2f });
    glBegin(GL_LINES);
    glVertex2f(x + 15, y + height - 25);
    glVertex2f(x + width - 15, y + height - 25);
    glVertex2f(x + 15, y + height - 40);
    glVertex2f(x + width - 15, y + height - 40);
    glEnd();

    setColor({ 0.8f, 0.5f, 0.3f });
    drawCatPawPrint(x + width * 0.5f, y + 30, 8);
}

void drawDecorativeVase(float x, float y) {
    Color vase = { 0.7f, 0.6f, 0.5f };
    Color edge = { 0.3f, 0.25f, 0.2f };

    drawFilledRect(x - 8, y, 16, 25, vase, edge, 1.5f);
    drawEllipse(x, y + 25, 10, 6);
    setColor({ 0.3f, 0.5f, 0.3f });
    drawEllipse(x, y + 30, 8, 12);
    drawEllipse(x - 5, y + 28, 5, 8);
    drawEllipse(x + 5, y + 28, 5, 8);
}

void drawWallClock(float x, float y, float radius) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    Color face = { 0.95f, 0.95f, 0.9f };

    drawFilledRect(x - radius - 5, y - radius - 5, radius * 2 + 10, radius * 2 + 10, { 0.4f, 0.3f, 0.2f }, edge, 2.0f);
    drawCircleFilled(x, y, radius, 32);
    setColor(face);
    drawCircleFilled(x, y, radius - 3, 32);
    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 32; i++) {
        float angle = (float)i / 32.0f * TWO_PI;
        glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
    }
    glEnd();

    for (int i = 0; i < 12; i++) {
        float angle = (float)i / 12.0f * TWO_PI - PI * 0.5f;
        float markLength = (i % 3 == 0) ? 8.0f : 4.0f;
        glLineWidth((i % 3 == 0) ? 2.5f : 1.5f);
        glBegin(GL_LINES);
        glVertex2f(x + cosf(angle) * (radius - markLength), y + sinf(angle) * (radius - markLength));
        glVertex2f(x + cosf(angle) * (radius - 2), y + sinf(angle) * (radius - 2));
        glEnd();
    }

    float hourAngle = (clockTime * 0.1f) - PI * 0.5f;
    float minuteAngle = (clockTime * 6.0f) - PI * 0.5f;

    glLineWidth(3.0f);
    setColor(edge);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + cosf(hourAngle) * radius * 0.5f, y + sinf(hourAngle) * radius * 0.5f);
    glEnd();

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + cosf(minuteAngle) * radius * 0.7f, y + sinf(minuteAngle) * radius * 0.7f);
    glEnd();

    setColor(edge);
    drawCircleFilled(x, y, 3);
}

void drawChalkboardMenu(float x, float y, float width, float height) {
    Color edge = { 0.4f, 0.3f, 0.2f };
    Color board = { 0.15f, 0.15f, 0.15f };
    Color chalk = { 0.9f, 0.9f, 0.85f };

    drawFilledRect(x - 8, y - 8, width + 16, height + 16, { 0.5f, 0.4f, 0.3f }, edge, 2.5f);
    drawFilledRect(x, y, width, height, board, board, 0);

    setColor(chalk);
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    glVertex2f(x + 20, y + height - 25);
    glVertex2f(x + width - 20, y + height - 25);
    glEnd();

    float itemY = y + height - 55;
    float lineEndX = width * 0.6f;
    for (int itemIndex = 0; itemIndex < 4; itemIndex++) {
        glBegin(GL_LINES);
        glVertex2f(x + 15, itemY);
        glVertex2f(x + lineEndX, itemY);
        glEnd();

        drawCircleFilled(x + width - 25, itemY, 2);
        drawCircleFilled(x + width - 20, itemY, 2);
        drawCircleFilled(x + width - 15, itemY, 2);

        itemY -= 30;
    }
}

void drawMug(float x, float y, Color fill) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    drawFilledRect(x, y, 24, 22, fill, edge, 1.5f);

    setColor(fill);
    glBegin(GL_POLYGON);
    glVertex2f(x + 24, y + 5);
    glVertex2f(x + 32, y + 7);
    glVertex2f(x + 32, y + 17);
    glVertex2f(x + 24, y + 19);
    glEnd();

    setColor(edge);
    glLineWidth(1.2f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x + 24, y + 5);
    glVertex2f(x + 32, y + 7);
    glVertex2f(x + 32, y + 17);
    glVertex2f(x + 24, y + 19);
    glEnd();
}

void drawCatPlaypen(float x, float y, float width, float height) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color wood = { 0.6f, 0.4f, 0.25f };
    Color darkWood = { 0.5f, 0.3f, 0.15f };
    Color blue = { 0.3f, 0.5f, 0.7f };
    Color darkBlue = { 0.2f, 0.3f, 0.5f };
    Color beige = { 0.9f, 0.85f, 0.75f };

    setColor(darkBlue);
    drawEllipse(x + width * 0.5f, y + 5, width * 0.55f, 8);

    float centerPoleX = x + width * 0.5f;
    drawFilledRect(centerPoleX - 12, y + 5, 24, height - 50, wood, edge, 2.0f);

    glLineWidth(1.0f);
    setColor(darkWood);
    for (float lineY = y + 15; lineY < y + height - 50; lineY += 15) {
        glBegin(GL_LINES);
        glVertex2f(centerPoleX - 10, lineY);
        glVertex2f(centerPoleX + 10, lineY);
        glEnd();
    }

    float platform1Y = y + 35;
    setColor(blue);
    drawEllipse(centerPoleX, platform1Y + 35, 45, 12);
    drawFilledRect(centerPoleX - 45, platform1Y, 90, 35, blue, edge, 0);
    setColor(darkBlue);
    drawEllipse(centerPoleX, platform1Y, 45, 12);
    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(centerPoleX - 45, platform1Y);
    glVertex2f(centerPoleX - 45, platform1Y + 35);
    glVertex2f(centerPoleX + 45, platform1Y);
    glVertex2f(centerPoleX + 45, platform1Y + 35);
    glEnd();

    float leftPoleX = x + width * 0.2f;
    drawFilledRect(leftPoleX - 8, y + 5, 16, height - 30, wood, edge, 2.0f);
    glLineWidth(1.0f);
    setColor(darkWood);
    for (float lineY = y + 15; lineY < y + height - 30; lineY += 15) {
        glBegin(GL_LINES);
        glVertex2f(leftPoleX - 6, lineY);
        glVertex2f(leftPoleX + 6, lineY);
        glEnd();
    }

    float rightPoleX = x + width * 0.8f;
    drawFilledRect(rightPoleX - 8, y + 5, 16, height - 30, wood, edge, 2.0f);
    glLineWidth(1.0f);
    setColor(darkWood);
    for (float lineY = y + 15; lineY < y + height - 30; lineY += 15) {
        glBegin(GL_LINES);
        glVertex2f(rightPoleX - 6, lineY);
        glVertex2f(rightPoleX + 6, lineY);
        glEnd();
    }

    float platform2Y = y + 100;
    setColor(beige);
    drawFilledRect(leftPoleX - 35, platform2Y, 70, 8, beige, edge, 2.0f);
    glLineWidth(0.5f);
    setColor({ 0.8f, 0.75f, 0.65f });
    for (float tx = leftPoleX - 30; tx < leftPoleX + 30; tx += 3) {
        glBegin(GL_LINES);
        glVertex2f(tx, platform2Y + 2);
        glVertex2f(tx, platform2Y + 6);
        glEnd();
    }

    float boxY = y + 95;
    drawFilledRect(rightPoleX - 30, boxY, 60, 35, { 0.7f, 0.5f, 0.3f }, edge, 2.0f);
    setColor({ 0.2f, 0.2f, 0.2f });
    drawCircleFilled(rightPoleX, boxY + 18, 12);
    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 20.0f * TWO_PI;
        glVertex2f(rightPoleX + cosf(angle) * 12, boxY + 18 + sinf(angle) * 12);
    }
    glEnd();

    float platform3Y = y + height - 30;
    setColor(wood);
    drawFilledRect(centerPoleX - 40, platform3Y, 80, 8, wood, edge, 2.0f);

    float perchY = y + height - 15;
    setColor(beige);
    drawFilledRect(centerPoleX - 25, perchY, 50, 6, beige, edge, 2.0f);
    glLineWidth(0.5f);
    setColor({ 0.8f, 0.75f, 0.65f });
    for (float tx = centerPoleX - 20; tx < centerPoleX + 20; tx += 3) {
        glBegin(GL_LINES);
        glVertex2f(tx, perchY + 2);
        glVertex2f(tx, perchY + 4);
        glEnd();
    }

    float ball1X = leftPoleX + 20;
    float ball1Y = platform2Y - 20;
    glLineWidth(1.5f);
    setColor({ 0.3f, 0.3f, 0.3f });
    glBegin(GL_LINES);
    glVertex2f(leftPoleX + 15, platform2Y);
    glVertex2f(ball1X, ball1Y);
    glEnd();
    setColor({ 0.9f, 0.3f, 0.3f });
    drawCircleFilled(ball1X, ball1Y, 6);

    float toy2X = centerPoleX + 30;
    float toy2Y = platform3Y - 18;
    glLineWidth(1.5f);
    setColor({ 0.3f, 0.3f, 0.3f });
    glBegin(GL_LINES);
    glVertex2f(centerPoleX + 25, platform3Y);
    glVertex2f(toy2X, toy2Y);
    glEnd();
    setColor({ 0.9f, 0.7f, 0.2f });
    drawEllipse(toy2X, toy2Y, 5, 10, 8);
    setColor({ 0.8f, 0.5f, 0.1f });
    drawEllipse(toy2X - 2, toy2Y, 3, 8, 8);
}

void drawBarista(float x, float y, float scale) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color skin = { 0.85f, 0.65f, 0.50f };
    Color apron = { 0.55f, 0.35f, 0.20f };
    Color shirt = { 0.95f, 0.92f, 0.88f };

    drawFilledRect(x + 15 * scale, y, 14 * scale, 55 * scale, { 0.3f, 0.25f, 0.2f }, edge, 1.5f);
    drawFilledRect(x + 38 * scale, y, 14 * scale, 55 * scale, { 0.3f, 0.25f, 0.2f }, edge, 1.5f);

    drawFilledRect(x + 12 * scale, y - 10 * scale, 20 * scale, 10 * scale, { 0.2f, 0.15f, 0.1f }, edge, 1.5f);
    drawFilledRect(x + 35 * scale, y - 10 * scale, 20 * scale, 10 * scale, { 0.2f, 0.15f, 0.1f }, edge, 1.5f);

    drawFilledRect(x + 8 * scale, y + 55 * scale, 50 * scale, 70 * scale, shirt, edge, 2.0f);

    drawFilledRect(x + 10 * scale, y + 62 * scale, 46 * scale, 60 * scale, apron, edge, 2.0f);

    glLineWidth(2.5f * scale);
    setColor(apron);
    glBegin(GL_LINES);
    glVertex2f(x + 18 * scale, y + 122 * scale);
    glVertex2f(x + 24 * scale, y + 142 * scale);
    glVertex2f(x + 48 * scale, y + 122 * scale);
    glVertex2f(x + 42 * scale, y + 142 * scale);
    glEnd();

    drawFilledRect(x + 25 * scale, y + 75 * scale, 18 * scale, 16 * scale, { 0.5f, 0.3f, 0.15f }, edge, 1.0f);

    drawFilledRect(x - 8 * scale, y + 95 * scale, 18 * scale, 14 * scale, shirt, edge, 1.5f);
    drawFilledRect(x - 28 * scale, y + 88 * scale, 22 * scale, 12 * scale, shirt, edge, 1.5f);

    drawFilledRect(x + 58 * scale, y + 85 * scale, 14 * scale, 40 * scale, shirt, edge, 1.5f);

    setColor(skin);
    drawCircleFilled(x - 25 * scale, y + 94 * scale, 7 * scale);
    drawCircleFilled(x + 65 * scale, y + 78 * scale, 7 * scale);

    drawFilledRect(x - 45 * scale, y + 92 * scale, 60 * scale, 5 * scale, { 0.6f, 0.5f, 0.4f }, edge, 1.5f);

    drawFilledRect(x - 36 * scale, y + 97 * scale, 14 * scale, 16 * scale, { 0.95f, 0.95f, 0.9f }, edge, 1.0f);
    drawFilledRect(x - 15 * scale, y + 97 * scale, 14 * scale, 16 * scale, { 0.75f, 0.9f, 0.85f }, edge, 1.0f);

    drawFilledRect(x + 26 * scale, y + 125 * scale, 14 * scale, 12 * scale, skin, edge, 1.0f);

    setColor(skin);
    drawCircleFilled(x + 33 * scale, y + 155 * scale, 22 * scale);

    setColor(skin);
    drawCircleFilled(x + 13 * scale, y + 155 * scale, 8 * scale);
    drawCircleFilled(x + 53 * scale, y + 155 * scale, 8 * scale);

    setColor({ 0.35f, 0.25f, 0.18f });
    drawCircleFilled(x + 33 * scale, y + 168 * scale, 24 * scale);
    drawFilledRect(x + 11 * scale, y + 148 * scale, 44 * scale, 22 * scale, { 0.35f, 0.25f, 0.18f }, { 0.35f, 0.25f, 0.18f }, 0);

    setColor({ 0.35f, 0.25f, 0.18f });
    drawCircleFilled(x + 20 * scale, y + 165 * scale, 10 * scale);
    drawCircleFilled(x + 33 * scale, y + 170 * scale, 10 * scale);
    drawCircleFilled(x + 46 * scale, y + 165 * scale, 10 * scale);

    setColor({ 1.0f, 1.0f, 1.0f });
    drawCircleFilled(x + 24 * scale, y + 155 * scale, 5 * scale);
    drawCircleFilled(x + 42 * scale, y + 155 * scale, 5 * scale);
    setColor({ 0.2f, 0.15f, 0.1f });
    drawCircleFilled(x + 25 * scale, y + 155 * scale, 3 * scale);
    drawCircleFilled(x + 43 * scale, y + 155 * scale, 3 * scale);

    setColor({ 1.0f, 1.0f, 1.0f });
    drawCircleFilled(x + 26 * scale, y + 157 * scale, 1.5f * scale);
    drawCircleFilled(x + 44 * scale, y + 157 * scale, 1.5f * scale);

    glLineWidth(2.0f * scale);
    setColor({ 0.3f, 0.2f, 0.15f });
    glBegin(GL_LINES);
    glVertex2f(x + 19 * scale, y + 162 * scale);
    glVertex2f(x + 28 * scale, y + 163 * scale);
    glVertex2f(x + 38 * scale, y + 163 * scale);
    glVertex2f(x + 47 * scale, y + 162 * scale);
    glEnd();

    setColor({ 0.75f, 0.55f, 0.40f });
    drawCircleFilled(x + 33 * scale, y + 148 * scale, 2.5f * scale);

    glLineWidth(2.0f * scale);
    setColor({ 0.6f, 0.3f, 0.3f });
    glBegin(GL_LINE_STRIP);
    glVertex2f(x + 24 * scale, y + 142 * scale);
    glVertex2f(x + 28 * scale, y + 139 * scale);
    glVertex2f(x + 33 * scale, y + 138 * scale);
    glVertex2f(x + 38 * scale, y + 139 * scale);
    glVertex2f(x + 42 * scale, y + 142 * scale);
    glEnd();

    glColor4f(0.95f, 0.6f, 0.6f, 0.4f);
    drawCircleFilled(x + 18 * scale, y + 146 * scale, 6 * scale);
    drawCircleFilled(x + 48 * scale, y + 146 * scale, 6 * scale);
}

void drawCatHouse(float x, float y, float scale) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color wood = { 0.7f, 0.5f, 0.3f };

    drawFilledRect(x, y, 80 * scale, 60 * scale, wood, edge, 2.0f);

    setColor({ 0.6f, 0.4f, 0.25f });
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 10 * scale, y + 60 * scale);
    glVertex2f(x + 40 * scale, y + 90 * scale);
    glVertex2f(x + 90 * scale, y + 60 * scale);
    glEnd();

    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 10 * scale, y + 60 * scale);
    glVertex2f(x + 40 * scale, y + 90 * scale);
    glVertex2f(x + 90 * scale, y + 60 * scale);
    glEnd();

    setColor({ 0.2f, 0.2f, 0.2f });
    drawCircleFilled(x + 40 * scale, y + 30 * scale, 18 * scale);
    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 20.0f * TWO_PI;
        glVertex2f(x + 40 * scale + cosf(angle) * 18 * scale, y + 30 * scale + sinf(angle) * 18 * scale);
    }
    glEnd();

    setColor({ 0.9f, 0.9f, 0.85f });
    drawCircleFilled(x + 65 * scale, y + 42 * scale, 6 * scale);
}

void drawCatBowl(float x, float y, float scale) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color bowl = { 0.9f, 0.3f, 0.4f };

    float centerX = x + 20 * scale;
    float centerY = y + 10 * scale;
    setColor(bowl);
    drawEllipse(centerX, centerY + 8 * scale, 20 * scale, 6 * scale);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    for (int i = 0; i <= 12; i++) {
        float angle = 3.14159f * (1.0f + (float)i / 12.0f);
        glVertex2f(centerX + cosf(angle) * 20 * scale, centerY + 8 * scale + sinf(angle) * 8 * scale);
    }
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; i++) {
        float angle = 3.14159f * (1.0f + (float)i / 12.0f);
        glVertex2f(centerX + cosf(angle) * 20 * scale, centerY + 8 * scale + sinf(angle) * 8 * scale);
    }
    glEnd();
}

void drawCatFoodBag(float x, float y, float scale) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color bag = { 0.7f, 0.5f, 0.3f };

    drawFilledRect(x, y, 35 * scale, 50 * scale, bag, edge, 2.0f);

    setColor({ 0.6f, 0.4f, 0.25f });
    glBegin(GL_QUADS);
    glVertex2f(x, y + 50 * scale);
    glVertex2f(x + 35 * scale, y + 50 * scale);
    glVertex2f(x + 30 * scale, y + 58 * scale);
    glVertex2f(x + 5 * scale, y + 58 * scale);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y + 50 * scale);
    glVertex2f(x + 35 * scale, y + 50 * scale);
    glVertex2f(x + 30 * scale, y + 58 * scale);
    glVertex2f(x + 5 * scale, y + 58 * scale);
    glEnd();

    drawFilledRect(x + 8 * scale, y + 20 * scale, 20 * scale, 20 * scale, { 0.95f, 0.9f, 0.85f }, edge, 1.0f);
}

void drawCouchArea(float x, float y, float scale = 1.0f) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color couchColor = { 0.92f, 0.89f, 0.85f };
    Color cushion = { 0.95f, 0.92f, 0.88f };
    Color armrestColor = { 0.90f, 0.87f, 0.83f };

    // couch scaling
    float armrestWidth = 12 * scale;
    float armrestHeight = 60 * scale;
    float couchWidth = 250 * scale;
    float backrestHeight = 60 * scale;
    float baseHeight = 80 * scale;
    float backCushionWidth1 = 120 * scale;
    float backCushionWidth2 = 119 * scale;
    float backCushionHeight = 75 * scale;
    float seatCushionWidth = 125 * scale;
    float seatCushionHeight = 30 * scale;

    drawFilledRect(x - armrestWidth, y, armrestWidth, armrestHeight, armrestColor, edge, 2.0f); // Left armrest
    drawFilledRect(x + couchWidth, y, armrestWidth, armrestHeight, armrestColor, edge, 2.0f); // Right armrest

    drawFilledRect(x, y, couchWidth, backrestHeight, couchColor, edge, 2.0f); // Couch base
    drawFilledRect(x, y + 50 * scale, couchWidth, baseHeight, couchColor, edge, 2.0f); // Couch backrest

    drawFilledRect(x + 5 * scale, y + 50 * scale, backCushionWidth1, backCushionHeight, cushion, edge, 1.5f); // Left back cushion
    drawFilledRect(x + 125 * scale, y + 50 * scale, backCushionWidth2, backCushionHeight, cushion, edge, 1.5f); // Right back cushion

    drawFilledRect(x, y + 18 * scale, seatCushionWidth, seatCushionHeight, cushion, edge, 1.5f); // Left seat cushion
    drawFilledRect(x + 125 * scale, y + 18 * scale, seatCushionWidth, seatCushionHeight, cushion, edge, 1.5f); // Right seat cushion
}

void drawCoffeeTable(float x, float y) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color tableTop = { 0.6f, 0.4f, 0.2f };
    Color tableTopDark = { 0.5f, 0.3f, 0.15f };
    Color leg = { 0.5f, 0.3f, 0.15f };
    Color legDark = { 0.4f, 0.25f, 0.1f };

    float tableWidth = 120;
    float tableDepth = 8;
    float legHeight = 35;
    float legWidth = 12;
    float perspective = 4.0f;

    setColor(tableTop);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + tableWidth, y);
    glVertex2f(x + tableWidth + perspective, y + tableDepth);
    glVertex2f(x + perspective, y + tableDepth);
    glEnd();

    setColor(tableTopDark);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + perspective, y + tableDepth);
    glVertex2f(x + perspective, y + tableDepth + 2);
    glVertex2f(x, y + 2);
    glEnd();

    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + tableWidth, y);
    glVertex2f(x + tableWidth + perspective, y + tableDepth);
    glVertex2f(x + perspective, y + tableDepth);
    glEnd();

    float leftLegX = x + 10;
    float rightLegX = x + 98;

    setColor(leg);
    glBegin(GL_QUADS);
    glVertex2f(leftLegX, y - legHeight);
    glVertex2f(leftLegX + legWidth, y - legHeight);
    glVertex2f(leftLegX + legWidth + perspective * 0.3f, y - legHeight + perspective * 0.3f);
    glVertex2f(leftLegX + perspective * 0.3f, y - legHeight + perspective * 0.3f);
    glEnd();

    setColor(legDark);
    glBegin(GL_QUADS);
    glVertex2f(leftLegX, y - legHeight);
    glVertex2f(leftLegX + perspective * 0.3f, y - legHeight + perspective * 0.3f);
    glVertex2f(leftLegX + perspective * 0.3f, y + perspective * 0.3f);
    glVertex2f(leftLegX, y);
    glEnd();

    setColor(leg);
    glBegin(GL_QUADS);
    glVertex2f(rightLegX, y - legHeight);
    glVertex2f(rightLegX + legWidth, y - legHeight);
    glVertex2f(rightLegX + legWidth + perspective * 0.3f, y - legHeight + perspective * 0.3f);
    glVertex2f(rightLegX + perspective * 0.3f, y - legHeight + perspective * 0.3f);
    glEnd();

    setColor(legDark);
    glBegin(GL_QUADS);
    glVertex2f(rightLegX, y - legHeight);
    glVertex2f(rightLegX + perspective * 0.3f, y - legHeight + perspective * 0.3f);
    glVertex2f(rightLegX + perspective * 0.3f, y + perspective * 0.3f);
    glVertex2f(rightLegX, y);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(leftLegX, y - legHeight);
    glVertex2f(leftLegX, y);
    glVertex2f(leftLegX + legWidth, y - legHeight);
    glVertex2f(leftLegX + legWidth, y);
    glVertex2f(rightLegX, y - legHeight);
    glVertex2f(rightLegX, y);
    glVertex2f(rightLegX + legWidth, y - legHeight);
    glVertex2f(rightLegX + legWidth, y);
    glEnd();
}

void drawEntryArea(float x, float y) { // Door
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color doorWood = { 0.55f, 0.35f, 0.20f };
    Color doorDark = { 0.45f, 0.25f, 0.10f };

    drawFilledRect(x, y + 120, 160, 340, doorWood, edge, 3.0f); // Main door square
    drawFilledRect(x + 10, y + 300, 140, 150, doorDark, edge, 2.0f); // Top middle square
    drawFilledRect(x + 10, y + 130, 140, 150, doorDark, edge, 2.0f); // Bottom middle square
    setColor({ 0.8f, 0.7f, 0.3f }); // Brown
    drawCircleFilled(x + 130, y + 290, 8); // Door handle - circle
    drawFilledRect(x + 125, y + 285, 20, 10, { 0.8f, 0.7f, 0.3f }, edge, 1.0f); // Door handle - rect
    drawFilledRect(x + 10, y + 70, 140, 45, { 0.7f, 0.3f, 0.3f }, edge, 2.0f); // Mat
    float boardX = x + 200, boardY = y + 160;
}

void drawScenePixels() {
    Color wall = { 1.0f, 0.96f, 0.86f };
    Color ceiling = { 1.0f, 0.98f, 0.92f };
    Color edge = { 0.06f, 0.03f, 0.01f };

    setColor(wall);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(SCENE_WIDTH, 0);
    glVertex2f(SCENE_WIDTH, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    drawFilledRect(0, windowHeight - 24, SCENE_WIDTH, 24, ceiling, edge, 2.0f); // Ceiling crown molding

    glLineWidth(2.0f); // Horizontal line decor
    setColor(edge);
    float horizontalLineY = windowHeight * 0.32f;
    glBegin(GL_LINES);
    glVertex2f(0, horizontalLineY);
    glVertex2f(SCENE_WIDTH, horizontalLineY);
    glEnd();

    float baseY = windowHeight * 0.18f;
    float midY = windowHeight * 0.52f;
    float upperY = windowHeight * 0.58f;

    drawPlantPixel(660, baseY, (plantColorIndex + 2) % numPlantColors, plantSwayAngle);

    float paintingX = 150; // Painting frame (left side of door)
    float paintingY = windowHeight * 0.49f;
    drawFilledRect(paintingX, paintingY, 200, 240, { 0.3f, 0.2f, 0.1f }, edge, 3.0f);
    drawFilledRect(paintingX + 10, paintingY + 10, 180, 220, { 0.95f, 0.95f, 0.9f }, edge, 2.0f);

    setColor({ 0.8f, 0.3f, 0.4f }); // Painting circles
    drawCircleFilled(paintingX + 100, windowHeight * 0.62f, 40);
    setColor({ 0.3f, 0.6f, 0.8f });
    drawCircleFilled(paintingX + 80, windowHeight * 0.58f, 30);
    setColor({ 0.9f, 0.7f, 0.2f });
    drawCircleFilled(paintingX + 120, windowHeight * 0.6f, 25);

    drawEntryArea(400, baseY);
    drawCatPlaypen(60, baseY, 150, 180);
    drawWindowPixel(700, midY, 220, 260, curtainOpenAmount + curtainSway);

    float counterX = 1150; // Counter setup
    float counterWidth = windowWidth * 0.52f;
    float counterHeight = windowHeight * 0.22f;
    float counterTop = baseY + counterHeight;

    drawCountertopStyled(counterX - 8, counterTop, counterWidth + 16, 22);
    drawCounterStyled(counterX, baseY, counterWidth, counterHeight);

    drawCoffeeMachineStyled(counterX + 70, counterTop + 6, 1.0f); // Coffee machine
    drawBrownCoffeeMakerStyled(counterX + 220, counterTop + 6, 1.0f); // Coffee maker
    drawMug(counterX + 320, counterTop + 8, { 0.75f, 0.9f, 0.8f }); // Mug with steam
    drawMug(counterX + 360, counterTop + 8, { 0.9f, 0.75f, 0.6f }); // Mug
    drawCashRegisterPixel(counterX + counterWidth - 120, counterTop + 6, 1.0f);
    drawBarista(counterX + 480, baseY, 1.4f);

    drawShelvesWithItems(counterX + 380, upperY);
    float boardX = 2025;
    float boardY = windowHeight * 0.56f;
    drawFilledRect(boardX, boardY + 10, 140, 150, { 0.77f, 0.56f, 0.33f }, edge, 2.0f);
    drawFilledRect(boardX + 20, boardY + 110, 40, 40, { 0.96f, 0.89f, 0.49f }, edge);
    drawFilledRect(boardX + 70, boardY + 100, 40, 40, { 0.68f, 0.86f, 0.96f }, edge);

    drawPendantLamp(counterX + 200, windowHeight - 30, lightBrightness);
    drawPendantLamp(counterX + 450, windowHeight - 30, lightBrightness);

    drawWallClock(1200, windowHeight * 0.65f, 25);
    drawChalkboardMenu(1000, windowHeight * 0.55f, 140, 180);

    drawCatPawPrint(180, windowHeight * 0.4f, 6);
    drawCatPawPrint(200, windowHeight * 0.38f, 6);
    drawCatPawPrint(220, windowHeight * 0.4f, 6);
    drawCatPawPrint(1900, windowHeight * 0.45f, 5);
    drawCatPawPrint(1920, windowHeight * 0.43f, 5);

    drawPlantPixel(950, baseY, plantColorIndex, plantSwayAngle * 1.3f);

    drawCatPixel(counterX - 80, baseY - 24, 0.9f, catColorIndex, catRotationAngle);
    drawSleepingCatPixel(counterX + 200, baseY - 60, 1.0f);

    drawCatPixel(counterX + 500, counterTop + 15, 0.7f, 1, 0.0f, 0.0f, false);
    for (const auto& cat : movingCats) {
        drawCatPixel(cat.x, cat.y, 0.8f, cat.colorIndex, 0.0f, cat.walkCycle, cat.isWalking && !cat.isIdle);
    }

    float couchX = SCENE_WIDTH - 550; // Move couch left or right
    float couchY = baseY + 115; // Move couch up or down
    drawCouchArea(couchX, couchY, couchScale);

    float couchWidth = 250; // Main couch width
    float tableWidth = 120;
    float tableX = couchX + (couchWidth - tableWidth) / 2.0f; // Center coffee table horizontally under couch
    float tableY = couchY - 80; // Position below the couch
    drawCoffeeTable(tableX, tableY);

    drawSleepingCatPixel(SCENE_WIDTH - 500, 60, 1.2f);

    drawCatBowl(2300, baseY - 15, 1.0f);
    drawCatFoodBag(2400, baseY - 15, 1.0f);
    drawCatHouse(2500, baseY - 15, 1.0f);
}

struct Smoke { float x, y, life, offsetX; };
vector<Smoke> smokeParticles;
float smokeTimer = 0.0f;

void spawnSmoke(float x, float y) {
    Smoke s;
    s.x = x + (rand() % 20 - 10);
    s.y = y;
    s.life = 1.0f;
    s.offsetX = (rand() % 60 - 30) * 0.003f;
    smokeParticles.push_back(s);
}

void updateSmoke(float deltaTime) {
    smokeTimer += deltaTime;
    float baseY = windowHeight * 0.18f;
    float counterTop = baseY + windowHeight * 0.22f;

    if (smokeTimer >= 1.8f && (rand() % 100) < 30) {
        float mugX = 1150 + 320;
        float mugY = counterTop + 8;
        spawnSmoke(mugX + 12, mugY + 24);
        smokeTimer = 0.0f;
    }
    if (smokeTimer >= 2.5f && (rand() % 100) < 15) {
        float coffeeMachineX = 1150 + 70;
        spawnSmoke(coffeeMachineX + 57, counterTop + 80);
    }
    if (smokeTimer >= 3.0f && (rand() % 100) < 8) {
        float coffeeMakerX = 1150 + 220;
        spawnSmoke(coffeeMakerX + 35, counterTop + 100);
        smokeTimer = 0.0f;
    }
    if (smokeTimer > 4.0f) {
        smokeTimer = 0.0f;
    }

    for (int particleIndex = (int)smokeParticles.size() - 1; particleIndex >= 0; --particleIndex) {
        Smoke& particle = smokeParticles[particleIndex];
        particle.life -= deltaTime * 0.5f;
        particle.y += deltaTime * 35.0f;
        particle.x += particle.offsetX * deltaTime * 50.0f;
        if (particle.life <= 0)
            smokeParticles.erase(smokeParticles.begin() + particleIndex);
    }
}

void drawSmokePixels() {
    for (auto& particle : smokeParticles) {
        float alpha = particle.life * 0.4f;
        glColor4f(0.85f, 0.85f, 0.85f, alpha);
        drawCircleFilled(particle.x, particle.y, 4.0f + (1.0f - particle.life) * 6.0f);
    }
}

void drawDustParticles() {
    for (const auto& p : dustParticles) {
        float alpha = p.life * 0.3f;
        glColor4f(0.8f, 0.8f, 0.75f, alpha);
        drawCircleFilled(p.x, p.y, p.size);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float maxCameraX = SCENE_WIDTH - windowWidth;
    cameraX = max(0.0f, min(cameraX, maxCameraX));

    glPushMatrix();
    glTranslatef(-cameraX, 0.0f, 0.0f);
    drawScenePixels();
    drawDustParticles();
    drawSmokePixels();
    glPopMatrix();

    glutSwapBuffers();
}

void onResize(int width, int height) {
    windowWidth = max(200, width);
    windowHeight = max(120, height);

    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void updateDustParticles(float deltaTime) {
    dustTimer += deltaTime;
    if (dustTimer >= 0.5f) {
        Particle p;
        p.x = (rand() % SCENE_WIDTH);
        p.y = windowHeight * 0.3f + (rand() % (int)(windowHeight * 0.5f));
        p.life = 1.0f;
        p.speed = 5.0f + (rand() % 10) * 0.5f;
        p.size = 1.0f + (rand() % 3);
        dustParticles.push_back(p);
        dustTimer = 0.0f;
    }

    for (int i = (int)dustParticles.size() - 1; i >= 0; --i) {
        Particle& p = dustParticles[i];
        p.life -= deltaTime * 0.3f;
        p.y += p.speed * deltaTime * 20.0f;
        p.x += sinf(animationTime + p.x * 0.01f) * deltaTime * 10.0f;
        if (p.life <= 0 || p.y > windowHeight + 20)
            dustParticles.erase(dustParticles.begin() + i);
    }
}

void updateAnimations(float deltaTime) {
    animationTime += deltaTime;
    clockTime += deltaTime * 60.0f;
    if (clockTime > 43200.0f) clockTime -= 43200.0f;
    plantSwayAngle = sinf(animationTime * 0.4f) * 0.15f;
    lightBlinkTimer += deltaTime;
    if (lightBlinkTimer > 5.0f) lightBlinkTimer = 0.0f;
    lightBrightness = 0.85f + 0.15f * (1.0f + sinf(lightBlinkTimer * 0.8f)) * 0.5f;
    curtainSway = sinf(animationTime * 0.3f) * 0.02f;
    updateDustParticles(deltaTime);

    for (auto& cat : movingCats) {
        if (cat.isIdle) {
            cat.idleTimer += deltaTime;
            if (cat.idleTimer > 2.0f + (rand() % 30) * 0.1f) {
                cat.isIdle = false;
                cat.isWalking = true;
                cat.walkTimer = cat.idleTimer = 0.0f;
            }
        }
        else if (cat.isWalking) {
            cat.walkTimer += deltaTime;
            if (cat.walkTimer > 3.0f + (rand() % 50) * 0.1f) {
                cat.isIdle = true;
                cat.isWalking = false;
                cat.walkTimer = 0.0f;
            }
            else {
                cat.x += cat.speed * cat.direction * deltaTime * 60.0f;
                cat.walkCycle += deltaTime * 2.5f;
                if (cat.walkCycle > 1.0f) cat.walkCycle -= 1.0f;

                if (cat.x > SCENE_WIDTH - 100 || cat.x < 100) {
                    cat.direction = (cat.x > SCENE_WIDTH - 100) ? -1.0f : 1.0f;
                    cat.isIdle = true;
                    cat.isWalking = false;
                    cat.walkTimer = 0.0f;
                }
            }
        }
    }
}

void timer(int v) {
    float deltaTime = 0.016f;
    updateSmoke(deltaTime);
    updateAnimations(deltaTime);
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            catRotationAngle += 15.0f;
            catColorIndex = (catColorIndex + 1) % numCatColors;
            glutPostRedisplay();
        }
        else if (button == GLUT_RIGHT_BUTTON)
            plantColorIndex = (plantColorIndex + 1) % numPlantColors;
        else if (button == 3) {
            couchScale = min(2.0f, couchScale + 0.1f);
            glutPostRedisplay();
        }
        else if (button == 4) {
            couchScale = max(0.5f, couchScale - 0.1f);
            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC
        exit(0);
        break;
    case 'a':
    case 'A':
        cameraX -= CAMERA_STEP;
        break;
    case 'd':
    case 'D':
        cameraX += CAMERA_STEP;
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX -= CAMERA_STEP;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += CAMERA_STEP;
        break;
    case GLUT_KEY_UP:
        curtainOpenAmount = min(1.0f, curtainOpenAmount + 0.1f);
        break;
    case GLUT_KEY_DOWN:
        curtainOpenAmount = max(0.0f, curtainOpenAmount - 0.1f);
        break;
    }
    glutPostRedisplay();
}

void initGL() {
    glClearColor(1.0f, 0.96f, 0.86f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

int main(int argc, char** argv) {
    srand(time(0));

    float baseY = WIN_H * 0.18f;
    MovingCat cats[] = {
        {300.0f, baseY - 24, 4.0f, 1.0f, 0, 0.0f, true, 0.0f, 0.0f, false},
        {800.0f, baseY - 24, 3.5f, -1.0f, 2, 0.3f, false, 1.0f, 0.0f, true},
        {1400.0f, baseY + 50, 5.0f, 1.0f, 1, 0.6f, true, 0.0f, 0.0f, false},
        {1800.0f, baseY + 30, 3.0f, -1.0f, 3, 0.2f, false, 0.5f, 0.0f, true}
    };
    movingCats.assign(cats, cats + 4);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Cat Cafe Scene - OpenGL Fundamentals");

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Init error: %s\n", glewGetErrorString(err));
        return 1;
    }

    glutFullScreen();
    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);
    glutTimerFunc(16, timer, 0);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
