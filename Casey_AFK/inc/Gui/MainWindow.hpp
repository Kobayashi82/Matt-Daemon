#pragma once
#include <gtkmm.h>
#include "Network/NetworkManager.hpp"
#include "Log/LogHandler.hpp"

// ===============================
//  Casey_AFK Main Window
// ===============================
class MainWindow : public Gtk::Window {
public:
	MainWindow();              // Constructor: initializes the interface
	virtual ~MainWindow();     // Destructor

private:
	// --- Main Widgets ---
	Gtk::Box _vBox;                // Main vertical container
	Gtk::Box _HBoxConnection;      // Horizontal container for IP, port, user and buttons
	Gtk::Label _labelIP;           // "IP:" label
	Gtk::Label _labelPort;         // "Port:" label
	Gtk::Label _labelUser;         // "Username:" label
	Gtk::Entry _entryIP;           // IP address or host entry
	Gtk::Entry _entryPort;         // Port entry
	Gtk::Entry _entryUser;         // Username entry
	Gtk::Button _btnConnect;       // "Connect" button
	Gtk::Button _btnDisconnect;    // "Disconnect" button
	Gtk::Button _btnQuit;          // "Close Server" button
	Gtk::ScrolledWindow _scrolledWindow; // Scroll area for logs
	Gtk::TextView _textView;       // Read-only text area for logs
	Gtk::Entry _entryMessage;      // Message input field
	Gtk::Button _buttonSend;       // Send message button
	Gtk::Label _statusLabel;       // Status label (connected/disconnected)

	// --- Modular Components ---
	NetworkManager _networkManager; // Handles network connections
	LogHandler _logHandler;         // Handles log processing

	// --- Event Methods ---
	void connectButton();          // Handles connect event
	void disconnectButton();       // Handles disconnect event
	void quitButton();             // Handles close server event
	void sendButton();             // Handles send message event
	void setConnectedState(bool connected); // Changes widget states
	void onLogReceived();          // Handles log arrival from thread
};
