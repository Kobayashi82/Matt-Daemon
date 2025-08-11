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

### 🔧 Funcionalidades Básicas
- **Daemon Real**: Proceso que se ejecuta en segundo plano sin intervención del usuario
- **Comunicación de Red**: Comunicación segura en red
- **Sistema de Logging**: Gestión completa de logs
- **Control de Instancias**: Solo permite una instancia ejecutándose simultáneamente
- **Gestión de Señales**: Intercepta y maneja señales del sistema correctamente
- **Multi-cliente**: Permite hasta multiples conexiones simultáneas

### 🚀 Características Avanzadas

#### **Shell Remoto Seguro (Cliente)**
- **Conexión Remota**: Cliente de terminal que se conecta al daemon
- **Autenticación**: Sistema de autenticación seguro para verificar usuarios
- **Encriptación**: Comunicación encriptada entre cliente y daemon
- **Comandos Remotos**: Ejecución de comandos del sistema de forma remota

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
```bash
# Ejecutar como root
sudo ./MattDaemon

# Verificar que está ejecutándose
ps aux | grep Matt
sudo ls -la /var/lock/ | grep matt

# Ver logs en tiempo real
sudo tail -f /var/log/matt_daemon/matt_daemon.log
```

### Conexión Básica
```bash
# Conexión simple con netcat
nc localhost 4242

# Enviar mensajes (se guardan en log)
Hello World
Test message

# Terminar daemon
quit
```

### Cliente Shell Remoto (Ben_AFK)
```bash
# Conectar con el cliente seguro
./Ben_AFK -u admin -p password localhost -P port

# Una vez autenticado, usar como terminal normal
ls -la
pwd
ps aux
exit
```

## 🧪 Testing

### Pruebas Básicas
```bash
# Test de instancia única
sudo ./MattDaemon
sudo ./MattDaemon  # Debería mostrar error de archivo bloqueado

# Test de múltiples clientes (3 max)
nc localhost 4242 &
nc localhost 4242 &
nc localhost 4242 &
nc localhost 4242   # El cuarto debería rechazarse
```

### Pruebas de Señales
```bash
# Ejecutar daemon
sudo ./MattDaemon
PID=$(pgrep MattDaemon)

# Probar diferentes señales
sudo kill -15 $PID  # SIGTERM
sudo kill -2 $PID   # SIGINT
sudo kill -1 $PID   # SIGHUP

# Verificar logs
sudo tail /var/log/matt_daemon/matt_daemon.log
```

### Pruebas de Shell Remoto
```bash
# Test de autenticación
./Ben_AFK -u admin -p password localhost -P port
# Probar credenciales incorrectas
# Probar credenciales correctas

# Test de encriptación
# Las comunicaciones deben ser encriptadas automáticamente
tcpdump -i lo port 4242  # No debería mostrar texto plano
```

## 🔒 Configuración de Seguridad

### Autenticación
```bash
# Configurar usuarios autorizados (ejemplo)
echo "admin:hashed_password" >> /etc/matt_daemon/users.conf
echo "user1:hashed_password" >> /etc/matt_daemon/users.conf
```

## 📝 Ejemplos de Log

### Log de Inicio Normal
```
[11/01/2025-14:34:58] [ INFO ] - Matt_daemon:  Started
[11/01/2025-14:34:58] [ INFO ] - Matt_daemon:  Entering Daemon mode
[11/01/2025-14:34:58] [ INFO ] - Matt_daemon:  Started. PID: 6498
[11/01/2025-14:34:58] [ INFO ] - Matt_daemon:  Creating server
[11/01/2025-14:34:58] [ INFO ] - Matt_daemon:  Server created
```

### Log de Conexión de Cliente
```
[11/01/2025-14:36:43] [ INFO ] - Matt_daemon:  Client connected from 127.0.0.1:54321
[11/01/2025-14:36:44] [ LOG ]  - User input:   Hello World
[11/01/2025-14:36:47] [ INFO ] - User request: Request quit
[11/01/2025-14:36:47] [ INFO ] - Matt_daemon:  Client disconnected
[11/01/2025-14:36:47] [ INFO ] - Matt_daemon:  Quitting
```

### Log de Shell Remoto
```
[11/01/2025-15:00:12] [ INFO ] - Matt_daemon:  Ben_AFK client connected.
[11/01/2025-15:00:15] [ INFO ] - Matt_daemon:  Authentication successful for user: admin
[11/01/2025-15:00:20] [ LOG ]  - User admin:   Remote command executed: ls -la
[11/01/2025-15:00:25] [ LOG ]  - User admin:   Remote command executed: ps aux
[11/01/2025-15:00:30] [ INFO ] - Matt_daemon:  Ben_AFK client disconnected.
```

## 🔧 Arquitectura Técnica

### Estructura del Daemon
- **Fork**: Creación del proceso hijo para funcionar en segundo plano
- **Chdir**: Cambio al directorio raíz del sistema
- **Flock**: Bloqueo de archivo para control de instancias únicas
- **Signal**: Manejo de señales del sistema (SIGTERM, SIGINT, SIGHUP)

### Comunicación de Red
- **Puerto**: 4242 (configurable)
- **Protocolo**: TCP/IP
- **Conexiones**: Máximo 3 simultáneas (configurable)
- **Timeout**: Control de conexiones inactivas

### Sistema de Encriptación
- **Algoritmo**: Encriptación simétrica de sesión
- **Intercambio**: Negociación automática de claves
- **Integridad**: Verificación de integridad de mensajes

## 📚 Clases Principales

### Tintin_reporter
```cpp
// Sistema de logging con diferentes niveles
class Tintin_reporter {
public:
    void log(LogLevel level, const std::string& message);
    void info(const std::string& message);
    void error(const std::string& message);
    void setLogFile(const std::string& filename);
};
```

### MattDaemon
```cpp
// Clase principal del daemon
class MattDaemon {
public:
    void daemonize();
    void createServer();
    void handleConnections();
    void cleanup();
};
```

### Ben_AFK (Cliente)
```cpp
// Cliente de shell remoto
class Ben_AFK {
public:
    bool connect(const std::string& host, int port);
    bool authenticate();
    void startShell();
    void encryptMessage(std::string& message);
};
```

## 🚨 Gestión de Errores

### Errores Comunes
- **Permisos insuficientes**: El daemon requiere permisos de root
- **Puerto ocupado**: Verificar que el puerto 4242 esté libre
- **Archivo bloqueado**: Solo una instancia puede ejecutarse

### Códigos de Salida
- **0**: Salida normal
- **1**: Error de permisos
- **2**: Error de configuración
- **3**: Error de red
- **4**: Instancia ya ejecutándose

## 📄 Licencia

Este proyecto está licenciado bajo la WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**🌐 Desarrollado como parte del curriculum de 42 School 🌐**

*"Because background processes need style too"*

</div>
