#pragma once
#include <gtkmm.h>

// Ventana principal del cliente Casey_AFK
class MainWindow : public Gtk::Window {
public:
	MainWindow();  // Constructor: inicializa la interfaz
	virtual ~MainWindow(); // Destructor

private:
	// Contenedor vertical principal
	Gtk::Box _vBox;
	// Contenedor horizontal para IP, puerto , user y botones de conexión
	Gtk::Box _HBoxConnection;
	// Etiquetas para los campos de IP, puerto y usuario
	Gtk::Label _labelIP;      // "IP:"
	Gtk::Label _labelPort;    // "Puerto:"
	Gtk::Label _labelUser;    // "Usuario:"
	// Campos de entrada para IP y puerto
	Gtk::Entry _entryIP;      // Entrada de dirección IP o host
	Gtk::Entry _entryPort;    // Entrada de puerto
    // Campos de entrada para el usuario
    Gtk::Entry _entryUser;    // Entrada de nombre de usuario
	// Botones para conectar y desconectar
	Gtk::Button _btnConnect;      // Botón "Conectar"
	Gtk::Button _btnDisconnect;   // Botón "Desconectar"
	// Área de scroll para mostrar los logs
	Gtk::ScrolledWindow _scrolledWindow;
	// Cuadro de texto solo lectura para los logs
	Gtk::TextView _textView;
	// Campo de entrada para escribir mensajes
	Gtk::Entry _entryMessage;
	// Botón para enviar mensajes
	Gtk::Button _buttonSend;
	// Etiqueta de estado (conectado/desconectado)
	Gtk::Label _statusLabel;

	// Función que maneja el evento de conectar
	void connectButton();
	// Función que maneja el evento de desconectar
	void disconnectButton();
	// Función que maneja el evento de enviar mensaje
	void sendButton();
	// Cambia el estado de los widgets según conexión
	void setConnectedState(bool connected);
};
