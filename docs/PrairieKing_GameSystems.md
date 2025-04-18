# Sistemas y Mecánicas del Juego

## Sistema de Movimiento

### Jugador
- Movimiento basado en tiles (16x16 píxeles)
- Velocidad base de 3.0 unidades
- Movimiento en 8 direcciones (N, NE, E, SE, S, SW, W, NW)
- Sistema de colisiones con el mapa y enemigos
- Animaciones de movimiento y pasos

### Enemigos
- Diferentes velocidades según tipo:
  - Orco: 2 unidades
  - Fantasma: 3 unidades
  - Ogro: 1 unidad
  - Spikey: 3 unidades
- Patrones de movimiento específicos por tipo
- Sistema de persecución del jugador
- Colisiones con el mapa y otros enemigos

## Sistema de Combate

### Balas
- Velocidad base: 8 unidades
- Sistema de daño escalable
- Colisiones con enemigos y mapa
- Diferentes tipos de balas:
  - Normales
  - Dispersas
  - Rápidas
  - Especiales

### Daño y Salud
- Sistema de salud por puntos
- Daño base configurable
- Modificadores de daño por power-ups
- Efectos visuales al recibir daño

## Sistema de Power-ups

### Tipos de Power-ups
1. **Log** (Tronco)
   - Bloquea el movimiento de enemigos
   - Duración: 7500ms

2. **Skull** (Calavera)
   - Efecto especial de daño
   - Duración: 10000ms

3. **Heart** (Corazón)
   - Restaura salud
   - Efecto instantáneo

4. **Spread** (Dispersión)
   - Dispara en múltiples direcciones
   - Duración: 10000ms

5. **Rapidfire** (Disparo Rápido)
   - Aumenta velocidad de disparo
   - Duración: 10000ms

6. **Nuke** (Bomba)
   - Daño en área
   - Efecto instantáneo

7. **Zombie** (Zombi)
   - Efecto especial de control
   - Duración: 10000ms

8. **Speed** (Velocidad)
   - Aumenta velocidad de movimiento
   - Duración: 10000ms

9. **Shotgun** (Escopeta)
   - Disparo en cono
   - Duración: 10000ms

10. **Life** (Vida Extra)
    - Añade una vida
    - Efecto instantáneo

11. **Teleport** (Teletransporte)
    - Movimiento instantáneo
    - Efecto instantáneo

12. **Sherriff** (Sheriff)
    - Power-up especial
    - Duración: 10000ms

## Sistema de Tienda

### Items Disponibles
1. **Firespeed1/2/3**
   - Mejora velocidad de disparo
   - Niveles 1-3

2. **Runspeed1/2**
   - Mejora velocidad de movimiento
   - Niveles 1-2

3. **Life**
   - Vida extra

4. **Ammo1/2/3**
   - Mejora munición
   - Niveles 1-3

5. **SpreadPistol**
   - Arma de disparo disperso

6. **Star**
   - Item especial

7. **Skull**
   - Power-up calavera

8. **Log**
   - Power-up tronco

## Sistema de Oleadas

### Estructura
- Duración base: 80000ms (80 segundos)
- Tiempo entre oleadas: 5000ms (5 segundos)
- Dificultad progresiva
- Generación de enemigos basada en probabilidades

### Tipos de Enemigos por Oleada
1. **Orco**
   - Salud: 1
   - Velocidad: 2
   - Daño: 1

2. **Fantasma**
   - Salud: 1
   - Velocidad: 3
   - Daño: 1

3. **Ogro**
   - Salud: 3
   - Velocidad: 1
   - Daño: 2

4. **Momia**
   - Salud: 2
   - Velocidad: 2
   - Daño: 1

5. **Diablo**
   - Salud: 4
   - Velocidad: 2
   - Daño: 2

6. **Hongo**
   - Salud: 2
   - Velocidad: 1
   - Daño: 1

7. **Spikey**
   - Salud: 2
   - Velocidad: 3
   - Daño: 1

8. **Drácula**
   - Salud: 5
   - Velocidad: 2
   - Daño: 3

## Sistema de Mapas

### Tipos de Tiles
1. **Barrier1/2** (Barrera)
   - Impenetrable
   - Bloquea movimiento y disparos

2. **Rocky1** (Rocoso)
   - Impenetrable
   - Bloquea movimiento

3. **Desert** (Desierto)
   - Transitable
   - Sin efectos especiales

4. **Grassy** (Hierba)
   - Transitable
   - Efecto visual

5. **Cactus**
   - Impenetrable
   - Daño al contacto

6. **Fence** (Valla)
   - Semi-penetrable
   - Bloquea algunos tipos de movimiento

7. **Trench1/2** (Trinchera)
   - Efectos especiales
   - Modifica movimiento

8. **Bridge** (Puente)
   - Conexión entre áreas
   - Efectos especiales

## Sistema de Guardado

### Datos Guardados
1. **Estado del Jugador**
   - Vidas restantes
   - Puntuación
   - Monedas
   - Power-ups activos
   - Mejoras compradas

2. **Estado del Juego**
   - Oleada actual
   - Tiempo restante
   - Enemigos activos
   - Balas en pantalla

3. **Configuración**
   - Nivel de dificultad
   - Opciones de juego
   - Controles personalizados

## Sistema de Sonido

### Efectos de Sonido
- Disparos
- Impactos
- Power-ups
- Muerte de enemigos
- Música de fondo
- Efectos ambientales

## Sistema de Partículas

### Tipos de Partículas
1. **Explosiones**
   - Daño en área
   - Efecto visual

2. **Humo**
   - Efecto visual
   - Duración temporal

3. **Chispas**
   - Efecto visual
   - Indicador de daño

4. **Efectos Especiales**
   - Power-ups
   - Habilidades especiales 