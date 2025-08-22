#include "Gui/MainWindow.hpp"

// Constructor
MainWindow::MainWindow() :
	_vBox(Gtk::Orientation::VERTICAL),
	_HBoxConnection(Gtk::Orientation::HORIZONTAL),
	_optionsBox(Gtk::Orientation::HORIZONTAL),
	_userBox(Gtk::Orientation::HORIZONTAL),
	_entryIP(),
	_entryPort(),
	_entryUsername(),
	_checkInsecure("Allow insecure connection (--insecure)"),
	_labelIP("IP:"),
	_labelPort("Port:"),
	_labelUsername("Username:"),
	_labelVersion("Ben_AFK v1.0 - Matt_Daemon Client"),
	_btnConnect("Connect"),
	_btnDisconnect("Disconnect"),
	_buttonSend("Send"),
	_textView(),
	_scrolledWindow(),
	_entryMessage(),
	_statusLabel("")
{
	// Initial widget configuration
	set_title("Ben_AFK - Matt_Daemon Graphical Client");
	set_default_size(500, 400);

	// Widget flags
	_labelVersion.set_halign(Gtk::Align::CENTER);
	_labelVersion.set_markup("<b>Ben_AFK v1.0</b> - Matt_Daemon Client");
	_entryUsername.set_placeholder_text("Username (optional)");
	_entryUsername.set_text(getenv("USER") ? getenv("USER") : ""); // Current user by default
	_checkInsecure.set_active(false); // Secure by default

	// Connection panel
	_entryIP.set_placeholder_text("Server IP address");
	_entryPort.set_placeholder_text("Port (4242)");

	_HBoxConnection.append(_labelIP);
	_HBoxConnection.append(_entryIP);
	_HBoxConnection.append(_labelPort);
	_HBoxConnection.append(_entryPort);
	_HBoxConnection.append(_btnConnect);
	_HBoxConnection.append(_btnDisconnect);

	// User panel
	_userBox.append(_labelUsername);
	_userBox.append(_entryUsername);

	// Options panel
	_optionsBox.append(_checkInsecure);

	// Message area
	_scrolledWindow.set_child(_textView);
	_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	_textView.set_editable(false);

	// Message input and send button
	Gtk::Box* hBoxSend = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	_entryMessage.set_placeholder_text("Type your message...");
	hBoxSend->append(_entryMessage);
	hBoxSend->append(_buttonSend);

	// Status message
	_statusLabel.set_text("Disconnected");

	// Add everything to main container
	_vBox.append(_labelVersion);		// Version at top
	_vBox.append(_HBoxConnection);		// IP and Port
	_vBox.append(_userBox);				// Username
	_vBox.append(_optionsBox);			// Options (insecure checkbox)
	_vBox.append(_scrolledWindow);		// Message area
	_vBox.append(*hBoxSend);			// Send message
	_vBox.append(_statusLabel);			// Status

	set_child(_vBox);

	// Connect signals
	_btnConnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::connectButton));
	_btnDisconnect.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::disconnectButton));
	_entryMessage.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::entryActivated));
	_buttonSend.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::sendButton));

	// Initial state: disconnected
	setConnectedState(false);
}

// Destructor
MainWindow::~MainWindow() {}

// Handler for connect button
void MainWindow::connectButton()
{
	_statusLabel.set_text("Connecting...");
	// Here goes the real connection code...
	setConnectedState(true);
	_statusLabel.set_text("Connected");
}

// Handler for disconnect button
void MainWindow::disconnectButton()
{
	_statusLabel.set_text("Disconnecting...");
	// Here goes the code to disconnect from daemon...
	setConnectedState(false);
	_statusLabel.set_text("Disconnected");
}

// Handler for sending message with Enter
void MainWindow::entryActivated()
{
	sendButton();
}

// Handler for send button
void MainWindow::sendButton()
{
	if (!_entryMessage.get_text().empty())
	{
		// Simulate showing the message in the message area
		auto buffer = _textView.get_buffer();
		std::string current = buffer->get_text();
		std::string news = "You: " + _entryMessage.get_text() + "\n";
		buffer->set_text(current + news);

		// Real code for sending to daemon

		_entryMessage.set_text(""); // Clear the input
	}
}

void	MainWindow::setConnectedState(bool connected)
{
	_btnConnect.set_sensitive(!connected);      // Only enabled if NOT connected
	_btnDisconnect.set_sensitive(connected);    // Only enabled if connected
	_entryIP.set_sensitive(!connected);         // Not editable if connected
	_entryPort.set_sensitive(!connected);       // Not editable if connected
	_entryUsername.set_sensitive(!connected);   // Not editable if connected
	_checkInsecure.set_sensitive(!connected);   // Not editable if connected
	_entryMessage.set_sensitive(connected);     // Can only write if connected
	_buttonSend.set_sensitive(connected);       // Can only send if connected
}
