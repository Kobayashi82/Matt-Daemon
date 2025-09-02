#include <gtkmm.h>
#include "Gui/MainWindow.hpp"

int main(int argc, char** argv) 
{
	auto app = Gtk::Application::create("org.caseyafk.gui");
	app->signal_activate().connect([&]() {
		MainWindow* window = new MainWindow();
		app->add_window(*window);
		window->show();
	});
	return app->run(argc, argv);
}
