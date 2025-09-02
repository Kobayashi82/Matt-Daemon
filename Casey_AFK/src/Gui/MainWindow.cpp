#include "Gui/MainWindow.hpp"
#include <iostream>
#include <string>

std::string get_css() {
	return (
		"/* === Root window === */\n"
		"window#main-window {\n"
		"    background-color: #2e3440;\n"
		"    color: #00d0ff;\n"
		"    font-family: \"Segoe UI\", \"Roboto\", sans-serif;\n"
		"    font-size: 14px;\n"
		"}\n"
		"\n"
		"/* Estilos base para todos los botones */\n"
		"button {\n"
		"    background-image: none;\n"
		"    background-color: #4c566a;\n"
		"    color: #eceff4;\n"
		"    border-radius: 6px;\n"
		"    padding: 2px 14px;\n"
		"    border: 1px solid #3b4252;\n"
		"    box-shadow: 0 2px 2px rgba(0,0,0,0.35), inset 0 1px 0 rgba(255,255,255,0.04);\n"
		"    transition: background-color 120ms ease, transform 80ms ease, box-shadow 120ms ease;\n"
		"}\n"
		"\n"
		"button:focus {\n"
		"    outline: 2px solid #88c0d0;\n"
		"    outline-offset: 1px;\n"
		"}\n"
		"\n"
		"button:hover {\n"
		"    background-color: #556074;\n"
		"}\n"
		"\n"
		"button:active {\n"
		"    background-color: #3f4755;\n"
		"    transform: translateY(1px);\n"
		"    box-shadow: 0 1px 1px rgba(0,0,0,0.4), inset 0 1px 0 rgba(0,0,0,0.4);\n"
		"}\n"
		"\n"
		"button:disabled {\n"
		"    background-color: #3b4252;\n"
		"    color: #6d7682;\n"
		"    box-shadow: none;\n"
		"}\n"
		"\n"
		"/* Button variants */\n"
		"/* Connect and Send Button */\n"
		"button.btn-primary {\n"
		"    background-color: #5e81ac;\n"
		"    border-color: #517094;\n"
		"}\n"
		"button.btn-primary:hover { background-color: #678cb9; }\n"
		"button.btn-primary:active { background-color: #4f6d8e; }\n"
		"button.btn-primary:disabled { background-color: #374356; color: #6f7b89; }\n"
		"\n"
		"/* Disconnect Button */\n"
		"button.btn-secondary {\n"
		"    background-color: #5e81ac;\n"
		"    border-color: #517094;\n"
		"}\n"
		"button.btn-secondary:hover { background-color: #678cb9; }\n"
		"button.btn-secondary:active { background-color: #4f6d8e; }\n"
		"button.btn-secondary:disabled { background-color: #374356; color: #6f7b89; }\n"
		"\n"
		"/* Clear Logs Button */\n"
		"button.btn-accent {\n"
		"    background-color: #5e81ac;\n"
		"    border-color: #517094;\n"
		"}\n"
		"button.btn-accent:hover { background-color: #678cb9; }\n"
		"button.btn-accent:active { background-color: #4f6d8e; }\n"
		"button.btn-accent:disabled { background-color: #374356; color: #6f7b89; }\n"
		"\n"
		"/* Close Server Button */\n"
		"button.btn-danger {\n"
		"    background-color: #bf616a;\n"
		"    border-color: #a75058;\n"
		"}\n"
		"button.btn-danger:hover { background-color: #c86f77; }\n"
		"button.btn-danger:active { background-color: #ac565f; }\n"
		"button.btn-danger { color: #ffecec; }\n"
		"button.btn-danger:disabled { background-color: #593136; color: #b78389; }\n"
		"\n"
		"/* Legacy primary selector kept (no-op or fallback) */\n"
		"button.primary { background-color: #88c0d0; }\n"
		"\n"
		"/* Log area (TextView) */\n"
		"textview#log-view {\n"
		"    background-color: #252b35;  /* Darker to contrast with main window (#2e3440) */\n"
		"    color: #e5e9f0;\n"
		"    font-family: \"JetBrains Mono\", \"Fira Mono\", monospace;\n"
		"    font-size: 13px;\n"
		"    border: 1px solid #434c5e;\n"
		"    border-radius: 6px;\n"
		"    padding: 4px;\n"
		"}\n"
		"\n"
		"/* Text inside textview */\n"
		"textview text { background: transparent; }\n"
		"\n"
		"/* ScrolledWindow for logs */\n"
		"scrolledwindow.log-view {\n"
		"    background-color: #252b35;\n"
		"    border: none;\n"
		"}\n"
		"\n"
		"scrolledwindow.log-view viewport {\n"
		"    background-color: #252b35;\n"
		"}\n"
		"\n"
		"/* Input fields: IP, Port, User, Logs, Message */\n"
		"entry {\n"
		"    background-color: #3a4250;\n"
		"    color: #eceff4;\n"
		"    border: 1px solid #434c5e;\n"
		"    border-radius: 5px;\n"
		"    padding: 0px 6px;\n"
		"    caret-color: #88c0d0;\n"
		"    transition: border-color 120ms ease, background-color 120ms ease;\n"
		"}\n"
		"entry:focus { border-color: #5e81ac; background-color: #424b59; }\n"
		"entry:disabled { background-color: #2c313a; color: #6d7682; }\n"
		"\n"
		"/* Placeholder text */\n"
		"entry > text placeholder { color: #7d8896; }\n"
		"\n"
		"/* Selected text in entries */\n"
		"entry selection {\n"
		"    background-color: #5e81ac;\n"
		"    color: #eceff4;\n"
		"}\n"
		"\n"
		"/* Scrollbars */\n"
		"scrollbar {\n"
		"    background-color: #2e3440;\n"
		"    border-radius: 6px;\n"
		"}\n"
		"\n"
		"scrollbar slider {\n"
		"    background-color: #5e81ac;\n"
		"    border-radius: 6px;\n"
		"}\n"
		"\n"
		"/* Thin scrollbar hover */\n"
		"scrollbar slider:hover { background-color: #678cb9; }\n"
		"\n"
		"/* Connection status label */\n"
		"label#status-label.connected { color: #ffffff; }  /* White for connected */\n"
		"label#status-label.disconnected { color: #ffffff; }  /* White for disconnected */\n");
}

MainWindow::MainWindow() :
	_vBox(Gtk::Orientation::VERTICAL),
	_HBoxConnection(Gtk::Orientation::VERTICAL),
	_labelIP("IP:"),
	_labelPort("Port:"),
    _labelUser("Username:"),
	_btnConnect("Connect"),
	_btnDisconnect("Disconnect"),
	_btnClearLogs("Clear Logs"),
	_btnQuit("Close Server"),
	_buttonSend("Send"),
	_statusLabel("Disconnected"),
	_labelNumLogs("Logs")
{
	set_title("Casey_AFK - Graphical log client");
	set_default_size(800, 450);
	set_name("main-window");
	loadCss();
	_statusLabel.set_name("status-label");

	// Assign custom style classes to buttons
	_btnConnect.get_style_context()->add_class("btn-primary");
	_btnDisconnect.get_style_context()->add_class("btn-secondary");
	_btnClearLogs.get_style_context()->add_class("btn-accent");
	_btnQuit.get_style_context()->add_class("btn-danger");
	_buttonSend.get_style_context()->add_class("btn-primary");

	// Name the TextView for specific styles
	_textView.set_name("log-view");

	// Create sub-box for inputs (horizontal)
    auto* inputBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    inputBox->set_spacing(10);  // 10 pixel spacing between elements in this row
    inputBox->set_margin(5);   // Small margin around the sub-box

	// Add entries to the sub-box (IP, port, user)
	inputBox->append(_labelIP);
	inputBox->append(_entryIP);
	inputBox->append(_labelPort);
	inputBox->append(_entryPort);
	inputBox->append(_labelUser);
	inputBox->append(_entryUser);
	// Add flexible space to push log controls to the right
	{
		auto* spacer = Gtk::make_managed<Gtk::Label>(" ");
		spacer->set_hexpand(true);
		spacer->set_halign(Gtk::Align::FILL);
		inputBox->append(*spacer);
	}
	// Log controls aligned to the right
	inputBox->append(_labelNumLogs);
	inputBox->append(_entryLogs);
	inputBox->append(_btnClearLogs);
//	inputBox->append(_btnQuit);  // Remove Quit from inputBox

    // Create sub-box for buttons (horizontal)
    auto* buttonBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    buttonBox->set_spacing(10);
    buttonBox->set_margin(5); 

	   // Add buttons to the sub-box
	   buttonBox->append(_btnConnect);
	   buttonBox->append(_btnDisconnect);

	   // Sub-box to center the label and align Close Server to the right
	   auto* centerBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	   centerBox->set_hexpand(true);
	   centerBox->set_halign(Gtk::Align::FILL);
	   _statusLabel.set_hexpand(true);
	   _statusLabel.set_halign(Gtk::Align::CENTER);
	   centerBox->append(_statusLabel);
	   _btnQuit.set_hexpand(false);
	   _btnQuit.set_halign(Gtk::Align::END);
	   centerBox->append(_btnQuit);

	   buttonBox->append(*centerBox);

    // Add sub-boxes to _HBoxConnection (now vertical)
    _HBoxConnection.set_spacing(15);  // 15 pixel spacing between rows (inputs and buttons)
    _HBoxConnection.append(*inputBox);
    _HBoxConnection.append(*buttonBox);

    // Optional: Add visual separator between rows
    auto* separator = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    _HBoxConnection.append(*separator);  // Subtle dividing line

		// Configuration of input fields
		_entryMessage.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::sendButton));
		_entryIP.set_text("localhost");
		_entryIP.set_placeholder_text("IP address or host");
		_entryIP.set_width_chars(15);
		_entryIP.set_max_width_chars(15);
		_entryIP.set_hexpand(false);
		_entryIP.set_halign(Gtk::Align::START);

		_entryPort.set_text("4242");
		_entryPort.set_placeholder_text("Port");
		_entryPort.set_width_chars(5);
		_entryPort.set_max_width_chars(5);
		_entryPort.set_hexpand(false);
		_entryPort.set_halign(Gtk::Align::START);

		_entryLogs.set_text("20");
		_entryLogs.set_placeholder_text("");
		_entryLogs.set_width_chars(2);
		_entryLogs.set_max_width_chars(2);
		_entryLogs.set_hexpand(false);
		_entryLogs.set_halign(Gtk::Align::START);

	// Autocomplete with system user if exists
	const char* sysuser = getenv("USER");
	if (sysuser && strlen(sysuser) > 0)
		_entryUser.set_text(sysuser);
	_entryUser.set_placeholder_text("Username");
	_entryUser.set_width_chars(10);  // Width for usernames
	_entryUser.set_size_request(100, -1);  // Force width in pixels

	// Adjust log area size
	_scrolledWindow.set_child(_textView);
	_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	_scrolledWindow.set_min_content_height(400);
	_scrolledWindow.set_margin_bottom(10);  // Reduced separation between logs and send box
	_textView.set_editable(false);
	_textView.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);

	// Box for sending messages: entry on the left and button on the right
	Gtk::Box* hBoxSend = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	hBoxSend->set_spacing(5);
	hBoxSend->set_hexpand(true);  // to occupy the full available width

	// Message entry: expand to fill space
	_entryMessage.set_hexpand(true);
	_entryMessage.set_halign(Gtk::Align::FILL);

	// Send button: stuck to the right
	_buttonSend.set_hexpand(false);
	_buttonSend.set_halign(Gtk::Align::END);

	hBoxSend->append(_entryMessage);
	hBoxSend->append(_buttonSend);

	_vBox.append(_HBoxConnection);
	_vBox.append(_scrolledWindow);
	_vBox.append(*hBoxSend);

	// Margin to the main box
	_vBox.set_margin(10);

	set_child(_vBox);

	_btnConnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::connectButton));
	_btnDisconnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::disconnectButton));
	_btnQuit.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::quitButton));
	_btnClearLogs.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::clearLogsButton));
	_buttonSend.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::sendButton));

	// Connect the dispatcher for safe inter-thread communication
	_networkManager.getDispatcher().connect(sigc::mem_fun(*this, &MainWindow::onLogReceived));
	_networkManager.getDisconnectDispatcher().connect(sigc::mem_fun(*this, &MainWindow::onConnectionLost));

	// Handle window close request to properly disconnect
	signal_close_request().connect([this]() -> bool {
		_networkManager.disconnectFromServer();
		return false; // Allow the window to close
	}, false);

	setConnectedState(false);
	_btnConnect.grab_focus();
}

MainWindow::~MainWindow() {
	_networkManager.disconnectFromServer();
}

void MainWindow::connectButton() {
	std::string ip = _entryIP.get_text();
	std::string portStr = _entryPort.get_text();
	std::string user = _entryUser.get_text();
	std::string logStr = _entryLogs.get_text();

	if (ip == "localhost") {
		ip = "127.0.0.1";
	}

	if (user.empty()) {
		_statusLabel.set_text("Error: Username is required.");
		return; 
	}
	int port = portStr.empty() ? 4242 : std::stoi(portStr);
	int numLogs = 0;
	try {
		numLogs = std::stoi(logStr);
		if (numLogs < 0) numLogs = 0;
	} catch (...) {
		numLogs = 0; 
	}

	if (_networkManager.connectToServer(ip, port, user, numLogs)) {
		_statusLabel.set_text("Connected to " + ip + ":" + std::to_string(port));
		setConnectedState(true);
		// Clear old logs on connect
		_logHandler.clearDisplay();
		auto buffer = _textView.get_buffer();
		buffer->set_text("");
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
			_entryMessage.set_text("");
		}
	}
}

void MainWindow::onLogReceived() {
	std::string rawData = _networkManager.getPendingData();
	if (!rawData.empty()) {
		// Check for connection timeout message
		if (rawData.find("Connection time-out") != std::string::npos) {
			_statusLabel.set_text("Connection time-out");
			return;  // Do not process as log
		}
		// Check for maximum connection reached message
		if (rawData.find("Maximun connection reached") != std::string::npos) {
			_statusLabel.set_text("Maximum connection reached");
			return;  // Do not process as log
		}
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

	// Update CSS classes of the status label
	auto ctx = _statusLabel.get_style_context();
	ctx->remove_class("connected");
	ctx->remove_class("disconnected");
	ctx->add_class(connected ? "connected" : "disconnected");
}

void MainWindow::quitButton() {
    // Show confirmation dialog
    auto dialog = new Gtk::Dialog("Close Server", *this, true); // modal
    dialog->set_default_size(400, 150);

    // Apply CSS to dialog
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path("/home/lxuxer/Matt-Daemon/Casey_AFK/res/styles.css");
    auto style_context = dialog->get_style_context();
    style_context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    auto* box = dialog->get_content_area();
    auto* vBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    vBox->set_spacing(10);
    vBox->set_margin(15);

    auto* label = Gtk::make_managed<Gtk::Label>("Are you sure you want to close the server?");
    label->set_halign(Gtk::Align::CENTER);
    vBox->append(*label);

    auto* detail_label = Gtk::make_managed<Gtk::Label>("This action will close the server and disconnect all clients.");
    detail_label->set_halign(Gtk::Align::CENTER);
    vBox->append(*detail_label);

    // Button box with separation
    auto* button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    button_box->set_spacing(10);
    button_box->set_margin_top(10);
    button_box->set_halign(Gtk::Align::CENTER);

    auto* cancel_btn = Gtk::make_managed<Gtk::Button>("Cancel");
    cancel_btn->get_style_context()->add_class("btn-secondary");
    cancel_btn->signal_clicked().connect([dialog]() { dialog->response(Gtk::ResponseType::CANCEL); });
    button_box->append(*cancel_btn);

    auto* close_btn = Gtk::make_managed<Gtk::Button>("Close Server");
    close_btn->get_style_context()->add_class("btn-danger");
    close_btn->signal_clicked().connect([dialog]() { dialog->response(Gtk::ResponseType::ACCEPT); });
    button_box->append(*close_btn);

    vBox->append(*button_box);

    box->append(*vBox);

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

void MainWindow::onConnectionLost() {
	_statusLabel.set_text("Connection lost");
	setConnectedState(false);
	_networkManager.disconnectFromServer();
}

void MainWindow::clearLogsButton() {
	_logHandler.clearDisplay(); 
	auto buffer = _textView.get_buffer();
	buffer->set_text("");
}

void MainWindow::loadCss() {
   auto provider = Gtk::CssProvider::create();
   provider->load_from_data(get_css());
   auto display = Gdk::Display::get_default();
   Gtk::StyleContext::add_provider_for_display(
	   display,
	   provider,
	   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
   );
}
