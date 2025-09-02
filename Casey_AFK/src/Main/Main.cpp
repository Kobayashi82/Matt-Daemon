#include <gtkmm.h>
#include <csignal>
#include "Gui/MainWindow.hpp"

// Global pointer to window for signal handling
MainWindow* global_window = nullptr;

void signal_handler(int signal) {
	if (global_window) {
		// Force disconnect when receiving SIGINT (Ctrl+C) or SIGTERM
		delete global_window;
		global_window = nullptr;
	}
	exit(signal);
}

int main() 
{
	// Set up signal handlers for proper cleanup
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	auto app = Gtk::Application::create("org.caseyafk.gui");
	app->signal_activate().connect([&]() {
		global_window = new MainWindow();
		app->add_window(*global_window);
		global_window->show();
	});
	
	int result = app->run();
	
	// Clean up
	if (global_window) {
		delete global_window;
		global_window = nullptr;
	}
	
	return result;
}
