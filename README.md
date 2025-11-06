# Practica-5-Simulador de colisionnes
![Estado del proyecto](https://img.shields.io/badge/estado-en%20proceso-yellow)  
![Lenguaje](https://img.shields.io/badge/lenguaje-C++-blue)  


---

## Resumen  

Este proyecto tiene como objetivo aplicar los principios de la programación orientada a objetos (POO) y la física en el desarrollo de una simulación y un videojuego educativo.
A través del modelamiento y la implementación de sistemas físicos, se busca comprender cómo los fenómenos como el movimiento y las colisiones pueden ser representados y visualizados mediante código.

## Simulacion sistemas fisicos

En esta etapa se desarrollará un programa que simula el movimiento de partículas dentro de un espacio cerrado, considerando diferentes tipos de colisiones (elásticas, inelásticas y completamente inelásticas).
La simulación generará un archivo de salida con las posiciones y eventos de colisión, que luego podrá ser graficado externamente.

- [ ] Aplicar los principios de la programación orientada a objetos (POO) para modelar partículas, obstáculos y el entorno de simulación.

- [ ] Implementar las leyes del movimiento y la conservación del momento lineal en tiempo discreto.

- [ ] Simular los siguientes tipos de colisiones:

- [ ] Perfectamente elásticas (contra las paredes).

- [ ] Inelásticas (contra obstáculos).

- [ ] Completamente inelásticas (entre partículas).

- [ ] Actualizar posiciones y velocidades de las partículas en cada intervalo de tiempo (Δt).

- [ ] Registrar en un archivo de texto las posiciones, velocidades y colisiones detectadas durante la simulación.

- [ ] Graficar las trayectorias de las partículas usando herramientas externas (por ejemplo, Python y matplotlib).

- [ ] Implementar una simulación con al menos cuatro partículas activas simultáneamente.

## Desarrollo de un juego interactivo con Qt:
En la segunda etapa, se aprovechará la simulación anterior para construir un juego por turnos donde dos jugadores lanzan proyectiles aplicando principios de la física.
El juego incluirá colisiones, cálculo de daños, manejo de turnos y una interfaz gráfica desarrollada con Qt.
Diseñar una interfaz gráfica con Qt que permita visualizar la simulación en tiempo real.

- [ ] Implementar el sistema de turnos entre dos jugadores.

- [ ] Permitir que cada jugador configure el ángulo y la velocidad inicial del proyectil antes de lanzarlo.

- [ ] Simular el movimiento parabólico y las colisiones de los proyectiles con los límites y obstáculos.

- [ ] Calcular el daño infligido a los obstáculos en función del momento lineal del proyectil en el impacto.

- [ ] Gestionar la resistencia y destrucción de los obstáculos según el daño recibido.

- [ ] Definir las condiciones de victoria basadas en la destrucción total de la infraestructura enemiga.

- [ ] Integrar la lógica física con la animación visual y la actualización de la interfaz.

- [ ] Mantener el seguimiento del avance del proyecto mediante actualizaciones en el repositorio.
## Tecnologías y herramientas  

- **Lenguaje principal:** C++  
- **Paradigma:** Programación Orientada a Objetos (POO)  
- **Entorno de desarrollo:** Qt Creator  
- **Control de versiones:** Git y GitHub  

## Clonacion del repositorio
```bash
git clone [https://github.com/B-Alejandro/Practica-5.git]
```
---

## Autores  

Proyecto desarrollado como parte del curso **Informática II**.  

**Integrantes:**  
- Alejandro Bedoya Zuluaga  
- Jeronimo Herrera Espinos
