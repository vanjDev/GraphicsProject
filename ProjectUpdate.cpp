#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <cstdlib>

const int WIN_W = 1365;
const int WIN_H = 768;
const int SCENE_WIDTH = 2800; // Extended scene width for panning
const int INITIAL_OFFSET = 500; // Start offset to show left content initially

int windowWidth = WIN_W;
int windowHeight = WIN_H;

// Camera for panning - fixed to work properly
float cameraX = INITIAL_OFFSET; // Start with offset to show left content
const float CAMERA_STEP = 50.0f;

// Cat colors
int catColorIndex = 1;
float catColors[][3] = {
    {0.65f, 0.50f, 0.35f}, // Brown
    {0.96f, 0.80f, 0.56f}, // Orange
    {0.3f, 0.3f, 0.3f},    // Gray
    {1.0f, 1.0f, 1.0f},    // White
    {0.2f, 0.2f, 0.2f}     // Black
};
int numCatColors = 5;

// Plant colors
int plantColorIndex = 0;
float plantColors[][3] = {
    {0.2f,0.6f,0.3f}, {0.6f,0.2f,0.6f}, {0.6f,0.4f,0.2f}, {0.2f,0.4f,0.6f}, {0.8f,0.2f,0.2f}
};
int numPlantColors = 5;

float curtainOpenAmount = 0.0f;

struct Color { GLfloat r, g, b; };
inline void setColor(const Color& c) { glColor3f(c.r, c.g, c.b); }

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

void drawCircleFilled(float cx, float cy, float r, int segs = 36) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; i++) {
        float a = (float)i / segs * 3.14159f * 2.0f;
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

void drawEllipse(float cx, float cy, float rx, float ry, int segs = 36) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; i++) {
        float a = (float)i / segs * 3.14159f * 2.0f;
        glVertex2f(cx + cosf(a) * rx, cy + sinf(a) * ry);
    }
    glEnd();
}

void drawSteamSquares(float cx, float cy, float size, int count) {
    Color white = { 1.0f,1.0f,1.0f };
    Color outline = { 0.98f,0.98f,0.98f };
    for (int i = 0; i < count; i++) drawFilledRect(cx - size / 2, cy + i * (size + 4), size, size, white, outline, 1.0f);
}

// Improved realistic cat
void drawCatPixel(float cx, float cy, float scale, int colorIndex) {
    Color body = { catColors[colorIndex][0], catColors[colorIndex][1], catColors[colorIndex][2] };
    Color darker = { body.r * 0.7f, body.g * 0.7f, body.b * 0.7f };
    Color lighter = { std::min(1.0f, body.r * 1.2f), std::min(1.0f, body.g * 1.2f), std::min(1.0f, body.b * 1.2f) };
    
    // Body - more realistic oval shape
    setColor(body);
    drawEllipse(cx, cy, 32.0f * scale, 28.0f * scale);
    
    // Chest highlight
    setColor(lighter);
    drawEllipse(cx - 5 * scale, cy - 8 * scale, 18.0f * scale, 20.0f * scale);
    
    // Head - more rounded
    setColor(body);
    drawCircleFilled(cx - 22.0f * scale, cy + 22.0f * scale, 20.0f * scale);
    
    // Muzzle/face lighter part
    setColor(lighter);
    drawEllipse(cx - 22.0f * scale, cy + 18.0f * scale, 14.0f * scale, 12.0f * scale);
    
    // Ears - more realistic triangular shape
    setColor(body);
    drawTriangleFilled(cx - 36 * scale, cy + 34 * scale, cx - 28 * scale, cy + 50 * scale, cx - 20 * scale, cy + 36 * scale);
    drawTriangleFilled(cx - 24 * scale, cy + 34 * scale, cx - 16 * scale, cy + 50 * scale, cx - 8 * scale, cy + 36 * scale);
    
    // Inner ears (pink)
    setColor({0.95f, 0.7f, 0.7f});
    drawTriangleFilled(cx - 33 * scale, cy + 36 * scale, cx - 28 * scale, cy + 45 * scale, cx - 23 * scale, cy + 37 * scale);
    drawTriangleFilled(cx - 21 * scale, cy + 36 * scale, cx - 16 * scale, cy + 45 * scale, cx - 11 * scale, cy + 37 * scale);
    
    // Eyes - more detailed
    setColor({1.0f, 1.0f, 0.9f});
    drawEllipse(cx - 28 * scale, cy + 24 * scale, 4 * scale, 5 * scale);
    drawEllipse(cx - 16 * scale, cy + 24 * scale, 4 * scale, 5 * scale);
    
    // Pupils
    setColor({0.1f, 0.1f, 0.1f});
    drawEllipse(cx - 28 * scale, cy + 24 * scale, 2 * scale, 4 * scale);
    drawEllipse(cx - 16 * scale, cy + 24 * scale, 2 * scale, 4 * scale);
    
    // Nose
    setColor({0.9f, 0.6f, 0.6f});
    drawTriangleFilled(cx - 22 * scale, cy + 18 * scale, cx - 20 * scale, cy + 14 * scale, cx - 24 * scale, cy + 14 * scale);
    
    // Whiskers
    glLineWidth(1.0f);
    setColor({0.3f, 0.3f, 0.3f});
    glBegin(GL_LINES);
    // Left whiskers
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 40 * scale, cy + 18 * scale);
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 40 * scale, cy + 14 * scale);
    // Right whiskers
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 4 * scale, cy + 18 * scale);
    glVertex2f(cx - 22 * scale, cy + 16 * scale); glVertex2f(cx - 4 * scale, cy + 14 * scale);
    glEnd();
    
    // Paws
    setColor(body);
    drawEllipse(cx - 12 * scale, cy - 22 * scale, 8 * scale, 6 * scale);
    drawEllipse(cx + 8 * scale, cy - 22 * scale, 8 * scale, 6 * scale);
    
    // Tail - curved and fluffy
    setColor(body);
    drawCircleFilled(cx + 24 * scale, cy - 4 * scale, 10 * scale);
    drawCircleFilled(cx + 32 * scale, cy - 12 * scale, 9 * scale);
    drawCircleFilled(cx + 38 * scale, cy - 20 * scale, 8 * scale);
    
    // Tail tip (darker)
    setColor(darker);
    drawCircleFilled(cx + 42 * scale, cy - 26 * scale, 6 * scale);
}

// Improved sleeping cat
void drawSleepingCatPixel(float x, float y, float scale) {
    Color body = {0.96f, 0.80f, 0.56f};
    Color darker = {0.86f, 0.70f, 0.46f};
    Color lighter = {1.0f, 0.9f, 0.7f};
    
    // Body
    setColor(body);
    drawEllipse(x, y, 20 * scale, 16 * scale);
    
    // Head
    drawCircleFilled(x + 14 * scale, y + 2 * scale, 14 * scale);
    
    // Muzzle area
    setColor(lighter);
    drawEllipse(x + 24 * scale, y + 4 * scale, 12 * scale, 8 * scale);
    
    // Ears
    setColor(darker);
    drawTriangleFilled(x + 18 * scale, y + 12 * scale, x + 22 * scale, y + 26 * scale, x + 26 * scale, y + 12 * scale);
    drawTriangleFilled(x + 26 * scale, y + 12 * scale, x + 30 * scale, y + 26 * scale, x + 34 * scale, y + 12 * scale);
    
    // Closed eyes
    glLineWidth(2.0f);
    setColor({0.2f, 0.2f, 0.2f});
    glBegin(GL_LINES);
    glVertex2f(x + 18 * scale, y + 8 * scale); glVertex2f(x + 22 * scale, y + 8 * scale);
    glVertex2f(x + 26 * scale, y + 8 * scale); glVertex2f(x + 30 * scale, y + 8 * scale);
    glEnd();
    
    // Tail curled
    setColor(body);
    drawCircleFilled(x - 12 * scale, y + 8 * scale, 6 * scale);
    drawCircleFilled(x - 16 * scale, y + 14 * scale, 5 * scale);
}

// Improved realistic plant
void drawPlantPixel(float cx, float cy, int colorIndex) {
    Color edge = { 0.06f,0.03f,0.01f };
    Color potColor = { 0.79f,0.37f,0.18f };
    Color potHighlight = { 0.89f,0.47f,0.28f };
    
    // Terracotta pot with gradient effect
    drawFilledRect(cx - 40, cy - 40, 80, 40, potColor, edge, 2.0f);
    // Pot rim
    drawFilledRect(cx - 42, cy - 2, 84, 8, potHighlight, edge, 1.5f);
    // Pot highlight
    setColor(potHighlight);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx - 30, cy - 35);
    glVertex2f(cx - 20, cy - 35);
    glVertex2f(cx - 25, cy - 10);
    glEnd();
    
    // Soil
    drawFilledRect(cx - 36, cy - 6, 72, 8, {0.4f, 0.3f, 0.2f}, edge, 1.0f);
    
    Color leafColor = { plantColors[colorIndex][0], plantColors[colorIndex][1], plantColors[colorIndex][2] };
    Color leafDark = { leafColor.r * 0.7f, leafColor.g * 0.7f, leafColor.b * 0.7f };
    Color leafLight = { std::min(1.0f, leafColor.r * 1.3f), std::min(1.0f, leafColor.g * 1.3f), std::min(1.0f, leafColor.b * 1.3f) };
    
    // Stems
    glLineWidth(3.0f);
    setColor({0.2f, 0.5f, 0.2f});
    glBegin(GL_LINES);
    glVertex2f(cx, cy); glVertex2f(cx, cy + 45);
    glVertex2f(cx, cy + 20); glVertex2f(cx - 35, cy + 25);
    glVertex2f(cx, cy + 20); glVertex2f(cx + 35, cy + 25);
    glEnd();
    
    // Draw realistic monstera-style leaves
    struct Leaf { float x, y, angle, size; };
    Leaf leaves[] = {
        {cx, cy + 45, -15, 1.2f},
        {cx - 40, cy + 20, 10, 1.0f},
        {cx + 40, cy + 20, 20, 1.0f}
    };
    
    for (auto& leaf : leaves) {
        glPushMatrix();
        glTranslatef(leaf.x, leaf.y, 0);
        glRotatef(leaf.angle, 0, 0, 1);
        
        // Main leaf shape (monstera leaf)
        setColor(leafColor);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = 0; i <= 20; i++) {
            float a = (float)i / 20.0f * 3.14159f * 2.0f;
            float r = 28 * leaf.size;
            // Create leaf indentations
            if (i % 4 == 0) r *= 0.7f;
            glVertex2f(cosf(a) * r, sinf(a) * r);
        }
        glEnd();
        
        // Leaf veins
        glLineWidth(1.5f);
        setColor(leafDark);
        glBegin(GL_LINES);
        glVertex2f(0, 0); glVertex2f(0, 25 * leaf.size);
        glVertex2f(0, 15 * leaf.size); glVertex2f(-15 * leaf.size, 20 * leaf.size);
        glVertex2f(0, 15 * leaf.size); glVertex2f(15 * leaf.size, 20 * leaf.size);
        glVertex2f(0, 5 * leaf.size); glVertex2f(-18 * leaf.size, 8 * leaf.size);
        glVertex2f(0, 5 * leaf.size); glVertex2f(18 * leaf.size, 8 * leaf.size);
        glEnd();
        
        // Leaf highlights
        setColor(leafLight);
        drawEllipse(-8 * leaf.size, 12 * leaf.size, 6 * leaf.size, 8 * leaf.size);
        
        glPopMatrix();
    }
}

void drawWindowPixel(float x, float y, float w, float h, float openAmount) {
    Color frame = { 0.73f,0.56f,0.36f };
    Color glass = { 0.68f,0.85f,0.90f };
    
    drawFilledRect(x, y, w, h, frame, { 0.06f,0.03f,0.01f }, 2.0f);
    drawFilledRect(x + 8, y + 8, w - 16, h - 16, glass, { 0.06f,0.03f,0.01f }, 1.2f);
    
    // Window panes
    glLineWidth(3.0f);
    setColor({0.73f,0.56f,0.36f});
    glBegin(GL_LINES);
    glVertex2f(x + w/2, y + 8); glVertex2f(x + w/2, y + h - 8);
    glVertex2f(x + 8, y + h/2); glVertex2f(x + w - 8, y + h/2);
    glEnd();
    
    drawFilledRect(x - 10, y + h - 12, w + 20, 6, frame, { 0.06f,0.03f,0.01f }, 1.5f);
    
    for (float cx = x + 12; cx <= x + w - 12; cx += 48) drawCircleFilled(cx, y + h - 9, 6);
    
    float offset = openAmount * (w * 0.35f);
    drawFilledRect(x + 10 - offset, y + 10, (w / 2) - 14, h - 20, { 0.95f,0.95f,0.95f }, { 0.9f,0.9f,0.9f }, 1.0f);
    drawFilledRect(x + w / 2 + 4 + offset, y + 10, (w / 2) - 14, h - 20, { 0.95f,0.95f,0.95f }, { 0.9f,0.9f,0.9f }, 1.0f);
}

void drawCoffeeMachinePixel(float x, float y, float scale) {
    drawFilledRect(x, y, 72 * scale, 12 * scale, { 0.27f,0.27f,0.27f }, { 0.06f,0.03f,0.01f }, 1.5f);
    drawFilledRect(x + 8 * scale, y + 12 * scale, 56 * scale, 48 * scale, { 0.46f,0.46f,0.46f }, { 0.06f,0.03f,0.01f }, 1.5f);
    drawFilledRect(x + 18 * scale, y + 60 * scale, 36 * scale, 10 * scale, { 0.27f,0.27f,0.27f }, { 0.06f,0.03f,0.01f });
    drawFilledRect(x + 22 * scale, y + 26 * scale, 24 * scale, 8 * scale, { 0.27f,0.27f,0.27f }, { 0.06f,0.03f,0.01f });
    drawFilledRect(x + 46 * scale, y + 46 * scale, 12 * scale, 10 * scale, { 0.35f,0.35f,0.35f }, { 0.06f,0.03f,0.01f });
}

void drawCashRegisterPixel(float x, float y, float scale) {
    drawFilledRect(x, y, 68 * scale, 36 * scale, { 0.35f,0.25f,0.18f }, { 0.06f,0.03f,0.01f });
    drawFilledRect(x + 6 * scale, y + 6 * scale, 40 * scale, 18 * scale, { 0.92f,0.88f,0.80f }, { 0.06f,0.03f,0.01f });
    for (int r = 0; r < 2; r++) for (int c = 0; c < 4; c++) drawFilledRect(x + 50 * scale + c * 6 * scale, y + 6 * scale + r * 6 * scale, 4 * scale, 4 * scale, { 0.6f,0.5f,0.45f }, { 0.06f,0.03f,0.01f }, 0.8f);
}

// New scene elements - Far Left Area
void drawEntryArea(float x, float y) {
    Color edge = {0.06f, 0.03f, 0.01f};
    Color doorWood = {0.55f, 0.35f, 0.20f};
    Color doorDark = {0.45f, 0.25f, 0.10f};
    
    // Door frame
    drawFilledRect(x + 50, y, 140, 320, doorWood, edge, 3.0f);
    
    // Door panels
    drawFilledRect(x + 60, y + 10, 120, 140, doorDark, edge, 2.0f);
    drawFilledRect(x + 60, y + 160, 120, 140, doorDark, edge, 2.0f);
    
    // Door handle
    setColor({0.8f, 0.7f, 0.3f}); // Gold handle
    drawCircleFilled(x + 160, y + 160, 8);
    drawFilledRect(x + 155, y + 155, 20, 10, {0.8f, 0.7f, 0.3f}, edge, 1.0f);
    
    // Welcome mat
    drawFilledRect(x + 60, y - 50, 120, 40, {0.7f, 0.3f, 0.3f}, edge, 2.0f);
    // Mat pattern
    glLineWidth(2.0f);
    setColor({0.8f, 0.4f, 0.4f});
    glBegin(GL_LINES);
    for (int i = 0; i < 3; i++) {
        glVertex2f(x + 70, y - 40 + i * 10);
        glVertex2f(x + 170, y - 40 + i * 10);
    }
    glEnd();
    
    // Coat rack
    float rackX = x + 250;
    float rackY = y + 50;
    
    // Pole
    drawFilledRect(rackX, rackY, 12, 240, doorWood, edge, 2.0f);
    
    // Base
    setColor(doorWood);
    drawCircleFilled(rackX + 6, rackY, 30);
    setColor(edge);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 36; i++) {
        float a = (float)i / 36 * 3.14159f * 2.0f;
        glVertex2f(rackX + 6 + cosf(a) * 30, rackY + sinf(a) * 30);
    }
    glEnd();
    
    // Hooks with items
    for (int i = 0; i < 4; i++) {
        float hookY = rackY + 60 + i * 50;
        
        // Hook
        setColor({0.3f, 0.3f, 0.3f});
        drawCircleFilled(rackX + 12, hookY, 5);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(rackX + 12, hookY);
        glVertex2f(rackX + 30, hookY);
        glEnd();
        
        // Hanging items
        if (i == 0) {
            // Scarf
            setColor({0.8f, 0.3f, 0.4f});
            drawFilledRect(rackX + 25, hookY - 40, 15, 60, {0.8f, 0.3f, 0.4f}, edge, 1.0f);
            setColor({0.9f, 0.4f, 0.5f});
            for (int s = 0; s < 3; s++) {
                drawFilledRect(rackX + 27, hookY - 35 + s * 20, 11, 3, {0.9f, 0.4f, 0.5f}, {0.9f, 0.4f, 0.5f}, 0.5f);
            }
        } else if (i == 1) {
            // Hat
            setColor({0.4f, 0.5f, 0.3f});
            drawCircleFilled(rackX + 32, hookY - 10, 20);
            setColor({0.3f, 0.4f, 0.2f});
            drawEllipse(rackX + 32, hookY - 15, 28, 8);
        } else if (i == 2) {
            // Bag
            setColor({0.6f, 0.4f, 0.3f});
            drawFilledRect(rackX + 20, hookY - 35, 30, 40, {0.6f, 0.4f, 0.3f}, edge, 1.5f);
            // Strap
            glLineWidth(4.0f);
            glBegin(GL_LINES);
            glVertex2f(rackX + 25, hookY);
            glVertex2f(rackX + 25, hookY - 35);
            glEnd();
        }
    }
    
    // Menu board on wall
    float boardX = x + 350;
    float boardY = y + 140;
    
    // Chalkboard
    drawFilledRect(boardX, boardY, 180, 200, {0.2f, 0.2f, 0.2f}, {0.5f, 0.35f, 0.2f}, 4.0f);
    
    // Menu text (simplified as rectangles)
    setColor({0.9f, 0.9f, 0.9f});
    for (int line = 0; line < 5; line++) {
        drawFilledRect(boardX + 20, boardY + 150 - line * 30, 60, 4, {0.9f, 0.9f, 0.9f}, {0.9f, 0.9f, 0.9f}, 0.5f);
        drawFilledRect(boardX + 100, boardY + 150 - line * 30, 40, 4, {0.9f, 0.9f, 0.9f}, {0.9f, 0.9f, 0.9f}, 0.5f);
    }
    
    // Menu title
    drawFilledRect(boardX + 40, boardY + 170, 100, 8, {0.9f, 0.9f, 0.9f}, {0.9f, 0.9f, 0.9f}, 0.5f);
}

// New scene elements for left side
void drawBookshelf(float x, float y) {
    Color wood = {0.55f, 0.35f, 0.20f};
    Color woodLight = {0.65f, 0.45f, 0.30f};
    Color edge = {0.06f, 0.03f, 0.01f};
    
    // Bookshelf frame
    drawFilledRect(x, y, 180, 280, wood, edge, 2.0f);
    
    // Shelves
    for (int i = 0; i < 4; i++) {
        drawFilledRect(x + 5, y + 20 + i * 65, 170, 8, woodLight, edge, 1.5f);
    }
    
    // Pre-defined book widths for variety
    float bookWidths[] = {22, 28, 25, 30, 24, 26, 32, 23, 27, 29, 25, 31};
    
    for (int shelf = 0; shelf < 4; shelf++) {
        float bx = x + 10;
        for (int book = 0; book < 6; book++) {
            // Define book colors inline
            Color bc;
            switch ((shelf * 6 + book) % 6) {
                case 0: bc = {0.8f, 0.2f, 0.2f}; break; // Red
                case 1: bc = {0.2f, 0.5f, 0.8f}; break; // Blue
                case 2: bc = {0.3f, 0.7f, 0.3f}; break; // Green
                case 3: bc = {0.9f, 0.7f, 0.2f}; break; // Yellow
                case 4: bc = {0.6f, 0.3f, 0.7f}; break; // Purple
                case 5: bc = {0.9f, 0.5f, 0.3f}; break; // Orange
            }
            float bw = bookWidths[(shelf * 6 + book) % 12];
            drawFilledRect(bx, y + 28 + shelf * 65, bw, 55, bc, edge, 1.2f);
            bx += bw + 3;
        }
    }
}

// New scene elements for right side
void drawCouchArea(float x, float y) {
    Color edge = {0.06f, 0.03f, 0.01f};
    Color couchColor = {0.45f, 0.35f, 0.55f};
    Color cushion = {0.55f, 0.45f, 0.65f};
    
    // Couch base
    drawFilledRect(x, y, 220, 60, couchColor, edge, 2.0f);
    // Couch back
    drawFilledRect(x, y + 50, 220, 80, couchColor, edge, 2.0f);
    
    // Cushions
    drawFilledRect(x + 15, y + 55, 60, 60, cushion, edge, 1.5f);
    drawFilledRect(x + 85, y + 55, 60, 60, cushion, edge, 1.5f);
    drawFilledRect(x + 155, y + 55, 50, 60, cushion, edge, 1.5f);
    
    // Coffee table
    float tableX = x + 250;
    drawFilledRect(tableX, y - 20, 120, 8, {0.6f, 0.4f, 0.2f}, edge, 2.0f);
    drawFilledRect(tableX + 10, y - 55, 12, 35, {0.5f, 0.3f, 0.15f}, edge, 1.5f);
    drawFilledRect(tableX + 98, y - 55, 12, 35, {0.5f, 0.3f, 0.15f}, edge, 1.5f);
}

void drawScenePixels() {
    Color wall = { 1.0f,0.96f,0.86f };
    Color ceiling = { 1.0f,0.98f,0.92f };
    Color edge = { 0.06f,0.03f,0.01f };
    Color woodTop = { 0.79f,0.58f,0.36f };
    Color woodFill = { 0.82f,0.64f,0.41f };

    // Extended background
    setColor(wall);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(SCENE_WIDTH, 0);
    glVertex2f(SCENE_WIDTH, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Ceiling
    drawFilledRect(0, windowHeight - 24, SCENE_WIDTH, 24, ceiling, edge, 2.0f);

    // Floor divider
    glLineWidth(2.0f);
    setColor(edge);
    glBegin(GL_LINES);
    glVertex2f(0, windowHeight * 0.32f);
    glVertex2f(SCENE_WIDTH, windowHeight * 0.32f);
    glEnd();

    // FAR LEFT SCENE - Entry area
    drawEntryArea(50, windowHeight * 0.18f);

    // LEFT SIDE SCENE - Bookshelf area
    drawBookshelf(750, windowHeight * 0.45f);
    
    // Small plant on left
    drawPlantPixel(950, windowHeight * 0.16f, (plantColorIndex + 2) % numPlantColors);

    // MAIN CENTER SCENE
    float winX = 1050, winY = windowHeight * 0.52f, winW = 220, winH = 260;
    drawWindowPixel(winX, winY, winW, winH, curtainOpenAmount);

    float counterX = 1150;
    float counterY = windowHeight * 0.18f;
    float counterW = windowWidth * 0.52f;
    float counterH = windowHeight * 0.22f;
    
    drawFilledRect(counterX - 8, counterY + counterH, counterW + 16, 22, woodTop, edge, 2.0f);
    
    int planks = 4;
    float plankH = counterH / planks;
    for (int i = 0; i < planks; ++i) drawFilledRect(counterX, counterY + i * plankH, counterW, plankH - 6, woodFill, edge, 2.0f);

    float cmX = counterX + 70, cmY = counterY + counterH + 6;
    drawCoffeeMachinePixel(cmX, cmY, 1.0f);

    setColor({ 0.27f,0.2f,0.15f });
    drawCircleFilled(cmX + 120, cmY + 56, 18);
    drawFilledRect(cmX + 100, cmY + 10, 48, 36, { 0.62f,0.45f,0.36f }, edge, 1.2f);

    float regX = counterX + counterW - 120;
    float regY = counterY + counterH - 10;
    drawCashRegisterPixel(regX, regY, 1.0f);

    float mugX = counterX + counterW - 200; 
    float mugY = counterY + counterH + 6;
    drawFilledRect(mugX, mugY - 10, 28, 24, { 0.92f,0.93f,0.94f }, edge, 1.2f);
    std::vector<std::pair<float, float>> handle = { {mugX + 28,mugY + 4},{mugX + 36,mugY + 6},{mugX + 36,mugY - 6},{mugX + 28,mugY - 8} };
    setColor({ 0.92f,0.93f,0.94f });
    glBegin(GL_POLYGON); for (auto& p : handle) glVertex2f(p.first, p.second); glEnd();
    glLineWidth(1.2f); setColor(edge); glBegin(GL_LINE_LOOP); for (auto& p : handle) glVertex2f(p.first, p.second); glEnd();
    drawSteamSquares(mugX + 12, mugY + 28, 10, 4);

    float shelfLeft = 1550;
    float shelfY1 = windowHeight * 0.64f;
    float shelfY2 = windowHeight * 0.56f;

    drawFilledRect(shelfLeft, shelfY1, 300, 12, woodTop, edge, 2.0f);
    drawFilledRect(shelfLeft + 8, shelfY1 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);
    drawFilledRect(shelfLeft + 260, shelfY1 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);

    drawFilledRect(shelfLeft, shelfY2, 340, 12, woodTop, edge, 2.0f);
    drawFilledRect(shelfLeft + 8, shelfY2 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);
    drawFilledRect(shelfLeft + 300, shelfY2 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);

    float boardX = 1950;
    float boardY = windowHeight * 0.56f;

    drawFilledRect(boardX, boardY + 10, 160, 180, { 0.77f,0.56f,0.33f }, edge, 2.0f);
    drawFilledRect(boardX + 20, boardY + 130, 36, 36, { 0.96f,0.89f,0.49f }, edge);
    drawFilledRect(boardX + 70, boardY + 140, 36, 36, { 0.45f,0.7f,0.56f }, edge);
    drawFilledRect(boardX + 20, boardY + 100, 46, 46, { 0.96f,0.47f,0.47f }, edge);
    drawFilledRect(boardX + 80, boardY + 80, 46, 46, { 0.68f,0.86f,0.96f }, edge);

    float plantCx = 1080;
    float plantCy = windowHeight * 0.16f;
    drawPlantPixel(plantCx, plantCy, plantColorIndex);

    drawFilledRect(1850, 12.0f, 200, 60, { 0.99f,0.75f,0.79f }, edge, 2.0f);
    drawFilledRect(2020, 18.0f, 40, 18, { 0.64f,0.73f,0.56f }, edge, 1.2f);

    drawCatPixel(counterX + 20, counterY - 24, 0.9f, catColorIndex);
    drawSleepingCatPixel(2100, 80, 1.0f);

    // RIGHT SIDE SCENE - Couch area
    drawCouchArea(SCENE_WIDTH - 650, windowHeight * 0.18f);
    
    // Another cat on the right side
    drawSleepingCatPixel(SCENE_WIDTH - 500, 60, 1.2f);
    
    // Wall art on right
    drawFilledRect(SCENE_WIDTH - 280, windowHeight * 0.5f, 180, 220, {0.95f, 0.95f, 0.9f}, edge, 2.0f);
    // Frame
    drawFilledRect(SCENE_WIDTH - 290, windowHeight * 0.49f, 200, 240, {0.3f, 0.2f, 0.1f}, edge, 3.0f);
    
    // Abstract art inside frame
    setColor({0.8f, 0.3f, 0.4f});
    drawCircleFilled(SCENE_WIDTH - 190, windowHeight * 0.62f, 40);
    setColor({0.3f, 0.6f, 0.8f});
    drawCircleFilled(SCENE_WIDTH - 210, windowHeight * 0.58f, 30);
    setColor({0.9f, 0.7f, 0.2f});
    drawCircleFilled(SCENE_WIDTH - 170, windowHeight * 0.6f, 25);
}

struct Smoke { float x, y; float life; float offsetX; };
std::vector<Smoke> smokeParticles;
float smokeTimer = 0.0f;

void spawnSmoke(float x, float y) { 
    Smoke s; 
    s.x = x; 
    s.y = y; 
    s.life = 1.0f; 
    s.offsetX = (rand() % 100 - 50) / 200.0f; 
    smokeParticles.push_back(s); 
}

void updateSmoke(float dt) {
    smokeTimer += dt;
    if (smokeTimer >= 0.25f) {
        float counterX = 1150;
        float counterY = windowHeight * 0.18f;
        float counterW = windowWidth * 0.52f;
        float mugX = counterX + counterW - 200;
        float mugY = counterY + windowHeight * 0.22f + 6;
        float smokeX = mugX + 14;
        float smokeY = mugY + 24;
        spawnSmoke(smokeX, smokeY);
        smokeTimer = 0.0f;
    }
    for (int i = (int)smokeParticles.size() - 1; i >= 0; --i) {
        smokeParticles[i].life -= dt * 0.4f;
        smokeParticles[i].y += dt * 40.0f;
        smokeParticles[i].x += smokeParticles[i].offsetX * dt * 60.0f;
        if (smokeParticles[i].life <= 0) smokeParticles.erase(smokeParticles.begin() + i);
    }
}

void drawSmokePixels() {
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& s : smokeParticles) {
        float a = s.life * 0.6f;
        glColor4f(0.9f, 0.9f, 0.9f, a);
        drawCircleFilled(s.x, s.y, 6.0f + (1.0f - s.life) * 10.0f);
    }
    glDisable(GL_BLEND);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Fixed camera bounds
    float maxCameraX = SCENE_WIDTH - windowWidth;
    if (cameraX < 0.0f) cameraX = 0.0f;
    if (cameraX > maxCameraX) cameraX = maxCameraX;

    glPushMatrix();
    glTranslatef(-cameraX, 0.0f, 0.0f);

    drawScenePixels();
    drawSmokePixels();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void onResize(int w, int h) {
    windowWidth = std::max(200, w);
    windowHeight = std::max(120, h);
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int v) {
    updateSmoke(0.016f);
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) catColorIndex = (catColorIndex + 1) % numCatColors;
        else if (button == GLUT_RIGHT_BUTTON) plantColorIndex = (plantColorIndex + 1) % numPlantColors;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: exit(0); break; // Escape
    case 'a': case 'A':
        cameraX -= CAMERA_STEP;
        break;
    case 'd': case 'D':
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
        curtainOpenAmount += 0.1f;
        if (curtainOpenAmount > 1.0f) curtainOpenAmount = 1.0f;
        break;
    case GLUT_KEY_DOWN:
        curtainOpenAmount -= 0.1f;
        if (curtainOpenAmount < 0.0f) curtainOpenAmount = 0.0f;
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
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Enhanced Cafe Scene with Panning");

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
