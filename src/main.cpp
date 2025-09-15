#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

// Variáveis globais para controle da janela
int window_width = 800;
int window_height = 600;

// Variáveis globais para controle visual
bool hover_circle_btn = false;
bool hover_ellipse_btn = false;
bool hover_menu_btn = false;
bool hover_reset_btn = false;
std::vector<std::pair<int, int>> curve_points;
int center_x = 0, center_y = 0;
int radius_x = 0, radius_y = 0;
int click_count = 0;
int algorithm_choice = 0; // 0 = nenhum, 1 = círculo, 2 = elipse
bool show_menu = true;

// Converter coordenadas de tela para OpenGL
float screen_to_gl_x(int screen_x) {
    return (2.0f * screen_x / window_width) - 1.0f;
}

float screen_to_gl_y(int screen_y) {
    return 1.0f - (2.0f * screen_y / window_height);
}

// Função para resetar o programa
void reset_program() {
    click_count = 0;
    curve_points.clear();
    center_x = center_y = 0;
    radius_x = radius_y = 0;
    if (!show_menu) {
        std::cout << "Reset - ";
        if (algorithm_choice == 1) {
            std::cout << "Clique para definir novo centro..." << std::endl;
        } else if (algorithm_choice == 2) {
            std::cout << "Clique para definir novo centro..." << std::endl;
        }
    }
}

// Algoritmo do ponto médio para círculo
void draw_circle_midpoint(int cx, int cy, int radius) {
    curve_points.clear();

    int x = 0;
    int y = radius;
    int p = 1 - radius;

    // Função para plotar os 8 pontos simétricos
    auto plot_circle_points = [&](int x, int y) {
        curve_points.push_back({cx + x, cy + y});
        curve_points.push_back({cx - x, cy + y});
        curve_points.push_back({cx + x, cy - y});
        curve_points.push_back({cx - x, cy - y});
        curve_points.push_back({cx + y, cy + x});
        curve_points.push_back({cx - y, cy + x});
        curve_points.push_back({cx + y, cy - x});
        curve_points.push_back({cx - y, cy - x});
    };

    plot_circle_points(x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        plot_circle_points(x, y);
    }

    std::cout << "Círculo desenhado com raio " << radius << " ("
              << curve_points.size() << " pontos)" << std::endl;
}

// Algoritmo do ponto médio para elipse
void draw_ellipse_midpoint(int cx, int cy, int a, int b) {
    curve_points.clear();

    int x = 0;
    int y = b;
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;
    long long fa2 = 4 * a2;
    long long fb2 = 4 * b2;

    // Função para plotar os 4 pontos simétricos
    auto plot_ellipse_points = [&](int x, int y) {
        curve_points.push_back({cx + x, cy + y});
        curve_points.push_back({cx - x, cy + y});
        curve_points.push_back({cx + x, cy - y});
        curve_points.push_back({cx - x, cy - y});
    };

    // Região 1: |slope| < 1
    long long sigma = 2 * b2 + a2 * (1 - 2 * b);

    while (b2 * x <= a2 * y) {
        plot_ellipse_points(x, y);

        if (sigma >= 0) {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
        x++;
    }

    // Região 2: |slope| >= 1
    sigma = 2 * a2 + b2 * (1 - 2 * a);

    while (y >= 0) {
        plot_ellipse_points(x, y);

        if (sigma >= 0) {
            sigma += fb2 * (1 - x);
            x++;
        }
        sigma += a2 * ((4 * y) + 6);
        y--;
    }

    std::cout << "Elipse desenhada com semi-eixos a=" << a << ", b=" << b
              << " (" << curve_points.size() << " pontos)" << std::endl;
}

// Callback do mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Verificar se clique foi no menu
        if (show_menu) {
            if (ypos < 120) { // Área do menu ajustada para o novo design
                if (xpos < window_width / 2) {
                    algorithm_choice = 1; // Círculo
                    show_menu = false;
                    std::cout << "Algoritmo selecionado: CÍRCULO" << std::endl;
                    std::cout << "Clique para definir o centro..." << std::endl;
                } else {
                    algorithm_choice = 2; // Elipse
                    show_menu = false;
                    std::cout << "Algoritmo selecionado: ELIPSE" << std::endl;
                    std::cout << "Clique para definir o centro..." << std::endl;
                }
                return;
            }
        }

        // Verificar cliques na barra de status (botões de controle)
        if (!show_menu && ypos > window_height - 120) {
            // Converter coordenadas para verificar botões
            float norm_x = (2.0f * xpos / window_width) - 1.0f;

            // Botão Menu (M)
            if (norm_x >= 0.1f && norm_x <= 0.22f) {
                show_menu = true;
                algorithm_choice = 0;
                reset_program();
                std::cout << "Voltando ao menu..." << std::endl;
                return;
            }
            // Botão Reset (R)
            else if (norm_x >= 0.25f && norm_x <= 0.37f) {
                reset_program();
                return;
            }
        }

        if (algorithm_choice == 1) { // Círculo
            switch (click_count) {
                case 0:
                    // Primeiro clique: centro
                    center_x = (int)xpos;
                    center_y = (int)ypos;
                    click_count++;
                    std::cout << "Centro definido em (" << center_x << ", " << center_y << ")" << std::endl;
                    std::cout << "Clique para definir o raio..." << std::endl;
                    break;

                case 1:
                {
                    // Segundo clique: raio
                    int radius_point_x = (int)xpos;
                    int radius_point_y = (int)ypos;
                    int dx = radius_point_x - center_x;
                    int dy = radius_point_y - center_y;
                    int radius = (int)sqrt(dx * dx + dy * dy);

                    if (radius > 0) {
                        draw_circle_midpoint(center_x, center_y, radius);
                        radius_x = radius; // Armazenar para referência visual
                        click_count++;
                        std::cout << "Raio definido: " << radius << std::endl;
                        std::cout << "Botão direito ou 'R' para resetar, 'M' para menu" << std::endl;
                    }
                    break;
                }
            }
        } else if (algorithm_choice == 2) { // Elipse
            switch (click_count) {
                case 0:
                    // Primeiro clique: centro
                    center_x = (int)xpos;
                    center_y = (int)ypos;
                    click_count++;
                    std::cout << "Centro definido em (" << center_x << ", " << center_y << ")" << std::endl;
                    std::cout << "Clique para definir o primeiro semi-eixo..." << std::endl;
                    break;

                case 1:
                {
                    // Segundo clique: primeiro semi-eixo
                    int first_radius_x_point = (int)xpos;
                    int first_radius_y_point = (int)ypos;
                    int dx = first_radius_x_point - center_x;
                    int dy = first_radius_y_point - center_y;
                    radius_x = (int)sqrt(dx * dx + dy * dy);

                    click_count++;
                    std::cout << "Primeiro semi-eixo definido: " << radius_x << std::endl;
                    std::cout << "Clique para definir o segundo semi-eixo..." << std::endl;
                    break;
                }

                case 2:
                {
                    // Terceiro clique: segundo semi-eixo
                    int second_radius_x_point = (int)xpos;
                    int second_radius_y_point = (int)ypos;
                    int dx = second_radius_x_point - center_x;
                    int dy = second_radius_y_point - center_y;
                    radius_y = (int)sqrt(dx * dx + dy * dy);

                    if (radius_x > 0 && radius_y > 0) {
                        draw_ellipse_midpoint(center_x, center_y, radius_x, radius_y);
                        click_count++;
                        std::cout << "Segundo semi-eixo definido: " << radius_y << std::endl;
                        std::cout << "Botão direito ou 'R' para resetar, 'M' para menu" << std::endl;
                    }
                    break;
                }
            }
        }

    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Reset
        reset_program();
    }
}

// Callback para teclas
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        // Reset
        reset_program();
    } else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        // Voltar ao menu
        show_menu = true;
        algorithm_choice = 0;
        reset_program();
        std::cout << "Voltando ao menu..." << std::endl;
    } else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        // Selecionar círculo via teclado
        algorithm_choice = 1;
        show_menu = false;
        reset_program();
        std::cout << "Algoritmo selecionado: CÍRCULO" << std::endl;
        std::cout << "Clique para definir o centro..." << std::endl;
    } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        // Selecionar elipse via teclado
        algorithm_choice = 2;
        show_menu = false;
        reset_program();
        std::cout << "Algoritmo selecionado: ELIPSE" << std::endl;
        std::cout << "Clique para definir o centro..." << std::endl;
    }
}

// Callback para redimensionamento
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
}

// Callback para movimento do mouse
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    // Reset hover states
    hover_circle_btn = hover_ellipse_btn = hover_menu_btn = hover_reset_btn = false;

    if (show_menu) {
        // Menu hover detection
        if (ypos < 120) {
            if (xpos < window_width / 2) {
                hover_circle_btn = true;
            } else {
                hover_ellipse_btn = true;
            }
        }
    } else {
        // Status bar buttons hover detection
        if (ypos > window_height - 120) {
            float norm_x = (2.0f * xpos / window_width) - 1.0f;

            if (norm_x >= 0.1f && norm_x <= 0.22f) {
                hover_menu_btn = true;
            } else if (norm_x >= 0.25f && norm_x <= 0.37f) {
                hover_reset_btn = true;
            }
        }
    }
}

// Função para desenhar o menu simples
void draw_menu() {
    // Fundo do menu
    glColor3f(0.1f, 0.1f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, 0.75f);
    glVertex2f(-1.0f, 0.75f);
    glEnd();

    // Botão Círculo (esquerda)
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(-0.95f, 0.95f);
    glVertex2f(-0.05f, 0.95f);
    glVertex2f(-0.05f, 0.8f);
    glVertex2f(-0.95f, 0.8f);
    glEnd();

    // Botão Elipse (direita)
    glColor3f(0.8f, 0.4f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0.05f, 0.95f);
    glVertex2f(0.95f, 0.95f);
    glVertex2f(0.95f, 0.8f);
    glVertex2f(0.05f, 0.8f);
    glEnd();

    // Bordas dos botões
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    // Borda do botão círculo
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.95f, 0.95f);
    glVertex2f(-0.05f, 0.95f);
    glVertex2f(-0.05f, 0.8f);
    glVertex2f(-0.95f, 0.8f);
    glEnd();

    // Borda do botão elipse
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.05f, 0.95f);
    glVertex2f(0.95f, 0.95f);
    glVertex2f(0.95f, 0.8f);
    glVertex2f(0.05f, 0.8f);
    glEnd();

    // Desenhar símbolos dos algoritmos

    // Símbolo do círculo
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    int segments = 32;
    float radius = 0.06f;
    float center_x = -0.5f;
    float center_y = 0.875f;
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(center_x + radius * cos(angle), center_y + radius * sin(angle));
    }
    glEnd();

    // Símbolo da elipse
    glBegin(GL_LINE_LOOP);
    float a = 0.08f; // Semi-eixo maior
    float b = 0.04f; // Semi-eixo menor
    center_x = 0.5f;
    center_y = 0.875f;
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(center_x + a * cos(angle), center_y + b * sin(angle));
    }
    glEnd();

    // Texto informativo na parte inferior do menu
    glColor3f(0.8f, 0.8f, 0.8f);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    // Simulação simples de texto usando pontos (apenas indicativo)
    for (int i = 0; i < 50; i++) {
        glVertex2f(-0.9f + i * 0.036f, 0.77f);
    }
    glEnd();
}

// Função para desenhar informações de status
void draw_status_info() {
    if (show_menu) return;

    // Fundo da barra de status
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.85f);
    glVertex2f(1.0f, -0.85f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Indicador do algoritmo atual
    if (algorithm_choice == 1) {
        glColor3f(0.2f, 0.4f, 0.8f);
    } else {
        glColor3f(0.8f, 0.4f, 0.2f);
    }

    glBegin(GL_QUADS);
    glVertex2f(-0.98f, -0.87f);
    glVertex2f(-0.7f, -0.87f);
    glVertex2f(-0.7f, -0.98f);
    glVertex2f(-0.98f, -0.98f);
    glEnd();

    // Indicador de progresso
    glColor3f(0.0f, 0.8f, 0.0f);
    float progress_width = 0.0f;
    if (algorithm_choice == 1) { // Círculo
        progress_width = (click_count >= 2) ? 1.0f : click_count * 0.5f;
    } else if (algorithm_choice == 2) { // Elipse
        progress_width = (click_count >= 3) ? 1.0f : click_count * 0.33f;
    }

    if (progress_width > 0) {
        glBegin(GL_QUADS);
        glVertex2f(-0.6f, -0.87f);
        glVertex2f(-0.6f + 0.4f * progress_width, -0.87f);
        glVertex2f(-0.6f + 0.4f * progress_width, -0.98f);
        glVertex2f(-0.6f, -0.98f);
        glEnd();
    }
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Criar janela
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                         "Algoritmos de Curvas - Ponto Medio", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar janela" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Configurar callbacks
    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Configuração OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    std::cout << "=== ALGORITMOS DE TRACADO DE CURVAS ===" << std::endl;
    std::cout << "CONTROLES:" << std::endl;
    std::cout << "  1 = Selecionar Círculo" << std::endl;
    std::cout << "  2 = Selecionar Elipse" << std::endl;
    std::cout << "  M = Voltar ao Menu" << std::endl;
    std::cout << "  R = Reset" << std::endl;
    std::cout << "  ESC = Sair" << std::endl;
    std::cout << "Clique nos botões ou use as teclas para selecionar" << std::endl;

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Limpar tela
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhar menu se visível
        if (show_menu) {
            draw_menu();
        } else {
            // Desenhar centro se definido
            if (click_count >= 1) {
                glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
                glPointSize(8.0f);
                glBegin(GL_POINTS);
                glVertex2f(screen_to_gl_x(center_x), screen_to_gl_y(center_y));
                glEnd();

                // Círculo ao redor do centro para melhor visibilidade
                glColor3f(1.0f, 0.5f, 0.5f);
                glLineWidth(2.0f);
                glBegin(GL_LINE_LOOP);
                int segments = 16;
                float radius = 0.02f;
                for (int i = 0; i < segments; i++) {
                    float angle = 2.0f * M_PI * i / segments;
                    glVertex2f(screen_to_gl_x(center_x) + radius * cos(angle),
                              screen_to_gl_y(center_y) + radius * sin(angle));
                }
                glEnd();
            }

            // Desenhar curva se calculada
            if (click_count >= ((algorithm_choice == 1) ? 2 : 3) && !curve_points.empty()) {
                glColor3f(1.0f, 1.0f, 1.0f); // Branco
                glPointSize(2.0f);
                glBegin(GL_POINTS);
                for (const auto& point : curve_points) {
                    if (point.first >= 0 && point.first < window_width &&
                        point.second >= 0 && point.second < window_height) {
                        glVertex2f(screen_to_gl_x(point.first), screen_to_gl_y(point.second));
                    }
                }
                glEnd();
            }

            // Desenhar eixos de referência para elipse
            if (algorithm_choice == 2 && click_count >= 2) {
                glColor3f(0.3f, 0.3f, 0.8f); // Azul escuro
                glLineWidth(1.0f);

                if (radius_x > 0) {
                    // Eixo horizontal
                    glBegin(GL_LINES);
                    glVertex2f(screen_to_gl_x(center_x - radius_x), screen_to_gl_y(center_y));
                    glVertex2f(screen_to_gl_x(center_x + radius_x), screen_to_gl_y(center_y));
                    glEnd();
                }

                if (radius_y > 0) {
                    // Eixo vertical
                    glBegin(GL_LINES);
                    glVertex2f(screen_to_gl_x(center_x), screen_to_gl_y(center_y - radius_y));
                    glVertex2f(screen_to_gl_x(center_x), screen_to_gl_y(center_y + radius_y));
                    glEnd();
                }
            }

            // Desenhar informações de status
            draw_status_info();
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}