//------Informática Aplicada 2----------
//------Cátedra: Tirigall---------------
//------Alumna: Giselle Llamas----------
//------TP3 - Título: Río Babel---------

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
ofSetFrameRate( 60 );

// ===== SETUP CÁMARA ANAGLIFO (para modo 3D) =====
cam.setupPerspective(true);
cam.setFov(80.0); // FOV (campo de vista) más amplio para ver mejor la escena
cam.setNearClip(1.0);
cam.setFarClip(10000.0);
cam.setPosition(ofGetWidth()/2, ofGetHeight()/2, 400); // Cámara MUY cerca para objetos grandes
cam.lookAt(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0));
cam.enableStereo();
cam.enableColorMask();

if (cam.isStereo()) {
	ofLogNotice() << "Anaglyph stereo habilitado correctamente!";
} else {
	ofLogError() << "El stereo no se habilito correctamente!";
}

// Iniciamos en modo interactivo (sin cámara, simple 2D)
modoAnaglifo = false;

// Algunos ajustes para la libreria de las letras
ofBackgroundHex(0xfdefc2);
ofSetLogLevel(OF_LOG_NOTICE);
ofSetVerticalSync(true);

// Setup de Box2d
box2d.init();
box2d.setGravity(0, 0);
box2d.createBounds();
box2d.setFPS(60.0);
box2d.registerGrabbing(); // Se habilitará/deshabilitará según el modo

// Crear un cuerpo estático (ground body) para anclar mouse joints en modo anaglifo
b2BodyDef groundBodyDef;
groundBody = box2d.getWorld()->CreateBody(&groundBodyDef);

// Paredes rotativas centrales
float w = 400, h = 300, t = 20;
float cx = ofGetWidth()/2, cy = ofGetHeight()/2;

// Pared de arriba
auto arriba = make_shared<ofxBox2dRect>();
arriba->setPhysics(0.5, 0.5, 0.3);
arriba->setup(box2d.getWorld(), cx, cy - h/2, w, t);
arriba->body->SetType(b2_kinematicBody);
walls.push_back(arriba);

// Pared de abajo
auto abajo = make_shared<ofxBox2dRect>();
abajo->setPhysics(0.5, 0.5, 0.3);
abajo->setup(box2d.getWorld(), cx, cy + h/2, w, t);
abajo->body->SetType(b2_kinematicBody);
walls.push_back(abajo);

// Pared de izquierda
auto izquierda = make_shared<ofxBox2dRect>();
izquierda->setPhysics(0.5, 0.5, 0.3);
izquierda->setup(box2d.getWorld(), cx - w/2, cy, t, h);
izquierda->body->SetType(b2_kinematicBody);
walls.push_back(izquierda);

// Cargo el shader del rio
rio.load("shadersGL2/rio");
if (rio.isLoaded()) {
	ofLogNotice() << "El shader del rio cargo bien!";
} else {
	ofLogError() << "El shader del rio no esta funcionando...";
}

// Cargo el shader del tunel
malla.load("shadersGL2/tunelMalla.vert", "shadersGL2/tunelMalla.frag");
if (malla.isLoaded()) {
	ofLogNotice() << "El shader del rio cargo bien!";
} else {
	ofLogError() << "El shader del rio no esta funcionando...";
}

// Cargo las tipografías
nombresTipografia = {
	"Bungee.ttf",
	"Doto.ttf",
	"Major.ttf",
	"Caesar.ttf",
	"Londrina.ttf",
	"Unifraktur.ttf",
	"Delius.ttf",
};

for (auto &nombre : nombresTipografia) {
	auto tipografia = make_shared<ofTrueTypeFont>();
	bool ok = tipografia->load("tipografia/" + nombre, 20, true, true, true);
	if (ok) {
		tipografias.push_back(tipografia);
		ofLogNotice() << "Tipografía cargada correctamente: " << nombre;
	} else {
		ofLogError() << "No se pudo cargar la tipografía: tipografía/" + nombre;
	}
}

// Seteamos el joystick
miJoystick.setup(GLFW_JOYSTICK_1);

// Seteamos las variables del Librero
float libreroX = ofGetWidth() / 2;
float libreroY = ofGetHeight() / 2;

// Rectangulo central del cuerpo del Librero
cuerpoLibrero = make_shared<ofxBox2dRect>();
cuerpoLibrero->setPhysics(3.0, 0.5, 0.3);
cuerpoLibrero->setup(box2d.getWorld(), libreroX, libreroY, 40, 80); // Más grande: 40x80 en vez de 30x60

// Circulo de la cabeza del Librero
cabezaLibrero = make_shared<ofxBox2dCircle>();
cabezaLibrero->setPhysics(1.0, 0.5, 0.3);
cabezaLibrero->setup(box2d.getWorld(), libreroX, libreroY - 50, 20); // Radio 20 en vez de 15

// Conectamos la cabeza al cuerpo
conectarGirando(cuerpoLibrero->body, cabezaLibrero->body,
				cabezaLibrero->getPosition(),
				-15, 15); // la cabeza puede girar ±15°
	
if (cuerpoLibrero->body == nullptr) {
	ofLogError() << "cuerpoLibrero body es null!";
}
if (cabezaLibrero->body == nullptr) {
	ofLogError() << "cabezaLibrero body es null!";
}

// Agregamos los brazos y piernas al Librero
float anchoBrazo = 10;
float altoBrazo  = 40;
float anchoPierna = 12;
float altoPierna  = 50;

// Brazo izquierdo
brazoIzquierdo = make_shared<ofxBox2dRect>();
brazoIzquierdo->setPhysics(1.5, 0.5, 0.3);
brazoIzquierdo->setup(box2d.getWorld(),
					  libreroX - 30, libreroY - 10,
					  anchoBrazo, altoBrazo);

// Punto de anclaje en el “hombro” (parte superior del brazo)
ofPoint puntoHombroIzq(libreroX - 15, libreroY - 20);

// Conectamos el hombro al costado del cuerpo
conectarGirando(cuerpoLibrero->body, brazoIzquierdo->body,
				puntoHombroIzq, -45, 45);

// Brazo derecho
brazoDerecho = make_shared<ofxBox2dRect>();
brazoDerecho->setPhysics(1.5, 0.5, 0.3);
brazoDerecho->setup(box2d.getWorld(),
					libreroX + 30, libreroY - 10,
					anchoBrazo, altoBrazo);

// Punto de anclaje en el “hombro” (parte superior del brazo)
ofPoint puntoHombroDer(libreroX + 15, libreroY - 20);

// Conectamos el hombro al costado del cuerpo
conectarGirando(cuerpoLibrero->body, brazoDerecho->body,
				puntoHombroDer, -45, 45);

// Pierna izquierda
piernaIzquierda = make_shared<ofxBox2dRect>();
piernaIzquierda->setPhysics(2.0, 0.5, 0.3);
piernaIzquierda->setup(box2d.getWorld(),
					   libreroX - 10, libreroY + 50,
					   anchoPierna, altoPierna);
conectarGirando(cuerpoLibrero->body, piernaIzquierda->body,
				piernaIzquierda->getPosition(), -25, 25);

// Pierna derecha
piernaDerecha = make_shared<ofxBox2dRect>();
piernaDerecha->setPhysics(2.0, 0.5, 0.3);
piernaDerecha->setup(box2d.getWorld(),
					 libreroX + 10, libreroY + 50,
					 anchoPierna, altoPierna);
conectarGirando(cuerpoLibrero->body, piernaDerecha->body,
				piernaDerecha->getPosition(), -25, 25);

// Guardamos el linear damping* original del librero para el modo agujero negro (* es cuánto se frena o reduce la velocidad lineal de un cuerpo en movimiento con el tiempo)
	//Valor bajo (ej: 0.0 - 0.5): El objeto mantiene su velocidad por más tiempo, casi como si estuviera en el espacio sin ninguna fricción del aire
	//Valor alto (ej: 2.0 - 5.0): El objeto se detiene rápidamente, como si estuviera moviéndose en un líquido viscoso o con mucha resistencia del aire
if (cuerpoLibrero && cuerpoLibrero->body) {
	dampeningOriginal = cuerpoLibrero->body->GetLinearDamping();
}

// Calculamos el centro absoluto de la composición (centro de las paredes rotativas)
centroAbsoluto.set(ofGetWidth()/2, ofGetHeight()/2);

// Logueo debug para los brazos y piernas
ofLogNotice() << "brazoIzquierdo pos: " << brazoIzquierdo->getPosition();
ofLogNotice() << "brazoDerecho pos: " << brazoDerecho->getPosition();
ofLogNotice() << "piernaIzquierda pos: " << piernaIzquierda->getPosition();
ofLogNotice() << "piernaDerecha pos: " << piernaDerecha->getPosition();

// Escribimos las palabras para el sistema de particulas:
palabras = {
	"río", "memoria", "fluye", "entre", "nosotros",
	"lenguas", "mezcladas", "y", "voces", "antiguas",
	"que", "resuenan", "en", "el", "tiempo", "yo", "afirmo",
	"que", "la", "biblioteca", "es", "interminable", "el",
	"número", "de", "símbolos", "naturales", "es", "veinticinco",
	"lograr", "un", "lugar", "entre", "las", "dimensiones", "del",
	"infinito", "irrefutable", "la", "inmensidad", "todos", "los",
	"libros", "por", "más", "diversos", "que", "sean", "constan",
	"de", "elementos", "iguales", "el", "espacio", "el", "punto",
	"la","coma", "las", "veintidós", "letras", "del", "alfabeto",
	"no", "hay", "en", "la", "vasta", "biblioteca", "dos", "libros",
	"idénticos", "los", "misterios", "básicos", "de", "la", "humanidad",
	"son", "el", "origen", "de", "la", "biblioteca", "y", "del",
	"tiempo", "llegar", "a", "la", "orilla", "de", "la",
	"corriente", "cómo", "localizar", "el", "venerado", "hexágono",
	"secreto", "la", "biblioteca", "es", "ilimitada", "y", "periódica",
	"fluir", "sin", "un", "fín", "más", "que", "fluir"
};

// Cargamos los sonidos para cada uno de los cuadrantes
for (int i = 0; i < 4; i++) {
	sonidos[i].load("sonidos/c" + ofToString(i+1) + ".mp3");
	sonidos[i].setLoop(true);          // Estan en loop mientras se encuentran activos
	sonidos[i].setMultiPlay(false);    // Que no haya multiplay
}

// Cargamos el sonido especial para el modo centro especial
if (!sonidoCentroEspecial.load("sonidos/a1.mp3")) {
	ofLogError() << "No se pudo cargar el sonido a1.mp3";
} else {
	ofLogNotice() << "Sonido a1.mp3 cargado correctamente";
	sonidoCentroEspecial.setLoop(true);        // Loop habilitado para a1.mp3
	sonidoCentroEspecial.setMultiPlay(false);
}

// Tipografía específica para el instructivo en pantalla (UI)
if (!tipografiaInstructivo.load("tipografia/Caption.ttf", 9, true, true, true)) {
	ofLogError() << "No se pudo cargar la tipografía para el instructivo!";
} else {
	ofLogNotice() << "Se cargó la fuente del instructivo?" << tipografiaInstructivo.isLoaded();
}

// Cargamos los iconos PNG
if (!iconoNormalBase.load("iconos/Normal-Base.png")) {
	ofLogError() << "No se pudo cargar Normal-Base.png";
} else {
	ofLogNotice() << "Icono Normal-Base cargado correctamente";
}

if (!iconoKeyboardAnaglifo.load("iconos/Keyboard-anaglifo.png")) {
	ofLogError() << "No se pudo cargar Keyboard-anaglifo.png";
} else {
	ofLogNotice() << "Icono Keyboard-anaglifo cargado correctamente";
}

if (!iconoJoystickAnaglifo.load("iconos/Joystick-Anaglifo.png")) {
	ofLogError() << "No se pudo cargar Joystick-Anaglifo.png";
} else {
	ofLogNotice() << "Icono Joystick-Anaglifo cargado correctamente";
}

if (!iconoKeyboardAgujero.load("iconos/Keyboard-agujero.png")) {
	ofLogError() << "No se pudo cargar Keyboard-agujero.png";
} else {
	ofLogNotice() << "Icono Keyboard-agujero cargado correctamente";
}

if (!iconoJoystickAgujero.load("iconos/Joystick-agujero.png")) {
	ofLogError() << "No se pudo cargar Joystick-agujero.png";
} else {
	ofLogNotice() << "Icono Joystick-agujero cargado correctamente";
}

if (!iconoKeyboardGravedad.load("iconos/Keyboard-gravedad.png")) {
	ofLogError() << "No se pudo cargar Keyboard-gravedad.png";
} else {
	ofLogNotice() << "Icono Keyboard-gravedad cargado correctamente";
}

if (!iconoJoystickGravedad.load("iconos/Joystick-Gravedad.png")) {
	ofLogError() << "No se pudo cargar Joystick-Gravedad.png";
} else {
	ofLogNotice() << "Icono Joystick-Gravedad cargado correctamente";
}

// Inicializamos el timer del instructivo
tiempoInicioInstructivo = ofGetElapsedTimef();

// Inicializamos el timer de la aplicación para el modo centro especial
tiempoInicioApp = ofGetElapsedTimef();
}

//--------------------------------------------------------------

void ofApp::update() {

// Se esta mapeando la separacion de ojo y la distancia focal en base a la posicion del librero en pantalla
ofVec2f posLibrero = cuerpoLibrero->getPosition();

// Ajustamos la separación de ojos para un efecto 3D
cam.eyeSeparation = ofMap(posLibrero.x, 0, ofGetWidth(), 0.5, 5.0, true);

ofVec2f centroCam(ofGetWidth()/2, ofGetHeight()/2);

float dist = posLibrero.distance(centroCam);
// Ajustamos la distancia focal
cam.focalLength = ofMap(dist, 0, ofGetWidth()/2, 50, 200, true);

// Nos aseguramos que la cámara siempre esté centrada y mirando al centro
cam.setPosition(ofGetWidth()/2, ofGetHeight()/2, 400);
cam.lookAt(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0));

// Creamos la rotacion completa de paredes cada X cantidad de segundos, pero esta vez tambien tomamos del joystick!
float xAxisParedes = miJoystick.getAxis(0); // derecha/izquierda
float movimientoIgnorado = 0.15f; 			// Se define aqui la Tolerancia de movimiento del Joystick
float velocidadMax = 2.0f;        			// Controlamos qué tan rápido rota

// Verificamos si se esta moviendo el joystick (tuve que meter un booleano aca porque si es que no habia input, la caja central nunca iba a rotar y la idea es que rote constantemente!)
bool joystickActivo = fabs(xAxisParedes) > movimientoIgnorado;

if (joystickActivo) {
	// Ajustamos el eje, teniendo en cuenta la tolerancia de movimiento del Joystick
	if (xAxisParedes > 0)
		xAxisParedes = ofMap(xAxisParedes, movimientoIgnorado, 1.0f, 0.0f, 1.0f, true);
	else
		xAxisParedes = ofMap(xAxisParedes, -movimientoIgnorado, -1.0f, 0.0f, -1.0f, true);

	// Invertimos la dirección (signo negativo)
	float velAngular = -xAxisParedes * velocidadMax;

	// Suavizamos el movimiento para que se sienta fluido
	static float movimientoSuavizado = 0.0f;
	movimientoSuavizado = ofLerp(movimientoSuavizado, velAngular, 0.1f);

	// Actualizamos el ángulo
	anguloParedes += movimientoSuavizado;

} else {
	//Esto se activa si no hay input del joystick, que puede pasar
	float periodo = 5.0f; // Definimos los segundos para la rotación completa
	anguloParedes = fmod(ofGetElapsedTimef() * (360.0f / periodo), 360.0f);
}

// Mantener ángulo en el rango de 0–360
if (anguloParedes > 360.0f) anguloParedes -= 360.0f;
	if (anguloParedes < 0.0f) anguloParedes += 360.0f;

// Aplicar la rotación a las paredes Box2D
for (auto &w : walls) {
	if (w && w->body) {
		w->body->SetTransform(b2Vec2(ofGetWidth()/2, ofGetHeight()/2),
							  ofDegToRad(anguloParedes));
	}
}

// Emitir una palabra cada 15 frames (se puede ajustar con estas variables)
if (ofGetFrameNum() % 15 == 0 && indexPalabra < palabras.size()) {
	float px = ofGetWidth() / 2.0;
	float py = ofGetHeight() / 2.0;

	// Formula para la espiral
	float x = px + cos(anguloEspiral) * radioEspiral;
	float y = py + sin(anguloEspiral) * radioEspiral;

	// Creo el sistema de particulas de las palabras
	auto p = make_shared<AbecedarioParticula>(
		box2d.getWorld(), x, y, ofRandom(25, 50)
	);
	p->setWord(palabras[indexPalabra]);
	
	// Asignar fuente aleatoria
	if (!tipografias.empty()) {
		int idx = ofRandom(tipografias.size());
		p->setFontIndex(idx);
	}

	// Fuerza que empuja a las particulas con forma de espiral
	float forceX = cos(anguloEspiral) * 5.0;
	float forceY = sin(anguloEspiral) * 5.0;
	p->addForce(ofVec2f(forceX, forceY), 3.0);

	particles.push_back(p);

	// Incrementamos el angulo, radio de la espiral y sumamos al index de las palabras
	anguloEspiral += 0.4;
	radioEspiral += 6.0;
	indexPalabra++;

	// Reseteamos a indexPalabra para que loopee dentro de si misma en caso de que llegue al final
	if (indexPalabra >= palabras.size()) {
		indexPalabra = 0;
		anguloEspiral = 0.0;
		radioEspiral = 20.0;
	}
}

if (ofGetFrameNum() % 30) {
		
	// Creo un circulo compartido
	auto circle = make_shared<ofxBox2dCircle>();
		
	// Seteo la fisica de dicho circulo
	circle->setPhysics(1, 0.7, 0.7);
		
	// Seteo la posicion y el tamaño del circulo y luego lo agrego al mundo
	circle->setup(box2d.getWorld(), ofGetWidth()/2 + ofRandom(-10, 10), ofGetHeight()/2 + ofRandom(-10, 10), 10);
		
	// lo agrego al vector
	circles.push_back(circle);
		
}

//Aca tomo el eje del Joystick con la libreria ofxJoystick

float moverConJoystick = 1500.0;

float xAxis = miJoystick.getAxis(0); // derecha/izquierda
float yAxis = miJoystick.getAxis(1); // arriba/abajo

// Invertir el eje Y en modo anaglifo porque la cámara invierte la vista
if (modoAnaglifo) {
	yAxis = -yAxis;
}

//Mandar un mensaje a ver si esta tomandolo bien (lineas de debug)
//ofLogNotice() << "xAxis: " << xAxis << "  yAxis: " << yAxis;

if (fabs(xAxis) < movimientoIgnorado) xAxis = 0.0f;
else xAxis = (xAxis - (xAxis > 0 ? movimientoIgnorado : -movimientoIgnorado)) / (1.0f - movimientoIgnorado);

if (fabs(yAxis) < movimientoIgnorado) yAxis = 0.0f;
else yAxis = (yAxis - (yAxis > 0 ? movimientoIgnorado : -movimientoIgnorado)) / (1.0f - movimientoIgnorado);

// Computar la fuerza del vector que le estamos aplicandodesde la direccion del joystick
ofVec2f moverDir(xAxis, yAxis);

// Normalizar asi no es mas rapido en las diagonales
if (moverDir.lengthSquared() > 1e-5)
	moverDir.normalize();

//Agregamos un poco de interpolacion para calmar un toque el movimiento al aplicar esta fuerza continuamente
// PERO solo si el librero no está inmovilizado por el modo centro especial
if (!libreroInmovilizado) {
	static ofVec2f suavizarMovim;
	suavizarMovim.interpolate(moverDir, 0.2); // 0.1–0.3 = ejemplos de rango
	cuerpoLibrero->addForce(suavizarMovim * moverConJoystick, 1.0);
}

// Movimiento discreto con botones direccionales del joystick (solo si el librero no está inmovilizado)
if (!libreroInmovilizado && cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
	float fuerzaDiscreta = 2000.0f; // Fuerza para movimiento discreto
	
	// Invertir dirección Y en modo anaglifo porque la cámara invierte la vista
	float direccionY = modoAnaglifo ? 1.0f : -1.0f;
	
	if (miJoystick.isPressed(14)) { // Arriba
		cuerpoLibrero->addForce(ofVec2f(0, fuerzaDiscreta * direccionY), 1.0);
		ofLog() << "Joystick: Moviendo librero hacia ARRIBA";
	}
	if (miJoystick.isPressed(15)) { // Derecha
		cuerpoLibrero->addForce(ofVec2f(fuerzaDiscreta, 0), 1.0);
		ofLog() << "Joystick: Moviendo librero hacia DERECHA";
	}
	if (miJoystick.isPressed(16)) { // Abajo
		cuerpoLibrero->addForce(ofVec2f(0, -fuerzaDiscreta * direccionY), 1.0);
		ofLog() << "Joystick: Moviendo librero hacia ABAJO";
	}
	if (miJoystick.isPressed(17)) { // Izquierda
		cuerpoLibrero->addForce(ofVec2f(-fuerzaDiscreta, 0), 1.0);
		ofLog() << "Joystick: Moviendo librero hacia IZQUIERDA";
	}
}

// Mantenemos el librero adentro de la composicion para que no desaparezca
if (cuerpoLibrero && cuerpoLibrero->body) {
	auto pos = cuerpoLibrero->getPosition();
	if (pos.x < 0 || pos.x > ofGetWidth() || pos.y < 0 || pos.y > ofGetHeight()) {
		cuerpoLibrero->setPosition(ofGetWidth()/2, ofGetHeight()/2);
		cuerpoLibrero->setVelocity(0, 0);
		ofLogNotice() << "Se reseteo el librero al centro.";
	}
}

// Atraigo el librero al centro de la composición SOLO cuando no hay modos especiales activos
// Esto evita conflictos con las fuerzas de los modos Paredes Magnéticas, Ultimate Agujero Negro y Centro Especial
ofVec2f centro(ofGetWidth()/2, ofGetHeight()/2);
if (!paredesMagneticas && !modoUltimateAgujeroNegro && !libreroInmovilizado) {
	ofVec2f alCentro = (centro - cuerpoLibrero->getPosition());
	float atraccionCentral = 10.0; // Tuve que usar un valor bien bajo para estabilizarlo porque sino casi no me podia mover!
	cuerpoLibrero->addForce(alCentro * 0.01 * atraccionCentral, 1.0);
}

	// ===== MODO PAREDES MAGNÉTICAS / FUERZA MAGNÉTICA =====
	// Cuando está activo, las paredes del polígono central actúan como un imán
	// que atrapa al librero tanto desde adentro como desde afuera
	// IMPORTANTE: Solo se ejecuta si el Ultimate Agujero Negro NO está activo
	if (paredesMagneticas && !modoUltimateAgujeroNegro && cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
	
	// DEBUG LOG
	if (ofGetFrameNum() % 30 == 0) {
		ofLogNotice() << ">>> PAREDES MAGNETICAS: EJECUTANDOSE (paredesMagneticas=" << paredesMagneticas
					  << " | modoUltimateAgujeroNegro=" << modoUltimateAgujeroNegro << ")";
	}
	
		// Aumentamos el damping para que se sienta más "pesado" y atrapado
		if (cuerpoLibrero->body->IsActive()) {
			cuerpoLibrero->body->SetLinearDamping(3.5f);
		}
	
	// Obtenemos la posición del librero
	ofVec2f posLibrero = cuerpoLibrero->getPosition();
	
	// Calculamos las 4 paredes del rectángulo rotado
	float w = 400, h = 300;
	float anguloRad = ofDegToRad(anguloParedes);
	
	// Esquinas del rectángulo en el sistema local (antes de rotar)
	ofVec2f esquinas[4] = {
		ofVec2f(-w/2, -h/2),  // Superior izquierda
		ofVec2f( w/2, -h/2),  // Superior derecha
		ofVec2f( w/2,  h/2),  // Inferior derecha
		ofVec2f(-w/2,  h/2)   // Inferior izquierda
	};
	
	// Rotamos y trasladamos las esquinas al mundo
	ofVec2f esquinasRotadas[4];
	for (int i = 0; i < 4; i++) {
		float x = esquinas[i].x * cos(anguloRad) - esquinas[i].y * sin(anguloRad);
		float y = esquinas[i].x * sin(anguloRad) + esquinas[i].y * cos(anguloRad);
		esquinasRotadas[i] = ofVec2f(x + centro.x, y + centro.y);
	}
	
	// Encontramos el punto más cercano en cada una de las 3 paredes (arriba, abajo, izquierda)
	// Pared de arriba: esquina 0 -> esquina 1
	// Pared de abajo: esquina 2 -> esquina 3
	// Pared izquierda: esquina 0 -> esquina 3
	
	ofVec2f puntoMasCercano;
	float distanciaMinima = 999999.0f;
	
	// Array de paredes: [inicio, fin]
	ofVec2f paredes[3][2] = {
		{esquinasRotadas[0], esquinasRotadas[1]},  // Pared arriba
		{esquinasRotadas[3], esquinasRotadas[2]},  // Pared abajo
		{esquinasRotadas[0], esquinasRotadas[3]}   // Pared izquierda
	};
	
	// Para cada pared, encontramos el punto más cercano al librero
	for (int i = 0; i < 3; i++) {
		ofVec2f A = paredes[i][0];
		ofVec2f B = paredes[i][1];
		
		// Vector de A a B
		ofVec2f AB = B - A;
		// Vector de A al librero
		ofVec2f AP = posLibrero - A;
		
		// Proyección de AP sobre AB (normalizada)
		float t = AP.dot(AB) / AB.dot(AB);
		t = ofClamp(t, 0.0f, 1.0f);  // Mantener dentro del segmento
		
		// Punto más cercano en el segmento
		ofVec2f puntoEnPared = A + AB * t;
		
		// Distancia al punto
		float dist = posLibrero.distance(puntoEnPared);
		
		if (dist < distanciaMinima) {
			distanciaMinima = dist;
			puntoMasCercano = puntoEnPared;
		}
	}
	
	// Aplicamos una fuerza magnética FUERTE hacia el punto más cercano
	ofVec2f direccionMagnetica = puntoMasCercano - posLibrero;
	
	// La fuerza es inversamente proporcional a la distancia (como un imán de verdad)
	// Pero le pongo un mínimo para evitar divisiones por cero
	float distNormalizada = ofClamp(distanciaMinima, 20.0f, 300.0f);
	float intensidad = fuerzaMagnetica / (distNormalizada * 0.5f);
	
	// Aplicamos la fuerza magnética
	cuerpoLibrero->addForce(direccionMagnetica.getNormalized() * intensidad, 1.0);
	
	// Log para debug (opcional)
	if (ofGetFrameNum() % 30 == 0) {
		ofLogNotice() << "Paredes Magneticas activas - Dist: " << distanciaMinima
					  << " Fuerza: " << intensidad;
	}
	
	} else if (!modoUltimateAgujeroNegro) {
		// Restauramos el damping original solo si NINGUNO de los dos modos especiales está activo
		if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
			cuerpoLibrero->body->SetLinearDamping(dampeningOriginal);
		}
	}

//Revisamos la posicion del librero
ofVec2f posicionLibrero = cuerpoLibrero->getPosition();
int libreroW = ofGetWidth();
int libreroH = ofGetHeight();

//Determinamos en que cuadrante está
int c = 0;
if (posicionLibrero.x > libreroW/2 && posicionLibrero.y < libreroH/2) c = 0;      		// Arriba a la derecha
else if (posicionLibrero.x <= libreroW/2 && posicionLibrero.y < libreroH/2) c = 1; 		// Arriba a la izquierda
else if (posicionLibrero.x <= libreroW/2 && posicionLibrero.y >= libreroH/2) c = 2; 	// Abajo a la izquierda
else c = 3;                                   											// Abajo a la derecha

// Si el cuadrante cambió, hacemos fade y pasamos al otro sonido
if (c != cuadranteActual) {
	// Paramos el sonido actual
	if (cuadranteActual != -1 && sonidos[cuadranteActual].isPlaying()) {
		sonidos[cuadranteActual].stop();
		contadorReplaySonidos[cuadranteActual] = 0;
	}
	// Empezamos uno nuevo
	cuadranteActual = c;
	contadorReplaySonidos[c] = 0;
	fadeVolumen = 1.0f;
	fading = false;
	sonidos[c].setVolume(1.0f);
	sonidos[c].play();
}

//Ajustamos el tono de los sonidos mapeandolo al eje del joystick!
float tonoMusica = ofMap(yAxis, -1, 1, 1.5, 0.7, true);
for (int i = 0; i < 4; i++) {
	if (sonidos[i].isPlaying()) sonidos[i].setSpeed(tonoMusica);
}

	//Actualizamos Box2D PRIMERO antes de limpiar partículas
	box2d.update();
	
	// ===== MODO CENTRO ESPECIAL =====
	// La ACTIVACIÓN solo funciona cuando el modo Ultimate Agujero Negro está activo, han pasado 3 segundos,
	// y no está en cooldown (10 segundos después del último uso)
	// Pero el MANEJO Y RESTAURACIÓN debe ejecutarse siempre que modoCentroEspecial esté activo
	bool puedeActivarCentroEspecial = modoUltimateAgujeroNegro &&
									  (ofGetElapsedTimef() - tiempoInicioApp) >= tiempoMinimoApp &&
									  (ofGetElapsedTimef() - tiempoFinCentroEspecial) >= cooldownCentroEspecial;
	
	// Bloque de ACTIVACIÓN - solo si se cumplen todas las condiciones
	if (puedeActivarCentroEspecial && !modoCentroEspecial) {
		
		// Verificar si el librero está en el centro
		if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
			ofVec2f posLibrero = cuerpoLibrero->getPosition();
			ofVec2f centro(ofGetWidth()/2, ofGetHeight()/2);
			float distanciaAlCentro = posLibrero.distance(centro);
			
			// Si el librero está en el centro y no está ya en modo especial
			if (distanciaAlCentro <= toleranciaCentro && !libreroInmovilizado) {
				// Activar modo centro especial
				modoCentroEspecial = true;
				libreroInmovilizado = true;
				tiempoInicioCentro = ofGetElapsedTimef();
				
				// SILENCIAR COMPLETAMENTE todos los sonidos de cuadrantes
				for (int i = 0; i < 4; i++) {
					if (sonidos[i].isPlaying()) {
						sonidos[i].stop();
					}
					sonidos[i].setVolume(0.0f); // Volumen a 0 para asegurar silencio total
				}
				
				// Reproducir sonido especial
				sonidoCentroEspecial.setVolume(1.0f);
				sonidoCentroEspecial.play();
				
				ofLogNotice() << "MODO CENTRO ESPECIAL ACTIVADO! Librero inmovilizado por " << duracionCentroEspecial << " segundos";
			}
		}
	}
	
	// Bloque de MANEJO Y RESTAURACIÓN - se ejecuta siempre que modoCentroEspecial esté activo
	// Esto permite que el modo se restaure correctamente aunque se desactive modoUltimateAgujeroNegro
	if (modoCentroEspecial) {
		float tiempoTranscurrido = ofGetElapsedTimef() - tiempoInicioCentro;
		
		// Si se desactivó modoUltimateAgujeroNegro mientras estaba en modo centro especial,
		// forzamos la restauración inmediata
		bool forzarRestauracion = !modoUltimateAgujeroNegro;
		
		if (!forzarRestauracion && tiempoTranscurrido <= duracionCentroEspecial) {
			// Durante los 15 segundos: oscurecer fondo gradualmente y mantener librero en centro
			float progreso = tiempoTranscurrido / duracionCentroEspecial;
			
			// Fade del background hacia negro (más dramático y rápido)
			if (progreso <= 0.2f) { // primeros 3 segundos
				fadeBackground = ofLerp(1.0f, 0.02f, progreso * 5.0f); // 5.0 = 1/0.2, mucho más oscuro
			} else {
				fadeBackground = 0.02f; // mantener EXTREMADAMENTE oscuro (casi negro total)
			}
			
			// Inmovilizar el librero en el centro
			if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
				ofVec2f centro(ofGetWidth()/2, ofGetHeight()/2);
				cuerpoLibrero->setPosition(centro);
				cuerpoLibrero->setVelocity(0, 0);
				// Aumentar damping extremo para evitar cualquier movimiento
				cuerpoLibrero->body->SetLinearDamping(50.0f);
			}
			
		} else if (!forzarRestauracion) {
			// Después de 15 segundos: restaurar gradualmente
			float tiempoRestauracion = tiempoTranscurrido - duracionCentroEspecial;
			float duracionRestauracion = 3.0f; // 3 segundos para restaurar
			
			if (tiempoRestauracion <= duracionRestauracion) {
				// Restaurar background gradualmente
				float progresoRestauracion = tiempoRestauracion / duracionRestauracion;
				fadeBackground = ofLerp(0.02f, 1.0f, progresoRestauracion);
				
				// Restaurar damping gradualmente
				if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
					float dampingActual = ofLerp(50.0f, dampeningOriginal, progresoRestauracion);
					cuerpoLibrero->body->SetLinearDamping(dampingActual);
				}
				
				// Fade out del sonido especial
				float volumenEspecial = ofLerp(1.0f, 0.0f, progresoRestauracion);
				sonidoCentroEspecial.setVolume(volumenEspecial);
				
			} else {
				// Restauración completa (natural después de 15+3 segundos)
				modoCentroEspecial = false;
				libreroInmovilizado = false;
				fadeBackground = 1.0f;
				tiempoFinCentroEspecial = ofGetElapsedTimef(); // Marcar tiempo de finalización para cooldown
				
				// Detener sonido especial y restaurar volumen de los cuadrantes
				sonidoCentroEspecial.stop();
				for (int i = 0; i < 4; i++) {
					sonidos[i].setVolume(1.0f); // Restaurar volumen de cuadrantes
				}
				
				// Restaurar damping completamente
				if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
					cuerpoLibrero->body->SetLinearDamping(dampeningOriginal);
				}
				
				ofLogNotice() << "MODO CENTRO ESPECIAL FINALIZADO - Sistema restaurado. Cooldown de " << cooldownCentroEspecial << " segundos activado.";
			}
		} else {
			// Restauración FORZADA (se desactivó modoUltimateAgujeroNegro)
			modoCentroEspecial = false;
			libreroInmovilizado = false;
			fadeBackground = 1.0f;
			tiempoFinCentroEspecial = ofGetElapsedTimef();
			
			// Detener sonido especial inmediatamente
			sonidoCentroEspecial.stop();
			for (int i = 0; i < 4; i++) {
				sonidos[i].setVolume(1.0f);
			}
			
			// Restaurar damping
			if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
				cuerpoLibrero->body->SetLinearDamping(dampeningOriginal);
			}
			
			ofLogNotice() << "MODO CENTRO ESPECIAL INTERRUMPIDO - modoUltimateAgujeroNegro desactivado. Sistema restaurado.";
		}
	}
	
	// Marcar partículas para eliminación después de Box2D update
	vector<int> particlesToRemove;
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i] && particles[i]->body && particles[i]->isDead()) {
			particlesToRemove.push_back(i);
		}
	}
	
	// Remover partículas marcadas (desde el final hacia el principio)
	for (int i = particlesToRemove.size() - 1; i >= 0; i--) {
		int index = particlesToRemove[i];
		if (index < particles.size() && particles[index]) {
			particles[index]->destroy();
			particles.erase(particles.begin() + index);
		}
	}

	// Mantener solo 40 particulas de palabras en pantalla para prevenir problemas de performance
	if (particles.size() > 40) {
		int numToRemove = particles.size() - 40;
		for (int i = 0; i < numToRemove; i++) {
			if (particles[i]) {
				particles[i]->destroy();
			}
		}
		particles.erase(particles.begin(), particles.begin() + numToRemove);
	}

// ===== MODO ULTIMATE AGUJERO NEGRO (Tecla 'J') =====
// Este modo ABSORBE TODO hacia el centro absoluto de la composición usando fuerzas normales
if (modoUltimateAgujeroNegro) {
	
	// 1. ABSORBER EL LIBRERO hacia el centro usando fuerzas
	if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
		ofVec2f posLibrero = cuerpoLibrero->getPosition();
		ofVec2f direccionAlCentro = centroAbsoluto - posLibrero;
		float distanciaAlCentro = direccionAlCentro.length();
		
		if (distanciaAlCentro > 5.0) {
			direccionAlCentro.normalize();
			
			// Fuerza moderada y constante hacia el centro
			float fuerza = 150000.0; // Fuerza moderada
			float intensidadLibrero = fuerza / (distanciaAlCentro * 0.5);
			
			// Aplicar la fuerza
			cuerpoLibrero->addForce(direccionAlCentro * intensidadLibrero, 1.0);
			
			// Damping moderado para movimiento suave
			if (cuerpoLibrero->body->IsActive()) {
				cuerpoLibrero->body->SetLinearDamping(3.0f);
			}
			
			// DEBUG LOG
			if (ofGetFrameNum() % 30 == 0) {
				ofLogNotice() << ">>> AGUJERO NEGRO: Pos librero=" << posLibrero
							  << " | CentroAbsoluto=" << centroAbsoluto
							  << " | Distancia=" << distanciaAlCentro
							  << " | Intensidad=" << intensidadLibrero;
			}
		}
	}
	
	// 2. ABSORCIÓN DE TODAS LAS PALABRAS/PARTÍCULAS hacia el centro
	for (auto &p : particles) {
		if (p && p->body && p->body->IsActive()) {
			ofVec2f posParticula = p->getPosition();
			ofVec2f direccionAlCentro = centroAbsoluto - posParticula;
			float distanciaAlCentro = direccionAlCentro.length();
			
			if (distanciaAlCentro > 1.0) {
				direccionAlCentro.normalize();
				
				// Fuerza proporcional al tamaño de la partícula
				float radio = p->getRadius();
				float masa = p->body->GetMass();
				float fuerza = 20000.0 * masa;
				float intensidadParticula = fuerza / (distanciaAlCentro * 0.3);
				intensidadParticula *= ofMap(radio, 10, 60, 0.3, 1.0, true);
				
				// Aplicamos la fuerza de absorción
				p->addForce(direccionAlCentro * intensidadParticula, 1.0);
				
				// Aumentamos el damping de las partículas
				if (p->body->IsActive()) {
					p->body->SetLinearDamping(2.5f);
				}
			}
		}
	}
	
	// Log de debug cada 30 frames
	if (ofGetFrameNum() % 30 == 0) {
		ofLogNotice() << "ULTIMATE AGUJERO NEGRO ACTIVO - Absorbiendo hacia: "
					  << centroAbsoluto << " | Partículas: " << particles.size();
	}
	
} else {
	// Restauramos el damping de las partículas cuando el modo ultimate está desactivado
	for (auto &p : particles) {
		if (p && p->body && p->body->IsActive()) {
			p->body->SetLinearDamping(0.5f);
		}
	}
}

	
	// Seteamos un boton del Joystick para sacar el screenshot (previamente los habia mapeado - Este es un botón con un Circulo blanco de mi Joystick PowerA)
	bool botonScreenshot = miJoystick.isPressed(13);
if (botonScreenshot && !botonScreenshotPrevio) {
	captura.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	captura.save("Fluir_" + ofGetTimestampString() + ".png");
	ofLogNotice() << "Captura de pantalla guardada desde el joystick.";
}

botonScreenshotPrevio = botonScreenshot;

// Ocultamos el instructivo automáticamente después de 10 segundos
if (mostrarInstructivo && (ofGetElapsedTimef() - tiempoInicioInstructivo) > 10.0f) {
	mostrarInstructivo = false;
	ofLogNotice() << "Instructivo ocultado automáticamente después de 10 segundos";
}

// Cambiamos entre modo interactivo y modo anaglifo con el botón "+" del joystick
bool botonModoToggle = miJoystick.isPressed(9); // Botón "+" (Plus)

if (botonModoToggle && !botonModoTogglePrevio) {
	modoAnaglifo = !modoAnaglifo;
	ofLogNotice() << "Modo: " << (modoAnaglifo ? "Anaglifo 3D" : "Interactivo");
}

botonModoTogglePrevio = botonModoToggle;

// Toggle del modo "Paredes Magnéticas" con el botón 'Home' (12) del joystick
bool botonParedesToggle = miJoystick.isPressed(12); // Botón "Home"

if (botonParedesToggle && !botonParedesPrevio) {
	paredesMagneticas = !paredesMagneticas;
	ofLogNotice() << "Modo paredesMagneticas " << (paredesMagneticas ? "ACTIVADO" : "DESACTIVADO") << " desde joystick";
}

botonParedesPrevio = botonParedesToggle;

// Toggle del modo "Ultimate Agujero Negro" con el botón '-' (8) del joystick
// También se puede activar/desactivar con la tecla 'J' del teclado.
// El botón '-' corresponde a miJoystick.isPressed(8).
// Al presionar cualquiera de estos, el modo Ultimate Agujero Negro se alterna.
bool botonAgujeroNegroToggle = miJoystick.isPressed(8); // Botón "-" (Minus)

if (botonAgujeroNegroToggle && !botonAgujeroNegroPrevio) {
	modoUltimateAgujeroNegro = !modoUltimateAgujeroNegro;
	ofLogNotice() << "ULTIMATE AGUJERO NEGRO " << (modoUltimateAgujeroNegro ? "ACTIVADO - Absorbiendo TODO al centro!" : "DESACTIVADO") << " desde joystick";
}

botonAgujeroNegroPrevio = botonAgujeroNegroToggle;

// Animación del fade del instructivo
float targetAlpha = mostrarInstructivo ? 255.0f : 0.0f;
alphaInstructivo = ofLerp(alphaInstructivo, targetAlpha, 0.08f);

}

//--------------------------------------------------------------
void ofApp::draw() {

ofEnableDepthTest();

if (modoAnaglifo) {
	// ===== MODO ANAGLIFO 3D =====
	// En este modo, dibujamos la cámara anaglifo para efecto 3D
	// La física AHORA ES interactiva! Podes agarrar objetos
	
	if( cam.isStereo() ) {
		cam.beginLeft();
	} else {
		cam.begin();
	}
	
	// Dibujamos la escena para el ojo izquierdo (o mono si stereo está deshabilitado)
	dibujarEscena();
	
	// Dibujamos la escena nuevamente para el ojo derecho
	if( cam.isStereo() ) {
		cam.beginRight();
		dibujarEscena();
	}
		
	if( cam.isStereo() ) {
		cam.endStereo();
	} else {
		cam.end();
	}
	
} else {
	// ===== MODO INTERACTIVO =====
	// En este modo, usamos vista 2D ortográfica simple
	// Las coordenadas de Box2D (px) coinciden EXACTAMENTE con las coordenadas de pantalla
	// = Mouse grabbing funciona correctamente y es mucho más fácil interactuar con los objetos
	
	dibujarEscena();
}

ofDisableDepthTest();

// Ya no dibujamos cursor en modo anaglifo porque no hay interacción con mouse

//Prueba antigua del mapeo del Joystick (Debug)
// Aca es donde empiezo a probar cual botón del joystick corresponde a los numeros, segun los logs son 18 en total, voy a chusmear los que mas me interesan para la funcionalidad
// Dibujo del instructivo (UI) en pantalla
if (alphaInstructivo > 5.0f && tipografiaInstructivo.isLoaded()) {
	ofPushStyle();

	string texto =
		"Río Babel - Controles:\n"
		"- Mové al Librero con la palanca izquierda del Joystick!\n"
		"- También podés usar los botones direccionales del Joystick o las flechas del teclado.\n"
		"- Esto también acelera y desacelera la música y hace girar al cuadrado central.\n"
		"- Presioná 'S' o el botón 'Círculo' del Joystick para capturar la pantalla.\n"
		"- También podés agarrar al librero o a las palabras haciéndoles click con tu mouse.\n"
		"- Presioná '+' en el Joystick o la tecla 'M' del teclado para cambiar entre modo Interactivo y modo Anaglifo 3D.\n"
		"- Presioná 'G' o 'Home' del Joystick para activar Paredes Magnéticas.\n"
		"- Presioná 'J' o '-' del Joystick para activar el Agujero Negro.\n"
		"- Mirá los iconos en la esquina inferior derecha para ver qué modos están activos!";
	
	// Coordenadas donde se va a dibujar el texto y la caja
	float xInst = 50;
	float yInst = 50;
	float padding = 14;
	// Dibujamos cada renglon de texto
	ofSetColor(255, (int)alphaInstructivo);
	if (!tipografiaInstructivo.isLoaded()) {
		ofLogError() << "Attempting to draw instructivo before font was loaded!";
		return;
	}
	tipografiaInstructivo.drawString(texto, xInst, yInst);
	// Calculamos el bounding box del texto completo y lo asignamos a la caja
	ofRectangle cajaInst = tipografiaInstructivo.getStringBoundingBox(texto, xInst, yInst);
	// Dibujamos el borde del rectángulo de la caja del Instructivo
	ofNoFill();
	ofSetLineWidth(1.5f);
	ofSetColor(255, (int)alphaInstructivo);
	ofDrawRectangle(cajaInst.x - padding, cajaInst.y - padding,
					cajaInst.width + padding * 2, cajaInst.height + padding * 2);
	ofPopStyle();
}

// ===== SISTEMA DE ICONOS EN PANTALLA =====
ofPushStyle();
ofDisableDepthTest();

// Definimos los márgenes
float margen = 20.0f;
float espacioEntreIconos = 10.0f;
float escalaIcono = 0.225f; // Escala para reducir el tamaño pero mantener proporciones (50% más grande que 0.15)

// Determinamos si mostramos iconos de Keyboard o Joystick (alterna cada 3 segundos)
bool mostrarKeyboard = (int(ofGetElapsedTimef() / 3.0f) % 2 == 0);

// --- ICONO NORMAL-BASE (esquina inferior derecha) ---
float anchoBase = iconoNormalBase.getWidth() * escalaIcono;
float altoBase = iconoNormalBase.getHeight() * escalaIcono;
float xBase = ofGetWidth() - anchoBase - margen;
float yBase = ofGetHeight() - altoBase - margen;

// Normal-Base está activo (opaco) cuando NO estamos en modo anaglifo
int opacidadNormal = (!modoAnaglifo) ? 255 : 51;
ofSetColor(255, 255, 255, opacidadNormal);
iconoNormalBase.draw(xBase, yBase, anchoBase, altoBase);

// --- ICONO ANAGLIFO (arriba del Normal-Base) ---
ofImage* iconoAnaglifo = mostrarKeyboard ? &iconoKeyboardAnaglifo : &iconoJoystickAnaglifo;
float anchoAnaglifo = iconoAnaglifo->getWidth() * escalaIcono;
float altoAnaglifo = iconoAnaglifo->getHeight() * escalaIcono;
float xAnaglifo = ofGetWidth() - anchoAnaglifo - margen;
float yAnaglifo = yBase - altoAnaglifo - espacioEntreIconos;

// Anaglifo está activo (opaco) cuando estamos en modo anaglifo
int opacidadAnaglifo = modoAnaglifo ? 255 : 51;
ofSetColor(255, 255, 255, opacidadAnaglifo);
iconoAnaglifo->draw(xAnaglifo, yAnaglifo, anchoAnaglifo, altoAnaglifo);

// --- ICONO GRAVEDAD (arriba del Anaglifo) ---
ofImage* iconoGravedad = mostrarKeyboard ? &iconoKeyboardGravedad : &iconoJoystickGravedad;
float anchoGravedad = iconoGravedad->getWidth() * escalaIcono;
float altoGravedad = iconoGravedad->getHeight() * escalaIcono;
float xGravedad = ofGetWidth() - anchoGravedad - margen;
float yGravedad = yAnaglifo - altoGravedad - espacioEntreIconos;

// Gravedad está activo (opaco) si paredesMagneticas está activo Y no está el ultimate agujero negro
// Si ultimate agujero negro está activo, gravedad baja a 20%
int opacidadGravedad = (paredesMagneticas && !modoUltimateAgujeroNegro) ? 255 : 51;
ofSetColor(255, 255, 255, opacidadGravedad);
iconoGravedad->draw(xGravedad, yGravedad, anchoGravedad, altoGravedad);

// --- ICONO AGUJERO NEGRO (arriba del Gravedad) ---
ofImage* iconoAgujero = mostrarKeyboard ? &iconoKeyboardAgujero : &iconoJoystickAgujero;
float anchoAgujero = iconoAgujero->getWidth() * escalaIcono;
float altoAgujero = iconoAgujero->getHeight() * escalaIcono;
float xAgujero = ofGetWidth() - anchoAgujero - margen;
float yAgujero = yGravedad - altoAgujero - espacioEntreIconos;

// Agujero Negro está activo (opaco) si modoUltimateAgujeroNegro está activo
// Si está activo, gravedad automáticamente baja a 20% (lógica ya implementada arriba)
int opacidadAgujero = modoUltimateAgujeroNegro ? 255 : 51;
ofSetColor(255, 255, 255, opacidadAgujero);
iconoAgujero->draw(xAgujero, yAgujero, anchoAgujero, altoAgujero);

ofPopStyle();

}

//--------------------------------------------------------------

void ofApp::dibujarEscena(){
// IMPORTANTE: NO usar ofPushView/ofPopView aca porque rompe la proyección de la cámara anaglifo
	
// ===== PASO 1: Dibujar shaders de fondo en 2D (estos siempre son 2D) =====
ofPushStyle();
ofDisableDepthTest();

// Guardar las matrices actuales
ofPushMatrix();

// Cambiar temporalmente a proyección ortográfica 2D para los shaders de fondo
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -1000, 1000);

glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();

// Shader del río (con fade para modo centro especial)
rio.begin();
rio.setUniform1f("time", ofGetElapsedTimef());
rio.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
// Aplicar fade del background para el modo centro especial
ofSetColor(255 * fadeBackground, 255 * fadeBackground, 255 * fadeBackground);
ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
rio.end();

// Restaurar matrices
glPopMatrix();
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
ofPopMatrix();
ofPopStyle();

// Shader de malla con blend mode ADD
ofPushStyle();
ofDisableDepthTest();
ofEnableBlendMode(OF_BLENDMODE_ADD);
ofEnableAlphaBlending();

ofPushMatrix();

// Cambiar temporalmente a proyección ortográfica 2D
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -1000, 1000);

glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();
glTranslatef(0, 0, -50);

ofSetColor(255 * fadeBackground, 255 * fadeBackground, 255 * fadeBackground, 255);
malla.begin();
malla.setUniform1f("tiempo", ofGetElapsedTimef());
malla.setUniform2f("resolucion", ofGetWidth(), ofGetHeight());
ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
malla.end();

// Restaurar matrices
glPopMatrix();
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
ofPopMatrix();

ofDisableBlendMode();
ofDisableAlphaBlending();
ofPopStyle();

// ===== OVERLAY NEGRO PARA MODO CENTRO ESPECIAL =====
// Se dibuja después de los shaders para garantizar el oscurecimiento
if (modoCentroEspecial && fadeBackground < 0.5f) {
	ofPushStyle();
	ofDisableDepthTest();
	
	// Cambiar temporalmente a proyección ortográfica 2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -1000, 1000);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// Dibujar rectángulo negro semi-transparente sobre toda la pantalla
	float overlayAlpha = ofMap(fadeBackground, 1.0f, 0.02f, 0.0f, 200.0f, true); // Más opacidad cuando fadeBackground es menor
	ofSetColor(0, 0, 0, overlayAlpha);
	ofFill();
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	
	// Restaurar matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	ofPopStyle();
}

// ===== PASO 2: Dibujar objetos 3D (Box2D objects) =====
// En modo anaglifo, estos usan la proyección 3D de la cámara
// En modo interactivo, usan coordenadas de pantalla directas

ofEnableDepthTest();

// La cámara anaglifo está posicionada en (width/2, height/2, 1000) mirando a (width/2, height/2, 0)
// Los objetos Box2D se dibujan en sus coordenadas de píxeles naturales
// En modo interactivo (2D), las coordenadas coinciden perfectamente con la pantalla
// En modo anaglifo (3D), la cámara ve los objetos en el plano Z=0

	// En modo anaglifo, aseguramos que todo esté en Z=0 para que la cámara lo vea bien
	if (modoAnaglifo) {
		ofPushMatrix();
		// NO aplicamos ninguna transformación - dejamos que la cámara haga su trabajo
	}
	
	// Dibujamos las paredes centrales (visual)
	ofPushStyle();
	ofSetColor(ofRandom(128, 255),ofRandom(128, 255),ofRandom(128, 255));
	ofSetLineWidth(4);
	ofNoFill();
	
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 0); // Explícitamente Z=0
	ofRotateDeg(anguloParedes);
	float w = 400, h = 300, t = 20;

// Dibujamos los bordes del objeto poligonal central que va a rotar (mi idea era que fuera como un libro con una pared abierta, por eso es rectangular)
ofDrawLine(-w/2, -h/2,  w/2, -h/2); // Pared de arriba
ofDrawLine(-w/2,  h/2,  w/2,  h/2); // Pared de abajo
ofDrawLine(-w/2, -h/2, -w/2,  h/2); // Pared de la izquierda
// Lo deje abierto para que las letras puedan escapar!

ofPopMatrix();
ofPopStyle();

// Dibujamos los cuerpos de Box2D de las paredes para que tengan colisiones reales
ofPushStyle();
ofSetColor(255, 0, 0, 100); // Rojo semi-transparente para debug
ofFill();
for (auto &wall : walls) {
	if (wall && wall->body) {
		wall->draw();
	}
}
ofPopStyle();

// Dibujo la representacion abstracta del librero para referencia de los reflejos
ofPushMatrix();
ofPushStyle();
ofDisableBlendMode();

// En modo anaglifo, aseguramos que el librero esté exactamente en Z=0
if (modoAnaglifo) {
	glTranslatef(0, 0, 0); // Forzamos Z=0
}

// Le ajusto a color blanco con un poco de transparencia
ofSetColor(255, 255, 255, 180);

if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) cuerpoLibrero->draw();
if (cabezaLibrero && cabezaLibrero->body && cabezaLibrero->body->IsActive()) cabezaLibrero->draw();
if (brazoIzquierdo && brazoIzquierdo->body && brazoIzquierdo->body->IsActive()) brazoIzquierdo->draw();
if (brazoDerecho && brazoDerecho->body && brazoDerecho->body->IsActive()) brazoDerecho->draw();
if (piernaIzquierda && piernaIzquierda->body && piernaIzquierda->body->IsActive()) piernaIzquierda->draw();
if (piernaDerecha && piernaDerecha->body && piernaDerecha->body->IsActive()) piernaDerecha->draw();

ofPopStyle();
ofPopMatrix();

//Aca dibujo los reflejos, distinguiendolos del librero que solamente es lo que puedo controlar con el joystick
//Se llama dibujo anti null porque tuve un problema con el null y tuve que ajustarlo con shared_ptr para asegurarme de que estaba tomando 'cuerpoLibrero', etc... bien!
auto dibujoAntiNull = [](shared_ptr<ofxBox2dBaseShape> s) {
	if (s && s->body && s->body->IsActive()) s->draw();
};

ofVec2f centroLibrero(ofGetWidth()/2, ofGetHeight()/2);

// Cuantas veces se ve reflejado
int reflejos = 7;

// Cuan mas chiquititos se ven su reflejos
float escalaEspejo = 0.70;

// Cuanto el color se difumina en cada iteracion del loop
float ajustarAlpha = 35;

for (int i = 0; i < reflejos; i++) {
	float angulo = ofMap(i, 0, reflejos, 0, TWO_PI); // Distribuir ángulos
	ofPushMatrix();
	ofPushStyle();

	// Mover al centro, espejar y escalar para afuera
	ofTranslate(centroLibrero);
	float espejo = (i % 2 == 0) ? 0.85 : -0.85;   // transformar cada iteracion de los reflejos
	ofScale(espejo * pow(escalaEspejo, i+0.20), pow(escalaEspejo, i+0.20));

	// Mover de vuelta al centro para que el punto de origen sea correcto
	ofTranslate(-centroLibrero);
	
	// Aplicar rotación radial
	ofRotateRad(angulo);
	
	// Estoy armando el primer reflejo transparente
	int alpha = (i == 0) ? 0 : ofClamp(255 - i * ajustarAlpha, 40, 255);
	
	// Colores random del segundo en adelante, si, pero bajando la opacidad por cada reflejo!
	ofColor colorReflejo(ofRandom(128, 255),ofRandom(128, 255),ofRandom(128, 255));
	ofSetColor(colorReflejo, ofClamp(255 - i * ajustarAlpha, 40, 255));
	
	ofFill();
	ofSetColor(colorReflejo, ofClamp(255 - i * ajustarAlpha, 40, 255));
	
	// Dibujamos los mismos cuerpos box2D
	
	dibujoAntiNull(brazoIzquierdo);
	dibujoAntiNull(brazoDerecho);
	dibujoAntiNull(piernaIzquierda);
	dibujoAntiNull(piernaDerecha);
	dibujoAntiNull(cuerpoLibrero);
	dibujoAntiNull(cabezaLibrero);
	
	ofPopStyle();
	ofPopMatrix();
}

// Ahora dibujo el librero principal de nuevo, para que aparezca arriba de los reflejos
ofPushStyle();
ofDisableBlendMode();
// Color blanco con (255)
ofSetColor(255, 255, 255, 180);

if (cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) cuerpoLibrero->draw();
if (cabezaLibrero && cabezaLibrero->body && cabezaLibrero->body->IsActive()) cabezaLibrero->draw();
if (brazoIzquierdo && brazoIzquierdo->body && brazoIzquierdo->body->IsActive()) brazoIzquierdo->draw();
if (brazoDerecho && brazoDerecho->body && brazoDerecho->body->IsActive()) brazoDerecho->draw();
if (piernaIzquierda && piernaIzquierda->body && piernaIzquierda->body->IsActive()) piernaIzquierda->draw();
if (piernaDerecha && piernaDerecha->body && piernaDerecha->body->IsActive()) piernaDerecha->draw();

ofPopStyle();

// Se dibujan las particulas de lasLetras
ofPushStyle();
ofDisableDepthTest();
ofEnableAlphaBlending();

// En modo anaglifo, invertimos el eje Y para que el texto no aparezca al revés
if (modoAnaglifo) {
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 0);
	ofScale(1, -1, 1); // Flip Y axis
	ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2, 0);
}

for (auto &p : particles) {
	if (p->indiceTipografia >= 0 && p->indiceTipografia < tipografias.size()) {
		p->draw(*tipografias[p->indiceTipografia]);
	}
}

if (modoAnaglifo) {
	ofPopMatrix();
}

ofDisableAlphaBlending();
ofPopStyle();
	
// Dibujamos los otros circulos (que van a aparecer como lineas) - Surgen desde: El punto central de la pantalla
ofPushStyle();
ofDisableDepthTest();
ofEnableAlphaBlending();
for(auto &circle : circles) {
	float radius = circle->getRadius();
	auto pos = circle->getPosition();
	auto angle = circle->getRotation();
	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(pos);
	ofRotateDeg(angle);
	ofDrawLine(-radius, 0, radius, 0);
	ofPopMatrix();
}

//Si llegan a mas de 500 lineas dibujadas, se borran para no causar lag
if (circles.size() > 500) {
	// Destruir el cuerpo de Box2D antes de eliminar el círculo
	if (circles[0] && circles[0]->body && circles[0]->body->IsActive()) {
		circles[0]->destroy();
	}
	circles.erase(circles.begin());
}

ofPopStyle();

// Cerramos la transformación del modo anaglifo si estaba abierta
if (modoAnaglifo) {
	ofPopMatrix();
}

}

//--------------------------------------------------------------

void ofApp::conectarGirando(b2Body* a, b2Body* b, const ofPoint& anchor, float anguloMin, float anguloMax) {
if (!a || !b) {
	ofLogError() << "conectarGirando: Uno de los cuerpos es null";
	return;
}

b2RevoluteJointDef definirConexion;
b2Vec2 worldAnchor = ofxBox2d::toB2d(anchor);
definirConexion.Initialize(a, b, worldAnchor);
definirConexion.collideConnected = false;
definirConexion.enableLimit = true;
definirConexion.lowerAngle = ofDegToRad(anguloMin);
definirConexion.upperAngle = ofDegToRad(anguloMax);

box2d.getWorld()->CreateJoint(&definirConexion);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
// Toggle entre modo interactivo y anaglifo con la tecla 'M' ó '+' del joystick
if (key == 'm' || key == 'M' ) {
	modoAnaglifo = !modoAnaglifo;
	ofLogNotice() << "Modo cambiado a: " << (modoAnaglifo ? "Anaglifo 3D" : "Interactivo");
}

// Toggle del modo "paredesMagneticas" con la tecla 'G' ó 'Home' del joystick
if (key == 'g' || key == 'G' ) {
	paredesMagneticas = !paredesMagneticas;
	ofLogNotice() << "Modo paredesMagneticas " << (paredesMagneticas ? "ACTIVADO" : "DESACTIVADO");
}

// Toggle del modo "agujeroNegro" con la tecla 'J' ó '-' del Joystick
if (key == 'j' || key == 'J' ) {
	modoUltimateAgujeroNegro = !modoUltimateAgujeroNegro;
	ofLogNotice() << "ULTIMATE AGUJERO NEGRO " << (modoUltimateAgujeroNegro ? "ACTIVADO - Absorbiendo TODO al centro!" : "DESACTIVADO");
}

// Screenshot con la tecla 'S' ó 'Circulo blanco' del Joystick
if (key == 's' || key == 'S' ) {
	captura.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	captura.save("Fluir_" + ofGetTimestampString() + ".png");
	ofLogNotice() << "Captura de pantalla guardada desde el teclado.";
}

// Movimiento discreto con teclas de flecha (solo si el librero no está inmovilizado)
if (!libreroInmovilizado && cuerpoLibrero && cuerpoLibrero->body && cuerpoLibrero->body->IsActive()) {
	float fuerzaDiscreta = 2000.0f; // Fuerza para movimiento discreto
	
	if (key == OF_KEY_UP) {
		cuerpoLibrero->addForce(ofVec2f(0, -fuerzaDiscreta), 1.0);
		ofLog() << "Teclado: Moviendo librero hacia ARRIBA";
	}
	else if (key == OF_KEY_RIGHT) {
		cuerpoLibrero->addForce(ofVec2f(fuerzaDiscreta, 0), 1.0);
		ofLog() << "Teclado: Moviendo librero hacia DERECHA";
	}
	else if (key == OF_KEY_DOWN) {
		cuerpoLibrero->addForce(ofVec2f(0, fuerzaDiscreta), 1.0);
		ofLog() << "Teclado: Moviendo librero hacia ABAJO";
	}
	else if (key == OF_KEY_LEFT) {
		cuerpoLibrero->addForce(ofVec2f(-fuerzaDiscreta, 0), 1.0);
		ofLog() << "Teclado: Moviendo librero hacia IZQUIERDA";
	}
}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	// No permitimos agarrar en modo anaglifo (no se pueden agarrar cosas)
	if (modoAnaglifo) {
		return;
	}
	
	// En modo interactivo, Box2D maneja el agarre automáticamente
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	// El modo anaglifo es para visualización 3D, el modo interactivo con mouse es para agarrar objetos
	if (modoAnaglifo) {
		ofLogNotice() << "El modo anaglifo es para visualización 3D - cambiar a modo normal (apretar M) para agarrar objetos";
		return;
	}
	// En modo interactivo (Normal), las coordenadas coinciden perfectamente
	// y registerGrabbing() funciona automáticamente (Box2D)
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	// En modo anaglifo no hay grabbing, así que no hay nada que liberar
	if (modoAnaglifo) {
		return;
	}
	// En modo interactivo, Box2D maneja esto automáticamente
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}
