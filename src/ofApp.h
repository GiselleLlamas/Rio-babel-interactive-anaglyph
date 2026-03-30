//------Informática Aplicada 2----------
//------Cátedra: Tirigall---------------
//------Alumna: Giselle Llamas----------
//------TP3 - Título: Río Babel---------

#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxCameraAnaglyph.h"
#include "ofxJoystick.h"
#include "GLFW/glfw3.h"

class Letras {
public:
	ofColor color;
	string  name;
	int		id;
};

// Creamos esta clase de particulas customizadas (ahora son palabras!)
// Declarando variables y clases publicas para el sistema de particulas que vamos a llamar en ofApp.cpp
	
class AbecedarioParticula : public ofxBox2dCircle {

public:
		
		string palabra;       	// La palabra de la particula
		ofColor color;        	// Color base
		float tiempoDeVida;     // El tiempo que le queda antes de desaparecer (0–255)
		float velocidadFade;    // La velocidad con la que desaparece
		int indiceTipografia; 	// Indice del vector de tipografias

		AbecedarioParticula(b2World * world, float x, float y, float radius) {
			setPhysics(1.0, 0.5, 0.3);
			setup(world, x, y, radius);
			setVelocity(ofRandom(-2, 2), ofRandom(-2, 2));

			// Color claro pero de tono random por cada palabra (no cada frame, sino cada iteración de palabra)
			color.setHsb(ofRandom(255), 180, 255);
			
			// Completamente opaco al principio
			tiempoDeVida = 255.0f;
			velocidadFade = ofRandom(0.3, 1.0);  // Velocidad de desaparecimiento (fade) random
			indiceTipografia = 0;
		}

		// Ponemos acá la palabra con su índice de tipografía
		void setWord(const string &w) { palabra = w; }
		void setFontIndex(int idx) { indiceTipografia = idx; }

		// Actualizamos el tiempo de vida por cada frame
		bool isDead() {
			tiempoDeVida -= velocidadFade;
			return tiempoDeVida <= 0;
		}

		void draw(ofTrueTypeFont &tipografia) {
			ofPushMatrix();
			ofTranslate(getPosition());
			ofRotateDeg(getRotation());

			// Apagamos el alpha de las palabras de a poco
			ofColor colorApagado = color;
			colorApagado.a = ofClamp(tiempoDeVida, 15, 255);

			// Dibujamos el texto escalado al tamaño de la partícula y aplicamos la tipografia
			ofPushStyle();
			ofSetColor(colorApagado);
			float tamanioTexto =  10.0f; // tamaño default
			float escala = ofMap(getRadius(), 10, 60, 0.5, 2.0, true); //ajustamos el tamaño de la fuente basado en el radio de las particulas

			ofPushMatrix();
			ofScale(escala, escala);

			float x = -tipografia.stringWidth(palabra) / 2.0f;
			float y = tipografia.stringHeight(palabra) / 2.0f;
			tipografia.drawString(palabra, x, y);

			ofPopMatrix();
			ofPopStyle();
			ofPopMatrix();
		}
};

// ------------------------------------------------- App

class ofApp : public ofBaseApp {
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void conectarGirando(b2Body* a, b2Body* b, const ofPoint& anchor, float anguloPiernaAbajo, float anguloPiernaArriba); //Metodo para conectar las extremidades (segundo intento)
	void dibujarEscena(); //Nueva funcion para encapsular el código de la escena
	
	ofxCameraAnaglyph 								cam;
	ofxBox2d                            			box2d;
	vector<shared_ptr<AbecedarioParticula>> 		particles;
	vector<string> 									palabras;
	float 											anguloEspiral = 0.0; 	// El angulo de direccion de la espiral de las particulas
	float 											radioEspiral = 20.0; 	// El radio de la espiral
	int 											indexPalabra = 0; 		// Esta variable cuenta el index de las palabras que van rotando
	vector<shared_ptr<ofTrueTypeFont>> 				tipografias; 			// Vector para las tipografias
	vector<string> 									nombresTipografia; 		// Vector para cargar y manipular las asignaciones de tipografias (sus nombres)
	vector <shared_ptr<ofxBox2dCircle>> 			circles;
	vector<shared_ptr<ofxBox2dRect>> 				walls;
	float 											anguloParedes = 0.0f; 	// Inicializo el angulo de las paredes
	ofImage 										captura; 				// Este es un objeto 'ofImage' que vamos a usar para guardar la captura de pantalla
	ofShader 										rio; 					// Este es el shader que voy a usar, llamado 'rio'
	ofShader 										malla; 					// Este es el nuevo shader que estoy usando para simular una malla en el espacio en forma de túnel
	ofxJoystick 									miJoystick; 			// Le pongo un nombre a mi Joystick para su configuracion
	float 											joystickX = 0.0;
	float 											joystickY = 0.0;
	//Agrego variables globales para armar al muñeco Librero
	shared_ptr<ofxBox2dRect> 	cuerpoLibrero;
	shared_ptr<ofxBox2dCircle> 	cabezaLibrero;
	shared_ptr<ofxBox2dRect> 	brazoIzquierdo;
	shared_ptr<ofxBox2dRect> 	brazoDerecho;
	shared_ptr<ofxBox2dRect> 	piernaIzquierda;
	shared_ptr<ofxBox2dRect> 	piernaDerecha;
	//Cargamos los sonidos y las variables para la manipulacion de los mismos
	ofSoundPlayer 	sonidos[4];
	int 			contadorReplaySonidos[4] = {0,0,0,0};
	int 			cuadranteActual = -1;
	float 			fadeVolumen = 1.0f;
	bool 			fading = false;
	//Variables agregadas para manejar el tema del instructivo on screen
	bool 			mostrarInstructivo = true;
	ofTrueTypeFont 	tipografiaInstructivo;
	bool 			botonScreenshotPrevio = false;  	// Variable para trackear el estado de la toma de la captura
	float 			alphaInstructivo = 255.0f;     		// Para la animacion del fade del instructivo
	float 			tiempoInicioInstructivo = 0.0f;		// Timer para ocultar el instructivo automaticamente
	
	// Iconos para mostrar los modos en pantalla
	ofImage 		iconoNormalBase;
	ofImage 		iconoKeyboardAnaglifo;
	ofImage 		iconoJoystickAnaglifo;
	ofImage 		iconoKeyboardAgujero;
	ofImage 		iconoJoystickAgujero;
	ofImage 		iconoKeyboardGravedad;
	ofImage 		iconoJoystickGravedad;
	
	// Variables para el modo de visualización
	bool 			modoAnaglifo = false;               // false = modo interactivo, true = modo anaglifo 3D
	bool 			botonModoTogglePrevio = false;      // Seguimos el estado del boton del toggle de modo
	
	// Variables para mouse grabbing manual en modo anaglifo
	b2MouseJoint* 	mouseJoint = nullptr;               // Joint para agarrar objetos con el mouse
	b2Body* 		groundBody = nullptr;               // Cuerpo estático para anclar el mouse joint
	ofVec2f 		lastClickWorldPos;                  // Última posición de click transformada (para debug)
	bool 			showClickDebug = false;              // Mostrar debug del último click
	
	// Variables para el modo "Paredes Magnéticas" / Fuerza Magnética
	bool 			paredesMagneticas = false;           // Estado del modo paredes magnéticas
	float 			fuerzaMagnetica = 15000.0f;          // Intensidad de la atracción magnética (aumentada 30%)
	float 			dampeningOriginal = 0.0f;           // Guardamos el linear damping original del librero
	bool 			botonParedesPrevio = false;         // Seguimos el estado del botón Home (12) del joystick
	
	// Variables para el ULTIMATE Agujero Negro (tecla 'J')
	bool 			modoUltimateAgujeroNegro = false;   // Estado del modo ultimate agujero negro
	float 			fuerzaUltimateMagnetica = 50000.0f; // Fuerza extrema para simular un agujero negro
	ofVec2f 		centroAbsoluto;                     // Centro exacto de la composición (punto de absorción)
	bool 			botonAgujeroNegroPrevio = false;    // Seguimos el estado del botón '-' (8) del joystick
	
	// Variables para el Modo Centro Especial
	bool 			modoCentroEspecial = false;         // Estado del modo centro especial
	float 			tiempoInicioApp = 0.0f;             // Tiempo de inicio de la aplicación
	float 			tiempoInicioCentro = 0.0f;          // Tiempo cuando se detectó que el librero está en el centro
	float 			toleranciaCentro = 50.0f;           // Tolerancia en pixeles para considerar que está en el centro (aumentado para modo anaglifo)
	float 			duracionCentroEspecial = 15.0f;     // Duración del modo centro especial (15 segundos)
	float 			tiempoMinimoApp = 3.0f;             // Tiempo mínimo de app (3 segundos) antes de permitir centro especial
	float 			cooldownCentroEspecial = 10.0f;     // Cooldown de 10 segundos después del modo centro especial
	float 			tiempoFinCentroEspecial = 0.0f;     // Tiempo cuando terminó el último modo centro especial
	ofSoundPlayer 	sonidoCentroEspecial;               // Sonido a1.mp3 para el modo centro especial
	float 			fadeBackground = 1.0f;              // Factor de fade del background (1.0 = normal, 0.0 = negro)
	bool 			libreroInmovilizado = false;        // Estado de inmovilización del librero
};
