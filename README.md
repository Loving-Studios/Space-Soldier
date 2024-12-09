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
- También está comentado pero no implementado el sonido de andar del PJ, por falta de tiempo y querer priorizar otras cosas del proyecto hemos dejado comentado este apartado ya que nos estaba dando unos problemitas, solo sonada el sonido cuando colisionabas con el suelo pero no constantemente en bucle, es fácil de solucionar y sabemos como hacerlo así que para las siguiente entrega estará.
- Hemos creado el segundo nivel ya que al principio pensamos que debíamos hacerlo y está en el proyecto, pero no está activo para jugar. 
- Hemos dejado implementada la textura en el código de lo que será en un futuro la animación de agacharse, aunque de momento no está implementada en el juego.
- El player tiene su canvas, el enemigo terrestre tiene su canvas y el enemigo volador tiene su canvas.
- Hemos implementado los métodos de Load and Save, donde guardan el estado del juego y lo puedes cargar. Cuando el juego inicia por primera vez empezarás desde el inicio del nivel pero podrás cargar con el F6 tu estado del juego.
- Hemos añadido en el config.xml a cada enemigo un atributo de posición original, ya que el load and save sobreescribe posiciones y puede dar problemitas.
- Hemos implementado dos enemigos terrestres en el nivel.
- El enemigo terrestre es capaz de saltar obstáculos.
- Hemos implementado dos enemigos voladores en el nivel.
- Cada enemigo tiene su walkable area, su zona de patrullaje y su zona de ataque, cuando te acercas lo suficiente a ellos van a por ti.
- Para matar a los enemigos debes pisarles por encima.
- Para que los enemigos te maten debes dejar que te choquen por el lado o arriba.
- Los paths son visibles desde el estado Debug Mode F9 Show Collisions.
- Hay varios checkpoints por el mapa, para que cuando el player los atraviese sea como si aprietas el F5 y guardas partida, si te caes al vacío volverás al último checkpoint.
- Está montado un sistema para los voladores es decir, una cárcel de la que no pueden salir, mientras te persiguen no pueden salir de su zona de patrullaje.
- Para los enemigos terrestre queríamos implementar un método en el que si se caían del mapa hiciesen respawn en su posición de spawn original, no hemos podido pulirlo para que funcione aún así está en el código, no hace nada.
