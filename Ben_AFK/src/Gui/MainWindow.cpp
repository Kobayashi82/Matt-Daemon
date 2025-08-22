#include "Gui/MainWindow.hpp"

// Constructor
MainWindow::MainWindow() :
	_vBox(Gtk::Orientation::VERTICAL),
	_HBoxConnection(Gtk::Orientation::HORIZONTAL),
	_btnConnect("Conectar"),
	_btnDisconnect("Desconectar"),
	_buttonSend("Enviar"),
	_statusLabel("")
{
	// Configuracion inicial de widgets
	set_title("Ben_AFK - Cliente grafico Matt_Daemon");
	set_default_size(500, 400);

	// Panel de conexion
	_entryIP.set_placeholder_text("IP del servidor");
	_entryPort.set_placeholder_text("Puerto (4242)");
	_HBoxConnection.append(_entryIP);
	_HBoxConnection.append(_entryPort);
	_HBoxConnection.append(_btnConnect);
	_HBoxConnection.append(_btnDisconnect);

	// Area de mensajes
	_scrolledWindow.set_child(_textView);
	_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	_textView.set_editable(false);

	// Entrada de mensaje y boton enviar
	Gtk::Box* hBoxSend = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	_entryMessage.set_placeholder_text("Escribe tu mensaje...");
	hBoxSend->append(_entryMessage);
	hBoxSend->append(_buttonSend);

	// Mensaje de estado
	_statusLabel.set_text("Desconectado");

	// Añadir todo al contenedor principal
	_vBox.append(_HBoxConnection);
	_vBox.append(_scrolledWindow);
	_vBox.append(*hBoxSend);
	_vBox.append(_statusLabel);

	set_child(_vBox);

	// Conectar señales
	_btnConnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::connectButton));
	_btnDisconnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::disconnectButton));
	_entryMessage.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::entryActivated));
	_buttonSend.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::sendButton));

	// Estado inicial: desconectado
	setConnectedState(false);
}

// Destructor
MainWindow::~MainWindow() {}

// Handler para boton conectar
void MainWindow::connectButton()
{
	_statusLabel.set_text("Conectando...");
	// Aqui el codigo para la conexion real....
	setConnectedState(true);
	_statusLabel.set_text("Conectado");
}

// Handler para boton desconectar
void MainWindow::disconnectButton()
{
	_statusLabel.set_text("Desconectando...");
	// Aqui el codigo para desconectar del daemon...
	setConnectedState(false);
	_statusLabel.set_text("Desconectado");
}

// Handler para enviar mensaje con Enter
void MainWindow::entryActivated()
{
	sendButton();
}

// Handler para boton enviar
void MainWindow::sendButton()
{
	if (!_entryMessage.get_text().empty())
	{
		// Simuula mostrar el mensaje en el area de mensajes
		auto buffer = _textView.get_buffer();
		std::string current = buffer->get_text();
		std::string news = "Tu: " + _entryMessage.get_text() + "\n";
		buffer->set_text(current + news);

		// Codigo real para el envio al daemo

		_entryMessage.set_text(""); //Limpia la entrada
	}
}

void	MainWindow::setConnectedState(bool connected)
{
	_btnConnect.set_sensitive(!connected);	// Solo habilitado si NO esta conectado
	_btnDisconnect.set_sensitive(connected);// Solo habilitado si esta conectado
	_entryIP.set_sensitive(!connected);		// No editable si esta conectado
	_entryPort.set_sensitive(!connected);	// No editable si esta conectado
	_entryMessage.set_sensitive(connected);	// Solo se puede escribir si esta conectado
	_buttonSend.set_sensitive(connected);	// Solo se puede enviar si esta conectado
}
