# Space-Soldier
Video Game Development Project CITM
by Pablo Sanjose & Víctor González

### *Controles:*
- A mover hacia la izquierda
- D mover hacia la derecha
- Space para saltar
- H Mostrar el Help Menu
- F1 Ir al inicio del nivel 1
- F2 Ir al inicio del nivel 2
- F3 Empezar desde e inicio del nivel actual
- F4 Hacer animación de muerte
- F5 Guardar estado del juego
- F6 Cargar estado del juego
- F9 Mostrar los colliders del juego
- F10 God Mode
- F11 Capar/descapar los fps a 30
- ESC Menú de Pausa

- ESC cerrar el juego

### *Implemented Features* 

- Sonidos: Canción de fondo, sonido al saltar, sonido al morir, sonido al matar enemigo, sonido de pop-up al cargar HelpMenu, sonido de cargar partida, sonido de guardar partida, sonido de checkpoint y sonido de chocar con el item.
- También está comentado pero no implementado el sonido de andar del PJ, por falta de tiempo y querer priorizar otras cosas del proyecto hemos dejado comentado este apartado ya que nos estaba dando unos problemitas, solo sonaba el sonido cuando colisionabas con el suelo pero constantemente en bucle.
- Segundo Nivel.
- Items para recuperar vida.
- Colleccionables.
- Pickups.
- Tp al final del primer nivel para ir al segundo nivel.
- Boss final con mecánica diferente.
- GUI en cada estado del juego.
- Intro screen de 4 segundos
- Title screen con Botón de Play, Continue, Settings, Credits y Exit.
- Botón de Continue sólo activo cuando hay una partida en curso.
- Botón de Settings funcional pero por falta de tiempo no hemos podido implementar las GuiSlider y el GuiCheckBox.
- Gameplay HUD a medias, con funciones comentadas por fallos y falta de tiempo.
- Screen pause menu al pulsar el botón ESCAPE.
- Die screen cuando te quedas sin vidas.
- End screen / Level completed cuando llegas al final del segundo nivel.
- Técnica Frustrum culling implementada.
- Técnica restrict pathfinding search implementada.
- Tracy.
- Casi implementada animación de afk, Player agachandose.
- Hemos implementado los métodos de Load and Save.
- Enemigos terrestres y voladores en los dos niveles.
- El enemigo terrestre es capaz de saltar obstáculos.
- Cada enemigo tiene su walkable area, su zona de patrullaje y su zona de ataque, cuando te acercas lo suficiente a ellos van a por ti.
- Para matar a los enemigos debes pisarles por encima.
- Para que los enemigos te maten debes dejar que te choquen por el lado o arriba.
- Los paths son visibles desde el estado Debug Mode F9 Show Collisions.
- Checkpoints.
- Para los enemigos terrestre si se caen al perseguirte hemos implementado un método para que vuelvan a hacer spawn en su posición de origen.
