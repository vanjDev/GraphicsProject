#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <cstdlib>

const int WIN_W = 1365;
const int WIN_H = 768;

int windowWidth = WIN_W;
int windowHeight = WIN_H;

// Camera for panning
/* Idk kung pano ko ginawa to --- broken din ung panning, 
   walang space sa kaliwa, kanan lang may space
   to be fix
*/
float cameraX = 0.0f;
const float CAMERA_STEP = 50.0f; // pixels per key press

// Cat colors
int catColorIndex = 1; // start as orange
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

float curtainOpenAmount = 0.0f; // 0..1

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

void drawSteamSquares(float cx, float cy, float size, int count) {
    Color white = { 1.0f,1.0f,1.0f };
    Color outline = { 0.98f,0.98f,0.98f };
    for (int i = 0; i < count; i++) drawFilledRect(cx - size / 2, cy + i * (size + 4), size, size, white, outline, 1.0f);
}

// Gato na nagbabago kulay --- left cat
void drawCatPixel(float cx, float cy, float scale, int colorIndex) {
    Color body = { catColors[colorIndex][0], catColors[colorIndex][1], catColors[colorIndex][2] };
    Color darker = { body.r * 0.8f, body.g * 0.8f, body.b * 0.8f };
    // body
    setColor(body);
    drawCircleFilled(cx, cy, 28.0f * scale);
    // head
    drawCircleFilled(cx - 22.0f * scale, cy + 22.0f * scale, 18.0f * scale);
    // ears
    setColor(darker);
    drawTriangleFilled(cx - 36 * scale, cy + 36 * scale, cx - 28 * scale, cy + 52 * scale, cx - 18 * scale, cy + 36 * scale);
    drawTriangleFilled(cx - 26 * scale, cy + 36 * scale, cx - 18 * scale, cy + 52 * scale, cx - 10 * scale, cy + 36 * scale);
    // tail
    setColor(body);
    drawCircleFilled(cx + 22 * scale, cy - 8 * scale, 9 * scale);
    drawCircleFilled(cx + 34 * scale, cy - 20 * scale, 7 * scale);
}

// Gato na tulog --- right cat small
void drawSleepingCatPixel(float x, float y, float scale) {
    // body
    setColor({ 0.96f,0.80f,0.56f });
    drawCircleFilled(x, y, 18 * scale);
    // head
    drawCircleFilled(x + 12 * scale, y, 14 * scale);
    drawCircleFilled(x + 28 * scale, y + 6 * scale, 10 * scale);
    // ears
    setColor({ 0.86f,0.70f,0.46f });
    drawTriangleFilled(x + 20 * scale, y + 12 * scale, x + 24 * scale, y + 28 * scale, x + 28 * scale, y + 12 * scale);
    drawTriangleFilled(x + 28 * scale, y + 12 * scale, x + 32 * scale, y + 28 * scale, x + 36 * scale, y + 12 * scale);
}

// Window with curtains
// Broken ung window frame, di sya 1:1 sa orig .cpp --- to be fix pa
void drawWindowPixel(float x, float y, float w, float h, float openAmount) {
    Color frame = { 0.73f,0.56f,0.36f };
    Color glass = { 0.68f,0.85f,0.90f };
    // frame
    drawFilledRect(x, y, w, h, frame, { 0.06f,0.03f,0.01f }, 2.0f);
    // inner glass
    drawFilledRect(x + 8, y + 8, w - 16, h - 16, glass, { 0.06f,0.03f,0.01f }, 1.2f);
    // curtain rod
    drawFilledRect(x - 10, y + h - 12, w + 20, 6, frame, { 0.06f,0.03f,0.01f }, 1.5f);
    // holders
    for (float cx = x + 12; cx <= x + w - 12; cx += 48) drawCircleFilled(cx, y + h - 9, 6);
    // curtains 
    float offset = openAmount * (w * 0.35f);
    drawFilledRect(x + 10 - offset, y + 10, (w / 2) - 14, h - 20, { 0.95f,0.95f,0.95f }, { 0.9f,0.9f,0.9f }, 1.0f);
    drawFilledRect(x + w / 2 + 4 + offset, y + 10, (w / 2) - 14, h - 20, { 0.95f,0.95f,0.95f }, { 0.9f,0.9f,0.9f }, 1.0f);
}

// Coffee machine and cash register (BROKEN)
void drawCoffeeMachinePixel(float x, float y, float scale) {
    // base platform
    drawFilledRect(x, y, 72 * scale, 12 * scale, { 0.27f,0.27f,0.27f }, { 0.06f,0.03f,0.01f }, 1.5f);
    // main body
    drawFilledRect(x + 8 * scale, y + 12 * scale, 56 * scale, 48 * scale, { 0.46f,0.46f,0.46f }, { 0.06f,0.03f,0.01f }, 1.5f);
    // head/top
    drawFilledRect(x + 18 * scale, y + 60 * scale, 36 * scale, 10 * scale, { 0.27f,0.27f,0.27f }, { 0.06f,0.03f,0.01f });
    // portafilter slot
    drawFilledRect(x + 22 * scale, y + 26 * scale, 24 * scale, 8 * scale, { 0.27f,0.27f,0.27f }, { 0.06f,0.03f,0.01f });
    // small display
    drawFilledRect(x + 46 * scale, y + 46 * scale, 12 * scale, 10 * scale, { 0.35f,0.35f,0.35f }, { 0.06f,0.03f,0.01f });
}

void drawCashRegisterPixel(float x, float y, float scale) {
    drawFilledRect(x, y, 68 * scale, 36 * scale, { 0.35f,0.25f,0.18f }, { 0.06f,0.03f,0.01f });
    drawFilledRect(x + 6 * scale, y + 6 * scale, 40 * scale, 18 * scale, { 0.92f,0.88f,0.80f }, { 0.06f,0.03f,0.01f });
    // keypad
    for (int r = 0; r < 2; r++) for (int c = 0; c < 4; c++) drawFilledRect(x + 50 * scale + c * 6 * scale, y + 6 * scale + r * 6 * scale, 4 * scale, 4 * scale, { 0.6f,0.5f,0.45f }, { 0.06f,0.03f,0.01f }, 0.8f);
}

// Plant idk
void drawPlantPixel(float cx, float cy, int colorIndex) {
    Color edge = { 0.06f,0.03f,0.01f };
    drawFilledRect(cx - 40, cy - 40, 80, 40, { 0.79f,0.37f,0.18f }, edge, 2.0f); // pot
    struct Leaf { float dx, dy, angle; };
    Leaf leaves[] = { {0,40,-15},{-48,12,10},{48,12,20} };
    Color leafColor = { plantColors[colorIndex][0],plantColors[colorIndex][1],plantColors[colorIndex][2] };
    for (auto& leaf : leaves) {
        glPushMatrix();
        glTranslatef(cx + leaf.dx, cy + leaf.dy, 0);
        glRotatef(leaf.angle, 0, 0, 1);
        glBegin(GL_QUADS);
        setColor(leafColor);
        glVertex2f(-25, -25); glVertex2f(25, -25); glVertex2f(25, 25); glVertex2f(-25, 25);
        glEnd();
        glPopMatrix();
    }
}

void drawScenePixels() {
    // Colors
    Color wall = { 1.0f,0.96f,0.86f };
    Color ceiling = { 1.0f,0.98f,0.92f };
    Color edge = { 0.06f,0.03f,0.01f };
    Color woodTop = { 0.79f,0.58f,0.36f };
    Color woodFill = { 0.82f,0.64f,0.41f };

    // Background
    setColor(wall);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Ceiling
    drawFilledRect(0, windowHeight - 24, windowWidth, 24, ceiling, edge, 2.0f);

    // Floor divider
    glLineWidth(2.0f);
    setColor(edge);
    glBegin(GL_LINES);
    glVertex2f(20, windowHeight * 0.32f);
    glVertex2f(windowWidth - 20, windowHeight * 0.32f);
    glEnd();

    // Window
    float winX = 60, winY = windowHeight * 0.52f, winW = 220, winH = 260;
    drawWindowPixel(winX, winY, winW, winH, curtainOpenAmount);

    // Wooden table
    float counterX = windowWidth * 0.22f;
    float counterY = windowHeight * 0.18f;
    float counterW = windowWidth * 0.62f; // reduced
    float counterH = windowHeight * 0.22f; // reduced height
    // top surface
    drawFilledRect(counterX - 8, counterY + counterH, counterW + 16, 22, woodTop, edge, 2.0f);
    // planks
    int planks = 4;
    float plankH = counterH / planks;
    for (int i = 0; i < planks; ++i) drawFilledRect(counterX, counterY + i * plankH, counterW, plankH - 6, woodFill, edge, 2.0f);

    // Coffee machine (fixed, placed left on counter)
    float cmX = counterX + 70, cmY = counterY + counterH + 6;
    drawCoffeeMachinePixel(cmX, cmY, 1.0f);

    // Coffee grinder small
    setColor({ 0.27f,0.2f,0.15f });
    drawCircleFilled(cmX + 120, cmY + 56, 18);
    // Coffee grinder body
    drawFilledRect(cmX + 100, cmY + 10, 48, 36, { 0.62f,0.45f,0.36f }, edge, 1.2f);

    // Cash register near right of counter (fixed)
    float regX = counterX + counterW - 120;
    float regY = counterY + counterH - 10;
    drawCashRegisterPixel(regX, regY, 1.0f);

    // Mug and steam
    float mugX = counterX + counterW - 200; float mugY = counterY + counterH + 6;
    drawFilledRect(mugX, mugY - 10, 28, 24, { 0.92f,0.93f,0.94f }, edge, 1.2f);
    std::vector<std::pair<float, float>> handle = { {mugX + 28,mugY + 4},{mugX + 36,mugY + 6},{mugX + 36,mugY - 6},{mugX + 28,mugY - 8} };
    // handle outline
    setColor({ 0.92f,0.93f,0.94f });
    glBegin(GL_POLYGON); for (auto& p : handle) glVertex2f(p.first, p.second); glEnd();
    glLineWidth(1.2f); setColor(edge); glBegin(GL_LINE_LOOP); for (auto& p : handle) glVertex2f(p.first, p.second); glEnd();
    drawSteamSquares(mugX + 12, mugY + 28, 10, 4);

    // Shelves
    float shelfLeft = windowWidth * 0.42f;
    float shelfY1 = windowHeight * 0.64f;
    float shelfY2 = windowHeight * 0.56f;

    // Top shelf
    drawFilledRect(shelfLeft, shelfY1, 300, 12, woodTop, edge, 2.0f);
    drawFilledRect(shelfLeft + 8, shelfY1 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);
    drawFilledRect(shelfLeft + 260, shelfY1 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);

    // Bottom shelf
    drawFilledRect(shelfLeft, shelfY2, 340, 12, woodTop, edge, 2.0f);
    drawFilledRect(shelfLeft + 8, shelfY2 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);
    drawFilledRect(shelfLeft + 300, shelfY2 - 20, 8, 24, { 0.55f,0.32f,0.2f }, edge);


    // corkboard
    float boardX = windowWidth * 0.70f;
    float boardY = windowHeight * 0.56f;

    drawFilledRect(boardX, boardY + 10, 160, 180, { 0.77f,0.56f,0.33f }, edge, 2.0f);
    drawFilledRect(boardX + 20, boardY + 130, 36, 36, { 0.96f,0.89f,0.49f }, edge);
    drawFilledRect(boardX + 70, boardY + 140, 36, 36, { 0.45f,0.7f,0.56f }, edge);
    drawFilledRect(boardX + 20, boardY + 100, 46, 46, { 0.96f,0.47f,0.47f }, edge);
    drawFilledRect(boardX + 80, boardY + 80, 46, 46, { 0.68f,0.86f,0.96f }, edge);

    float plantCx = windowWidth * 0.08f;
    float plantCy = windowHeight * 0.16f;
    drawPlantPixel(plantCx, plantCy, plantColorIndex); // use selected color

    // Plant pot
    drawFilledRect(plantCx - 40, plantCy - 40, 80, 40, { 0.79f,0.37f,0.18f }, edge, 2.0f);

    // Leaves
    struct Leaf { float dx, dy, angle; };
    Leaf leaves[] = {
        { 0, 40, -15 },
        { -48, 12, 10 },
        { 48, 12, 20 }
    };
    
    // Rugs / mats
    drawFilledRect(windowWidth * 0.65f, 12.0f, 200, 60, { 0.99f,0.75f,0.79f }, edge, 2.0f);
    drawFilledRect(windowWidth * 0.84f, 18.0f, 40, 18, { 0.64f,0.73f,0.56f }, edge, 1.2f);

    // Place cats
    drawCatPixel(counterX + 20, counterY - 24, 0.9f, catColorIndex);
    drawSleepingCatPixel(windowWidth - 140, 80, 1.0f);
}

struct Smoke { float x, y; float life; float offsetX; };
std::vector<Smoke> smokeParticles;
float smokeTimer = 0.0f;

void spawnSmoke(float x, float y) { Smoke s; s.x = x; s.y = y; s.life = 1.0f; s.offsetX = (rand() % 100 - 50) / 200.0f; smokeParticles.push_back(s); }

void updateSmoke(float dt) {
    smokeTimer += dt;
    if (smokeTimer >= 0.25f) {
        // Spawn above the mug instead of coffee machine
        float counterX = windowWidth * 0.22f;
        float counterY = windowHeight * 0.18f;
        float counterW = windowWidth * 0.62f;
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
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    if (cameraX < 0.0f) cameraX = 0.0f;
    if (cameraX > windowWidth) cameraX = windowWidth; // no wider than window (scene width unchanged)

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

// Inputs
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) catColorIndex = (catColorIndex + 1) % numCatColors;
        else if (button == GLUT_RIGHT_BUTTON) plantColorIndex = (plantColorIndex + 1) % numPlantColors;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: exit(0); break; // Escape
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX -= CAMERA_STEP;
        if (cameraX < 0) cameraX = 0;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += CAMERA_STEP;
        if (cameraX > windowWidth) cameraX = windowWidth;
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
    glutCreateWindow("Integrated Cafe Scene");

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
