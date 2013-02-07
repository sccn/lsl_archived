#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Objbase.h>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

// names for the Windows virtual-key codes
const char *const key_names[256] = {"RESERVED00",
	"LBUTTON", "RBUTTON", "CANCEL", "MBUTTON", "XBUTTON1", "XBUTTON2", "BELL", "BACK", "TAB", "RESERVED0A", "RESERVED0B", "CLEAR", "RETURN",
	"RESERVED0E", "RESERVED0F", "SHIFT", "CONTROL", "MENU", "PAUSE", "CAPITAL", "KANA", "RESERVED16", "JUNJA", "FINAL", "KANJI", "UNASSIGNED1A",
	"ESCAPE", "CONVERT", "NONCONVERT", "ACCEPT", "MODECHANGE", "SPACE", "PRIOR", "NEXT", "END", "HOME", "LEFT", "UP", "RIGHT", "DOWN", "SELECT",
	"PRINT", "EXECUTE", "SNAPSHOT", "INSERT", "DELETE", "HELP", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "UNASSIGNED3A", "UNASSIGNED3B",
	"UNASSIGNED3C", "UNASSIGNED3D", "UNASSIGNED3E", "UNASSIGNED3F", "UNASSIGNED40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "LWIN", "RWIN", "APPS", "RESERVED5E", "SLEEP", "NUMPAD0", "NUMPAD1", "NUMPAD2",
	"NUMPAD3", "NUMPAD4", "NUMPAD5", "NUMPAD6", "NUMPAD7", "NUMPAD8", "NUMPAD9", "MULTIPLY", "ADD", "SEPARATOR", "SUBTRACT", "DECIMAL", "DIVIDE",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22",
	"F23", "F24", "UNASSIGNED88", "UNASSIGNED89", "UNASSIGNED8A", "UNASSIGNED8B", "UNASSIGNED8C", "UNASSIGNED8D", "UNASSIGNED8E", "UNASSIGNED8F",
	"NUMLOCK", "SCROLL", "OEM_FJ_JISHO", "OEM_FJ_MASSHOU", "OEM_FJ_TOUROKU", "OEM_FJ_LOYA", "OEM_FJ_ROYA",	"UNASSIGNED97", "UNASSIGNED98",
	"UNASSIGNED99", "UNASSIGNED9A", "UNASSIGNED9B", "UNASSIGNED9C", "UNASSIGNED9D", "UNASSIGNED9E", "UNASSIGNED9F", "LSHIFT", "RSHIFT", "LCONTROL",
	"RCONTROL", "LMENU", "RMENU", "BROWSER_BACK", "BROWSER_FORWARD", "BROWSER_REFRESH", "BROWSER_STOP", "BROWSER_SEARCH", "BROWSER_FAVORITES",
	"BROWSER_HOME", "VOLUME_MUTE", "VOLUME_DOWN", "VOLUME_UP", "MEDIA_NEXT_TRACK", "MEDIA_PREV_TRACK", "MEDIA_STOP", "MEDIA_PLAY_PAUSE", "LAUNCH_MAIL",
	"LAUNCH_MEDIA_SELECT", "LAUNCH_APP1", "LAUNCH_APP2", "RESERVEDB8", "RESERVEDB9", "US_SEMICOLON", "PLUS", "COMMA", "MINUS", "PERIOD", "US_SLASH",
	"US_TILDE", "RESERVEDC1", "RESERVEDC2", "RESERVEDC3", "RESERVEDC4", "RESERVEDC5", "RESERVEDC6", "RESERVEDC7", "RESERVEDC8", "RESERVEDC9", "RESERVEDCA",
	"RESERVEDCB", "RESERVEDCC", "RESERVEDCD", "RESERVEDCE", "RESERVEDCF", "RESERVEDD0", "RESERVEDD1", "RESERVEDD2", "RESERVEDD3", "RESERVEDD4", "RESERVEDD5",
	"RESERVEDD6", "RESERVEDD7", "UNASSIGNEDD8", "UNASSIGNEDD9", "UNASSIGNEDDA", "US_SQUARE_BRACKET_OPEN", "US_BACKSLASH", "US_SQUARE_BRACKET_CLOSE",
	"US_QUOTE", "OEM_8", "RESERVEDE0", "OEM_AX", "OEM_102", "ICO_HELP", "ICO_00", "PROCESSKEY", "ICO_CLEAR", "PACKET", "RESERVEDE8", "OEM_RESET",
	"OEM_JUMP", "OEM_PA1", "OEM_PA2", "OEM_PA3", "OEM_WSCTRL", "OEM_CUSEL", "OEM_ATTN", "OEM_FINISH", "OEM_COPY", "OEM_AUTO", "OEM_ENLW", "OEM_BACKTAB",
	"ATTN", "CRSEL", "EXSEL", "EREOF", "PLAY", "ZOOM", "NONAME", "PA1", "OEM_CLEAR", "RESERVEDFF"};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_keyboard()));
	theWindow = this;
	for (int k=0;k<256;k++)
		isPressed_[k] = false;
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (outlet_)
		ev->ignore();
}

void MainWindow::link_keyboard() {
	if (outlet_) {
		// === perform unlink action ===
		try {
			UnhookWindowsHookEx(kbdHook_);
			outlet_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Unable to unlink: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		bool hooked = false;
		try {
			// install low-level keyboard hook
			kbdHook_ = SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)keyboard_callback,GetModuleHandle(NULL),0);
			if (!kbdHook_)
				throw std::runtime_error("Cannot install keyboard hook. Please make sure that this program has the permission to grab the Windows keyboard events.");
			hooked = true;
			// create streaminfo and outlet
			lsl::stream_info info("Keyboard","Markers",1,lsl::IRREGULAR_RATE,lsl::cf_string,"KeyboardCapture_" + boost::asio::ip::host_name());
			outlet_.reset(new lsl::stream_outlet(info));
			// now we are linked
			ui->linkButton->setText("Unlink");
		} catch(std::runtime_error &e) {
			if (hooked)
				UnhookWindowsHookEx(kbdHook_);
			// and show message box
			QMessageBox::critical(this,"Initialization Error", (std::string("Unable to link: ") += e.what()).c_str(),QMessageBox::Ok);
		}		
	}
}

LRESULT MainWindow::on_key(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0 && outlet_) {
		unsigned char key = 0;
		std::string evstr;
		switch(wParam) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			key = ((KBDLLHOOKSTRUCT*)lParam)->vkCode & 0xFF;
			if (!isPressed_[key]) {
				evstr = key_names[key] + std::string(" pressed");
				isPressed_[key] = true;
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			key = ((KBDLLHOOKSTRUCT*)lParam)->vkCode & 0xFF;
			evstr = key_names[key] + std::string(" released");
			isPressed_[key] = false;
			break;
		default:
			;
		}
		if (!evstr.empty())
			outlet_->push_sample(&evstr);
	}
	return CallNextHookEx(kbdHook_, code, wParam, lParam);
}

LRESULT MainWindow::keyboard_callback(int code, WPARAM wParam, LPARAM lParam) {
	return theWindow->on_key(code,wParam,lParam);
}

MainWindow::~MainWindow() {
	delete ui;
}
