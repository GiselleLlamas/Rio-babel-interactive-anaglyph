#version 120

uniform float time;
uniform vec2 resolution;

// Creamos un constructor para colores random del fondo
vec3 randomColor(float seed) {
	vec3 base = vec3(fract(sin(seed * 12.9898) * 43758.5453),
					 fract(sin(seed * 78.233)  * 43758.5453),
					 fract(sin(seed * 45.164)  * 43758.5453));
	return mix(vec3(0.4), vec3(1.0), base); // Mantenemos los colores claritos
}

// Estoy armando la forma del rio usando una sin (sinusoidal) que va a tomar una amplitud desde el tiempo
float wave(vec2 uv, float speed, float scale, float offset) {
	return sin((uv.x * scale + offset) + time * speed);
}

void main() {
	// Normalizamos las coordenadas de los pixeles (0–1)
	vec2 uv = gl_FragCoord.xy / resolution.xy;

	// Armamos el gradiente de color del fondo:
	float t = floor((time + 0.1) / 10.0) + 1.0;

	vec3 c1 = randomColor(t * 1.1);
	vec3 c2 = randomColor(t * 2.2);
	vec3 c3 = randomColor(t * 3.3);
	vec3 c4 = randomColor(t * 4.4);

	vec3 topMix = mix(c1, c2, smoothstep(0.0, 1.0, uv.x));
	vec3 bottomMix = mix(c3, c4, smoothstep(0.0, 1.0, uv.x));
	vec3 bgColor = mix(bottomMix, topMix, smoothstep(0.0, 1.0, uv.y));

	// Armamos la mascara del centro del río
	vec2 flowUV = uv * 2.0 - 1.0;              // remapeamos a [-1,1]
	flowUV.x *= resolution.x / resolution.y;   // mantenemos las proporciones segun la resolución

	// Combinamos estas tres ondas sinusoidales
	float w1 = wave(flowUV, 0.5, 4.0, 0.0);
	float w2 = wave(flowUV, 0.7, 6.0, 1.5);
	float w3 = wave(flowUV, 0.3, 3.0, 4.0);
	float combinadas = (w1 + w2 + w3) / 3.0;

	// Creamos el area central de la corriente del río en blanco
	float corriente = 1.0 - smoothstep(0.10, 0.12, abs(combinadas - flowUV.y * 1.5));

	// La mezclamos
	vec3 color = mix(bgColor, vec3(1.0), corriente);

	gl_FragColor = vec4(color, 1.0);
}

