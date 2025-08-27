#include "Gui/MainWindow.hpp"
#include <iostream>

MainWindow::MainWindow() :
	_vBox(Gtk::Orientation::VERTICAL),
	_HBoxConnection(Gtk::Orientation::HORIZONTAL),
	_labelIP("IP:"),
	_labelPort("Port:"),
    _labelUser("Username:"),
	_btnConnect("Connect"),
	_btnDisconnect("Disconnect"),
	_btnQuit("Close Server"),
	_buttonSend("Send"),
	_statusLabel("Disconnected")
{
	set_title("Casey_AFK - Graphical log client");
	set_default_size(1000, 800);

	_entryMessage.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::sendButton));
	_entryIP.set_text("127.0.0.1");
	_entryIP.set_placeholder_text("IP address or host");
	_entryPort.set_text("4242");
	_entryPort.set_placeholder_text("Port (4242)");

	// Autocomplete with system user if exists
	const char* sysuser = getenv("USER");
	if (sysuser && strlen(sysuser) > 0)
		_entryUser.set_text(sysuser);
	_entryUser.set_placeholder_text("Username");

	_HBoxConnection.append(_labelIP);
	_HBoxConnection.append(_entryIP);
	_HBoxConnection.append(_labelPort);
	_HBoxConnection.append(_entryPort);
    _HBoxConnection.append(_labelUser);
    _HBoxConnection.append(_entryUser);
	_HBoxConnection.append(_btnConnect);
	_HBoxConnection.append(_btnDisconnect);
	_HBoxConnection.append(_btnQuit);

	// Ajustar tamaño del área de logs
	_scrolledWindow.set_child(_textView);
	_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	_scrolledWindow.set_min_content_height(400);
	_textView.set_editable(false);
	_textView.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);

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
	_btnQuit.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::quitButton));
	_buttonSend.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::sendButton));

	// Conectar el dispatcher para comunicación segura entre hilos
	_networkManager.getDispatcher().connect(sigc::mem_fun(*this, &MainWindow::onLogReceived));

	setConnectedState(false);
}

MainWindow::~MainWindow() {
	_networkManager.disconnectFromServer();
}

void MainWindow::connectButton() {
	std::string ip = _entryIP.get_text();
	std::string portStr = _entryPort.get_text();
	int port = portStr.empty() ? 4242 : std::stoi(portStr);
	std::string user = _entryUser.get_text();
	if (user.empty()) user = "Unknown";

	if (_networkManager.connectToServer(ip, port, user)) {
		_statusLabel.set_text("Connected to " + ip + ":" + std::to_string(port));
		setConnectedState(true);
		_networkManager.startReceiving();
	} else {
		_statusLabel.set_text("Cannot connect to server. The server may be closed.");
	}
}

void MainWindow::disconnectButton() {
	_statusLabel.set_text("Disconnecting...");
	setConnectedState(false);
	_networkManager.disconnectFromServer();
	_statusLabel.set_text("Disconnected");
}

void MainWindow::sendButton() {
	if (!_entryMessage.get_text().empty() && _networkManager.isConnected()) {
		std::string message = _entryMessage.get_text();
		if (_networkManager.sendMessage(message)) {
			std::string formattedMessage = _logHandler.formatUserMessage(message) + "\n";
			_logHandler.appendToDisplay(formattedMessage);
			auto buffer = _textView.get_buffer();
			buffer->set_text(_logHandler.getDisplayText());
			Glib::signal_idle().connect_once([this]() {
				auto adjustment = _scrolledWindow.get_vadjustment();
				adjustment->set_value(adjustment->get_upper() - adjustment->get_page_size());
			});
			_entryMessage.set_text("");
		}
	}
}

void MainWindow::onLogReceived() {
	std::string rawData = _networkManager.getPendingData();
	if (!rawData.empty()) {
		std::string formattedLogs = _logHandler.processLogs(rawData);
		if (!formattedLogs.empty()) {
			_logHandler.appendToDisplay(formattedLogs);
			auto buffer = _textView.get_buffer();
			buffer->set_text(_logHandler.getDisplayText());
			Glib::signal_idle().connect_once([this]() {
				auto adjustment = _scrolledWindow.get_vadjustment();
				adjustment->set_value(adjustment->get_upper() - adjustment->get_page_size());
			});
		}
	}
}

void MainWindow::setConnectedState(bool connected) {
	_btnConnect.set_sensitive(!connected);
	_btnDisconnect.set_sensitive(connected);
	_btnQuit.set_sensitive(connected);
	_entryIP.set_sensitive(!connected);
	_entryPort.set_sensitive(!connected);
	_entryMessage.set_sensitive(connected);
	_buttonSend.set_sensitive(connected);
}

void MainWindow::quitButton() {
    // Show confirmation dialog
    auto dialog = new Gtk::Dialog("Close Server", *this, true); // modal
    dialog->set_default_size(300, 100);

    auto* box = dialog->get_content_area();
    auto* label = Gtk::make_managed<Gtk::Label>("Are you sure you want to close the server?");
    label->set_margin(10);
    box->append(*label);

    auto* detail_label = Gtk::make_managed<Gtk::Label>("This action will close the server and disconnect all clients.");
    detail_label->set_margin(10);
    box->append(*detail_label);

    // Add buttons
    dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
    dialog->add_button("Close Server", Gtk::ResponseType::ACCEPT);

    dialog->signal_response().connect([this, dialog](int response) {
        if (response == Gtk::ResponseType::ACCEPT) {
            // Send "/QUIT" to server
            if (_networkManager.sendMessage("/QUIT")) {
                // Show message that server has been closed
                std::string serverClosedMessage = "[SERVER] The server has been closed.\n";
                _logHandler.appendToDisplay(serverClosedMessage);
                auto buffer = _textView.get_buffer();
                buffer->set_text(_logHandler.getDisplayText());
                Glib::signal_idle().connect_once([this]() {
                    auto adjustment = _scrolledWindow.get_vadjustment();
                    adjustment->set_value(adjustment->get_upper() - adjustment->get_page_size());
                });

                // Disconnect automatically
                disconnectButton();
            } else {
                _statusLabel.set_text("Error sending close command");
            }
        }
        delete dialog;
    });

    dialog->present();
}
