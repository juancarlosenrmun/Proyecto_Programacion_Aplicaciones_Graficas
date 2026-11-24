/**
* @file GUI.h
 * @author Juan Carlos Enríquez Muñoz
 * @date 23/09/2026
 * @brief Declaración de la clase GUI para la gestión de la interfaz con Dear ImGui (Sujeto/Publicador).
 */

#include "GUI.h"
#include <algorithm>

#include "Renderer.h"

namespace PAG {
    namespace {
        bool abrirSeccion(const char* titulo, bool& abierta) {
            // Conserva el estado de la sección cuando cambia de columna.
            ImGui::SetNextItemOpen(abierta, ImGuiCond_Always);
            abierta = ImGui::CollapsingHeader(titulo);
            return abierta;
        }

        void prepararPanel(const ImVec2& posicion, float ancho, float altoMaximo) {
            ImGui::SetNextWindowPos(posicion);
            ImGui::SetNextWindowSize(ImVec2(ancho, 0.0f));
            ImGui::SetNextWindowSizeConstraints(ImVec2(ancho, 0.0f), ImVec2(ancho, altoMaximo));
        }

        constexpr ImGuiWindowFlags flagsPanel =
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
    }

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

        ventanaPrincipal = window;
        estiloBase = ImGui::GetStyle();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }

    /**
     * @brief Renderiza la ventana correspondiente al selector de color en forma de rueda.
     */
    void GUI::selectorColorTriangular() {
        if (abrirSeccion("Color de fondo", colorAbierto)) {
            // Si el usuario modifica la rueda de color, notificamos a los observadores
            ImGui::SetNextItemWidth(std::min(ImGui::GetContentRegionAvail().x,
                                             13.0f * ImGui::GetFontSize()));
            if (ImGui::ColorPicker4("##ColorFondo", colorActual, ImGuiColorEditFlags_PickerHueWheel)) {
                notificarObservadoresColor();
            }
        }
    }

    /**
     * @brief Renderiza la ventana correspondiente a la consola de eventos.
     */
    void GUI::consola() {
        if (abrirSeccion("Consola", consolaAbierto)) {

            //Añadimos un botón para limpiar la consola
            if (ImGui::Button("Limpiar")) {
                mensajes.clear();
            }

            ImGui::SameLine();
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::Separator();

            const float altoRegistro = 8.0f * ImGui::GetTextLineHeightWithSpacing();
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, altoRegistro),
                              ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

            for (const auto& msg : mensajes) {
                ImGui::TextUnformatted(msg.c_str());
            }

            // Desplazar automáticamente hacia abajo si hay nuevos mensajes
            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
        }
    }

    /**
     * @brief Ventana para especificar el nombre base del Shader Program y cargarlo.
     */
    void GUI::ventanaShaderProgram() {
        if (abrirSeccion("Shader Program", shaderAbierto)) {
            ImGui::InputText("##shaderName", &nombreShaderBase,
                             ImGuiInputTextFlags_AutoSelectAll);

            if (ImGui::Button("Load")) {
                try {
                    notificarObservadoresShader();
                    addMensaje("Shader Program '" + nombreShaderBase + "' cargado.");
                } catch (const std::exception& e) {
                    addMensaje(e.what());
                }
            }
        }
    }

    void GUI::ventanaCamara() {
        if (abrirSeccion("Cámara", camaraAbierto)) {
            static int itemActual = 0;

            const char* movimientos[] = { "Pan", "Tilt", "Dolly", "Crane", "Orbit", "Zoom" };

            ImGui::Text("Movement");
            if (ImGui::Combo("##MovementCombo", &itemActual, movimientos, IM_ARRAYSIZE(movimientos))) {
                tipoMovimientoActivo = static_cast<TipoMovimiento>(itemActual);
                notificarObservadoresMovimientoCamara(tipoMovimientoActivo, 0.0f, 0.0f);
            }

            ImGui::Separator();

            switch (tipoMovimientoActivo) {
                case TipoMovimiento::ZOOM: {
                    static float fov = 45.0f;
                    ImGui::Text("Angle");
                    if (ImGui::SliderFloat("##AngleSlider", &fov, 1.0f, 120.0f, "%.0f")) {
                        float deltaFov = fov - fovAnterior;
                        notificarObservadoresMovimientoCamara(TipoMovimiento::ZOOM, deltaFov);
                        fovAnterior = fov;
                    }
                    break;
                }
                case TipoMovimiento::PAN:
                    ImGui::Text("Direction");
                    if (ImGui::Button("<- Left"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::PAN,  2.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("Right ->")) { notificarObservadoresMovimientoCamara(TipoMovimiento::PAN, -2.0f); }
                    break;

                case TipoMovimiento::TILT:
                    ImGui::Text("Direction");
                    if (ImGui::Button("^ Up ^"))    { notificarObservadoresMovimientoCamara(TipoMovimiento::TILT,  2.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("v Down v"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::TILT, -2.0f); }
                    break;

                case TipoMovimiento::DOLLY:
                    ImGui::Text("Direction");
                    // Disposición en cruz identica a la Figura 1
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f * ImGui::GetFontSize());
                    if (ImGui::Button("^ Front ^")) { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY, 0.0f, -0.1f); }

                    if (ImGui::Button("<- Left"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY, -0.1f, 0.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("Right ->")) { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY,  0.1f, 0.0f); }

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f * ImGui::GetFontSize());
                    if (ImGui::Button("v Back v"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::DOLLY, 0.0f,  0.1f); }
                    break;

                case TipoMovimiento::CRANE:
                    ImGui::Text("Direction");
                    if (ImGui::Button("^ Up ^"))    { notificarObservadoresMovimientoCamara(TipoMovimiento::CRANE,  0.1f); }
                    ImGui::SameLine();
                    if (ImGui::Button("v Down v"))  { notificarObservadoresMovimientoCamara(TipoMovimiento::CRANE, -0.1f); }
                    break;

                case TipoMovimiento::ORBIT:
                    ImGui::Text("Latitude");
                    if (ImGui::Button("^ Up ^")) { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT,  2.0f, 0.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("v Down v")) { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT, -2.0f, 0.0f); }

                    ImGui::Text("Longitude");
                    if (ImGui::Button("<- Left"))   { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT, 0.0f,  2.0f); }
                    ImGui::SameLine();
                    if (ImGui::Button("Right ->"))   { notificarObservadoresMovimientoCamara(TipoMovimiento::ORBIT, 0.0f, -2.0f); }
                    break;
            }
        }
    }

    void GUI::ventanaModelos() {
        if (abrirSeccion("Gestor de Modelos", modelosAbierto)) {
            // Opción para alternar entre Malla de Alambre y Sólido
            bool alambre = Renderer::getInstance().isModoAlambre();
            if (ImGui::Checkbox("Modo alambre", &alambre)) {
                Renderer::getInstance().setModoAlambre(alambre);
            }

            ImGui::Separator();
            ImGui::Text("Cargar Modelo OBJ");

            static std::string rutaModelo = "../modelos/dado.obj";
            ImGui::InputText("Ruta ##RutaOBJ", &rutaModelo);
            if (ImGui::Button("Cargar OBJ")) {
                try {
                    for (auto* listener : listeners) {
                        listener->notificarCargarModelo(rutaModelo);
                    }
                    addMensaje("Modelo cargado: " + rutaModelo);
                } catch (const std::exception& e) {
                    addMensaje(e.what());
                }
            }

            ImGui::Separator();
            ImGui::Text("Modelos en la Escena");

            const auto& modelos = Renderer::getInstance().getModelos();
            static int seleccionado = 0;

            if (modelos.empty()) {
                ImGui::TextDisabled("No hay modelos cargados.");
            } else {
                if (seleccionado >= static_cast<int>(modelos.size())) {
                    seleccionado = 0;
                }

                if (ImGui::BeginCombo("##ListaModelos", modelos[seleccionado]->getNombre().c_str())) {
                    for (int i = 0; i < static_cast<int>(modelos.size()); ++i) {
                        bool isSelected = (seleccionado == i);
                        if (ImGui::Selectable(modelos[i]->getNombre().c_str(), isSelected)) {
                            seleccionado = i;
                        }
                    }
                    ImGui::EndCombo();
                }

                // 1. Controles de Transformación de Modelado (M)
                auto& mod = modelos[seleccionado];
                glm::vec3 pos = mod->getPosicion();
                glm::vec3 rot = mod->getRotacion();
                glm::vec3 esc = mod->getEscala();

                bool modificado = false;
                modificado |= ImGui::DragFloat3("Posicion", &pos.x, 0.05f);
                modificado |= ImGui::DragFloat3("Rotacion", &rot.x, 1.0f);
                modificado |= ImGui::DragFloat3("Escala",   &esc.x, 0.05f, 0.01f, 10.0f);

                if (modificado) {
                    for (auto* listener : listeners) {
                        listener->notificarTransformarModelo(seleccionado, pos, rot, esc);
                    }
                }

                // Dentro de GUI::ventanaModelos()
                if (!modelos.empty()) {
                    auto& mod = modelos[seleccionado];
                    Material mat = mod->getMaterial();

                    ImGui::Separator();
                    ImGui::Text("Propiedades del Material");

                    bool matModificado = false;
                    matModificado |= ImGui::ColorEdit3("Ka (ambiente)", &mat.Ka.x);
                    matModificado |= ImGui::ColorEdit3("Kd (difuso)",   &mat.Kd.x);
                    matModificado |= ImGui::ColorEdit3("Ks (especular)", &mat.Ks.x);
                    matModificado |= ImGui::SliderFloat("Brillo",  &mat.shininess, 1.0f, 128.0f);

                    if (matModificado) {
                        mod->setMaterial(mat);
                    }
                }


                ImGui::Spacing();
                ImGui::Separator();
                if (ImGui::Button("Eliminar modelo")) {
                    try {
                        const std::string nombre = modelos[seleccionado]->getNombre();
                        for (auto* listener : listeners) {
                            listener->notificarEliminarModelo(seleccionado);
                        }
                        addMensaje("Modelo eliminado: " + nombre);
                    } catch (const std::exception& e) {
                        addMensaje(e.what());
                    }
                }
            }
        }
    }

    void GUI::ventanaLuces() {
        if (abrirSeccion("Luces", lucesAbierto)) {
            try {
                const char* tipos[] = {"Ambiente", "Puntual", "Direccional", "Foco"};
                auto& renderer = Renderer::getInstance();

                static int tipoNueva = 0;
                ImGui::Combo("Tipo nuevo", &tipoNueva, tipos, IM_ARRAYSIZE(tipos));
                if (ImGui::Button("Crear luz")) {
                    for (auto* listener : listeners) {
                        listener->notificarAnadirLuz(static_cast<TipoLuz>(tipoNueva));
                    }
                }

                const auto& luces = renderer.getLuces();
                static std::size_t seleccionada = 0;
                if (luces.empty()) {
                    ImGui::TextDisabled("La escena no tiene luces.");
                } else {
                    if (seleccionada >= luces.size()) seleccionada = luces.size() - 1;
                    const auto etiqueta = [&luces, &tipos](std::size_t indice) {
                        return std::to_string(indice + 1) + ". " +
                            tipos[static_cast<int>(luces[indice].getTipo())];
                    };

                    if (ImGui::BeginCombo("Luz", etiqueta(seleccionada).c_str())) {
                        for (std::size_t i = 0; i < luces.size(); ++i) {
                            if (ImGui::Selectable(etiqueta(i).c_str(), i == seleccionada)) seleccionada = i;
                        }
                        ImGui::EndCombo();
                    }

                    int tipo = static_cast<int>(luces[seleccionada].getTipo());
                    PropiedadesLuz propiedades = luces[seleccionada].getPropiedades();
                    bool cambio = ImGui::Combo("Tipo", &tipo, tipos, IM_ARRAYSIZE(tipos));
                    cambio |= ImGui::Checkbox("Activa", &propiedades.activa);
                    const auto tipoLuz = static_cast<TipoLuz>(tipo);

                    if (tipoLuz == TipoLuz::AMBIENTE) {
                        cambio |= ImGui::ColorEdit3("Ia", &propiedades.Ia.x);
                    } else {
                        cambio |= ImGui::ColorEdit3("Id", &propiedades.Id.x);
                        cambio |= ImGui::ColorEdit3("Is", &propiedades.Is.x);
                    }
                    if (tipoLuz == TipoLuz::PUNTUAL || tipoLuz == TipoLuz::FOCO) {
                        cambio |= ImGui::DragFloat3("Posicion", &propiedades.p.x, 0.1f);
                    }
                    if (tipoLuz == TipoLuz::DIRECCIONAL || tipoLuz == TipoLuz::FOCO) {
                        ImGui::TextUnformatted("Dirección hacia la escena");
                        ImGui::SetNextItemWidth(-1.0f);
                        cambio |= ImGui::DragFloat3("##DireccionLuz", &propiedades.d.x, 0.05f);
                    }
                    if (tipoLuz == TipoLuz::FOCO) {
                        cambio |= ImGui::SliderFloat("Ángulo (°)", &propiedades.gamma, 0.1f, 90.0f);
                        cambio |= ImGui::SliderFloat("Exponente", &propiedades.s, 0.0f, 128.0f);
                    }

                    if (ImGui::Button("Eliminar luz")) {
                        for (auto* listener : listeners) {
                            listener->notificarEliminarLuz(seleccionada);
                        }
                    } else if (cambio) {
                        for (auto* listener : listeners) {
                            listener->notificarEditarLuz(seleccionada, tipoLuz, propiedades);
                        }
                    }
                }
            } catch (const std::exception& e) {
                addMensaje(e.what());
            }
        }
    }

    void GUI::actualizarEscalaInterfaz() {
        const float escalaMonitor = ImGui_ImplGlfw_GetContentScaleForWindow(ventanaPrincipal);
        const float escala = escalaMonitor > 0.0f ? escalaMonitor : 1.0f;
        if (escala == escalaInterfaz) return;

        ImGuiStyle& estilo = ImGui::GetStyle();
        estilo = estiloBase;
        estilo.ScaleAllSizes(escala);
        estilo.FontScaleDpi = escala;
        escalaInterfaz = escala;
    }

    void GUI::dibujarHerramientas() {
        ventanaCamara();
        selectorColorTriangular();
        ventanaShaderProgram();
        consola();
    }

    void GUI::dibujarPaneles() {
        const ImGuiViewport& area = *ImGui::GetMainViewport();
        const float margen = 8.0f * escalaInterfaz;
        const float altoDisponible = area.WorkSize.y - 2.0f * margen;
        const float anchoDisponible = area.WorkSize.x - 2.0f * margen;
        if (anchoDisponible <= 0.0f || altoDisponible <= 0.0f) return;

        const float ancho = std::min(330.0f * escalaInterfaz, anchoDisponible);
        const bool dosColumnas = anchoDisponible >= 2.0f * ancho + 360.0f * escalaInterfaz;
        const ImVec2 posicionEscena(area.WorkPos.x + margen, area.WorkPos.y + margen);

        prepararPanel(posicionEscena, ancho, altoDisponible);
        if (ImGui::Begin("##Escena", nullptr, flagsPanel)) {
            ventanaModelos();
            ventanaLuces();
            if (!dosColumnas) dibujarHerramientas();
        }
        ImGui::End();

        if (dosColumnas) {
            const ImVec2 posicionHerramientas(posicionEscena.x + anchoDisponible - ancho, posicionEscena.y);
            prepararPanel(posicionHerramientas, ancho, altoDisponible);
            if (ImGui::Begin("##Herramientas", nullptr, flagsPanel)) {
                dibujarHerramientas();
            }
            ImGui::End();
        }
    }

    /**
     * @brief Genera y dibuja los controles de la interfaz en el fotograma actual.
     */
    void GUI::render() {
        //Preparamos el nuevo fragmento
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        actualizarEscalaInterfaz();
        ImGui::NewFrame();

        dibujarPaneles();

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
