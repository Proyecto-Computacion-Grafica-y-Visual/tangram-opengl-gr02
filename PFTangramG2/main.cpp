/****************************************************************************************************
* JUEGO TANGRAM - PROYECTO FINAL GRUPO 02                                                           *
*****************************************************************************************************
*                                                                                                   *
* INTRUCCIONES DE USO:                                                                              *
*   - Para iniciar el juego, presionar la tecla ENTER.                                              *
*   - Las figuras se pueden seleccionar con los numeros del 1 al 7 (solo teclado numerico).         *
*   - La traslacion se realiza con las teclas direccionales (derecha, izquierda, arriba, abajo).    *
*   - La rotacion se realiza con las teclas:                                                        *
*       A: Rotacion Antihoraria                                                                     *
*       H: Rotacion Horaria                                                                         *
*   - Para resetear las figuras a su posicion inicial, presionar la tecla de ESPACIO.               *
*                                                                                                   *
****************************************************************************************************/

#include <GLFW/glfw3.h>
#include <FGLUT/freeglut.h>
#define STB_IMAGE_IMPLEMENTATION                // Constante para colocar las texturas
#include <STB/stb_image.h>                      // Libreria de carga de imagenes
#include <string>

/***********************************
* GESTION DE TEXTURAS Y BIENVENIDA *
***********************************/

GLuint texturaBienvenida;                       // Identificador de la textura
bool mostrarBienvenida = true;                  // Indica si se muestra la pantalla de bienvenida

// Funcion para cargar la imagen/textura
bool cargarTextura(const char* ruta, GLuint& textura) {

	// Carga la imagen y valida si se pudo cargar
    int ancho, alto, canales;
    unsigned char* data = stbi_load(ruta, &ancho, &alto, &canales, 0);
    if (!data) {
		printf("Error al cargar la imagen\n");  // Mensaje de error si no se pudo cargar la imagen
		return false;                           // Termina la funcion
    }

	// Genera la textura y configura los parametros
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carga la textura en OpenGL segun la cantidad de canales
    if (canales == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ancho, alto, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (canales == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ancho, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

	// Libera la imagen de la memoria
    stbi_image_free(data);
    return true;
}

// Funcion para inicializar OpenGL
bool inicializarOpenGL() {

    // Configuracion de OpenGL
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    // Configuracion de la luz
    GLfloat luzAmbiente[] = { 1.0f,1.0f,1.0f,1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Carga la imagen desde el archivo
    if (!cargarTextura("texturas/img-bienvenida.png", texturaBienvenida)) {
        return false;
    }

    return true;
}

/**************************************
* CONTROL DE ELEMENTOS CON EL TECLADO *
**************************************/

// Estructura para almacenar la posicion, rotacion y seleccion de una figura
struct Figura {
	float x;                    // Posicion en el eje X
	float y;                    // Posicion en el eje Y
	float rotacion;             // Rotacion de la figura
	bool seleccionada;          // Indica si la figura esta seleccionada
};

// Array para las 7 figuras del Tangram (de tipo Figura), se inicializan
Figura figuras[7] = {
    {0.0f, 0.0f, 0.0f, false},  // Triangulo grande azul
    {0.0f, 0.0f, 0.0f, false},  // Triangulo grande verde
    {0.0f, 0.0f, 0.0f, false},  // Triangulo pequenio amarillo
    {0.0f, 0.0f, 0.0f, false},  // Cuadrado rojo
    {0.0f, 0.0f, 0.0f, false},  // Triangulo pequenio cian
    {0.0f, 0.0f, 0.0f, false},  // Triangulo mediano purpura
    {0.0f, 0.0f, 0.0f, false}   // Paralelogramo naranja
};

// Variable para almacenar la figura seleccionada actualmente (indice del array)
int figuraActual = 0;

// Funcion para resetear las figuras a su posicion y rotacion inicial
void resetearFiguras() {
    for (int i = 0; i < 7; i++) {
        figuras[i].x = 0.0f;
        figuras[i].y = 0.0f;
        figuras[i].rotacion = 0.0f;
        figuras[i].seleccionada = false;
    }
    // La primera figura queda seleccionada por defecto
    figuraActual = 0;
    figuras[0].seleccionada = true;
}

// Funcion para procesar las entradas del teclado
void procesarEntradas(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// Solo procesa si la tecla fue presionada o repetida
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

	// Valida si se muestra la pantalla de bienvenida
    if (mostrarBienvenida) {
        if (key == GLFW_KEY_ENTER) {
            mostrarBienvenida = false;
        }
    } else {
	    // Variables para controlar la velocidad de movimiento y rotacion
        float velocidadMovimiento = 0.03f;
        float velocidadRotacion = 5.0f;

	    // Procesa segun la tecla presionada
        switch (key) {
            // A. Selecciona las figuras (numeros del 1 al 7)
            case GLFW_KEY_KP_1: // Triangulo grande azul
            case GLFW_KEY_KP_2: // Triangulo grande verde
            case GLFW_KEY_KP_3: // Triangulo pequenio amarillo
            case GLFW_KEY_KP_4: // Cuadrado rojo
            case GLFW_KEY_KP_5: // Triangulo pequenio cian
            case GLFW_KEY_KP_6: // Triangulo mediano purpura
            case GLFW_KEY_KP_7: // Paralelogramo naranja
                figuras[figuraActual].seleccionada = false;
                figuraActual = key - GLFW_KEY_KP_1; // Ajusta el indice de la figura
                figuras[figuraActual].seleccionada = true;
                break;

            // B. Controla la traslacion con las flechas direccionales
            case GLFW_KEY_UP:
                figuras[figuraActual].y += velocidadMovimiento;
                break;
            case GLFW_KEY_DOWN:
                figuras[figuraActual].y -= velocidadMovimiento;
                break;
            case GLFW_KEY_LEFT:
                figuras[figuraActual].x -= velocidadMovimiento;
                break;
            case GLFW_KEY_RIGHT:
                figuras[figuraActual].x += velocidadMovimiento;
                break;

            // C. Controla la rotacion con A:Antihorario y H:Horario
            case GLFW_KEY_A:
                figuras[figuraActual].rotacion += velocidadRotacion;
                break;
            case GLFW_KEY_H:
                figuras[figuraActual].rotacion -= velocidadRotacion;
                break;

            // D. Resetea las figuras a su posicion inicial
            case GLFW_KEY_SPACE:
                resetearFiguras();
                break;
        }
    }
}

/*********************
* CREACION DE TEXTOS *
*********************/

// Funcion para dibujar texto en las figuras
void dibujarTexto(const char* texto, float x, float y, float escala = 1.0f) {
    glPushMatrix();                             // Guarda el estado actual de la matriz
    glTranslatef(x, y, 0.0f);                   // Traslada el texto
    glScalef(escala / 550, escala / 550, 1.0f); // Ajusta la escala del texto

	// Setea el color del texto a blanco
    glColor3f(1.0f, 1.0f, 1.0f);

	// Dibuja el texto caracter por caracter
    for (const char* c = texto; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}

/***************************
* CREACION DE FIGURAS BASE *
***************************/

// Funcion para dibujar un triangulo
void dibujarTriangulo(
    float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    float trasX, float trasY,
    float rotacion,
    const char* texto = nullptr,
    const char* posTexto = nullptr,
    const float variacion = 0.05f) {
	    glPushMatrix();                         // Guarda el estado actual de la matriz 
		glTranslatef(trasX, trasY, 0.0f);       // Traslada el triangulo
		glRotatef(rotacion, 0.0f, 0.0f, 1.0f);  // Rota el triangulo

		// Dibuja el triangulo
        glBegin(GL_TRIANGLES);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glVertex2f(x3, y3);
        glEnd();

        // Dibuja un texto en la figura, si lo hay
        if (texto != nullptr) {
            float centroX = (x1 + x2 + x3) / 3.0f;
            float centroY = (y1 + y2 + y3) / 3.0f;
            
            if (strcmp(posTexto, "sup") == 0) {
                dibujarTexto(texto, centroX, centroY + variacion, 0.15f);
            } else if (strcmp(posTexto, "inf") == 0) {
                dibujarTexto(texto, centroX, centroY - variacion, 0.15f);
            } else if (strcmp(posTexto, "der") == 0) {
                dibujarTexto(texto, centroX + variacion, centroY, 0.15f);
            } else if (strcmp(posTexto, "izq") == 0) {
                dibujarTexto(texto, centroX - variacion, centroY, 0.15f);
            } else if (strcmp(posTexto, "cen") == 0) {
                dibujarTexto(texto, centroX, centroY, 0.15f);
            }
        }

		glPopMatrix();                          // Restaura el estado anterior de la matriz
}

// Funcion para dibujar un poligono cuadrangular
void dibujarPoligonoCuadrangular(
    float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    float x4, float y4,
    float trasX, float trasY,
    float rotacion,
    const char* texto = nullptr,
    const char* posTexto = nullptr,
    const float variacion = 0.05f) {
	    glPushMatrix(); 					    // Guarda el estado actual de la matriz
		glTranslatef(trasX, trasY, 0.0f);	    // Traslada el poligono
		glRotatef(rotacion, 0.0f, 0.0f, 1.0f);  // Rota el poligono

		// Dibuja el poligono
        glBegin(GL_POLYGON);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glVertex2f(x3, y3);
            glVertex2f(x4, y4);
        glEnd();

        // Dibuja un texto en la figura, si lo hay
        if (texto != nullptr) {
            float centroX = (x1 + x2 + x3 + x4) / 4.0f;
            float centroY = (y1 + y2 + y3 + y4) / 4.0f;

            if (strcmp(posTexto, "sup") == 0) {
                dibujarTexto(texto, centroX, centroY + variacion, 0.15f);
            } else if (strcmp(posTexto, "inf") == 0) {
                dibujarTexto(texto, centroX, centroY - variacion, 0.15f);
            } else if (strcmp(posTexto, "der") == 0) {
                dibujarTexto(texto, centroX + variacion, centroY, 0.15f);
            } else if (strcmp(posTexto, "izq") == 0) {
                dibujarTexto(texto, centroX - variacion, centroY, 0.15f);
            } else if (strcmp(posTexto, "cen") == 0) {
                dibujarTexto(texto, centroX, centroY, 0.15f);
            }
        }

		glPopMatrix();                          // Restaura el estado anterior de la matriz
}

// Funcion para dibujar los ejes coordenados
void dibujarEjesCoordenados() {
    glPushMatrix();
    glBegin(GL_LINES);
	    // Eje X
	    glColor3f(0.72f, 0.79f, 0.88f);
	    glVertex2f(-1.0f, 0.0f);
	    glVertex2f(1.0f, 0.0f);

	    // Eje Y
        glColor3f(0.72f, 0.79f, 0.88f);
	    glVertex2f(0.0f, -1.0f);
	    glVertex2f(0.0f, 1.0f);
	glEnd();
	glPopMatrix();
}

/**********************************************
* CREACION DE FIGURAS PRINCIPALES DEL TANGRAM *
**********************************************/

// Funcion para dibujar las figuras del Tangram
void dibujarFigurasTangram() {

    // Trabajamos en la proporcion de 0 a 1 por cuadrante, centro en (0,0)
    float centroX = 0.0f;
    float centroY = 0.0f;
    float escala = 0.5f;

    // Triangulo grande azul
    // Posicion Inicial: (-0.5, 0.5), (0.5, 0.5), (0, 0)
    glColor3f(0.3f, 0.33f, 0.6f);
    dibujarTriangulo(
        centroX - escala, centroY + escala,
        centroX + escala, centroY + escala,
        centroX, centroY,
        figuras[0].x, figuras[0].y,
        figuras[0].rotacion,
		"1",
        "cen"
    );

    // Triangulo grande verde
    // Posicion Inicial: (-0.5, 0.5), (-0.5, -0.5), (0, 0)
    glColor3f(0.56f, 0.75f, 0.43f);
    dibujarTriangulo(
        centroX - escala, centroY + escala,
        centroX - escala, centroY - escala,
        centroX, centroY,
        figuras[1].x, figuras[1].y,
        figuras[1].rotacion,
		"2",
		"cen"
    );

    // Triangulo pequenio amarillo
    // Posicion Inicial: (0.25, 0.25), (0.5, 0.5), (0.5, 0)
    glColor3f(0.98f, 0.78f, 0.31f);
    dibujarTriangulo(
        centroX + escala / 2, centroY + escala / 2,
        centroX + escala, centroY + escala,
        centroX + escala, centroY,
        figuras[2].x, figuras[2].y,
        figuras[2].rotacion,
		"3",
		"cen"
    );

    // Cuadrado rojo
    // Posicion Inicial: (0.25, 0.25), (0.5, 0), (0.25, -0.25), (0, 0)
    glColor3f(0.98f, 0.25f, 0.27f);
    dibujarPoligonoCuadrangular(
        centroX + escala / 2, centroY + escala / 2,
        centroX + escala, centroY,
        centroX + escala / 2, centroY - escala / 2,
        centroX, centroY,
        figuras[3].x, figuras[3].y,
        figuras[3].rotacion,
		"4",
		"cen"
    );

    // Triangulo pequenio cian
    // Posicion Inicial: (0.25, -0.25), (-0.25, -0.25), (0, 0)
    glColor3f(0.26f, 0.67f, 0.55f);
    dibujarTriangulo(
        centroX + escala / 2, centroY - escala / 2,
        centroX - escala / 2, centroY - escala / 2,
        centroX, centroY,
        figuras[4].x, figuras[4].y,
        figuras[4].rotacion,
		"5",
		"cen"
    );

    // Triangulo mediano purpura
    // Posicion Inicial: (0.5, 0), (0.5, -0.5), (0, -0.5)
    glColor3f(0.68f, 0.29f, 0.68f);
    dibujarTriangulo(
        centroX + escala, centroY,
        centroX + escala, centroY - escala,
        centroX, centroY - escala,
        figuras[5].x, figuras[5].y,
        figuras[5].rotacion,
		"6",
		"cen"
    );

    // Paralelogramo naranja
    // Posicion Inicial: (-0.25, -0.25), (0.25, -0.25), (0, -0.5), (-0.5, -0.5)
    glColor3f(0.95f, 0.45f, 0.17f);
    dibujarPoligonoCuadrangular(
        centroX - escala / 2, centroY - escala / 2,
        centroX + escala / 2, centroY - escala / 2,
        centroX, centroY - escala,
        centroX - escala, centroY - escala,
        figuras[6].x, figuras[6].y,
        figuras[6].rotacion,
		"7",
		"cen"
    );
}

/**************************************
* CREACION DE SECCION PLANTILLAS GUIA *
**************************************/

// Funcion para dibujar la seccion de plantillas
void dibujarSeccionPlantillas() {

	glPushMatrix();     // Guarda el estado actual de la matriz
	glLoadIdentity();   // Resetea la matriz a su estado inicial

    // Franja rectangular contenedora
    glColor3f(0.22f, 0.26f, 0.31f);
    dibujarPoligonoCuadrangular(
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, -0.75f,
        -1.0f, -0.75f,
        0.0f, 0.0f,
        0.0f,
		"Plantillas:",
		"izq",
		0.9f
    );

    // Patron de arbol
    glColor3f(0.62f, 0.75f, 0.75f);
    dibujarTriangulo(
        -0.5f, -0.78f,
        -0.58f, -0.86f,
        -0.42f, -0.86f,
        0.0f, 0.0f,
        0.0f
    );
    dibujarTriangulo(
        -0.5f, -0.821f,
        -0.58f, -0.901f,
        -0.42f, -0.901f,
        0.0f, 0.0f,
        0.0f
    );
    dibujarPoligonoCuadrangular(
        -0.52f, -0.901f,
        -0.52f, -0.941f,
        -0.48f, -0.941f,
        -0.48f, -0.901f,
        0.0f, 0.0f,
        0.0f,
		"Arbol",
		"inf",
        0.06f
    );

    // Patron de bote
    glColor3f(0.62f, 0.75f, 0.75f);
    dibujarTriangulo(
        0.48f, -0.78f,
        0.48f, -0.8931f,
        0.3669f, -0.8931f,
        0.0f, 0.0f,
        0.0f
    );
    dibujarTriangulo(
        0.52f, -0.8931f,
        0.48f, -0.82f,
        0.595f, -0.85f,
        0.0f, 0.0f,
        0.0f
    );
    dibujarPoligonoCuadrangular(
        0.4434f, -0.8931f,
        0.5566f, -0.8931f,
        0.52f, -0.9331f,
        0.48f, -0.9331f,
        0.0f, 0.0f,
        0.0f,
		"Bote",
		"inf",
		0.07f
    );

    // Patron de casa
    glColor3f(0.62f, 0.75f, 0.75f);
    dibujarTriangulo(
        -0.02f, -0.81f,
        -0.082f, -0.87f,
        0.042f, -0.87f,
        0.0f, 0.0f,
        0.0f
    );
    dibujarPoligonoCuadrangular(
        0.042f, -0.87f,
        0.082f, -0.87f,
        0.045f, -0.835f,
        0.005f, -0.835f,
        0.0f, 0.0f,
        0.0f);
    dibujarPoligonoCuadrangular(
        0.045f, -0.795f,
        0.005f, -0.795f,
        0.005f, -0.835f,
        0.045f, -0.835f,
        0.0f, 0.0f,
        0.0f
    );
    dibujarPoligonoCuadrangular(
        -0.06f, -0.87f,
        0.06f, -0.87f,
        0.06f, -0.9331f,
        -0.06f, -0.9331f,
        0.0f, 0.0f,
        0.0f,
		"Casa",
		"inf",
		0.08f
    );

    glPopMatrix();      // Restaura el estado anterior de la matriz
}

/**********************************
* CREACION PANTALLA DE BIENVENIDA *
**********************************/

// Funcion para dibujar la pantalla de bienvenida
void dibujarPantallaBienvenida() {
	glEnable(GL_TEXTURE_2D);                            // Habilita el uso de texturas
	glBindTexture(GL_TEXTURE_2D, texturaBienvenida);    // Selecciona la textura a usar
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

	glDisable(GL_TEXTURE_2D);						   // Deshabilita el uso de texturas
}

/*********************************
* FUNCION PRINCIPAL DEL PROGRAMA *
*********************************/

int main() {

	// Inicializa GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Inicializa GLUT
    int argc = 1;
    char* argv[] = { (char*)"Dummy", nullptr };
    glutInit(&argc, argv);

	// Configura la ventana
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // No permite redimensionar la ventana
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Tangram Puzzle - Proyecto Grupo 02", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

	// Establece la ventana como contexto actual
    glfwMakeContextCurrent(window);

	// Inicializa OpenGL
    if (!inicializarOpenGL()) {
        return -1;
    }

	// Invoca la funcion que controla el teclado
    glfwSetKeyCallback(window, procesarEntradas);

	// Bucle principal
    while (!glfwWindowShouldClose(window)) {

        // Limpia el lienzo
        glClear(GL_COLOR_BUFFER_BIT);

        // Establece un color de fondo a todo el lienzo
        glClearColor(0.86f, 0.92f, 0.99f, 0.8f);

		// Valida si se muestra la pantalla de bienvenida o del juego
        if (mostrarBienvenida) {
            dibujarPantallaBienvenida();
        } else {
			// Dibuja las figuras del Tangram y la seccion de plantillas
			dibujarEjesCoordenados();
            dibujarSeccionPlantillas();
            dibujarFigurasTangram();
        }

		// Intercambia los buffers y procesa los eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}