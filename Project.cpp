#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>

// Window dimensions
int windowWidth = 1400;
int windowHeight = 800;

// Camera position
float cameraX = 0.0f;

// Cat color states
int catColorIndex = 0;
float catColors[][3] = {
    {0.65f, 0.50f, 0.35f}, // Brown
    {0.96f, 0.80f, 0.56f}, // Orange
    {0.3f, 0.3f, 0.3f},    // Gray
    {1.0f, 1.0f, 1.0f},    // White
    {0.2f, 0.2f, 0.2f}     // Black
};
int numCatColors = 5;

// Plant color states
int plantColorIndex = 0;
float plantColors[][3] = {
    {0.2f, 0.6f, 0.3f},    // Green
    {0.6f, 0.2f, 0.6f},    // Purple
    {0.6f, 0.4f, 0.2f},    // Brown
    {0.2f, 0.4f, 0.6f},    // Blue
    {0.8f, 0.2f, 0.2f}     // Red
};
int numPlantColors = 5;

// Curtain state
float curtainOpenAmount = 0.0f; // 0 = closed, 1 = fully open

// Smoke particles
struct Smoke {
    float x, y;
    float life; // 0 to 1, 1 = just created, 0 = dead
    float offsetX;
};
std::vector<Smoke> smokeParticles;
float smokeTimer = 0.0f;

void drawRectangle(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawCircle(float cx, float cy, float radius, int segments = 30) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawSmoke() {
    for (size_t i = 0; i < smokeParticles.size(); i++) {
        Smoke& s = smokeParticles[i];

        // Set color with alpha based on life
        float alpha = s.life * 0.6f;
        glColor4f(0.9f, 0.9f, 0.9f, alpha);

        // Draw smoke particle
        drawCircle(s.x, s.y, 0.2f + (1.0f - s.life) * 0.3f, 20);
    }
}

void updateSmoke(float deltaTime) {
    smokeTimer += deltaTime;

    // Create new smoke particle every second
    if (smokeTimer >= 1.0f) {
        Smoke newSmoke;
        newSmoke.x = 5.0f; // Coffee machine position
        newSmoke.y = 4.8f;
        newSmoke.life = 1.0f;
        newSmoke.offsetX = (rand() % 100 - 50) / 200.0f; // Random horizontal drift
        smokeParticles.push_back(newSmoke);
        smokeTimer = 0.0f;
    }

    // Update existing smoke particles
    for (int i = smokeParticles.size() - 1; i >= 0; i--) {
        smokeParticles[i].life -= deltaTime * 0.5f; // Fade over 2 seconds
        smokeParticles[i].y += deltaTime * 1.5f;     // Rise up
        smokeParticles[i].x += smokeParticles[i].offsetX * deltaTime; // Drift

        // Remove dead particles
        if (smokeParticles[i].life <= 0.0f) {
            smokeParticles.erase(smokeParticles.begin() + i);
        }
    }
}

void drawPlant() {
    // Pot
    glColor3f(0.76f, 0.45f, 0.28f);
    drawRectangle(-6.5f, -2, 1.5f, 1.2f);
    drawTriangle(-6.5f, -0.8f, -5.0f, -0.8f, -5.75f, -0.3f);

    // Leaves with current color
    glColor3fv(plantColors[plantColorIndex]);
    drawTriangle(-5.75f, 0.5f, -5.0f, 0, -5.75f, -0.5f);

    glColor3f(plantColors[plantColorIndex][0] * 0.8f,
        plantColors[plantColorIndex][1] * 0.8f,
        plantColors[plantColorIndex][2] * 0.8f);
    drawTriangle(-5.75f, 0.5f, -6.5f, 0, -5.75f, -0.5f);
}

void drawCat(float x, float y) {
    // Body with current color
    glColor3fv(catColors[catColorIndex]);
    drawCircle(x, y, 0.8f);

    // Head
    drawCircle(x - 0.5f, y + 0.5f, 0.5f);

    // Ears (slightly darker)
    glColor3f(catColors[catColorIndex][0] * 0.8f,
        catColors[catColorIndex][1] * 0.8f,
        catColors[catColorIndex][2] * 0.8f);
    drawTriangle(x - 0.8f, y + 0.8f, x - 0.6f, y + 1.2f, x - 0.4f, y + 0.8f);
    drawTriangle(x - 0.6f, y + 0.8f, x - 0.4f, y + 1.2f, x - 0.2f, y + 0.8f);

    // Tail
    drawCircle(x + 0.7f, y - 0.3f, 0.25f);
    drawCircle(x + 0.9f, y - 0.5f, 0.2f);
}

void drawScene() {
    // Background
    glColor3f(0.96f, 0.93f, 0.84f);
    drawRectangle(-20, -10, 40, 20);

    // Floor
    glColor3f(0.85f, 0.75f, 0.65f);
    drawRectangle(-20, -10, 40, 8);

    // Windows
    glColor3f(0.73f, 0.56f, 0.36f);
    // X, Y, WIDTH, HEIGHT
    //BROWN BOX
    drawRectangle(-7.94, 2, 3, 4);
    drawRectangle(-4.84, 2, 3, 4);
    //GLASS
    glColor3f(0.68f, 0.85f, 0.90f);
    drawRectangle(-7.8f, 2.2f, 2.6f, 3.6f);
    drawRectangle(-4.6f, 2.2f, 2.6f, 3.6f);

    // Curtain rod
    glColor3f(0.73f, 0.56f, 0.36f);
    drawRectangle(-9.15, 6.2f, 8.5f, 0.15f);

    // Curtain holders
    for (float cx = -8.8f; cx <= -0.3f; cx += 1.3f) {
        drawCircle(cx, 6.3f, 0.15f);
    }

    // Curtains (animated based on curtainOpenAmount)
    glColor3f(0.95f, 0.95f, 0.95f);
    float curtainOffset = curtainOpenAmount * 2.5f;

    // Left curtain
    drawRectangle(-7.8f - curtainOffset, 2, 2.7f, 4.2f);
    // Right curtain  
    drawRectangle(-4.7f + curtainOffset, 2, 2.7f, 4.2f);

    // Dog bed
    glColor3f(0.86f, 0.70f, 0.55f);
    drawCircle(-4, -2, 1.2f);
    glColor3f(0.95f, 0.92f, 0.88f);
    drawCircle(-4, -2, 1.0f);

    // Dog
    glColor3f(0.55f, 0.45f, 0.35f);
    drawCircle(-4.5f, -1.5f, 0.7f);
    drawCircle(-5.2f, -1.5f, 0.5f);
    glColor3f(0.45f, 0.35f, 0.25f);
    drawCircle(-5.4f, -1.2f, 0.25f);
    drawCircle(-5.0f, -1.2f, 0.25f);

    // Food bowls
    glColor3f(0.93f, 0.49f, 0.56f);
    drawCircle(-8.5f, -3, 0.35f);
    drawCircle(10, -3, 0.35f);

    // Counter
    glColor3f(0.73f, 0.56f, 0.36f);
    drawRectangle(2, -3, 12, 5);
    glColor3f(0.65f, 0.48f, 0.28f);
    drawRectangle(2.2f, -2.8f, 11.6f, 0.3f);
    drawRectangle(2.2f, -0.5f, 11.6f, 0.3f);
    drawRectangle(2.2f, 1.8f, 11.6f, 0.3f);

    // Coffee machine
    glColor3f(0.35f, 0.35f, 0.35f);
    drawRectangle(4, 2.2f, 2, 2.5f);
    glColor3f(0.25f, 0.25f, 0.25f);
    drawRectangle(4.2f, 2.4f, 1.6f, 0.3f);
    glColor3f(0.45f, 0.45f, 0.45f);
    drawRectangle(4.7f, 4.5f, 0.6f, 0.4f);

    // Coffee grinder
    glColor3f(0.45f, 0.35f, 0.30f);
    drawRectangle(7.5f, 2.2f, 1.5f, 1.8f);
    glColor3f(0.35f, 0.25f, 0.20f);
    drawRectangle(7.6f, 4.0f, 1.3f, 0.8f);
    glColor3f(0.40f, 0.30f, 0.25f);
    drawCircle(8.25f, 4.4f, 0.5f);

    // Shelves
    glColor3f(0.73f, 0.56f, 0.36f);
    drawRectangle(3.5f, 5.5f, 10, 0.25f);
    drawRectangle(3.5f, 7.5f, 10, 0.25f);

    // Coffee cups
    glColor3f(0.55f, 0.55f, 0.55f);
    drawRectangle(5, 7.6f, 0.8f, 0.6f);
    drawRectangle(6.5f, 7.6f, 0.8f, 0.6f);
    glColor3f(0.65f, 0.65f, 0.65f);
    drawRectangle(5, 5.6f, 0.8f, 0.6f);
    drawRectangle(6.5f, 5.6f, 0.8f, 0.6f);

    // Cork board
    glColor3f(0.73f, 0.56f, 0.36f);
    drawRectangle(10.5f, 3, 2, 2);
    glColor3f(0.2f, 0.2f, 0.2f);
    drawCircle(10.7f, 4.8f, 0.1f);
    drawCircle(12.3f, 4.8f, 0.1f);
    drawCircle(10.7f, 3.2f, 0.1f);
    drawCircle(12.3f, 3.2f, 0.1f);

    // Order sign
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRectangle(13, 5, 1.8f, 1.2f);
    glColor3f(0.2f, 0.2f, 0.2f);
    drawTriangle(13.9f, 6.3f, 13.6f, 6.6f, 14.2f, 6.6f);

    // Sleeping cat
    glColor3f(0.96f, 0.80f, 0.56f);
    drawCircle(12.0f, -2.5f, 0.6f);
    drawCircle(12.5f, -2.5f, 0.5f);
    drawCircle(12.9f, -2.3f, 0.4f);
    glColor3f(0.86f, 0.70f, 0.46f);
    drawTriangle(12.7f, -2.05f, 12.8f, -1.7f, 12.9f, -2.05f);
    drawTriangle(12.9f, -2.05f, 13.0f, -1.7f, 13.1f, -2.05f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLoadIdentity();
    glTranslatef(cameraX, 0, 0);

    drawScene();
    drawPlant();
    drawCat(-5.0f, -1.5f);
    drawSmoke();

    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    glOrtho(-10.0 * aspect, 10.0 * aspect, -10.0, 10.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    updateSmoke(0.016f); // ~16ms per frame
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            // Change cat color
            catColorIndex = (catColorIndex + 1) % numCatColors;
            glutPostRedisplay();
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            // Change plant color
            plantColorIndex = (plantColorIndex + 1) % numPlantColors;
            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
    case 'A':
        cameraX += 0.5f;
        if (cameraX > 5.0f) cameraX = 5.0f;
        break;
    case 'd':
    case 'D':
        cameraX -= 0.5f;
        if (cameraX < -5.0f) cameraX = -5.0f;
        break;
    case 27: // ESC
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX += 0.5f;
        if (cameraX > 5.0f) cameraX = 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        cameraX -= 0.5f;
        if (cameraX < -5.0f) cameraX = -5.0f;
        break;
    case GLUT_KEY_UP:
        // Open curtain
        curtainOpenAmount += 0.1f;
        if (curtainOpenAmount > 0.5f) curtainOpenAmount = 0.5f;
        break;
    case GLUT_KEY_DOWN:
        // Close curtain
        curtainOpenAmount -= 0.1f;
        if (curtainOpenAmount < 0.0f) curtainOpenAmount = 0.0f;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Cat Cafe");

    glutFullScreen();

    glewInit();

    glClearColor(0.96f, 0.93f, 0.84f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
