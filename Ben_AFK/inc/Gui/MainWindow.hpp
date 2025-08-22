#include <iostream>
#include <gtkmm.h>

// clase para crear una ventana grafica con Gtkmm4
class MainWindow : public Gtk::Window
{
	public:
		MainWindow();
		virtual ~MainWindow();

	protected:
		// Signal handlers
		void connectButton();
		void disconnectButton();
		void entryActivated();
		void sendButton();

		// habilitar/deshabilitar widgets segun estado
		void setConnectedState(bool connect);

	private:
		// Layout containers
		Gtk::Box			_vBox;					// Contenedor principal vertical
		Gtk::Box			_HBoxConnection;		// Contenedor horizontal para la conexión

		// Widgets para la conexion
		Gtk::Entry			_entryIP;				// Entrada para la IP
		Gtk::Entry			_entryPort;				// Entrada para el puerto
		
		// Buttons connect/diconected/send
		Gtk::Button			_btnConnect;			// Boton conectar
		Gtk::Button			_btnDisconnect;			// Botnon desconectar
		Gtk::Button			_buttonSend;			// Botón para enviar
		
		// Area de Mensajes
		Gtk::TextView		_textView;				// Area mensajes recibidos y enviados
		Gtk::ScrolledWindow _scrolledWindow;		// permite que el area de mensajes se desplaze si hay muchos mensajes
		
		// Entrada mensajes
		Gtk::Entry			_entryMessage;			// Donde el usuario escribe el mensaje a enviar

		// Mensajes de estado
		Gtk::Label			_statusLabel;			// Para mostrar mensajes de estado del usuario


};
