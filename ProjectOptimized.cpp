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

int plantColorIndex = 0;
float plantColors[][3] = {
    {0.2f,0.6f,0.3f}, {0.6f,0.2f,0.6f}, {0.6f,0.4f,0.2f}, {0.2f,0.4f,0.6f}, {0.8f,0.2f,0.2f}
};
int numPlantColors = 5;

float curtainOpenAmount = 0.0f;

struct Color { GLfloat r, g, b; };

const float PI = 3.14159f;
const float TWO_PI = 6.28318f;

inline void setColor(const Color& c) {
    glColor3f(c.r, c.g, c.b);
}

void drawFilledRect(float x, float y, float w, float h, const Color& fill, const Color& outline, float outlineWidth = 2.0f) {
    setColor(fill);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    glLineWidth(outlineWidth);
    setColor(outline);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawCircleFilled(float cx, float cy, float r, int segs = 16) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    const float step = TWO_PI / segs;
    for (int i = 0; i <= segs; i++) {
        float a = i * step;
        glVertex2f(cx + cosf(a) * r, cy + sinf(a) * r);
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

void drawEllipse(float cx, float cy, float rx, float ry, int segs = 16) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    const float step = TWO_PI / segs;
    for (int i = 0; i <= segs; i++) {
        float a = i * step;
        glVertex2f(cx + cosf(a) * rx, cy + sinf(a) * ry);
    }
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

void drawCounterStyled(float x, float y, float w, float h) {
    Color edge = { 0.0f, 0.0f, 0.0f };
    Color wood = { 0.8f, 0.6f, 0.4f };
    drawFilledRect(x, y, w, h, wood, edge, 1.5f);
    
    glLineWidth(1.5f);
    setColor(edge);
    float lineSpacing = h * 0.25f;
    for (int i = 1; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x, y + i * lineSpacing);
        glVertex2f(x + w - 20, y + i * lineSpacing);
        glEnd();
    }
}

void drawCountertopStyled(float x, float y, float w, float h) {
    Color edge = { 0.0f, 0.0f, 0.0f };
    drawFilledRect(x, y, w, h, { 0.8f, 0.6f, 0.4f }, edge, 1.5f);
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

void drawCatPixel(float cx, float cy, float scale, int colorIndex) {
    Color body = { catColors[colorIndex][0], catColors[colorIndex][1], catColors[colorIndex][2] };
    Color darker = darken(body);
    Color lighter = lighten(body);

    setColor(body);
    drawEllipse(cx, cy, 32 * scale, 28 * scale);
    setColor(lighter);
    drawEllipse(cx - 5 * scale, cy - 8 * scale, 18 * scale, 20 * scale);
    setColor(body);
    drawCircleFilled(cx - 22 * scale, cy + 22 * scale, 20 * scale);
    setColor(lighter);
    drawEllipse(cx - 22 * scale, cy + 18 * scale, 14 * scale, 12 * scale);

    setColor(body);
    drawTriangleFilled(cx - 36 * scale, cy + 34 * scale, cx - 28 * scale, cy + 50 * scale, cx - 20 * scale, cy + 36 * scale);
    drawTriangleFilled(cx - 24 * scale, cy + 34 * scale, cx - 16 * scale, cy + 50 * scale, cx - 8 * scale, cy + 36 * scale);

    setColor({ 0.95f, 0.7f, 0.7f });
    drawTriangleFilled(cx - 33 * scale, cy + 36 * scale, cx - 28 * scale, cy + 45 * scale, cx - 23 * scale, cy + 37 * scale);
    drawTriangleFilled(cx - 21 * scale, cy + 36 * scale, cx - 16 * scale, cy + 45 * scale, cx - 11 * scale, cy + 37 * scale);

    setColor({ 1.0f, 1.0f, 0.9f });
    drawEllipse(cx - 28 * scale, cy + 24 * scale, 4 * scale, 5 * scale);
    drawEllipse(cx - 16 * scale, cy + 24 * scale, 4 * scale, 5 * scale);

    setColor({ 0.1f, 0.1f, 0.1f });
    drawEllipse(cx - 28 * scale, cy + 24 * scale, 2 * scale, 4 * scale);
    drawEllipse(cx - 16 * scale, cy + 24 * scale, 2 * scale, 4 * scale);

    setColor({ 0.9f, 0.6f, 0.6f });
    drawTriangleFilled(cx - 22 * scale, cy + 18 * scale, cx - 20 * scale, cy + 14 * scale, cx - 24 * scale, cy + 14 * scale);

    glLineWidth(1.0f);
    setColor({ 0.3f, 0.3f, 0.3f });
    glBegin(GL_LINES);
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 40 * scale, cy + 18 * scale);
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 40 * scale, cy + 14 * scale);
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 4 * scale, cy + 18 * scale);
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 4 * scale, cy + 14 * scale);
    glEnd();

    setColor(body);
    drawEllipse(cx - 12 * scale, cy - 22 * scale, 8 * scale, 6 * scale);
    drawEllipse(cx + 8 * scale, cy - 22 * scale, 8 * scale, 6 * scale);
    drawCircleFilled(cx + 24 * scale, cy - 4 * scale, 10 * scale);
    drawCircleFilled(cx + 32 * scale, cy - 12 * scale, 9 * scale);
    drawCircleFilled(cx + 38 * scale, cy - 20 * scale, 8 * scale);
    setColor(darker);
    drawCircleFilled(cx + 42 * scale, cy - 26 * scale, 6 * scale);
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

void drawPlantPixel(float cx, float cy, int colorIndex) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color potColor = { 0.79f, 0.37f, 0.18f };
    Color potHighlight = { 0.89f, 0.47f, 0.28f };

    drawFilledRect(cx - 40, cy - 40, 80, 40, potColor, edge, 2.0f);
    drawFilledRect(cx - 42, cy - 2, 84, 8, potHighlight, edge, 1.5f);

    setColor(potHighlight);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx - 30, cy - 35);
    glVertex2f(cx - 20, cy - 35);
    glVertex2f(cx - 25, cy - 10);
    glEnd();

    drawFilledRect(cx - 36, cy - 6, 72, 8, { 0.4f, 0.3f, 0.2f }, edge, 1.0f);

    Color leafColor = { plantColors[colorIndex][0], plantColors[colorIndex][1], plantColors[colorIndex][2] };
    Color leafDark = darken(leafColor);
    Color leafLight = lighten(leafColor, 1.3f);

    glLineWidth(3.0f);
    setColor({ 0.2f, 0.5f, 0.2f });
    glBegin(GL_LINES);
    glVertex2f(cx, cy); glVertex2f(cx, cy + 45);
    glVertex2f(cx, cy + 20); glVertex2f(cx - 35, cy + 25);
    glVertex2f(cx, cy + 20); glVertex2f(cx + 35, cy + 25);
    glEnd();

    struct Leaf { float x, y, angle, size; };
    Leaf leaves[] = { {cx, cy + 45, -15, 1.2f}, {cx - 40, cy + 20, 10, 1.0f}, {cx + 40, cy + 20, 20, 1.0f} };

    for (auto& leaf : leaves) {
        glPushMatrix();
        glTranslatef(leaf.x, leaf.y, 0);
        glRotatef(leaf.angle, 0, 0, 1);

        setColor(leafColor);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = 0; i <= 12; i++) {
            float a = (float)i / 12.0f * TWO_PI;
            float r = 28 * leaf.size;
            if (i % 3 == 0) r *= 0.7f;
            glVertex2f(cosf(a) * r, sinf(a) * r);
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

void drawWindowPixel(float x, float y, float w, float h, float openAmount) {
    Color frame = { 0.73f, 0.56f, 0.36f };
    Color glass = { 0.68f, 0.85f, 0.90f };

    drawFilledRect(x, y, w, h, frame, { 0.06f, 0.03f, 0.01f }, 2.0f);
    drawFilledRect(x + 8, y + 8, w - 16, h - 16, glass, { 0.06f, 0.03f, 0.01f }, 1.2f);

    drawTreeThroughWindow(x + w * 0.5f, y + 20, 1.2f);

    glLineWidth(3.0f);
    setColor({ 0.73f, 0.56f, 0.36f });
    glBegin(GL_LINES);
    glVertex2f(x + w * 0.5f, y + 8); glVertex2f(x + w * 0.5f, y + h - 8);
    glVertex2f(x + 8, y + h * 0.5f); glVertex2f(x + w - 8, y + h * 0.5f);
    glEnd();

    drawFilledRect(x - 10, y + h - 12, w + 20, 6, frame, { 0.06f, 0.03f, 0.01f }, 1.5f);
    
    for (float cx = x + 12; cx <= x + w - 12; cx += 48)
        drawCircleFilled(cx, y + h - 9, 6);

    float offset = openAmount * w * 0.35f;
    drawFilledRect(x + 10 - offset, y + 10, w * 0.5f - 14, h - 20, { 0.95f, 0.95f, 0.95f }, { 0.9f, 0.9f, 0.9f }, 1.0f);
    drawFilledRect(x + w * 0.5f + 4 + offset, y + 10, w * 0.5f - 14, h - 20, { 0.95f, 0.95f, 0.95f }, { 0.9f, 0.9f, 0.9f }, 1.0f);
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
            float kx = x + 12 * scale + col * 9 * scale;
            float ky = y + 6 * scale + row * 8 * scale;
            drawFilledRect(kx, ky, 7 * scale, 6 * scale, keyColor, edge, 0.8f);
        }
    }

    drawFilledRect(x + 42 * scale, y + 6 * scale, 20 * scale, 6 * scale, { 0.7f, 0.9f, 0.7f }, edge, 0.8f);
    drawFilledRect(x + 42 * scale, y + 14 * scale, 20 * scale, 6 * scale, { 0.9f, 0.7f, 0.7f }, edge, 0.8f);
    drawFilledRect(x + 5 * scale, y - 2 * scale, 80 * scale, 6 * scale, { 0.2f, 0.2f, 0.2f }, edge, 1.0f);

    setColor({ 0.5f, 0.5f, 0.5f });
    drawFilledRect(x + 35 * scale, y - 1 * scale, 20 * scale, 3 * scale, { 0.5f, 0.5f, 0.5f }, edge, 0.5f);
}

void drawJar(float x, float y, float w, float h, Color fill) {
    Color edge = { 0.2f, 0.2f, 0.2f };

    setColor(fill);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y);
    glVertex2f(x + w - 2, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 2, y);
    glVertex2f(x + w - 2, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    drawFilledRect(x - 2, y + h, w + 4, 4, { 0.5f, 0.45f, 0.4f }, edge, 1.0f);
    drawFilledRect(x - 1, y + h + 4, w + 2, 3, { 0.6f, 0.55f, 0.5f }, edge, 1.0f);
}

void drawCup(float x, float y, float w, float h, Color fill) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    drawFilledRect(x, y, w, h, fill, edge, 1.5f);

    float h3 = h * 0.3f, h35 = h * 0.35f, h45 = h * 0.45f;
    float h65 = h * 0.65f, h75 = h * 0.75f, h8 = h * 0.8f;

    setColor(fill);
    glBegin(GL_POLYGON);
    glVertex2f(x + w, y + h3);
    glVertex2f(x + w + 8, y + h35);
    glVertex2f(x + w + 8, y + h75);
    glVertex2f(x + w, y + h8);
    glVertex2f(x + w, y + h65);
    glVertex2f(x + w + 4, y + h65);
    glVertex2f(x + w + 4, y + h45);
    glVertex2f(x + w, y + h45);
    glEnd();

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x + w, y + h3);
    glVertex2f(x + w + 8, y + h35);
    glVertex2f(x + w + 8, y + h75);
    glVertex2f(x + w, y + h8);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(x + w, y + h45);
    glVertex2f(x + w + 4, y + h45);
    glVertex2f(x + w + 4, y + h65);
    glVertex2f(x + w, y + h65);
    glEnd();
}

void drawBowl(float x, float y, float w, float h, Color fill) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    float cx = x + w * 0.5f, cy = y + h * 0.7f;
    float rw = w * 0.5f, rh = h * 0.5f;

    setColor(fill);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, y + h * 0.5f);
    for (int i = 0; i <= 12; i++) {
        float a = 3.14159f * (1.0f + (float)i / 12.0f);
        glVertex2f(cx + cosf(a) * rw, cy + sinf(a) * rh);
    }
    glEnd();

    drawFilledRect(x, cy, w, h * 0.3f, fill, edge, 0);

    setColor(edge);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; i++) {
        float a = 3.14159f * (1.0f + (float)i / 12.0f);
        glVertex2f(cx + cosf(a) * rw, cy + sinf(a) * rh);
    }
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(x, cy);
    glVertex2f(x, y + h);
    glVertex2f(x + w, cy);
    glVertex2f(x + w, y + h);
    glEnd();
}

void drawShelvesWithItems(float x, float y) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color woodTop = { 0.79f, 0.58f, 0.36f };

    float shelfY1 = y + 80;
    drawFilledRect(x, shelfY1, 280, 10, woodTop, edge, 2.0f);

    drawJar(x + 20, shelfY1 + 10, 30, 35, { 0.85f, 0.8f, 0.7f });
    drawJar(x + 60, shelfY1 + 10, 25, 30, { 0.75f, 0.85f, 0.8f });
    drawJar(x + 95, shelfY1 + 10, 35, 40, { 0.9f, 0.85f, 0.75f });
    drawJar(x + 145, shelfY1 + 10, 28, 32, { 0.7f, 0.8f, 0.75f });
    drawJar(x + 185, shelfY1 + 10, 32, 38, { 0.85f, 0.82f, 0.7f });
    drawJar(x + 230, shelfY1 + 10, 28, 34, { 0.9f, 0.88f, 0.75f });

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

    for (int i = 0; i < 4; i++) {
        float ly = y + 70 - i * 18;
        glBegin(GL_LINES);
        glVertex2f(x + 15, ly);
        glVertex2f(x + 60, ly);
        glEnd();
        drawCircleFilled(x + 75, ly, 3);
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

void drawTable(float x, float y, float w, float h) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color wood = { 0.7f, 0.55f, 0.35f };
    Color woodDark = { 0.6f, 0.45f, 0.25f };

    drawFilledRect(x, y + h - 10, w, 10, wood, edge, 2.0f);
    drawFilledRect(x, y + h - 12, w, 2, woodDark, woodDark, 0);

    float legWidth = 8, legInset = 10;
    drawFilledRect(x + legInset, y, legWidth, h - 10, woodDark, edge, 1.5f);
    drawFilledRect(x + w - legInset - legWidth, y, legWidth, h - 10, woodDark, edge, 1.5f);
}

void drawStool(float x, float y, float w, float h) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color wood = { 0.7f, 0.55f, 0.35f };
    Color woodDark = { 0.6f, 0.45f, 0.25f };

    drawFilledRect(x, y + h - 8, w, 8, wood, edge, 2.0f);
    drawFilledRect(x + 2, y + h - 10, w - 4, 2, woodDark, woodDark, 0);

    float legWidth = 5, legInset = 6;
    drawFilledRect(x + legInset, y, legWidth, h - 8, woodDark, edge, 1.2f);
    drawFilledRect(x + w - legInset - legWidth, y, legWidth, h - 8, woodDark, edge, 1.2f);

    glLineWidth(2.0f);
    setColor(woodDark);
    float h3 = y + h * 0.333f;
    glBegin(GL_LINES);
    glVertex2f(x + legInset + legWidth * 0.5f, h3);
    glVertex2f(x + w - legInset - legWidth * 0.5f, h3);
    glEnd();
}

void drawPendantLamp(float x, float y) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    Color shade = { 0.95f, 0.85f, 0.7f };

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

    glColor4f(1.0f, 0.95f, 0.7f, 0.3f);
    drawCircleFilled(x, y - 35, 20);
}

void drawWallClock(float x, float y, float radius) {
    Color edge = { 0.2f, 0.2f, 0.2f };
    Color face = { 0.98f, 0.98f, 0.95f };

    setColor(face);
    drawCircleFilled(x, y, radius);

    setColor(edge);
    glLineWidth(2.0f);
    for (int i = 0; i < 12; i++) {
        float a = (float)i * 0.52359f; // 2*PI/12 precomputed
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + cosf(a) * radius, y + sinf(a) * radius);
        glEnd();
    }

    setColor({ 0.3f, 0.3f, 0.3f });
    float rm8 = radius - 8;
    for (int i = 0; i < 12; i++) {
        float a = (float)i * 0.52359f - 1.5708f; // -PI/2 offset
        drawCircleFilled(x + cosf(a) * rm8, y + sinf(a) * rm8, 3);
    }

    glLineWidth(3.0f);
    setColor({ 0.2f, 0.2f, 0.2f });
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + radius * 0.5f, y);
    glVertex2f(x, y);
    glVertex2f(x, y + radius * 0.7f);
    glEnd();

    setColor({ 0.8f, 0.3f, 0.3f });
    drawCircleFilled(x, y, 5);
}

void drawChalkboardMenu(float x, float y, float w, float h) {
    Color edge = { 0.4f, 0.3f, 0.2f };
    Color board = { 0.15f, 0.15f, 0.15f };
    Color chalk = { 0.9f, 0.9f, 0.85f };

    drawFilledRect(x - 8, y - 8, w + 16, h + 16, { 0.5f, 0.4f, 0.3f }, edge, 2.5f);
    drawFilledRect(x, y, w, h, board, board, 0);

    setColor(chalk);
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    glVertex2f(x + 20, y + h - 25);
    glVertex2f(x + w - 20, y + h - 25);
    glEnd();

    float itemY = y + h - 55;
    float w6 = w * 0.6f;
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 15, itemY);
        glVertex2f(x + w6, itemY);
        glEnd();

        drawCircleFilled(x + w - 25, itemY, 2);
        drawCircleFilled(x + w - 20, itemY, 2);
        drawCircleFilled(x + w - 15, itemY, 2);

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

void drawBookshelf(float x, float y) {
    Color wood = { 0.55f, 0.35f, 0.20f };
    Color woodLight = { 0.65f, 0.45f, 0.30f };
    Color edge = { 0.06f, 0.03f, 0.01f };

    drawFilledRect(x, y, 200, 300, wood, edge, 2.5f);

    for (int i = 0; i < 4; i++) {
        float shelfY = y + 20 + i * 70;
        drawFilledRect(x + 5, shelfY - 2, 190, 3, { 0.35f, 0.25f, 0.15f }, { 0.35f, 0.25f, 0.15f }, 0);
        drawFilledRect(x + 5, shelfY, 190, 10, woodLight, edge, 1.5f);
    }

    drawFilledRect(x, y, 8, 300, { 0.5f, 0.3f, 0.15f }, edge, 1.5f);
    drawFilledRect(x + 192, y, 8, 300, { 0.5f, 0.3f, 0.15f }, edge, 1.5f);

    static float bookWidths[] = { 18, 24, 20, 26, 22, 28, 25, 19, 23, 27, 21, 29 };
    static Color bookColors[] = {
        {0.8f, 0.2f, 0.2f}, {0.2f, 0.5f, 0.8f}, {0.3f, 0.7f, 0.3f},
        {0.9f, 0.7f, 0.2f}, {0.6f, 0.3f, 0.7f}, {0.9f, 0.5f, 0.3f},
        {0.7f, 0.2f, 0.5f}, {0.2f, 0.6f, 0.6f}, {0.8f, 0.5f, 0.2f},
        {0.4f, 0.4f, 0.8f}, {0.7f, 0.6f, 0.3f}, {0.5f, 0.2f, 0.4f}
    };

    for (int shelf = 0; shelf < 4; shelf++) {
        float bx = x + 12;
        int baseIdx = shelf * 6;
        for (int book = 0; book < 6; book++) {
            int idx = (baseIdx + book) % 12;
            Color bc = bookColors[idx];
            Color bcDark = { bc.r * 0.7f, bc.g * 0.7f, bc.b * 0.7f };
            Color bcLight = { min(1.0f, bc.r * 1.3f), min(1.0f, bc.g * 1.3f), min(1.0f, bc.b * 1.3f) };

            float bw = bookWidths[idx];
            float by = y + 30 + shelf * 70;
            float bookHeight = 58;

            drawFilledRect(bx, by, bw, bookHeight, bc, edge, 1.5f);
            drawFilledRect(bx + 2, by + 5, 3, bookHeight - 10, bcLight, bcLight, 0);

            setColor(bcDark);
            glLineWidth(1.5f);
            float bhw = bw * 0.5f;
            glBegin(GL_LINES);
            glVertex2f(bx + bhw - 4, by + 20);
            glVertex2f(bx + bhw + 4, by + 20);
            glVertex2f(bx + bhw - 3, by + 25);
            glVertex2f(bx + bhw + 3, by + 25);
            glEnd();

            bx += bw + 2;
        }
    }
}

void drawEntryArea(float x, float y) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color doorWood = { 0.55f, 0.35f, 0.20f };
    Color doorDark = { 0.45f, 0.25f, 0.10f };

    drawFilledRect(x, y, 160, 340, doorWood, edge, 3.0f);

    drawFilledRect(x + 10, y + 10, 140, 150, doorDark, edge, 2.0f);
    drawFilledRect(x + 10, y + 170, 140, 150, doorDark, edge, 2.0f);

    setColor({ 0.8f, 0.7f, 0.3f });
    drawCircleFilled(x + 130, y + 170, 8);
    drawFilledRect(x + 125, y + 165, 20, 10, { 0.8f, 0.7f, 0.3f }, edge, 1.0f);

    drawFilledRect(x + 10, y - 55, 140, 45, { 0.7f, 0.3f, 0.3f }, edge, 2.0f);

    float boardX = x + 200, boardY = y + 160;
    drawFilledRect(boardX, boardY, 200, 220, { 0.2f, 0.2f, 0.2f }, { 0.5f, 0.35f, 0.2f }, 4.0f);

    setColor({ 0.9f, 0.9f, 0.9f });
    for (int line = 0; line < 5; line++) {
        float ly = boardY + 170 - line * 35;
        drawFilledRect(boardX + 20, ly, 70, 5, { 0.9f, 0.9f, 0.9f }, { 0.9f, 0.9f, 0.9f }, 0.5f);
        drawFilledRect(boardX + 110, ly, 50, 5, { 0.9f, 0.9f, 0.9f }, { 0.9f, 0.9f, 0.9f }, 0.5f);
    }
    drawFilledRect(boardX + 45, boardY + 190, 110, 10, { 0.9f, 0.9f, 0.9f }, { 0.9f, 0.9f, 0.9f }, 0.5f);
}

void drawCouchArea(float x, float y) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color couchColor = { 0.45f, 0.35f, 0.55f };
    Color cushion = { 0.55f, 0.45f, 0.65f };

    drawFilledRect(x, y, 220, 60, couchColor, edge, 2.0f);
    drawFilledRect(x, y + 50, 220, 80, couchColor, edge, 2.0f);
    drawFilledRect(x + 15, y + 55, 60, 60, cushion, edge, 1.5f);
    drawFilledRect(x + 85, y + 55, 60, 60, cushion, edge, 1.5f);
    drawFilledRect(x + 155, y + 55, 50, 60, cushion, edge, 1.5f);

    float tableX = x + 250;
    drawFilledRect(tableX, y - 20, 120, 8, { 0.6f, 0.4f, 0.2f }, edge, 2.0f);
    drawFilledRect(tableX + 10, y - 55, 12, 35, { 0.5f, 0.3f, 0.15f }, edge, 1.5f);
    drawFilledRect(tableX + 98, y - 55, 12, 35, { 0.5f, 0.3f, 0.15f }, edge, 1.5f);
}

void drawScenePixels() {
    Color wall = { 1.0f, 0.96f, 0.86f };
    Color ceiling = { 1.0f, 0.98f, 0.92f };
    Color edge = { 0.06f, 0.03f, 0.01f };
    
    setColor(wall); // Wall background - single draw
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(SCENE_WIDTH, 0);
    glVertex2f(SCENE_WIDTH, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    drawFilledRect(0, windowHeight - 24, SCENE_WIDTH, 24, ceiling, edge, 2.0f); // Ceiling crown molding

    glLineWidth(2.0f); // Horizontal line decoration
    setColor(edge);
    float lineY = windowHeight * 0.32f;
    glBegin(GL_LINES);
    glVertex2f(0, lineY);
    glVertex2f(SCENE_WIDTH, lineY);
    glEnd();
    
    float baseY = windowHeight * 0.18f;
    float midY = windowHeight * 0.52f;
    float upperY = windowHeight * 0.58f;

    drawEntryArea(30, baseY);
    drawBookshelf(680, windowHeight * 0.45f);
    drawPlantPixel(660, baseY, (plantColorIndex + 2) % numPlantColors);
    
    drawWindowPixel(250, midY, 220, 260, curtainOpenAmount); // Window
    
    float tableX = 280; // Table and stools
    drawTable(tableX, baseY, 100, 90);
    drawStool(tableX - 50, baseY, 40, 60);
    drawStool(tableX + 110, baseY, 40, 60);
    
    float counterX = 1150; // Counter setup
    float counterW = windowWidth * 0.52f;
    float counterH = windowHeight * 0.22f;
    float counterTop = baseY + counterH;

    drawCountertopStyled(counterX - 8, counterTop, counterW + 16, 22);
    drawCounterStyled(counterX, baseY, counterW, counterH);
    
    drawCoffeeMachineStyled(counterX + 70, counterTop + 6, 1.0f); // Counter items at fixed offsets
    drawBrownCoffeeMakerStyled(counterX + 220, counterTop + 6, 1.0f);
    drawMug(counterX + 320, counterTop + 8, { 0.75f, 0.9f, 0.8f });
    drawMug(counterX + 360, counterTop + 8, { 0.9f, 0.75f, 0.6f });
    drawCashRegisterPixel(counterX + counterW - 120, counterTop + 6, 1.0f);
    
    drawShelvesWithItems(counterX + 380, upperY); // Wall decorations and signs
    drawCoffeeMenuSign(counterX + 710, windowHeight * 0.60f);
    drawOrderHereSign(counterX + 830, midY);
    
    float boardX = 1950; // Board with sticky notes
    float boardY = windowHeight * 0.56f;
    drawFilledRect(boardX, boardY + 10, 160, 180, { 0.77f, 0.56f, 0.33f }, edge, 2.0f);
    drawFilledRect(boardX + 20, boardY + 130, 36, 36, { 0.96f, 0.89f, 0.49f }, edge);
    drawFilledRect(boardX + 70, boardY + 140, 36, 36, { 0.45f, 0.7f, 0.56f }, edge);
    drawFilledRect(boardX + 20, boardY + 100, 46, 46, { 0.96f, 0.47f, 0.47f }, edge);
    drawFilledRect(boardX + 80, boardY + 80, 46, 46, { 0.68f, 0.86f, 0.96f }, edge);
    
    drawPendantLamp(counterX + 200, windowHeight - 30); // Pendant lamps
    drawPendantLamp(counterX + 450, windowHeight - 30);
    
    drawWallClock(900, windowHeight * 0.75f, 35);  // Wall clock and menu board
    drawChalkboardMenu(500, windowHeight * 0.55f, 140, 180);
    
    drawPlantPixel(950, baseY, plantColorIndex); // Plants
    drawPlantPixel(200, baseY, (plantColorIndex + 1) % numPlantColors);
    
    drawFilledRect(1850, 12.0f, 200, 60, { 0.99f, 0.75f, 0.79f }, edge, 2.0f); // Shelf decoration
    drawFilledRect(2020, 18.0f, 40, 18, { 0.64f, 0.73f, 0.56f }, edge, 1.2f);
    
    drawCatPixel(counterX - 80, baseY - 24, 0.9f, catColorIndex); // Cats
    drawSleepingCatPixel(counterX + 200, baseY - 60, 1.0f);
    
    float couchX = SCENE_WIDTH - 650; // Couch area
    drawCouchArea(couchX, baseY);
    drawSleepingCatPixel(SCENE_WIDTH - 500, 60, 1.2f);
    
    float paintX = SCENE_WIDTH - 290; // Painting frame
    float paintY = windowHeight * 0.49f;
    drawFilledRect(paintX, paintY, 200, 240, { 0.3f, 0.2f, 0.1f }, edge, 3.0f);
    drawFilledRect(paintX + 10, paintY + 10, 180, 220, { 0.95f, 0.95f, 0.9f }, edge, 2.0f);
    
    setColor({ 0.8f, 0.3f, 0.4f }); // Painting circles
    drawCircleFilled(paintX + 100, windowHeight * 0.62f, 40);
    setColor({ 0.3f, 0.6f, 0.8f });
    drawCircleFilled(paintX + 80, windowHeight * 0.58f, 30);
    setColor({ 0.9f, 0.7f, 0.2f });
    drawCircleFilled(paintX + 120, windowHeight * 0.6f, 25);
}

struct Smoke { float x, y, life, offsetX; };
vector<Smoke> smokeParticles;
float smokeTimer = 0.0f;

void spawnSmoke(float x, float y) {
    Smoke s;
    s.x = x;
    s.y = y;
    s.life = 1.0f;
    s.offsetX = (rand() % 100 - 50) * 0.005f;
    smokeParticles.push_back(s);
}

void updateSmoke(float dt) {
    smokeTimer += dt;
    if (smokeTimer >= 0.25f) {
        float mugX = 1150 + 320; // Mug position calculation -- counterX + offset
        float mugY = windowHeight * 0.18f + windowHeight * 0.22f + 8;
        spawnSmoke(mugX + 12, mugY + 24);
        smokeTimer = 0.0f;
    }
    
    for (int i = (int)smokeParticles.size() - 1; i >= 0; --i) {
        Smoke& s = smokeParticles[i];
        s.life -= dt * 0.4f;
        s.y += dt * 40.0f;
        s.x += s.offsetX * dt * 60.0f;
        if (s.life <= 0)
            smokeParticles.erase(smokeParticles.begin() + i);
    }
}

void drawSmokePixels() {
    for (auto& s : smokeParticles) {
        float a = s.life * 0.6f;
        glColor4f(0.9f, 0.9f, 0.9f, a);
        drawCircleFilled(s.x, s.y, 6.0f + (1.0f - s.life) * 10.0f);
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
    drawSmokePixels();
    glPopMatrix();

    glutSwapBuffers();
}

void onResize(int w, int h) {
    windowWidth = max(200, w);
    windowHeight = max(120, h);

    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
    updateSmoke(0.016f);
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON)
            catColorIndex = (catColorIndex + 1) % numCatColors;
        else if (button == GLUT_RIGHT_BUTTON)
            plantColorIndex = (plantColorIndex + 1) % numPlantColors;
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
