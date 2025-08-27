#pragma once
#include <gtkmm.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <glibmm/dispatcher.h>

// ===============================
//  Ventana principal Casey_AFK
// ===============================
class MainWindow : public Gtk::Window {
public:
	MainWindow();              // Constructor: inicializa la interfaz
	virtual ~MainWindow();     // Destructor

private:
	// --- Widgets principales ---
	Gtk::Box _vBox;                // Contenedor vertical principal
	Gtk::Box _HBoxConnection;      // Contenedor horizontal para IP, puerto, usuario y botones
	Gtk::Label _labelIP;           // Etiqueta "IP:"
	Gtk::Label _labelPort;         // Etiqueta "Puerto:"
	Gtk::Label _labelUser;         // Etiqueta "Usuario:"
	Gtk::Entry _entryIP;           // Entrada de dirección IP o host
	Gtk::Entry _entryPort;         // Entrada de puerto
	Gtk::Entry _entryUser;         // Entrada de nombre de usuario
	Gtk::Button _btnConnect;       // Botón "Conectar"
	Gtk::Button _btnDisconnect;    // Botón "Desconectar"
	Gtk::ScrolledWindow _scrolledWindow; // Área de scroll para logs
	Gtk::TextView _textView;       // Cuadro de texto solo lectura para logs
	Gtk::Entry _entryMessage;      // Campo de entrada para mensajes
	Gtk::Button _buttonSend;       // Botón para enviar mensajes
	Gtk::Label _statusLabel;       // Etiqueta de estado (conectado/desconectado)

	// --- Estado de red ---
	int _sockfd = -1;              // Descriptor del socket

	// --- Hilo y comunicación con GUI ---
	std::thread _recvThread;       // Hilo de recepción de logs
	std::atomic<bool> _running;    // Control de ejecución del hilo
	Glib::Dispatcher _dispatcher;  // Dispatcher para avisar a la GUI
	std::mutex _logMutex;          // Mutex para acceso a logs pendientes
	std::string _pendingLogs;      // Buffer de logs pendientes

	// --- Métodos de eventos ---
	void connectButton();          // Maneja el evento de conectar
	void disconnectButton();       // Maneja el evento de desconectar
	void sendButton();             // Maneja el evento de enviar mensaje
	void setConnectedState(bool connected); // Cambia el estado de los widgets
	void onLogReceived();          // Maneja la llegada de logs desde el hilo
};
