# Río Babel

**Autora:** Giselle Llamas  
**Cátedra de la UNA:** Tirigall - Informática Aplicada 2
**Universidad: UNA [Claustro de Artes Multimediales]:**  Link: [UNA (Buenos Aires, Argentina)](https://una.edu.ar/)

---

## Descripción

**Río Babel** es una experiencia artística interactiva desarrollada en C++ con openFrameworks. Inspirada en el concepto de la biblioteca infinita de Jorge Luis Borges, la obra explora la fluidez de la memoria y el lenguaje a través de un entorno dinámico y multisensorial.

La aplicación combina elementos 2D y 3D (anaglifo), física de partículas, shaders GLSL, y un paisaje sonoro reactivo para crear un "río" de palabras en constante movimiento. El usuario controla a un personaje abstracto, "El Librero", navegando por este espacio y alterando su entorno a través de sus acciones.

## Características Principales

- **Doble Modo de Visualización:**
  - **Modo Interactivo (2D):** Una vista cenital clásica donde se puede interactuar directamente con los objetos usando el mouse.
  - **Modo Anaglifo (3D):** Una experiencia estereoscópica que requiere gafas 3D rojo-cian. La profundidad y separación de los ojos se ajustan dinámicamente según la posición del personaje.

- **Física Interactiva:**
  - Utiliza **Box2D** para simular la física de todos los elementos: el personaje, las paredes y un sistema de partículas de palabras.
  - El personaje ("El Librero") es un muñeco de trapo (ragdoll) compuesto por varias formas unidas por articulaciones.

- **Visuales Generativos y Shaders:**
  - Un fondo dinámico generado por un shader GLSL que simula un "río" de ruido.
  - Un segundo shader superpuesto crea un efecto de "túnel de malla" para añadir profundidad.
  - Las palabras del texto de Borges son emitidas como partículas físicas en una formación en espiral.
  - Efecto de "espejo" que crea múltiples reflejos caleidoscópicos del personaje.

- **Paisaje Sonoro Reactivo:**
  - El espacio está dividido en cuatro cuadrantes, cada uno con su propia pista de audio en bucle.
  - El sonido cambia a medida que el jugador se mueve entre cuadrantes.
  - La velocidad (pitch) de la música se controla con el movimiento vertical del joystick.

- **Modos de Juego Especiales:**
  - **Paredes Magnéticas:** Las paredes centrales atraen al personaje, creando una sensación de estar atrapado en una corriente.
  - **Agujero Negro Definitivo:** Un modo que genera una fuerza de atracción masiva en el centro de la pantalla, absorbiendo todas las partículas y al personaje.
  - **Modo Centro Especial:** Un evento cinemático que se activa bajo ciertas condiciones, inmovilizando al personaje en el centro, cambiando la música y oscureciendo la escena.

## Controles

| Acción | Teclado | Joystick |
| :--- | :---: | :---: |
| **Mover Personaje** | Teclas de Flecha | Palanca Izquierda / D-Pad |
| **Alternar Modo 2D/3D** | `M` | `+` (Plus) |
| **Activar Paredes Magnéticas** | `G` | `Home` |
| **Activar Agujero Negro** | `J` | `-` (Minus) |
| **Captura de Pantalla** | `S` | `○` (Círculo / Botón Este) |
| **Agarrar Objetos** | Click y Arrastrar | 

*Nota: Agarrar objetos con el mouse solo es posible en el modo Interactivo (2D).*

## Requisitos

Este proyecto fue desarrollado con **openFrameworks** en **Windows** y requiere:

1. **Visual Studio 2022 Community** con el workload:
  - `Desktop development with C++`
  - Windows SDK (el instalador de VS lo ofrece automáticamente)
2. **openFrameworks para Visual Studio 2022**.
3. Addons en la carpeta `addons` de openFrameworks:
  - `ofxBox2d`
  - `ofxJoystick`
  - `ofxCameraAnaglyph` (addon custom; si no está en el repo, copiarlo manualmente)

> Importante: este repositorio no incluye archivos de proyecto de Visual Studio (`.sln` / `.vcxproj`), por lo que hay que generarlos con el **Project Generator** de openFrameworks.

## Instalación Paso a Paso (Windows)

### 1) Instalar Visual Studio 2022

Durante la instalación, activar:

- `Desktop development with C++`
- MSVC toolset (v143)
- Windows 10/11 SDK

### 2) Instalar openFrameworks

1. Descargar openFrameworks para Visual Studio 2022.
2. Descomprimir en una ruta simple, por ejemplo:
  - `C:\openFrameworks`

### 3) Copiar este proyecto dentro de openFrameworks

Mover o clonar este repo dentro de:

- `C:\openFrameworks\apps\myApps\Rio-babel-interactive-anaglyph`

Esto facilita que el Project Generator resuelva rutas y librerías.

### 4) Verificar addons

En `C:\openFrameworks\addons\` deben existir:

- `ofxBox2d`
- `ofxJoystick`
- `ofxCameraAnaglyph`

Si falta alguno, agregarlo antes de generar el proyecto.

### 5) Generar el proyecto con Project Generator

1. Abrir `projectGenerator.exe` (incluido en openFrameworks).
2. Seleccionar la carpeta del proyecto:
  - `apps/myApps/Rio-babel-interactive-anaglyph`
3. En Addons, agregar:
  - `ofxBox2d`
  - `ofxJoystick`
  - `ofxCameraAnaglyph`
4. Hacer clic en **Update** / **Generate**.

### 6) Compilar y correr

1. Abrir el `.sln` generado con Visual Studio 2022.
2. Seleccionar configuración:
  - `Debug` + `x64`
3. Ejecutar con `Local Windows Debugger`.

Si abre una ventana y se ven shaders, texto y sonido, el proyecto está OK.

## Uso Desde VS Code

VS Code puede usarse para editar, navegar y depurar, pero la compilación de openFrameworks en Windows depende del toolchain de Visual Studio.

Recomendado:

1. Instalar extensión **C/C++** (Microsoft).
2. Generar primero el `.sln` con Project Generator.
3. Compilar una vez en Visual Studio para validar entorno.
4. Luego abrir la carpeta del proyecto en VS Code.

## Checklist Rápido

Antes de reportar un bug, comprobar:

- Visual Studio 2022 con C++ instalado.
- openFrameworks para VS2022 instalado.
- Proyecto ubicado dentro de `openFrameworks/apps/myApps/`.
- Addons instalados (`ofxBox2d`, `ofxJoystick`, `ofxCameraAnaglyph`).
- Proyecto actualizado con Project Generator.
- Assets presentes en `bin/data` (shaders, sonidos, tipografías, iconos).

## Problemas Frecuentes

### Error: `Cannot open include file: ofxBox2d.h` (o similar)

Causa probable: addon faltante o no agregado en Project Generator.

Solución:

1. Verificar que el addon exista en `openFrameworks/addons/`.
2. Reabrir Project Generator.
3. Reagregar addon y hacer **Update**.

### Error de shader al iniciar

El proyecto usa shaders en `bin/data/shadersGL2` con `#version 120`.

Solución:

1. Confirmar que la carpeta `bin/data/shadersGL2` exista y tenga los 4 archivos (`rio.vert`, `rio.frag`, `tunelMalla.vert`, `tunelMalla.frag`).
2. Confirmar que no se movió la estructura de `bin/data`.

### No hay sonido / faltan archivos multimedia

Solución:

1. Verificar `bin/data/sonidos`, `bin/data/tipografia` e `bin/data/iconos`.
2. Ejecutar desde el proyecto generado (no desde carpetas copiadas parcialmente).

### El joystick no responde

El proyecto usa `GLFW_JOYSTICK_1`.

Solución:

1. Conectar joystick antes de abrir la app.
2. Verificar en Windows que esté reconocido.
3. Si no aparece, reiniciar la app con el joystick ya conectado.

## Estructura de Archivos

```
Rio-babel-interactive-anaglyph/
├── src/
│   ├── ofApp.cpp         # Lógica principal de la aplicación
│   ├── ofApp.h           # Definiciones de clases y variables
│   └── main.cpp          # Punto de entrada
└── bin/
    └── data/
        ├── shadersGL2/   # Shaders GLSL para el fondo
        ├── sonidos/      # Archivos de audio
        ├── tipografia/   # Archivos de fuentes .ttf
        └── iconos/       # Imágenes .png para la UI
```

---

## Licencia y Uso

Copyright (c) 2026 Giselle Llamas.  
Todos los derechos reservados.

Este proyecto se publica exclusivamente como muestra de portfolio de interactividad con física en C++/Box2D/OpenFrameworks, con fines artísticos y académicos.

La música utilizada en este proyecto se incluye con permiso del usuario **Seth_Makes_Sounds**. Perfil en Freesound: [Seth Makes Sounds in Freesound.org](https://freesound.org/people/Seth_Makes_Sounds/) - Por favor siganlo y denle apoyo en su [Patreon](https://www.patreon.com/c/sethmakessounds/) si te gustaron las canciones, él crea tracks de música excelentes.

No se concede ninguna licencia para:

- Uso comercial, directo o indirecto.
- Copia, redistribución o republicación total o parcial.
- Modificación, adaptación o creación de obras derivadas.
- Entrenamiento, ajuste o evaluación de modelos de IA/ML con este código, assets, documentación o capturas derivadas.
- Extracción masiva de datos (scraping), minería de contenido o indexación para datasets.

Cualquier uso distinto al permitido requiere autorización previa y por escrito de la autora.
