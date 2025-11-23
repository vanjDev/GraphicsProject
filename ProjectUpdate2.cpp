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

struct Color { GLfloat r, g, b; };

const float PI = 3.14159f;
const float TWO_PI = 6.28318f;

inline void setColor(const Color& c) {
    glColor3f(c.r, c.g, c.b);
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

void drawCatPixel(float centerX, float centerY, float scale, int colorIndex, float rotationAngle = 0.0f) {
    Color body = { catColors[colorIndex][0], catColors[colorIndex][1], catColors[colorIndex][2] };
    Color darker = darken(body);
    Color lighter = lighten(body);

    // Apply rotation around the center point of the cat
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-centerX, -centerY, 0.0f);

    setColor(body);
    drawEllipse(centerX, centerY, 32 * scale, 28 * scale);
    setColor(lighter);
    drawEllipse(centerX - 5 * scale, centerY - 8 * scale, 18 * scale, 20 * scale);
    setColor(body);
    drawCircleFilled(centerX - 22 * scale, centerY + 22 * scale, 20 * scale);
    setColor(lighter);
    drawEllipse(centerX - 22 * scale, centerY + 18 * scale, 14 * scale, 12 * scale);

    setColor(body);
    drawTriangleFilled(centerX - 36 * scale, centerY + 34 * scale, centerX - 28 * scale, centerY + 50 * scale, centerX - 20 * scale, centerY + 36 * scale);
    drawTriangleFilled(centerX - 24 * scale, centerY + 34 * scale, centerX - 16 * scale, centerY + 50 * scale, centerX - 8 * scale, centerY + 36 * scale);

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

    glPopMatrix(); // Restore matrix state after rotation
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

void drawPlantPixel(float centerX, float centerY, int colorIndex) {
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

    glLineWidth(3.0f);
    setColor({ 0.2f, 0.5f, 0.2f });
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY); glVertex2f(centerX, centerY + 45);
    glVertex2f(centerX, centerY + 20); glVertex2f(centerX - 35, centerY + 25);
    glVertex2f(centerX, centerY + 20); glVertex2f(centerX + 35, centerY + 25);
    glEnd();

    struct Leaf { float x, y, angle, size; };
    Leaf leaves[] = { {centerX, centerY + 45, -15, 1.2f}, {centerX - 40, centerY + 20, 10, 1.0f}, {centerX + 40, centerY + 20, 20, 1.0f} };

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

void drawPendantLamp(float x, float y) { // wtf is pendant lamp
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

void drawChalkboardMenu(float x, float y, float width, float height) { // Small chalkboard menu with lines and dots
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

void drawCouchArea(float x, float y, float scale = 1.0f) {
    Color edge = { 0.06f, 0.03f, 0.01f };
    Color couchColor = { 0.92f, 0.89f, 0.85f };
    Color cushion = { 0.95f, 0.92f, 0.88f };
    Color armrestColor = { 0.90f, 0.87f, 0.83f };

    // Uniform couch scaling
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
    drawFilledRect(x, y, 120, 8, { 0.6f, 0.4f, 0.2f }, edge, 2.0f); // Tabletop
    drawFilledRect(x + 10, y - 35, 12, 35, { 0.5f, 0.3f, 0.15f }, edge, 1.5f); // Left leg
    drawFilledRect(x + 98, y - 35, 12, 35, { 0.5f, 0.3f, 0.15f }, edge, 1.5f); // Right leg
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

    setColor(wall); // Wall background
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

    drawPlantPixel(660, baseY, (plantColorIndex + 2) % numPlantColors); // Middle plant

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

    drawEntryArea(400, baseY); // Entry door
    drawWindowPixel(700, midY, 220, 260, curtainOpenAmount); // Window

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
    drawCashRegisterPixel(counterX + counterWidth - 120, counterTop + 6, 1.0f); // Cash register

    drawShelvesWithItems(counterX + 380, upperY); // Wall decorations and signs
    drawCoffeeMenuSign(counterX + 730, windowHeight * 0.60f);
    drawOrderHereSign(counterX + 740, midY);

    // Board with sticky notes
    float boardX = 2025; // Move board left or right
    float boardY = windowHeight * 0.56f; // Move board up or down
    drawFilledRect(boardX, boardY + 10, 160, 180, { 0.77f, 0.56f, 0.33f }, edge, 2.0f); // Main board
    drawFilledRect(boardX + 20, boardY + 130, 36, 36, { 0.96f, 0.89f, 0.49f }, edge); // Sticky note - yellow
    drawFilledRect(boardX + 70, boardY + 140, 36, 36, { 0.45f, 0.7f, 0.56f }, edge); // Sticky note - green
    drawFilledRect(boardX + 20, boardY + 100, 46, 46, { 0.96f, 0.47f, 0.47f }, edge); // Sticky note - red
    drawFilledRect(boardX + 80, boardY + 80, 46, 46, { 0.68f, 0.86f, 0.96f }, edge); // Sticky note - blue

    drawPendantLamp(counterX + 200, windowHeight - 30); // Pendant lamps
    drawPendantLamp(counterX + 450, windowHeight - 30);

    drawChalkboardMenu(1000, windowHeight * 0.55f, 140, 180); // move on X-axis, move on Y-axis, width, height

    drawPlantPixel(950, baseY, plantColorIndex); // Right plant

    drawFilledRect(1850, 12.0f, 200, 60, { 0.99f, 0.75f, 0.79f }, edge, 2.0f); // Shelf decoration
    drawFilledRect(2020, 18.0f, 40, 18, { 0.64f, 0.73f, 0.56f }, edge, 1.2f);

    drawCatPixel(counterX - 80, baseY - 24, 0.9f, catColorIndex, catRotationAngle); // Cats
    drawSleepingCatPixel(counterX + 200, baseY - 60, 1.0f);

    float couchX = SCENE_WIDTH - 550; // Move couch left or right
    float couchY = baseY + 115; // Move couch up or down
    drawCouchArea(couchX, couchY, couchScale);

    float couchWidth = 250; // Main couch width
    float tableWidth = 120;
    float tableX = couchX + (couchWidth - tableWidth) / 2.0f; // Center coffee table horizontally under couch
    float tableY = couchY - 80; // Position below the couch
    drawCoffeeTable(tableX, tableY);

    drawSleepingCatPixel(SCENE_WIDTH - 500, 60, 1.2f);
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

void updateSmoke(float deltaTime) {
    smokeTimer += deltaTime;
    if (smokeTimer >= 0.25f) {
        float mugX = 1150 + 320; // Mug position
        float mugY = windowHeight * 0.18f + windowHeight * 0.22f + 8;
        spawnSmoke(mugX + 12, mugY + 24);
        smokeTimer = 0.0f;
    }

    for (int particleIndex = (int)smokeParticles.size() - 1; particleIndex >= 0; --particleIndex) {
        Smoke& particle = smokeParticles[particleIndex];
        particle.life -= deltaTime * 0.4f;
        particle.y += deltaTime * 40.0f;
        particle.x += particle.offsetX * deltaTime * 60.0f;
        if (particle.life <= 0)
            smokeParticles.erase(smokeParticles.begin() + particleIndex);
    }
}

void drawSmokePixels() {
    for (auto& particle : smokeParticles) {
        float alpha = particle.life * 0.6f;
        glColor4f(0.9f, 0.9f, 0.9f, alpha);
        drawCircleFilled(particle.x, particle.y, 6.0f + (1.0f - particle.life) * 10.0f);
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

void onResize(int width, int height) {
    windowWidth = max(200, width);
    windowHeight = max(120, height);

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
        if (button == GLUT_LEFT_BUTTON) {
            catRotationAngle += 15.0f; // Rotate cat by 15 degrees
            catColorIndex = (catColorIndex + 1) % numCatColors; // Change color every time LMB is clicked
            glutPostRedisplay();
        }
        else if (button == GLUT_RIGHT_BUTTON)
            plantColorIndex = (plantColorIndex + 1) % numPlantColors;
        else if (button == 3) { // Scroll wheel up
            couchScale = min(2.0f, couchScale + 0.1f); // Increase scale, max scale is 2.0
            glutPostRedisplay();
        }
        else if (button == 4) { // Scroll wheel down
            couchScale = max(0.5f, couchScale - 0.1f); // Decrease scale, min scale is 0.5
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
