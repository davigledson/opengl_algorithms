#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

// Variáveis globais
GLFWwindow* window = nullptr;
int window_width = 800, window_height = 600;

std::vector<std::pair<int, int>> curve_points;
int center_x = 0, center_y = 0, radius_x = 0, radius_y = 0;
int click_count = 0, algorithm = 0; // 0=menu, 1=círculo, 2=elipse

// Conversões de coordenadas
float to_gl_x(int x) { return (2.0f * x / window_width) - 1.0f; }
float to_gl_y(int y) { return 1.0f - (2.0f * y / window_height); }

void reset() {
    click_count = 0;
    curve_points.clear();
    center_x = center_y = radius_x = radius_y = 0;
}

// Algoritmo círculo - ponto médio
void circle_midpoint(int cx, int cy, int r) {
    curve_points.clear();

    std::cout << "\n=== ALGORITMO CIRCULO - PONTO MEDIO ===" << std::endl;
    std::cout << "Centro: (" << cx << ", " << cy << ")" << std::endl;
    std::cout << "Raio: " << r << std::endl;
    std::cout << "Inicializacao: x=0, y=" << r << ", p=" << (1-r) << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    int x = 0, y = r, p = 1 - r;
    int step = 1;

    auto plot = [&](int px, int py) {
        curve_points.push_back({cx+px, cy+py});
        curve_points.push_back({cx-px, cy+py});
        curve_points.push_back({cx+px, cy-py});
        curve_points.push_back({cx-px, cy-py});
        curve_points.push_back({cx+py, cy+px});
        curve_points.push_back({cx-py, cy+px});
        curve_points.push_back({cx+py, cy-px});
        curve_points.push_back({cx-py, cy-px});
    };

    plot(x, y);
    std::cout << "Passo " << step++ << ": x=" << x << ", y=" << y << ", p=" << p << std::endl;

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2*x + 1;
            std::cout << "Passo " << step++ << ": p < 0, p = p + 2x + 1 = " << p <<
                         " -> x=" << x << ", y=" << y << std::endl;
        } else {
            y--;
            p += 2*(x-y) + 1;
            std::cout << "Passo " << step++ << ": p >= 0, y--, p = p + 2(x-y) + 1 = " << p <<
                         " -> x=" << x << ", y=" << y << std::endl;
        }
        plot(x, y);
    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Total de pontos plotados: " << curve_points.size() << std::endl;
    std::cout << "Circulo completo! Pressione R para resetar ou M para menu." << std::endl;
}

// Algoritmo elipse - ponto médio
void ellipse_midpoint(int cx, int cy, int a, int b) {
    curve_points.clear();

    std::cout << "\n=== ALGORITMO ELIPSE - PONTO MEDIO ===" << std::endl;
    std::cout << "Centro: (" << cx << ", " << cy << ")" << std::endl;
    std::cout << "Semi-eixo a: " << a << ", Semi-eixo b: " << b << std::endl;

    int x = 0, y = b;
    long long a2 = a*a, b2 = b*b;
    long long sigma = 2*b2 + a2*(1-2*b);

    std::cout << "Constantes: a² = " << a2 << ", b² = " << b2 << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    auto plot = [&](int px, int py) {
        curve_points.push_back({cx+px, cy+py});
        curve_points.push_back({cx-px, cy+py});
        curve_points.push_back({cx+px, cy-py});
        curve_points.push_back({cx-px, cy-py});
    };

    int step = 1;

    // Região 1
    std::cout << "REGIAO 1 (|slope| < 1):" << std::endl;
    std::cout << "Inicializacao: x=0, y=" << b << ", sigma=" << sigma << std::endl;

    while (b2*x <= a2*y) {
        plot(x, y);
        std::cout << "Passo " << step++ << ": x=" << x << ", y=" << y << ", sigma=" << sigma;

        if (sigma >= 0) {
            sigma += 4*a2*(1-y);
            y--;
            std::cout << " -> sigma >= 0, y-- ";
        }
        sigma += b2*(4*x + 6);
        x++;
        std::cout << " -> x++, nova sigma=" << sigma << std::endl;
    }

    // Região 2
    std::cout << "\nREGIAO 2 (|slope| >= 1):" << std::endl;
    sigma = 2*a2 + b2*(1-2*a);
    std::cout << "Nova sigma = " << sigma << std::endl;

    while (y >= 0) {
        plot(x, y);
        std::cout << "Passo " << step++ << ": x=" << x << ", y=" << y << ", sigma=" << sigma;

        if (sigma >= 0) {
            sigma += 4*b2*(1-x);
            x++;
            std::cout << " -> sigma >= 0, x++ ";
        }
        sigma += a2*(4*y + 6);
        y--;
        std::cout << " -> y--, nova sigma=" << sigma << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Total de pontos plotados: " << curve_points.size() << std::endl;
    std::cout << "Elipse completa! Pressione R para resetar ou M para menu." << std::endl;
}

// Mouse callback
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (algorithm == 0) { // Menu
        if (y < 200) { // Área dos botões
            algorithm = (x < window_width/2) ? 1 : 2;
            reset();
            if (algorithm == 1) {
                std::cout << "\n>>> CIRCULO SELECIONADO <<<" << std::endl;
                std::cout << "Clique para definir o centro do circulo..." << std::endl;
            } else {
                std::cout << "\n>>> ELIPSE SELECIONADA <<<" << std::endl;
                std::cout << "Clique para definir o centro da elipse..." << std::endl;
            }
        }
        return;
    }

    if (algorithm == 1) { // Círculo
        if (click_count == 0) {
            center_x = x; center_y = y;
            click_count++;
            std::cout << "Centro definido em (" << center_x << ", " << center_y << ")" << std::endl;
            std::cout << "Agora clique para definir o raio..." << std::endl;
        } else if (click_count == 1) {
            int dx = x - center_x, dy = y - center_y;
            int r = sqrt(dx*dx + dy*dy);
            if (r > 0) {
                std::cout << "Raio definido: " << r << " pixels" << std::endl;
                circle_midpoint(center_x, center_y, r);
                click_count++;
            }
        }
    } else if (algorithm == 2) { // Elipse
        if (click_count == 0) {
            center_x = x; center_y = y;
            click_count++;
            std::cout << "Centro definido em (" << center_x << ", " << center_y << ")" << std::endl;
            std::cout << "Agora clique para definir o primeiro semi-eixo..." << std::endl;
        } else if (click_count == 1) {
            int dx = x - center_x, dy = y - center_y;
            radius_x = sqrt(dx*dx + dy*dy);
            click_count++;
            std::cout << "Primeiro semi-eixo definido: " << radius_x << " pixels" << std::endl;
            std::cout << "Agora clique para definir o segundo semi-eixo..." << std::endl;
        } else if (click_count == 2) {
            int dx = x - center_x, dy = y - center_y;
            radius_y = sqrt(dx*dx + dy*dy);
            if (radius_x > 0 && radius_y > 0) {
                std::cout << "Segundo semi-eixo definido: " << radius_y << " pixels" << std::endl;
                ellipse_midpoint(center_x, center_y, radius_x, radius_y);
                click_count++;
            }
        }
    }
}

// Teclado callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_1) {
        algorithm = 1; reset();
        std::cout << "\n>>> CIRCULO SELECIONADO <<<" << std::endl;
        std::cout << "Clique para definir o centro..." << std::endl;
    } else if (key == GLFW_KEY_2) {
        algorithm = 2; reset();
        std::cout << "\n>>> ELIPSE SELECIONADA <<<" << std::endl;
        std::cout << "Clique para definir o centro..." << std::endl;
    } else if (key == GLFW_KEY_M) {
        algorithm = 0; reset();
        std::cout << "\n>>> VOLTANDO AO MENU <<<" << std::endl;
        std::cout << "Clique em um dos botoes ou use 1/2 para selecionar..." << std::endl;
    } else if (key == GLFW_KEY_R) {
        reset();
        std::cout << "\n>>> RESETADO <<<" << std::endl;
        if (algorithm == 1) {
            std::cout << "Clique para definir novo centro do circulo..." << std::endl;
        } else if (algorithm == 2) {
            std::cout << "Clique para definir novo centro da elipse..." << std::endl;
        }
    }
}

// Renderizar
void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (algorithm == 0) { // Menu
        // Botão círculo
        glColor3f(0.2f, 0.4f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(-0.9f, 0.9f); glVertex2f(-0.1f, 0.9f);
        glVertex2f(-0.1f, 0.6f); glVertex2f(-0.9f, 0.6f);
        glEnd();

        // Botão elipse
        glColor3f(0.8f, 0.4f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(0.1f, 0.9f); glVertex2f(0.9f, 0.9f);
        glVertex2f(0.9f, 0.6f); glVertex2f(0.1f, 0.6f);
        glEnd();

        // Bordas dos botões
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.9f, 0.9f); glVertex2f(-0.1f, 0.9f);
        glVertex2f(-0.1f, 0.6f); glVertex2f(-0.9f, 0.6f);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex2f(0.1f, 0.9f); glVertex2f(0.9f, 0.9f);
        glVertex2f(0.9f, 0.6f); glVertex2f(0.1f, 0.6f);
        glEnd();

        // Símbolos
        // Círculo
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 32; i++) {
            float a = 2*M_PI*i/32;
            glVertex2f(-0.5f + 0.15f*cos(a), 0.75f + 0.15f*sin(a));
        }
        glEnd();

        // Elipse
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 32; i++) {
            float a = 2*M_PI*i/32;
            glVertex2f(0.5f + 0.2f*cos(a), 0.75f + 0.1f*sin(a));
        }
        glEnd();

        // Texto indicativo (pontos simples)
        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        // "CIRCULO"
        for (int i = 0; i < 7; i++) {
            glVertex2f(-0.75f + i*0.08f, 0.4f);
        }
        // "ELIPSE"
        for (int i = 0; i < 6; i++) {
            glVertex2f(0.25f + i*0.08f, 0.4f);
        }
        glEnd();

    } else {
        // Desenhar centro se definido
        if (click_count >= 1) {
            glColor3f(1.0f, 0.0f, 0.0f);
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glVertex2f(to_gl_x(center_x), to_gl_y(center_y));
            glEnd();

            // Círculo ao redor do centro
            glColor3f(1.0f, 0.5f, 0.5f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 16; i++) {
                float a = 2*M_PI*i/16;
                glVertex2f(to_gl_x(center_x) + 0.02f*cos(a), to_gl_y(center_y) + 0.02f*sin(a));
            }
            glEnd();
        }

        // Desenhar eixos para elipse em construção
        if (algorithm == 2) {
            if (click_count >= 2 && radius_x > 0) {
                glColor3f(0.0f, 1.0f, 0.0f);
                glLineWidth(2.0f);
                glBegin(GL_LINES);
                glVertex2f(to_gl_x(center_x - radius_x), to_gl_y(center_y));
                glVertex2f(to_gl_x(center_x + radius_x), to_gl_y(center_y));
                glEnd();
            }
            if (click_count >= 3 && radius_y > 0) {
                glColor3f(0.0f, 0.8f, 1.0f);
                glLineWidth(2.0f);
                glBegin(GL_LINES);
                glVertex2f(to_gl_x(center_x), to_gl_y(center_y - radius_y));
                glVertex2f(to_gl_x(center_x), to_gl_y(center_y + radius_y));
                glEnd();
            }
        }

        // Desenhar curva final
        if (!curve_points.empty()) {
            glColor3f(1.0f, 1.0f, 1.0f);
            glPointSize(2.0f);
            glBegin(GL_POINTS);
            for (auto& p : curve_points) {
                if (p.first >= 0 && p.first < window_width &&
                    p.second >= 0 && p.second < window_height) {
                    glVertex2f(to_gl_x(p.first), to_gl_y(p.second));
                }
            }
            glEnd();
        }

        // Barra de status
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.85f); glVertex2f(-1.0f, -0.85f);
        glEnd();

        // Indicador de progresso
        glColor3f(0.0f, 0.8f, 0.0f);
        float progress = 0.0f;
        if (algorithm == 1) {
            progress = (click_count >= 2) ? 1.0f : click_count * 0.5f;
        } else if (algorithm == 2) {
            progress = (click_count >= 3) ? 1.0f : click_count * 0.33f;
        }

        if (progress > 0) {
            glBegin(GL_QUADS);
            glVertex2f(-0.9f, -0.9f);
            glVertex2f(-0.9f + 1.8f * progress, -0.9f);
            glVertex2f(-0.9f + 1.8f * progress, -0.95f);
            glVertex2f(-0.9f, -0.95f);
            glEnd();
        }
    }

    glfwSwapBuffers(window);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(window_width, window_height, "Algoritmos de Curvas - Ponto Medio", NULL, NULL);

    if (!window) {
        std::cerr << "Erro ao criar janela" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD" << std::endl;
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    // Instruções iniciais
    std::cout << "========================================" << std::endl;
    std::cout << "    ALGORITMOS DE TRACADO DE CURVAS    " << std::endl;
    std::cout << "         Metodo do Ponto Medio         " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nCONTROLES:" << std::endl;
    std::cout << "  1 = Selecionar Algoritmo do Circulo" << std::endl;
    std::cout << "  2 = Selecionar Algoritmo da Elipse" << std::endl;
    std::cout << "  M = Voltar ao Menu Principal" << std::endl;
    std::cout << "  R = Reset (reiniciar algoritmo atual)" << std::endl;
    std::cout << "  ESC = Sair do programa" << std::endl;
    std::cout << "\nClique nos botoes da tela ou use as teclas!" << std::endl;
    std::cout << "Os calculos aparecerao aqui no console." << std::endl;
    std::cout << "========================================\n" << std::endl;

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        render();
        glfwWaitEventsTimeout(0.016); // ~60 FPS
    }

    std::cout << "\nPrograma encerrado. Obrigado!" << std::endl;
    glfwTerminate();
    return 0;
}