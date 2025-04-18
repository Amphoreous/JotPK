# Documentación de PrairieKing

## Visión General

PrairieKing es un juego de acción estilo arcade desarrollado en C++ utilizando la biblioteca Raylib. El juego está inspirado en el clásico juego de arcade "Gunfight" y presenta un estilo visual retro con píxeles.

## Arquitectura del Juego

El juego está estructurado en varias clases principales:

1. **PrairieKing**: La clase principal que maneja la lógica del juego
2. **CowboyMonster**: Clase base para los enemigos
3. **CowboyBullet**: Maneja las balas del jugador y enemigos
4. **CowboyPowerup**: Gestiona los power-ups del juego
5. **TemporaryAnimatedSprite**: Maneja animaciones temporales

## Mecánicas Principales

### Sistema de Movimiento
- El jugador puede moverse en 8 direcciones
- Los enemigos tienen diferentes patrones de movimiento
- El movimiento está basado en tiles

### Sistema de Combate
- Disparo en 8 direcciones
- Diferentes tipos de armas y power-ups
- Sistema de daño y salud

### Sistema de Oleadas
- El juego se divide en oleadas
- Cada oleada tiene diferentes tipos de enemigos
- Dificultad progresiva

### Sistema de Power-ups
- Varios tipos de power-ups disponibles
- Efectos temporales
- Pueden ser comprados en la tienda

## Constantes del Juego

El juego utiliza varias constantes definidas en el namespace `GameConstants`:

- Tamaños de tiles y zoom
- Velocidades de enemigos
- Duración de oleadas
- Tipos de monstruos
- Tipos de power-ups
- Tipos de items de tienda

## Estados del Juego

El juego maneja varios estados:
- Menú inicial
- Juego activo
- Tienda
- Game Over
- Entre oleadas
- Cutscenes

## Sistema de Guardado

El juego implementa un sistema de guardado que almacena:
- Progreso del jugador
- Mejoras compradas
- Estado actual del juego
- Puntuación y vidas 