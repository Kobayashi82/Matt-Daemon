#include "Gui/MainWindow.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <errno.h>

MainWindow::MainWindow() :
	_vBox(Gtk::Orientation::VERTICAL),
	_HBoxConnection(Gtk::Orientation::HORIZONTAL),
	_labelIP("IP:"),
	_labelPort("Puerto:"),
    _labelUser("Username:"),
	_btnConnect("Conectar"),
	_btnDisconnect("Desconectar"),
	_buttonSend("Enviar"),
	_statusLabel("Desconectado"),
	_running(false)
{
	set_title("Casey_AFK - Cliente gráfico de logs");
	set_default_size(1000, 800);

	_entryMessage.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::sendButton));
	_entryIP.set_text("127.0.0.1");
	_entryIP.set_placeholder_text("Dirección IP o host");
	_entryPort.set_text("4242");
	_entryPort.set_placeholder_text("Puerto (4242)");
	// Autocompletar con el usuario del sistema si existe
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


	// Ajustar tamaño del área de logs
	_scrolledWindow.set_child(_textView);
	_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	_scrolledWindow.set_min_content_height(400); // Altura mínima más grande
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
	_buttonSend.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::sendButton));

	// Conectar el dispatcher para comunicación segura entre hilos
	_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::onLogReceived));

	setConnectedState(false);
}

MainWindow::~MainWindow() {}

void MainWindow::connectButton() 
{
	std::string ip = _entryIP.get_text();
	std::string portStr = _entryPort.get_text();
	int port = portStr.empty() ? 4242 : std::stoi(portStr);

	// Crear socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		_running = false;
		if (_recvThread.joinable())
			_recvThread.join();
		_statusLabel.set_text("Error creando socket");
		return;
	}

	sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
		_statusLabel.set_text("IP inválida");
		::close(_sockfd);
		_sockfd = -1;
		return;
	}

	// Conectar
	if (connect(_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		_statusLabel.set_text("No se pudo conectar");
		::close(_sockfd);
		_sockfd = -1;
		return;
	}

	_statusLabel.set_text("Conectado a " + ip + ":" + std::to_string(port));
	setConnectedState(true);

	// Construir mensaje inicial
	std::string user = _entryUser.get_text();
	if (user.empty()) user = "Unknown";
	std::string msg = "/CASEY user=" + user + " 10\n";
	send(_sockfd, msg.c_str(), msg.size(), 0);

	// Lanzar hilo de recepción de logs
	_running = true;
	_recvThread = std::thread([this]() {
		char buffer[1024];
		while (_running) {
			ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
			if (n > 0) {
				buffer[n] = '\0';
				std::cout << "[DEBUG] Recibido del servidor: '" << buffer << "'" << std::endl; // Depuración
				{
					std::lock_guard<std::mutex> lock(_logMutex);
					_pendingLogs += buffer;
				}
				_dispatcher.emit();
			} else if (n == 0) {
				std::cout << "[DEBUG] Conexión cerrada por el servidor" << std::endl; // Depuración
				_running = false;
				break;
			} else {
				std::cout << "[DEBUG] Error en recv: " << strerror(errno) << std::endl; // Depuración
				_running = false;
				break;
			}
		}
	});
}

void MainWindow::disconnectButton() {
	_statusLabel.set_text("Desconectando...");
	setConnectedState(false);
	_statusLabel.set_text("Desconectado");
}

void MainWindow::sendButton() {
	if (!_entryMessage.get_text().empty() && _sockfd > 0) {
		std::string msg = _entryMessage.get_text() + "\n";
		send(_sockfd, msg.c_str(), msg.size(), 0);
		auto buffer = _textView.get_buffer();
		std::string current = buffer->get_text();
		std::string news = "Tú: " + _entryMessage.get_text() + "\n";
		buffer->set_text(current + news);
		_entryMessage.set_text("");
	}
}
// --- Recepción segura de logs desde el hilo ---
void MainWindow::onLogReceived() {
	std::cout << "[DEBUG] onLogReceived() llamada" << std::endl; // Depuración
	std::string logs;
	{
		std::lock_guard<std::mutex> lock(_logMutex);
		logs.swap(_pendingLogs);
	}
	std::cout << "[DEBUG] Logs a procesar: '" << logs << "'" << std::endl; // Depuración
	if (!logs.empty()) {
		// Prefijar cada línea recibida con [LOG]
		std::string formatted;
		size_t start = 0, end;
		while ((end = logs.find('\n', start)) != std::string::npos) {
			std::string line = logs.substr(start, end - start);
			if (!line.empty())
				formatted += "[LOG] " + line + "\n";
			start = end + 1;
		}
		// Si hay texto sin salto final
		if (start < logs.size())
			formatted += "[LOG] " + logs.substr(start) + "\n";
		std::cout << "[DEBUG] Texto formateado: '" << formatted << "'" << std::endl; // Depuración
		auto buffer = _textView.get_buffer();
		std::string current = buffer->get_text();
		buffer->set_text(current + formatted);
		std::cout << "[DEBUG] Texto actualizado en GUI" << std::endl; // Depuración
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
