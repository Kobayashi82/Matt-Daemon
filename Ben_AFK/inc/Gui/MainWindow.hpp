#include <iostream>
#include <gtkmm.h>

// Class to create a graphical window with Gtkmm4
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

		// Enable/disable widgets according to state
		void setConnectedState(bool connect);

	private:
		// Layout containers
		Gtk::Box			_vBox;					// Main vertical container
		Gtk::Box			_HBoxConnection;		// Horizontal container for connection
		Gtk::Box			_optionsBox;			// Container for flags
		Gtk::Box			_userBox;				// Container for username

		// Connection widgets
		Gtk::Entry			_entryIP;				// IP address entry
		Gtk::Entry			_entryPort;				// Port entry
		Gtk::Entry			_entryUsername;			// Username entry

		// Options/flags
		Gtk::CheckButton	_checkInsecure;			// Checkbox for --insecure

		// Labels
		Gtk::Label			_labelIP;				// Label "IP:"
		Gtk::Label			_labelPort;				// Label "Port:"
		Gtk::Label			_labelUsername;			// Label "Username:"
		Gtk::Label			_labelVersion;			// Show version
		
		// Connect/disconnect/send buttons
		Gtk::Button			_btnConnect;			// Connect button
		Gtk::Button			_btnDisconnect;			// Disconnect button
		Gtk::Button			_buttonSend;			// Send button
		
		// Message area
		Gtk::TextView		_textView;				// Area for received and sent messages
		Gtk::ScrolledWindow _scrolledWindow;		// Allows message area to scroll if there are many messages
		
		// Message input
		Gtk::Entry			_entryMessage;			// Where user types message to send

		// Status messages
		Gtk::Label			_statusLabel;			// To show user status messages
};
