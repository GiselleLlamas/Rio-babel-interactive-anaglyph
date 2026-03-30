#version 120

uniform float tiempo;
uniform vec2 resolucion;

void main() {
	// Normalizo el espacio del sketch (-1 a 1)
	vec2 uv = (gl_FragCoord.xy / resolucion.xy) * 2.0 - 1.0;
	uv.x *= resolucion.x / resolucion.y; // conservamos el aspecto

	// Convierto a coordenadas polares
	float radio = length(uv);
	float angulo = atan(uv.y, uv.x);

	// Animo la profundidad usando el tiempo y el radio
	float velocidad = 0.5;
	float profundidad = fract(tiempo * velocidad + radio * 1.5);

	// Genero los anillos
	float lineasCirculos = smoothstep(0.01, 0.0, abs(fract(radio * 10.0 - tiempo * 2.0) - 0.5));

	// Genero las líneas
	float lineasRadiales = smoothstep(0.015, 0.0, abs(fract((angulo / 3.14159) * 12.0) - 0.5));

	// Combino a los dos en la malla
	float malla = max(lineasCirculos, lineasRadiales);

	// Hago que parezca cilíndrico difuminándolos
	float borde = smoothstep(1.0, 0.3, radio);

	// Ajusto la intensidad final de la malla con el componente borde
	float brillo = malla * borde;

	// Hago que a traves del brillo, aparezca blanco en ciertas partes, y transparente en otras
	gl_FragColor = vec4(vec3(1.0), brillo);
}
