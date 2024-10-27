# Space-Soldier
Video Game Development Project CITM

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

-Nuestro God Mode otorga al jugador saltos infinitos con lo que por mucho que se caiga de las plataformas puede volver realizando varios saltos, hemos optado por esta manera de hacer el God MOde, ya que no conseguíamos parar las fuerzas en y que afectaba al personaje, y aunque de manera más lenta el personaje seguía cayendo hacia abajo.

-Hemos hecho la animación de muerte en un botón, ya que por ahora al no haber enemigos no podía morirse el personaje, además hemos puesto dos animaciones de muerte una mirando hacia cada lado para que más adelante sea más fácil de implementar, aunque por ahora la animación le falta un control de errores, ya que ahora mismo mientras está muerto como tiene una animación de muerte hacia la izquierda y otro hacia la derecha deja pasar de una a otra en vez de mantener al personaje en una.

-Para el HelpMenu Hemos decidido hacer una imagen con los controles la cual al pulsar la tecla H muestra por pantalla arriba a la izquierda, en la cual se puede ver tanto los controles de movimiento del personaje como todas las teclas del debugMode. Si se vuelve a pulsar la tecla H la imagen del HelpMenu se vuelve a esconder.

### *Implemented Features* 

- Hemos añadido una canción en bucle para el Background music
- Hemos cambiado el ruido que suena cuando el jugador interactúa con el ítem.
- Para el salto hemos hecho dos animaciones, ya que si saltaba mientras se movía hacia la izquierda, aunque se movía hacia la izquierda saltaba mirando hacia la derecha.
- Hemos dejado implementadas en el código y la textura la animación de agacharse, aunque no está implementada en el juego.
- Para los collieders hemos implementado una capa de objetos a nuestro mapa, en la que marcamos las partes con las que interactúa el jugador.
- Para las animaciones del personaje hemos juntado todas las imágenes de todas las animaciones del personaje y las hemos juntado en un solo fichero para, por un lado, solo llamar a un archivo por código y por otro para tener menos archivos en el proyecto, ya que en un futuro se tendrán que implementar enemigos, lo cual añadirá más archivos al proyecto.
