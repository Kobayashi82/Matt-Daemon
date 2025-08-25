#include "Gui/MainWindow.hpp"

MainWindow::MainWindow() :
	_vBox(Gtk::Orientation::VERTICAL),
	_HBoxConnection(Gtk::Orientation::HORIZONTAL),
	_labelIP("IP:"),
	_labelPort("Puerto:"),
    _labelUser("Username:"),
	_btnConnect("Conectar"),
	_btnDisconnect("Desconectar"),
	_buttonSend("Enviar"),
	_statusLabel("Desconectado")
{
	set_title("Casey_AFK - Cliente gráfico de logs");
	set_default_size(1000, 800);

	_entryIP.set_placeholder_text("Dirección IP o host");
	_entryPort.set_placeholder_text("Puerto (4242)");
    _entryUser.set_placeholder_text("Username");
	_HBoxConnection.append(_labelIP);
	_HBoxConnection.append(_entryIP);
	_HBoxConnection.append(_labelPort);
	_HBoxConnection.append(_entryPort);
    _HBoxConnection.append(_labelUser);
    _HBoxConnection.append(_entryUser);
	_HBoxConnection.append(_btnConnect);
	_HBoxConnection.append(_btnDisconnect);

	_scrolledWindow.set_child(_textView);
	_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	_textView.set_editable(false);

	Gtk::Box* hBoxSend = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	hBoxSend->append(_entryMessage);
	hBoxSend->append(_buttonSend);

	_vBox.append(_HBoxConnection);
	_vBox.append(_scrolledWindow);
	_vBox.append(*hBoxSend);
	_vBox.append(_statusLabel);

	set_child(_vBox);

	_btnConnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::connectButton));
	_btnDisconnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::disconnectButton));
	_buttonSend.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::sendButton));

	setConnectedState(false);
}

MainWindow::~MainWindow() {}

void MainWindow::connectButton() {
	_statusLabel.set_text("Conectando...");
	setConnectedState(true);
	_statusLabel.set_text("Conectado");
}

void MainWindow::disconnectButton() {
	_statusLabel.set_text("Desconectando...");
	setConnectedState(false);
	_statusLabel.set_text("Desconectado");
}

void MainWindow::sendButton() {
	if (!_entryMessage.get_text().empty()) {
		auto buffer = _textView.get_buffer();
		std::string current = buffer->get_text();
		std::string news = "Tú: " + _entryMessage.get_text() + "\n";
		buffer->set_text(current + news);
		_entryMessage.set_text("");
	}
}

void MainWindow::setConnectedState(bool connected) {
	_btnConnect.set_sensitive(!connected);
	_btnDisconnect.set_sensitive(connected);
	_entryIP.set_sensitive(!connected);
	_entryPort.set_sensitive(!connected);
	_entryMessage.set_sensitive(connected);
	_buttonSend.set_sensitive(connected);
}
