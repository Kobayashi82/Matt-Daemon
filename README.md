<div align="center">

![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Network Communication](https://img.shields.io/badge/Network-Communication-blue?style=for-the-badge)
![Daemon Process](https://img.shields.io/badge/Daemon-Process-green?style=for-the-badge)
![C++ Language](https://img.shields.io/badge/Language-C++-red?style=for-the-badge)

*Un daemon de comunicación con capacidades de shell remoto*

</div>

# Matt Daemon

## 🎯 Descripción

`Matt Daemon` es un proyecto de `42 School` que implementa un daemon completo de comunicación por red. Este daemon funciona como un servicio en segundo plano que escucha en un puerto específico, registra todas las actividades en archivos de log y proporciona capacidades avanzadas de comunicación segura.

## ✨ Características

- **Daemon Real**: Proceso que se ejecuta en segundo plano sin intervención del usuario
- **Comunicación de Red**: Comunicación segura en red con encriptación
- **Sistema de Logging**: Gestión completa de logs con rotación automática
- **Control de Instancias**: Solo permite una instancia ejecutándose simultáneamente
- **Gestión de Señales**: Intercepta y maneja señales del sistema correctamente
- **Control de Timeout**: Gestión de conexiones inactivas
- **Multi-Cliente**: Permite conexiones simultáneas (por defecto: 3)
- **Shell Interactivo**: Acceso completo a shell remoto (Ben_AFK)

## 🔧 Instalación

```bash
git clone https://github.com/Kobayashi82/Matt-Daemon.git
cd Matt-Daemon
make

# Ejecutables generados:
# MattDaemon    - El daemon principal
# Ben_AFK       - Cliente de shell remoto
```

## 🖥️ Uso

### Ejecución del Daemon

#### Opciones disponibles:

- `-k, --disable-encryption`: Deshabilitar encriptación para clientes Ben_AFK
- `-s, --disable-shell`: Deshabilitar acceso a shell remoto
- `-c, --max-clients=NUM`: Número máximo de clientes (por defecto: 3, ilimitado = 0)
- `-p, --port=PORT`: Puerto de escucha (por defecto: 4242)
- `-t, --timeout=SECOND`: Timeout para conexiones inactivas en segundos (por defecto: 600)
- `-f, --log-file=PATH`: Ruta del archivo de log
- `-l, --log-level=LEVEL`: Nivel de logging (DEBUG, INFO, LOG, WARNING, ERROR, CRITICAL)
- `-n, --log-new`: Crear nuevo archivo de log al iniciar
- `-m, --log-rotate-max=NUM`: Máximo número de archivos de log en rotación (por defecto: 5)
- `-r, --log-rotate-size=BYTE`: Tamaño mínimo para rotación de logs (por defecto: 10MB)
- `-x, --shell-path=PATH`: Ruta del shell a ejecutar
- `-h, --help`: Mostrar ayuda
- `-u, --usage`: Mostrar sintaxis
- `-V, --version`: Mostrar versión

#### Uso básico:

```bash
# Ejecutar con configuración por defecto
sudo ./MattDaemon

# Ejecutar en puerto personalizado
sudo ./MattDaemon --port 1234

# Ejecutar con máximo 5 clientes
sudo ./MattDaemon --max-clients 3

# Deshabilitar encriptación
sudo ./MattDaemon --disable-encryption

# Deshabilitar shell remoto
sudo ./MattDaemon --disable-shell
```

#### Opciones avanzadas:

```bash
# Configurar archivo de log y nivel de registro
sudo ./MattDaemon --log-file /var/log/my_daemon.log --log-level DEBUG

# Configurar rotación de logs
sudo ./MattDaemon --log-rotate-max 10 --log-rotate-size 52428800

# Usar shell personalizado
sudo ./MattDaemon --shell-path /bin/fish

# Configurar timeout de conexión (en segundos)
sudo ./MattDaemon --timeout 1800
```

#### Verificar estado:

```bash
# Verificar que está ejecutándose
ps aux | grep MattDaemon
sudo ls -la /var/lock/ | grep matt

# Ver logs en tiempo real
sudo tail -f /var/log/matt_daemon/matt_daemon.log
```

#### Conexión Básica

```bash
# Conexión simple con netcat
nc localhost 4242

# Enviar mensajes (se guardan en log)
Hello World
Mensaje de prueba

# Terminar daemon
quit
```

### Ejecución de Ben_AFK (Shell Remoto)

#### Opciones disponibles:

- `-k, --insecure`: Permitir comunicación no encriptada
- `-l, --login=USERNAME`: Especificar nombre de usuario
- `-p, --port=PORT`: Puerto de conexión (por defecto: 4242)
- `-h, --help`: Mostrar ayuda
- `-u, --usage`: Mostrar sintaxis
- `-V, --version`: Mostrar versión

#### Uso básico:

```bash
# Conexión básica (usuario actual, puerto por defecto)
./Ben_AFK localhost

# Especificar usuario
./Ben_AFK --login admin localhost

# Usuario en formato user@host
./Ben_AFK admin@localhost

# Puerto personalizado
./Ben_AFK --port 1234 localhost

# Permitir comunicación no encriptada
./Ben_AFK --insecure localhost

# Combinando opciones
./Ben_AFK --login admin --port 1234 --insecure localhost
```

## 🧪 Testing

### Pruebas Básicas

```bash
# Test de instancia única
sudo ./MattDaemon
sudo ./MattDaemon  # Debería mostrar error de archivo bloqueado

# Test de límite de clientes (por defecto 3)
nc localhost 4242 &
nc localhost 4242 &
nc localhost 4242 &
nc localhost 4242   # El cuarto debería rechazarse

# Test con límite personalizado
sudo ./MattDaemon --max-clients 1
nc localhost 4242 &
nc localhost 4242   # El segundo debería rechazarse
```

### Pruebas de Señales

```bash
# Ejecutar daemon
sudo ./MattDaemon && PID=$(pgrep MattDaemon)
sudo kill -15 $PID  # SIGTERM

# Verificar logs
sudo tail /var/log/matt_daemon/matt_daemon.log

# Repetir con diferentes señales
sudo kill -2 $PID   # SIGINT
sudo kill -1 $PID   # SIGHUP
```

### Pruebas de Logging

```bash
# Test de rotación de logs
sudo ./MattDaemon --log-rotate-size 10 --log-rotate-max 3
nc localhost 4242
# Enviar mensajes...

# Test de niveles de log
sudo ./MattDaemon --log-level DEBUG
sudo ./MattDaemon --log-level LOG

# Test de archivo de log personalizado
sudo ./MattDaemon --log-file /tmp/test_daemon.log
```

### Pruebas de Ben_AFK (Shell Remoto)

```bash
# Test de conexión básica
./Ben_AFK localhost

# Test con usuario específico
./Ben_AFK --login user localhost

# Test con puerto personalizado
sudo ./MattDaemon --port 1234
./Ben_AFK --port 1234 localhost

# Test de comunicación no encriptada
sudo ./MattDaemon --disable-encryption
./Ben_AFK --insecure localhost
```

## 📝 Ejemplos de Log

```
[18/08/2025-20:49:54]      [ DEBUG ] - Initiating daemon
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: First fork() completed
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: setsid() completed
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: Second fork() completed
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: All signal handlers successfully installed
[18/08/2025-20:49:54]      [ DEBUG ] - umask() set
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: Working directory changed
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: Standard file descriptors closed
[18/08/2025-20:49:54]      [ DEBUG ] - Daemon: Lock set
[18/08/2025-20:49:54]       [ INFO ] - Daemon: Started
[18/08/2025-20:49:54]      [ DEBUG ] - Epoll initialized
[18/08/2025-20:49:54]      [ DEBUG ] - Socket created
[18/08/2025-20:49:54]      [ DEBUG ] - Socket reusable option set
[18/08/2025-20:49:54]      [ DEBUG ] - Socket bind set
[18/08/2025-20:49:54]      [ DEBUG ] - Socket listen set
[18/08/2025-20:49:54]      [ DEBUG ] - Socket added to Epoll
[18/08/2025-20:49:54]       [ INFO ] - Daemon: Listening on port 4242
[18/08/2025-20:50:11]       [ INFO ] - Client: [127.0.0.1:49206] connected
[18/08/2025-20:50:11]      [ DEBUG ] - Client added to Epoll
[18/08/2025-20:50:11]       [ INFO ] - Client: [127.0.0.1:49206] wants to open a shell
[18/08/2025-20:50:14]    [ WARNING ] - Client: [127.0.0.1:49206] authorization failed for user: vzurera
[18/08/2025-20:50:21]       [ INFO ] - Client: [127.0.0.1:49206] authorization successful for user: vzurera
[18/08/2025-20:50:21]      [ DEBUG ] - Client: [127.0.0.1:49206] terminal size: 114x28
[18/08/2025-20:50:21]       [ INFO ] - Client: [127.0.0.1:49206] shell started with PID 48675 and PTY: /dev/pts/6
[18/08/2025-20:50:25]      [ DEBUG ] - Processing terminated PID: 48675
[18/08/2025-20:50:25]      [ DEBUG ] - Client: [127.0.0.1:49206] shell process 48675 terminated
[18/08/2025-20:50:25]      [ DEBUG ] - Scheduling client removal due to shell termination
[18/08/2025-20:50:25]      [ DEBUG ] - Client: [127.0.0.1:49206] scheduled for deferred removal
[18/08/2025-20:50:25]      [ DEBUG ] - Processing deferred removal of client: 127.0.0.1:49206
[18/08/2025-20:50:25]       [ INFO ] - Client: [127.0.0.1:49206] disconnected
[18/08/2025-20:50:31]       [ INFO ] - Client: [127.0.0.1:44438] connected
[18/08/2025-20:50:31]      [ DEBUG ] - Client added to Epoll
[18/08/2025-20:50:36]        [ LOG ] - Mensaje de prueba
[18/08/2025-20:50:38]    [ WARNING ] - Client: [127.0.0.1:44438] wants to close the daemon
[18/08/2025-20:50:38]      [ DEBUG ] - Socket close
[18/08/2025-20:50:38]       [ INFO ] - Daemon: Closed
```

## 🏗️ Arquitectura Técnica

### Estructura del Daemon
- **Fork**: Creación doble del proceso hijo para garantizar independencia del terminal
- **Chdir**: Cambio al directorio raíz del sistema
- **Flock**: Bloqueo de archivo para control de instancias únicas
- **Signal**: Manejo de señales del sistema (SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGPIPE, SIGSEV, SIGCHLD)

### Comunicación de Red
- **Puerto**: 4242 (configurable)
- **Protocolo**: TCP/IP
- **Conexiones**: Máximo de conexiones simultáneas (configurable)
- **Timeout**: Control de conexiones inactivas (configurable)

### Sistema de Encriptación
- **Encriptación**: Cifrado XOR con clave repetida
- **Cliente Seguro**: Ben_AFK soporta comunicación encriptada
- **Negociación**: Automática entre cliente y servidor

### Sistema de Logging
- **Niveles**: DEBUG, INFO, LOG, WARNING, ERROR, CRITICAL
- **Rotación**: Automática basada en tamaño y cantidad de archivos
- **Ubicación**: Configurable (por defecto: /var/log/matt_daemon/matt_daemon.log)

### Errores Comunes
- **Permisos insuficientes**: El daemon requiere permisos de root
- **Puerto ocupado**: Verificar que el puerto especificado esté libre
- **Archivo bloqueado**: Solo una instancia puede ejecutarse
- **Host desconocido**: Verificar que el hostname/IP sea válido (Ben_AFK)

## 📄 Licencia

Este proyecto está licenciado bajo la WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**🌍 Desarrollado como parte del curriculum de 42 School 🌍**

*"Because background processes need style too"*

</div>
