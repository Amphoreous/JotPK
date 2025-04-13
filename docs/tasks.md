# Tareas para completar el juego

## Funcionalidades que faltan implementar

### Sistema de Monstruos
- [ ] Implementar la lógica completa de spawn de monstruos según la ola actual
- [ ] Ajustar las probabilidades de spawn para cada tipo de monstruo
- [ ] Implementar el sistema de New Game Plus para monstruos
- [ ] Completar la implementación de los jefes (Gopher, Devil, etc.)
- [ ] Ajustar la IA de los monstruos para que sea más similar al original
- [ ] Implementar las clases específicas de monstruos (Dracula, Outlaw, etc.)
- [ ] Ajustar las fases y comportamientos de los jefes

### Sistema de Power-ups
- [ ] Implementar todos los power-ups del juego original
- [ ] Ajustar las duraciones y efectos de los power-ups
- [ ] Implementar el sistema de power-ups permanentes (vidas, munición, etc.)
- [ ] Ajustar las probabilidades de drop de power-ups
- [ ] Implementar power-ups específicos como POWERUP_SPREAD, POWERUP_RAPIDFIRE, etc.

### Sistema de Tienda
- [ ] Implementar la tienda del comerciante
- [ ] Ajustar los precios de los items
- [ ] Implementar la lógica de compra/venta
- [ ] Añadir las animaciones del comerciante
- [ ] Implementar items específicos como ITEM_FIRESPEED1, ITEM_RUNSPEED1, etc.

### Sistema de Mapas
- [ ] Implementar la generación procedural de mapas
- [ ] Ajustar los diferentes tipos de terreno
- [ ] Implementar el sistema de scroll entre mapas
- [ ] Añadir elementos decorativos (cactus, vallas, etc.)
- [ ] Implementar tipos de mapas específicos como MAP_BARRIER1, MAP_DESERT, etc.

### Sistema de Sonido
- [ ] Implementar todos los efectos de sonido
- [ ] Ajustar los volúmenes y la mezcla
- [ ] Implementar la música de fondo
- [ ] Añadir sonidos ambientales
- [ ] Implementar sonidos específicos para cada acción y evento

### Sistema de Animaciones
- [ ] Implementar las animaciones del jugador
- [ ] Añadir animaciones de monstruos
- [ ] Implementar efectos visuales (explosiones, destellos, etc.)
- [ ] Ajustar los timings de las animaciones
- [ ] Implementar la clase TemporaryAnimatedSprite para animaciones temporales

### Interfaz de Usuario
- [ ] Implementar el HUD completo
- [ ] Añadir indicadores de vida/munición
- [ ] Implementar el sistema de puntuación
- [ ] Añadir mensajes y notificaciones
- [ ] Implementar indicadores de estado del juego y opciones

## Correcciones necesarias

### Optimización
- [ ] Optimizar el sistema de colisiones
- [ ] Mejorar el rendimiento del renderizado
- [ ] Optimizar la gestión de memoria
- [ ] Reducir el uso de CPU
- [ ] Optimizar el manejo de recursos gráficos y de sonido

### Compatibilidad C# a C++
- [x] Ajustar las diferencias de tipos de datos
- [ ] Corregir la gestión de memoria
- [ ] Adaptar las estructuras de datos
- [ ] Ajustar los sistemas de coordenadas
- [ ] Asegurar la compatibilidad con las bibliotecas de C++ utilizadas

### Bugs conocidos
- [ ] Corregir problemas con el scroll del mapa
- [ ] Arreglar colisiones con monstruos
- [ ] Corregir problemas con power-ups
- [ ] Arreglar problemas de sincronización de sonido
- [ ] Corregir problemas de rendimiento y estabilidad
- [ ] Corregir problemas con el inicio de la canción de fondo, que no se reproduce hasta que haya pasado between wave timer y haya desaparecido controls instructions box
- [ ] Corregir el between wave timer que no funciona correctamente
- [x] Corregir el problema de monstruos invisibles que atacan al jugador

## Características específicas a implementar

### Jefes
- [ ] Implementar el jefe Gopher
- [ ] Implementar el jefe Devil
- [ ] Añadir patrones de ataque
- [ ] Implementar fases de batalla
- [ ] Implementar jefes específicos como Dracula y Outlaw con sus fases únicas

### Power-ups especiales
- [ ] Implementar power-up de velocidad
- [ ] Implementar power-up de disparo rápido
- [ ] Implementar power-up de escopeta
- [ ] Implementar power-up de zombie
- [ ] Implementar power-ups adicionales como POWERUP_NUKE, POWERUP_TELEPORT, etc.

### Sistema de guardado
- [ ] Implementar guardado de progreso
- [ ] Añadir sistema de New Game Plus
- [ ] Implementar guardado de high scores
- [ ] Añadir sistema de checkpoints
- [ ] Asegurar la compatibilidad con el sistema de guardado existente

### Efectos visuales
- [ ] Implementar sistema de partículas
- [ ] Añadir efectos de post-procesado
- [ ] Implementar efectos de pantalla
- [ ] Añadir efectos de transición
- [ ] Implementar efectos visuales específicos para cada tipo de monstruo y jefe

## Notas importantes
- Se ha eliminado el modo cooperativo/multiplayer
- Se mantiene la estructura básica del juego original
- Se han ajustado las posiciones de los sprites según la nueva imagen cursors_1.png
- Se necesita mantener la compatibilidad con el sistema de guardado existente
- Se deben implementar todas las clases y métodos definidos en PrairieKing.hpp
