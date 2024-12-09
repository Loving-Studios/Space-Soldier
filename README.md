# Space-Soldier
Video Game Development Project CITM
by Pablo Sanjose & Víctor González

### *Controles:*
- A mover hacia la izquierda
- D mover hacia la derecha
- Space para saltar
- H Mostrar el Help Menu
- F1 / F2 empezar desde el inicio del nivel (Hemos dejado implenetada la posibilidad del segundo nivel ya que lo hemos empezado a desarrollar)
- F3 Empezar desde e inicio del current level
- F4 Hacer animación de muerte
- F5 Guardar estado del juego
- F6 Cargar estado del juego
- F9 Mostrar los colliders del juego
- F10 God Mode
- F11 Capar/descapar los fps a 30

- ESC cerrar el juego

-El GodMode sigue como en la primera entrega, son saltos infinitos ya que por tiempo y preferencia de otras características así como mecánicas hemos priorizado el tiempo en perfeccionar otras cosas.

### *Implemented Features* 

- Hemos añadido más sonidos al juego, ahora hay: Canción de fondo, sonido al saltar, sonido al morir, sonido al matar enemigo, sonido de pop-up al cargar HelpMenu, sonido de cargar partida, sonido de guardar partida, sonido de checkpoint y sonido de chocar con el item.
- También está comentado pero no implementado el sonido de andar del PJ, por falta de tiempo y querer priorizar otras cosas del proyecto hemos dejado comentado este apartado ya que nos estaba dando unos problemitas, solo sonada el sonido cuando colisionabas con el suelo pero no constantemente en bucel, es fácil de solucionar y sabemos como hacerlo así que para las siguiente entrega estará.
- Se ha implementado una función para que cuando el personaje caiga en Y al vacío vuelva al inicio del mapa junto a la cámara.
- Hemos cambiado el ruido que suena cuando el jugador interactúa con el ítem.
- Para el salto hemos hecho dos animaciones, ya que si saltaba mientras se movía hacia la izquierda, aunque se movía hacia la izquierda saltaba mirando hacia la derecha.
- Hemos dejado implementada la textura en el código de lo que será en un futuro la animación de agacharse, aunque de momento no está implementada en el juego.
- Para los colliders hemos implementado una capa de objetos a nuestro mapa tmx, en la que marcamos las partes con las que interactúa el jugador. Esta capa es leída a tarvés de un bucle for que recorre el xml y dibuja todas las colisiones, hemos querido hacerlo de esta forma ya que creemos que es más óptimo para un futuro y nos será más fácil añadir muchos más niveles.
- Para las animaciones del personaje hemos juntado todas las imágenes de todas las animaciones del personaje en un canvas. Por una parte facilitas el código al solo llamar a un archivo y por otra para tener menos archivos en el proyecto, ya que en un futuro se tendrán que implementar enemigos, lo cual añadirá más archivos al proyecto.
