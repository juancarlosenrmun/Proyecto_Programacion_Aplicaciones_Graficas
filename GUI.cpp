/**
* @file GUI.h
 * @author Juan Carlos Enríquez Muñoz
 * @date 23/09/2026
 * @brief Declaración de la clase GUI para la gestión de la interfaz con Dear ImGui (Sujeto/Publicador).
 */

#include "GUI.h"
#include <algorithm>

namespace PAG {
    GUI* GUI::instance = nullptr; //Inicializamos puntero

    /**
     * @brief Constructor privado para evitar la instanciación directa.
     */
    GUI::GUI() = default;

    /**
     * @brief Destructor virtual de la clase GUI.
     */
    GUI::~GUI() = default;

    /**
     * @brief Consulta la instancia única de la clase GUI.
     * @return Referencia a la instancia de GUI.
     */
    GUI& GUI::getInstance() {
        if (!instance) {
            instance = new GUI(); //Inicialización perezosa
        }
        return *instance;
    }

    /**
     * @brief Inicializa el contexto de Dear ImGui y sus subsistemas para GLFW y OpenGL3.
     * @param window Puntero a la ventana nativa de GLFW.
     */
    void GUI::inicializar(GLFWwindow* window) {
        IMGUI_CHECKVERSION(); //Comprobamos versión
        ImGui::CreateContext(); // Creamos contexto
        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Habilitar navvegación por teclado

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }

    /**
     * @brief Renderiza la ventana correspondiente al selector de color en forma de rueda.
     */
    void GUI::selectorColorTriangular() {
        //Definimos ventana y controles
        ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(325, 300), ImGuiCond_Once); //Tenemos que establcer el tamaño de la ventan
        if (ImGui::Begin("COLOR DE FONDO")) {
            ImGui::SetWindowFontScale(1.0f);

            // Si el usuario modifica la rueda de color, notificamos a los observadores
            if (ImGui::ColorPicker4("Color de fondo", colorActual, ImGuiColorEditFlags_PickerHueWheel)) {
                notificarObservadoresColor();
            }
        }
        ImGui::End();
    }

    /**
     * @brief Renderiza la ventana correspondiente a la consola de eventos.
     */
    void GUI::consola() {
        ImGui::SetNextWindowPos(ImVec2(600,10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(360, 300), ImGuiCond_Once);
        if (ImGui::Begin("CONSOLA")) {

            //Añadimos un botón para limpiar la consola
            if (ImGui::Button("Limpiar")) {
                mensajes.clear();
            }

            ImGui::SameLine();
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::Separator();

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            for (const auto& msg : mensajes) {
                ImGui::TextUnformatted(msg.c_str());
            }

            // Desplazar automáticamente hacia abajo si hay nuevos mensajes
            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
        }

        ImGui::End();
    }

    /**
     * @brief Ventana para especificar el nombre base del Shader Program y cargarlo.
     */
    void GUI::ventanaShaderProgram() {
        ImGui::SetNextWindowPos(ImVec2(10, 450), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(325, 100), ImGuiCond_Once);
        if (ImGui::Begin("Shader Program")) {
            // Caja de texto vinculada a std::string con selección automática
            ImGui::InputText("##shaderName", &nombreShaderBase, ImGuiInputTextFlags_AutoSelectAll);

            // Botón de carga
            if (ImGui::Button("Load")) {
                notificarObservadoresShader();
            }
        }
        ImGui::End();
    }

    void GUI::ventanaCamara() {
        ImGui::SetNextWindowPos(ImVec2(600, 450), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(360, 180), ImGuiCond_Once);

        if (ImGui::Begin("Camera")) {
            static int itemActual = 0;
            const char* movimientos[] = { "Zoom", "Pan", "Tilt", "Dolly", "Crane", "Orbit" };

            ImGui::Text("Movement");
            if (ImGui::Combo("##MovimientoCombo", &itemActual, movimientos, IM_ARRAYSIZE(movimientos))) {
                tipoMovimientoActivo = static_cast<TipoMovimiento>(itemActual);
            }

            ImGui::Separator();

            switch (tipoMovimientoActivo) {
                case TipoMovimiento::ZOOM: {
                    static float fov = 45.0f;
                    if (ImGui::SliderFloat("Angle", &fov, 1.0f, 120.0f)) {
                        notificarObservadoresMovimientoCamara(TipoMovimiento::ZOOM, fov - fovAnterior);
                        fovAnterior = fov;
                    }
                    break;
                }
                case TipoMovimiento::PAN:
                    if (ImGui::Button("<- Left"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::PAN,  2.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("Right ->")) { notificarObservadoresMovimientoCamara(TipoMovimiento::PAN, -2.0f); }
                    break;

                case TipoMovimiento::TILT:
                    if (ImGui::Button("^ Up ^"))    { notificarObservadoresMovimientoCamara(TipoMovimiento::TILT,  2.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("v Down v"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::TILT, -2.0f); }
                    break;

                case TipoMovimiento::DOLLY:
                    if (ImGui::Button("<- Left"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY, -0.1f, 0.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("Right ->")) { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY,  0.1f, 0.0f); }
                    if (ImGui::Button("^ Front ^")) { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY, 0.0f, -0.1f); }
                    ImGui::SameLine();
                    if (ImGui::Button("v Back v"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY, 0.0f,  0.1f); }
                    break;

                case TipoMovimiento::CRANE:
                    if (ImGui::Button("^ Up ^"))    { notificarObservadoresMovimientoCamara(TipoMovimiento::CRANE,  0.1f); }
                    ImGui::SameLine();
                    if (ImGui::Button("v Down v"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::CRANE, -0.1f); }
                    break;

                case TipoMovimiento::ORBIT:
                    if (ImGui::Button("North")) { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT,  2.0f, 0.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("South")) { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT, -2.0f, 0.0f); }
                    if (ImGui::Button("West"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT, 0.0f,  2.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("East"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT, 0.0f, -2.0f); }
                    break;
            }
        }
        ImGui::End();
}
    /**
     * @brief Genera y dibuja los controles de la interfaz en el fotograma actual.
     */
    void GUI::render() {
        //Preparamos el nuevo fragmento
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        selectorColorTriangular();
        ventanaShaderProgram();
        ventanaCamara();
        consola();

        //Renderemos ImGui sobre el viewport de OpenGL
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * @brief Libera todos los recursos asociados a Dear ImGui.
     */
    void GUI::finalizar() {
        //Liberamos los recursos
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    /**
     * @brief Procesa y reenvía los eventos de los botones del ratón a Dear ImGui.
     * @param button Identificador del botón del ratón.
     * @param action Estado del botón (GLFW_PRESS o GLFW_RELEASE).
     */
    void GUI::procesarBotonRaton(int button, int action) {
        ImGuiIO& io = ImGui::GetIO();

        if (action == GLFW_PRESS) {
            io.AddMouseButtonEvent(button, true);
        }else if (action == GLFW_RELEASE) {
            io.AddMouseButtonEvent(button, false);
        }
    }

    /**
     * @brief Añade una nueva cadena de texto al log de la consola interna de la interfaz.
     * @param mensaje Cadena de texto a mostrar en la consola.
     */
    void GUI::addMensaje(const std::string &mensaje) {
        mensajes.push_back(mensaje);
    }

    // LISTENER

    /**
     * @brief Registra un nuevo observador en la lista.
     * @param listener Puntero al objeto que implementa la interfaz Listener.
     */
    void GUI::addListener(Listener* listener) {
        if (listener) {
            listeners.push_back(listener);
        }
    }

    /**
     * @brief Elimina un observador de la lista.
     * @param listener Puntero al objeto a eliminar.
     */
    void GUI::removeListener(Listener* listener) {
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }

    /**
     * @brief Notifica a todos los observadores registrados que el color ha cambiado.
     */
    void GUI::notificarObservadoresColor() {
        for (auto* listener : listeners) {
            listener->notificarCambioColor(colorActual[0], colorActual[1], colorActual[2], colorActual[3]);
        }
    }

    void GUI::notificarObservadoresShader() {
        for (auto* listener : listeners) {
            listener->notificarCargarShader(nombreShaderBase);
        }
    }

    void GUI::notificarObservadoresMovimientoCamara(TipoMovimiento tipo, float delta1, float delta2) {
        for (auto* listener : listeners) {
            listener->notificarMovimientoCamara(tipo, delta1, delta2);
        }
    }
}
