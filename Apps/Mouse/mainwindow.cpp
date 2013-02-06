#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Objbase.h>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_mouse()));
	theWindow = this;
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (outletButtons_ || outletPosition_)
		ev->ignore();
}

void MainWindow::link_mouse() {
	if (outletButtons_ && outletPosition_) {
		// === perform unlink action ===
		try {
			UnhookWindowsHookEx(mouseHook_);
			outletButtons_.reset();
			outletPosition_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Unable to unlink: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		bool hooked = false;
		try {
			// install low-level mouse hook
			mouseHook_ = SetWindowsHookEx(WH_MOUSE_LL,(HOOKPROC)mouse_callback,GetModuleHandle(NULL),0);
			if (!mouseHook_)
				throw std::runtime_error("Cannot install mouse hook. Please make sure that this program has the permission to grab the Windows mouse events.");
			hooked = true;
			// create streaminfo and outlet for buttons
			lsl::stream_info infoButtons("MouseButtons","Markers",1,lsl::IRREGULAR_RATE,lsl::cf_string,"MouseButtonCapture_" + boost::asio::ip::host_name());
			outletButtons_.reset(new lsl::stream_outlet(infoButtons));
			// create streaminfo and outlet for the position
			lsl::stream_info infoPosition("MousePosition","Position",2,lsl::IRREGULAR_RATE,lsl::cf_float32,"MousePositionCapture_" + boost::asio::ip::host_name());
			lsl::xml_element setup = infoPosition.desc().append_child("setup");
			lsl::xml_element display = setup.append_child("display");
			display.append_child_value("monitors",boost::lexical_cast<std::string>(GetSystemMetrics(SM_CMONITORS)).c_str());
			display.append_child("resolution_primary")
				.append_child_value("X",boost::lexical_cast<std::string>(GetSystemMetrics(SM_CXSCREEN)).c_str())
				.append_child_value("Y",boost::lexical_cast<std::string>(GetSystemMetrics(SM_CYSCREEN)).c_str());
			display.append_child("resolution_virtual")
				.append_child_value("X",boost::lexical_cast<std::string>(GetSystemMetrics(SM_XVIRTUALSCREEN)).c_str())
				.append_child_value("Y",boost::lexical_cast<std::string>(GetSystemMetrics(SM_YVIRTUALSCREEN)).c_str());
			lsl::xml_element channels = infoPosition.desc().append_child("channels");
			channels.append_child("channel")
				.append_child_value("label","MouseX")
				.append_child_value("type","PositionX")
				.append_child_value("unit","pixels")
				.append_child_value("origin","top");
			channels.append_child("channel")
				.append_child_value("label","MouseY")
				.append_child_value("type","PositionY")
				.append_child_value("unit","pixels")
				.append_child_value("origin","left");
			outletPosition_.reset(new lsl::stream_outlet(infoPosition));
			// now we are linked
			ui->linkButton->setText("Unlink");
		} catch(std::runtime_error &e) {
			if (hooked)
				UnhookWindowsHookEx(mouseHook_);
			// and show message box
			QMessageBox::critical(this,"Initialization Error", (std::string("Unable to link: ") += e.what()).c_str(),QMessageBox::Ok);
		}		
	}
}

LRESULT MainWindow::on_event(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0 && outletButtons_ && outletPosition_) {
		MSLLHOOKSTRUCT *ev = (MSLLHOOKSTRUCT*)lParam;
		std::string evstr;
		bool moved = false;
		short mouseUpper = ev->mouseData >> 16;
		switch(wParam) {
		case WM_LBUTTONDOWN:
			evstr = "MouseButtonLeft pressed";
			break;
		case WM_LBUTTONUP:
			evstr = "MouseButtonLeft released";
			break;
		case WM_RBUTTONDOWN:
			evstr = "MouseButtonRight pressed";
			break;
		case WM_RBUTTONUP:
			evstr = "MouseButtonRight released";
			break;
		case WM_MBUTTONDOWN:
			evstr = "MouseButtonMiddle pressed";
			break;
		case WM_MBUTTONUP:
			evstr = "MouseButtonMiddle released";
			break;
		case WM_XBUTTONDOWN:
			evstr = "MouseButtonX" + boost::lexical_cast<std::string>(ev->mouseData >> 16) + " pressed";
			break;
		case WM_XBUTTONUP:
			evstr = "MouseButtonX" + boost::lexical_cast<std::string>(ev->mouseData >> 16) + " released";
			break;
		case WM_MOUSEWHEEL:
			if (mouseUpper > 0)
				evstr = "MouseWheelUp" + boost::lexical_cast<std::string>(mouseUpper) + " pressed";
			else
				evstr = "MouseWheelDown" + boost::lexical_cast<std::string>(-mouseUpper) + " pressed";
			break;
		case WM_MOUSEHWHEEL:
			if (mouseUpper < 0)
				evstr = "MouseWheelLeft" + boost::lexical_cast<std::string>(-mouseUpper) + " pressed";
			else
				evstr = "MouseWheelRight" + boost::lexical_cast<std::string>(mouseUpper) + " pressed";
			break;
		case WM_MOUSEMOVE:
			moved = true;
			break;	
		default:
			;
		}
		if (!evstr.empty() || moved) {
			// we're using the same time stamp to transmit both the position and the button
			double now = lsl::local_clock();
			if (!evstr.empty())
				outletButtons_->push_sample(&evstr,now);
			float pos[] = {ev->pt.x,ev->pt.y};
			outletPosition_->push_sample(pos,now);
		}
	}
	return CallNextHookEx(mouseHook_, code, wParam, lParam);
}

LRESULT MainWindow::mouse_callback(int code, WPARAM wParam, LPARAM lParam) {
	return theWindow->on_event(code,wParam,lParam);
}

MainWindow::~MainWindow() {
	delete ui;
}
