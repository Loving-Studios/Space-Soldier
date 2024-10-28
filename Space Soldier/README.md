# Space-Soldier
Video Game Development Project CITM
by Pablo Sanjose & Víctor González

### *Controles:*
- A mover hacia la izquierda
- D mover hacia la derecha
- Space para saltar
- F4 Hacer animación de muerte
- F9 mostrar los colliders del juego
- F10 God Mode
- F11 Capar/descapar los fps a 30
- H mostrar el Help Menu
- ESC cerrar el juego

-Nuestro God Mode otorga al jugador saltos infinitos de tal forma que por mucho que se caiga de las plataformas puede volver realizando varios saltos, hemos optado por esta manera de hacer el God Mode ya que no conseguíamos parar las fuerzas en Y que afectaban al personaje. Por mucho que cambiásemos siempre de manera más lenta el personaje seguía cayendo hacia abajo.

-Para esta primera versión hemos implementado la animación de muerte en un botón, ya que por ahora al no haber enemigos no podía morirse el personaje. Además hemos añadido dos animaciones de muerte, una para cada lado con el objetivo de que más adelante sea más fácil de implementar. En estos momentos a la animación le falta un control de errores ya que ahora mismo mientras está muerto como tiene una animación de muerte hacia la izquierda y otro hacia la derecha deja pasar de una a otra en vez de mantener al personaje en una.

-Para el HelpMenu hemos decidido mostrar una imagen con los controles del juego. Esta imagen se muestra por pantalla al pulsar la tecla H arriba a la izquierda. En esta imagen se pueden ver tanto los controles de movimiento del personaje como todas las teclas del debugMode. Si se vuelve a pulsar la tecla H la imagen del HelpMenu se esconde.

### *Implemented Features* 

- Hemos añadido una canción en bucle para el Background music
- Se ha implementado una función para que cuando el personaje caiga en Y al vacío vuelva al inicio del mapa junto a la cámara.
- Hemos cambiado el ruido que suena cuando el jugador interactúa con el ítem.
- Para el salto hemos hecho dos animaciones, ya que si saltaba mientras se movía hacia la izquierda, aunque se movía hacia la izquierda saltaba mirando hacia la derecha.
- Hemos dejado implementada la textura en el código de lo que será en un futuro la animación de agacharse, aunque de momento no está implementada en el juego.
- Para los colliders hemos implementado una capa de objetos a nuestro mapa tmx, en la que marcamos las partes con las que interactúa el jugador. Esta capa es leída a tarvés de un bucle for que recorre el xml y dibuja todas las colisiones, hemos querido hacerlo de esta forma ya que creemos que es más óptimo para un futuro y nos será más fácil añadir muchos más niveles.
- Para las animaciones del personaje hemos juntado todas las imágenes de todas las animaciones del personaje en un canvas. Por una parte facilitas el código al solo llamar a un archivo y por otra para tener menos archivos en el proyecto, ya que en un futuro se tendrán que implementar enemigos, lo cual añadirá más archivos al proyecto.
