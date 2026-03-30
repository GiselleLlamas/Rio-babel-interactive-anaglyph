#version 120

void main() {
    // Pasamos las coordenadas del vértice directamente 
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}