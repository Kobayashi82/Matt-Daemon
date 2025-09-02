#include <gtkmm.h>
#include "Gui/MainWindow.hpp"

int main() 
{
	auto app = Gtk::Application::create("org.caseyafk.gui", Gio::Application::Flags::NON_UNIQUE);
	
	MainWindow* window = nullptr;
	
	app->signal_activate().connect([&]() {
		window = new MainWindow();
		app->add_window(*window);
		window->show();
	});
	
	int result = app->run();
	
	if (window) {
		delete window;
		window = nullptr;
	}
	
	return result;
}
