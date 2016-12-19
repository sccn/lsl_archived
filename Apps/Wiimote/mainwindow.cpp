#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include "lsl_cpp.h"
#include <QtGui>

#define MAX_WIIMOTES    4
std::vector<int> wm_led = {WIIMOTE_LED_1, WIIMOTE_LED_2, WIIMOTE_LED_3, WIIMOTE_LED_4};

const char* const expansion_names[] = {"None","Nunchuk","Classic","GuitarHero3","WiiBoard","Unknown","Unknown","Unknown"};

//TODO:
// - Add "refresh" button to scan for wiimotes
// - Display a selectable list of found devices
// - Replace "Link"/"Unlink" with "Start"/"Stop"
// - Enable IR and motion plus through GUI toggles, only for Wiimotes.

short any_wiimote_connected(wiimote** wm, int wiimotes) {
    int i;
    if (!wm) {
        return 0;
    }
    
    for (i = 0; i < wiimotes; i++) {
        if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
            return 1;
        }
    }
    
    return 0;
}

void
MainWindow::handle_event(int wm_ix,
                  double timestamp)
{
    wiimote* wm = mDevices[wm_ix];
    lsl::stream_outlet* marker_outlet = m_marker_outlets.at(wm_ix);
    lsl::stream_outlet* data_outlet = m_data_outlets.at(wm_ix);
    std::vector<float> data_buffer = m_data_buffers.at(wm_ix);

    switch (wm->event)
    {
        case WIIUSE_EVENT:
        {
            // === check for button events that go into the marker streams ===
            if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_TWO))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonTwo pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_TWO))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonTwo released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_ONE))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonTwo pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_ONE))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonOne released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_B))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonB pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_B))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonB released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_A))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonA pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_A))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonA released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_MINUS))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonMinus pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_MINUS))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonMinus released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_HOME))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonHome pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_HOME))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonHome released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_LEFT))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonLeft pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_LEFT))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonLeft released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_RIGHT))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonRight pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_RIGHT))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonLight released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_UP))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonUp pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_UP))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonUp released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_DOWN))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonDown pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_DOWN))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonDown released"),timestamp);
            if (IS_JUST_PRESSED(wm,WIIMOTE_BUTTON_PLUS))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonPlus pressed"),timestamp);
            if (IS_RELEASED(wm,WIIMOTE_BUTTON_PLUS))
                marker_outlet->push_sample(std::vector<std::string>(1,"WiimoteButtonPlus released"),timestamp);
            
            if (wm->exp.type == EXP_NUNCHUK || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) {
                struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;
                if (IS_JUST_PRESSED(nc, NUNCHUK_BUTTON_C))
                    marker_outlet->push_sample(std::vector<std::string>(1,"NunchukButtonC pressed"),timestamp);
                if (IS_RELEASED(nc, NUNCHUK_BUTTON_C))
                    marker_outlet->push_sample(std::vector<std::string>(1,"NunchukButtonC released"),timestamp);
                if (IS_JUST_PRESSED(nc, NUNCHUK_BUTTON_Z))
                    marker_outlet->push_sample(std::vector<std::string>(1,"NunchukButtonZ pressed"),timestamp);
                if (IS_RELEASED(nc, NUNCHUK_BUTTON_Z))
                    marker_outlet->push_sample(std::vector<std::string>(1,"NunchukButtonZ released"),timestamp);
            }
            
            if (wm->exp.type == EXP_CLASSIC) {
                struct classic_ctrl_t* cc = (classic_ctrl_t*)&wm->exp.classic;
                if (IS_JUST_PRESSED(cc, CLASSIC_CTRL_BUTTON_UP))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonUp pressed"),timestamp);
                if (IS_RELEASED(cc, CLASSIC_CTRL_BUTTON_UP))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonUp released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_LEFT))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonLeft pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_LEFT))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonLeft released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_ZR))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonZR pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_ZR))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonZR released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_X))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonX pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_X))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonX released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_A))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonA pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_A))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonA released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_Y))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonY pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_Y))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonY released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_B))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonB pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_B))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonB released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_ZL))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonZL pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_ZL))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonZL released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_FULL_L))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonFullL pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_FULL_L))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonFullL released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_FULL_R))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonFullR pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_FULL_R))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonFullR released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_PLUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonPlus pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_PLUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonPlus released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_HOME))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonHome pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_HOME))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonHome released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_MINUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonMinus pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_MINUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonMinus released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_DOWN))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonDown pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_DOWN))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonDown released"),timestamp);
                if (IS_JUST_PRESSED(cc,CLASSIC_CTRL_BUTTON_RIGHT))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonRight pressed"),timestamp);
                if (IS_RELEASED(cc,CLASSIC_CTRL_BUTTON_RIGHT))
                    marker_outlet->push_sample(std::vector<std::string>(1,"ClassicButtonRight released"),timestamp);
            }
            
            if (wm->exp.type == EXP_GUITAR_HERO_3) {
                struct guitar_hero_3_t* gh3 = (guitar_hero_3_t*)&wm->exp.gh3;
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_STRUM_UP))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonStrumUp pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_STRUM_UP))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonStrumUp released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_YELLOW))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonYellow pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_YELLOW))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonYellow released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_GREEN))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonGreen pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_GREEN))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonGreen released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_BLUE))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonBlue pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_BLUE))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonBlue released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_RED))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonRed pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_RED))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonRed released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_ORANGE))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonOrange pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_ORANGE))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonOrange released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_PLUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonPlus pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_PLUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonPlus released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_MINUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonMinus pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_MINUS))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonMinus released"),timestamp);
                if (IS_JUST_PRESSED(gh3,GUITAR_HERO_3_BUTTON_STRUM_DOWN))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonStrumDown pressed"),timestamp);
                if (IS_RELEASED(gh3,GUITAR_HERO_3_BUTTON_STRUM_DOWN))
                    marker_outlet->push_sample(std::vector<std::string>(1,"GH3ButtonStrumDown released"),timestamp);
            }
            
            // === construct the samples that go into the data streams ===
            std::vector<float>::iterator buf_it = data_buffer.begin();
            if (wm->exp.type == EXP_WII_BOARD) {
                // balance board
                struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
                
                // TODO: Only if print results button is pressed...
                float total = wb->tl + wb->tr + wb->bl + wb->br;
                float x = ((wb->tr + wb->br) / total) * 2 - 1;
                float y = ((wb->tl + wb->tr) / total) * 2 - 1;
                printf("Weight: %f kg @ (%f, %f)\n", total, x, y);
                printf("Interpolated weight: TL:%f  TR:%f  BL:%f  BR:%f\n", wb->tl, wb->tr, wb->bl, wb->br);
                printf("Raw: TL:%d  TR:%d  BL:%d  BR:%d\n", wb->rtl, wb->rtr, wb->rbl, wb->rbr);
                
                std::vector<float> sample =
                {
                    wb->tl,
                    wb->tr,
                    wb->bl,
                    wb->br,
                    static_cast<float>(wb->rtl),
                    static_cast<float>(wb->rtr),
                    static_cast<float>(wb->rbl),
                    static_cast<float>(wb->rbr)
                };
                buf_it = std::copy(sample.begin(), sample.end(),
                                   buf_it);
            }
            else
            {
                std::vector<float> sample =
                {
                    static_cast<float>(wm->accel.x),
                    static_cast<float>(wm->accel.y),
                    static_cast<float>(wm->accel.z),
                    wm->orient.pitch,
                    wm->orient.yaw,
                    wm->orient.roll,
                    wm->orient.a_pitch,
                    wm->orient.a_roll,
                    wm->gforce.x,
                    wm->gforce.y,
                    wm->gforce.z,
                    static_cast<float>(wm->ir.x),
                    static_cast<float>(wm->ir.y),
                    wm->ir.z
                };
                buf_it = std::copy(sample.begin(), sample.end(),
                                   buf_it);
                
                switch (wm->exp.type)
                {
                    case EXP_NUNCHUK:
                    case EXP_MOTION_PLUS_NUNCHUK:
                    {
                        // Wiimote with Nunchuk
                        std::vector<float> nchk_sample =
                        {
                            static_cast<float>(wm->exp.nunchuk.accel.x),
                            static_cast<float>(wm->exp.nunchuk.accel.y),
                            static_cast<float>(wm->exp.nunchuk.accel.z),
                            wm->exp.nunchuk.orient.pitch,
                            wm->exp.nunchuk.orient.yaw,
                            wm->exp.nunchuk.orient.roll,
                            wm->exp.nunchuk.orient.a_pitch,
                            wm->exp.nunchuk.orient.a_roll,
                            wm->exp.nunchuk.gforce.x,
                            wm->exp.nunchuk.gforce.y,
                            wm->exp.nunchuk.gforce.z,
                            wm->exp.nunchuk.js.ang,
                            wm->exp.nunchuk.js.mag
                        };
                        buf_it = std::copy(nchk_sample.begin(), nchk_sample.end(),
                                           buf_it);
                        break;
                    }
                        
                    case EXP_CLASSIC:
                    {
                        std::vector<float> clsc_sample =
                        {
                            wm->exp.classic.ljs.ang,
                            wm->exp.classic.ljs.mag,
                            wm->exp.classic.rjs.ang,
                            wm->exp.classic.rjs.mag,
                            wm->exp.classic.l_shoulder,
                            wm->exp.classic.r_shoulder
                        };
                        buf_it = std::copy(clsc_sample.begin(), clsc_sample.end(),
                                           buf_it);
                        break;
                    }
                        
                        
                    case EXP_GUITAR_HERO_3:
                    {
                        std::vector<float> gh3_sample =
                        {
                            wm->exp.gh3.js.ang,
                            wm->exp.gh3.js.mag,
                            wm->exp.gh3.whammy_bar
                        };
                        buf_it = std::copy(gh3_sample.begin(), gh3_sample.end(),
                                           buf_it);
                        break;
                    }
                }
                
                if (wm->exp.type == EXP_MOTION_PLUS ||
                    wm->exp.type == EXP_MOTION_PLUS_NUNCHUK)
                {
                    std::vector<float> mp_sample =
                    {
                        wm->exp.mp.angle_rate_gyro.pitch,
                        wm->exp.mp.angle_rate_gyro.yaw,
                        wm->exp.mp.angle_rate_gyro.roll
                    };
                    buf_it = std::copy(mp_sample.begin(), mp_sample.end(),
                                       buf_it);
                }
            }
            data_outlet->push_sample(data_buffer, timestamp);
            
            break;
        }

        case WIIUSE_STATUS:
            break;
            
        case WIIUSE_DISCONNECT:
        case WIIUSE_UNEXPECTED_DISCONNECT:
            std::cout << "Warning: One of the Wiimotes was disconnected." << std::endl;
            break;
        case WIIUSE_NUNCHUK_INSERTED:
        case WIIUSE_CLASSIC_CTRL_INSERTED:
        case WIIUSE_WII_BOARD_CTRL_INSERTED:
        case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
            throw std::runtime_error("An expansion was inserted at runtime.");
        case WIIUSE_NUNCHUK_REMOVED:
        case WIIUSE_CLASSIC_CTRL_REMOVED:
        case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
        case WIIUSE_WII_BOARD_CTRL_REMOVED:
            throw std::runtime_error("An expansion was removed at runtime.");
        default:
            break;
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pTimer = new QTimer(this);
    
    // make GUI connections
    QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
    QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(stream_func()));
}


MainWindow::~MainWindow() {
    delete ui;
}


void
MainWindow::stream_func(){

    if (any_wiimote_connected(mDevices, MAX_WIIMOTES)
        && wiiuse_poll(mDevices, MAX_WIIMOTES))
    {
        
        double now = lsl::local_clock();
        int wm_ix = 0;
        for (wm_ix=0; wm_ix<nDevices; ++wm_ix) {
            handle_event(wm_ix, now);
        }
    }
}

void MainWindow::link() {
    
    //Stop and join all threads
    if (m_pTimer->isActive())
    {
        m_pTimer->stop();
        
        int wm_ix = 0;
        for (wm_ix = 0; wm_ix < nDevices; ++wm_ix) {//MAX_WIIMOTES
            wiiuse_disconnect(mDevices[wm_ix]);
        }
        
		// Cleanup hardware
		// wiiuse_cleanup(mDevices, nDevices);  //causes crash in OSX
        
        // Cleanup marker_outlets and data_outlets
		// Causes crash on exit.
		// m_marker_outlets.clear();
		// m_data_outlets.clear();
        
        // Cleanup data_buffers ?
        
        // indicate that we are now successfully unlinked
        ui->linkButton->setText("Link");
    }
    else
    {
        
        // scan for hardware
        int found;
        mDevices =  wiiuse_init(MAX_WIIMOTES);
//        wiiuse_set_bluetooth_stack(wiimotes, max_wiimotes,WIIUSE_STACK_MS);
        found = wiiuse_find(mDevices, MAX_WIIMOTES, 5);
        if (found)
        {
            nDevices = wiiuse_connect(mDevices, MAX_WIIMOTES);
            if (nDevices)
            {
                // Allocate data buffers.
                m_data_buffers = std::vector<std::vector<float>>(nDevices, std::vector<float>(0));
                
                // Step through each entry
                int wm_ix = 0;
                for(wm_ix=0; wm_ix<nDevices; ++wm_ix)
                {
                    wiimote* wm = mDevices[wm_ix];
                    
                    //Indicate that we are setting up a new stream with LED and rumble.
                    wiiuse_set_leds(wm, wm_led.at(wm_ix));
                    wiiuse_rumble(wm, 1);
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    wiiuse_rumble(wm, 0);
                    
                    // Create marker stream info + outlet
                    lsl::stream_info marker_info("WiimoteMarkers" + std::to_string(wm->unid),
                                                 "Markers",
                                                 1,
                                                 lsl::IRREGULAR_RATE,
                                                 lsl::cf_string,
                                                 "Wiimote_markers_" + std::to_string(wm->unid));
                    append_setup_info(wm_ix, marker_info);
                    m_marker_outlets.push_back(new lsl::stream_outlet(marker_info));
                    
                    // Create data stream info + outlet
                    int channelCount = get_channel_count(wm_ix);
                    lsl::stream_info data_info("wiiuse" + std::to_string(wm->unid),
                                               "MoCap",
                                               channelCount,
                                               lsl::IRREGULAR_RATE,
                                               lsl::cf_float32,
                                               "Wiimote_sensor_data" + std::to_string(wm->unid));
                    append_setup_info(wm_ix, data_info);
                    append_channel_info(wm_ix, data_info);
                    // misc acquisition information
                    lsl::xml_element acq_info = data_info.desc().append_child("acquisition");
                    acq_info.set_name("acquisition");
                    acq_info.append_child_value("manufacturer","Nintendo")
                    .append_child_value("model","Wiimote")
                    .append_child_value("api","wiiuse")
                    .append_child_value("api_version", wiiuse_version());
                    std::cout << data_info.as_xml() << std::endl;
                    m_data_outlets.push_back(new lsl::stream_outlet(data_info));
                    
                    wiiuse_motion_sensing(wm, 0);  // disable motion sensing
                    wiiuse_set_ir(wm, 0);  // disable infrared tracking
                    m_data_buffers.at(wm_ix).resize(channelCount);
                }

                m_pTimer->start(0);
                
                ui->linkButton->setText("Unlink");
            }
        }
    }
}


void MainWindow::closeEvent(QCloseEvent *ev) {
    if (m_pTimer->isActive())
    {
        ev->setAccepted(false);
    }
}


void
MainWindow::append_setup_info(int wm_ix, lsl::stream_info& out_info)
{
    wiimote* wm = mDevices[wm_ix];
    
    lsl::xml_element setup = out_info.desc().append_child("setup");
    
    // Common setup to every device
    lsl::xml_element handset = setup.append_child("handset")
    .append_child_value("assigned_id",std::to_string(wm->unid))
    .append_child_value("state",std::to_string(wm->state))
    .append_child_value("battery_level",std::to_string(wm->battery_level))
    .append_child_value("expansion", expansion_names[wm->exp.type]);
    
    lsl::xml_element ir_bar = setup.append_child("screen_tracking")
    .append_child_value("bar_placement",wm->ir.pos==WIIUSE_IR_ABOVE ? "AboveScreen" : "BelowScreen")
    .append_child_value("aspect",wm->ir.aspect==WIIUSE_ASPECT_4_3 ? "4:3" : "16:9")
    .append_child_value("state",std::to_string(wm->ir.state));
    ir_bar.append_child("virtual_resolution")
    .append_child_value("X",std::to_string(wm->ir.vres[0]))
    .append_child_value("Y",std::to_string(wm->ir.vres[1]));
    ir_bar.append_child("correction_offset")
    .append_child_value("X",std::to_string(wm->ir.offset[0]))
    .append_child_value("Y",std::to_string(wm->ir.offset[1]));

    lsl::xml_element accel_calib = handset.append_child("accel_calib");
    accel_calib.append_child("cal_zero")
    .append_child_value("X",std::to_string(wm->accel_calib.cal_zero.x))
    .append_child_value("Y",std::to_string(wm->accel_calib.cal_zero.y))
    .append_child_value("Z",std::to_string(wm->accel_calib.cal_zero.z));
    accel_calib.append_child("cal_g")
    .append_child_value("X",std::to_string(wm->accel_calib.cal_g.x))
    .append_child_value("Y",std::to_string(wm->accel_calib.cal_g.y))
    .append_child_value("Z",std::to_string(wm->accel_calib.cal_g.z));
    accel_calib.append_child_value("smoothing_alpha",std::to_string(wm->accel_calib.st_alpha));
    
    //Additional extension-specific config info
    switch (wm->exp.type) {
        case EXP_NUNCHUK:
        {
            lsl::xml_element nunchuk = setup.append_child("nunchuk");
            lsl::xml_element nchk_accel_calib = nunchuk.append_child("accel_calib");
			nchk_accel_calib.append_child("cal_zero")
            .append_child_value("X",std::to_string(wm->exp.nunchuk.accel_calib.cal_zero.x))
            .append_child_value("Y",std::to_string(wm->exp.nunchuk.accel_calib.cal_zero.y))
            .append_child_value("Z",std::to_string(wm->exp.nunchuk.accel_calib.cal_zero.z));
			nchk_accel_calib.append_child("cal_g")
            .append_child_value("X",std::to_string(wm->exp.nunchuk.accel_calib.cal_g.x))
            .append_child_value("Y",std::to_string(wm->exp.nunchuk.accel_calib.cal_g.y))
            .append_child_value("Z",std::to_string(wm->exp.nunchuk.accel_calib.cal_g.z));
			nchk_accel_calib.append_child_value("smoothing_alpha",std::to_string(wm->exp.nunchuk.accel_calib.st_alpha));
            lsl::xml_element joystick_calib = nunchuk.append_child("joystick_calib");
            joystick_calib.append_child("min")
            .append_child_value("X",std::to_string(wm->exp.nunchuk.js.min.x))
            .append_child_value("Y",std::to_string(wm->exp.nunchuk.js.min.y));
            joystick_calib.append_child("max")
            .append_child_value("X",std::to_string(wm->exp.nunchuk.js.max.x))
            .append_child_value("Y",std::to_string(wm->exp.nunchuk.js.max.y));
            joystick_calib.append_child("center")
            .append_child_value("X",std::to_string(wm->exp.nunchuk.js.center.x))
            .append_child_value("Y",std::to_string(wm->exp.nunchuk.js.center.y));
            nunchuk.append_child_value("orient_threshold",std::to_string(wm->exp.nunchuk.orient_threshold));
            nunchuk.append_child_value("accel_threshold",std::to_string(wm->exp.nunchuk.accel_threshold));
            break;
        }
            
        case EXP_CLASSIC:
        {
            lsl::xml_element classic = setup.append_child("classic_controller");
            lsl::xml_element joyleft = classic.append_child("joystick_calib_left");
            joyleft.append_child("min")
            .append_child_value("X",std::to_string(wm->exp.classic.ljs.min.x))
            .append_child_value("Y",std::to_string(wm->exp.classic.ljs.min.y));
            joyleft.append_child("max")
            .append_child_value("X",std::to_string(wm->exp.classic.ljs.max.x))
            .append_child_value("Y",std::to_string(wm->exp.classic.ljs.max.y));
            joyleft.append_child("center")
            .append_child_value("X",std::to_string(wm->exp.classic.ljs.center.x))
            .append_child_value("Y",std::to_string(wm->exp.classic.ljs.center.y));
            lsl::xml_element joyright = classic.append_child("joystick_calib_right");
            joyright.append_child("min")
            .append_child_value("X",std::to_string(wm->exp.classic.rjs.min.x))
            .append_child_value("Y",std::to_string(wm->exp.classic.rjs.min.y));
            joyright.append_child("max")
            .append_child_value("X",std::to_string(wm->exp.classic.rjs.max.x))
            .append_child_value("Y",std::to_string(wm->exp.classic.rjs.max.y));
            joyright.append_child("center")
            .append_child_value("X",std::to_string(wm->exp.classic.rjs.center.x))
            .append_child_value("Y",std::to_string(wm->exp.classic.rjs.center.y));
            break;
        }
            
        case EXP_GUITAR_HERO_3:
        {
            lsl::xml_element joystick_calib = setup.append_child("gh3_controller").append_child("joystick_calib");
            joystick_calib.append_child("min")
            .append_child_value("X",std::to_string(wm->exp.gh3.js.min.x))
            .append_child_value("Y",std::to_string(wm->exp.gh3.js.min.y));
            joystick_calib.append_child("max")
            .append_child_value("X",std::to_string(wm->exp.gh3.js.max.x))
            .append_child_value("Y",std::to_string(wm->exp.gh3.js.max.y));
            joystick_calib.append_child("center")
            .append_child_value("X",std::to_string(wm->exp.gh3.js.center.x))
            .append_child_value("Y",std::to_string(wm->exp.gh3.js.center.y));
            break;
        }
            
        case EXP_WII_BOARD:
        {
            lsl::xml_element board_calib = setup.append_child("wiiboard").append_child("calib");
            board_calib.append_child("topleft")
            .append_child_value("A",std::to_string(wm->exp.wb.ctl[0]))
            .append_child_value("B",std::to_string(wm->exp.wb.ctl[1]))
            .append_child_value("C",std::to_string(wm->exp.wb.ctl[2]));
            board_calib.append_child("topright")
            .append_child_value("A",std::to_string(wm->exp.wb.ctr[0]))
            .append_child_value("B",std::to_string(wm->exp.wb.ctr[1]))
            .append_child_value("C",std::to_string(wm->exp.wb.ctr[2]));
            board_calib.append_child("bottomleft")
            .append_child_value("A",std::to_string(wm->exp.wb.cbl[0]))
            .append_child_value("B",std::to_string(wm->exp.wb.cbl[1]))
            .append_child_value("C",std::to_string(wm->exp.wb.cbl[2]));
            board_calib.append_child("bottomright")
            .append_child_value("A",std::to_string(wm->exp.wb.cbr[0]))
            .append_child_value("B",std::to_string(wm->exp.wb.cbr[1]))
            .append_child_value("C",std::to_string(wm->exp.wb.cbr[2]));
            break;
        }
            
        default:
            break;
    }
}

int
MainWindow::get_channel_count(int wm_ix)
{
    wiimote* wm = mDevices[wm_ix];
    int nChannels = 0;
    if (wm->exp.type == EXP_WII_BOARD)
    {
        nChannels += 8;
    }
    else
    {
        // Every other device has Wiimote channels
        nChannels += 3;
        
        nChannels += 5;
        
        nChannels += 3;
        
        nChannels += 3;
        
        // Some devices have additional channels
        switch (wm->exp.type) {
            case EXP_NUNCHUK:
            case EXP_MOTION_PLUS_NUNCHUK:
                nChannels += 3;
                
                nChannels += 5;

                nChannels += 3;

                nChannels += 3;
                break;
                
            case EXP_CLASSIC:
                nChannels += 4;

                nChannels += 2;
                break;
                
            case EXP_GUITAR_HERO_3:
                nChannels += 3;
                break;
                
            default:
                break;
        }
        if (wm->exp.type == EXP_MOTION_PLUS ||
            wm->exp.type == EXP_MOTION_PLUS_NUNCHUK)
        {
            nChannels += 3;
        }
    }
    return nChannels;
}

void
MainWindow::append_channel_info(int wm_ix, lsl::stream_info& out_info)
{
    wiimote* wm = mDevices[wm_ix];
    
    lsl::xml_element channels = out_info.desc().append_child("channels");

    if (wm->exp.type == EXP_WII_BOARD)
    {
        // balance board channels
        channels.append_child("channel")
        .append_child_value("label","Weight_TopLeft")
        .append_child_value("type","Force")
        .append_child_value("unit","kilograms");
        channels.append_child("channel")
        .append_child_value("label","Weight_TopRight")
        .append_child_value("type","Force")
        .append_child_value("unit","kilograms");
        channels.append_child("channel")
        .append_child_value("label","Weight_BottomLeft")
        .append_child_value("type","Force")
        .append_child_value("unit","kilograms");
        channels.append_child("channel")
        .append_child_value("label","Weight_BottomRight")
        .append_child_value("type","Force")
        .append_child_value("unit","kilograms");
        channels.append_child("channel")
        .append_child_value("label","Weight_TopLeftRaw")
        .append_child_value("type","Force")
        .append_child_value("unit","au");
        channels.append_child("channel")
        .append_child_value("label","Weight_TopRightRaw")
        .append_child_value("type","Force")
        .append_child_value("unit","au");
        channels.append_child("channel")
        .append_child_value("label","Weight_BottomLeftRaw")
        .append_child_value("type","Force")
        .append_child_value("unit","au");
        channels.append_child("channel")
        .append_child_value("label","Weight_BottomRightRaw")
        .append_child_value("type","Force")
        .append_child_value("unit","au");
    }
    else
    {
        // Every other device has Wiimote channels
        channels.append_child("channel")
        .append_child_value("label","Wiimote_RawAccel_X")
        .append_child_value("type","Acceleration")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","raw");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_RawAccel_Y")
        .append_child_value("type","Acceleration")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","raw");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_RawAccel_Z")
        .append_child_value("type","Acceleration")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","raw");
        
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Pitch")
        .append_child_value("type","Orientation")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","degrees");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Yaw")
        .append_child_value("type","Orientation")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","degrees");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Roll")
        .append_child_value("type","Orientation")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","degrees");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_AbsolutePitch")
        .append_child_value("type","Orientation")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","degrees");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_AbsoluteRoll")
        .append_child_value("type","Orientation")
        .append_child_value("device","Wiimote")
        .append_child_value("unit","degrees");
        
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Gravity_X")
        .append_child_value("type","Acceleration")
        .append_child_value("device","Wiimote");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Gravity_Y")
        .append_child_value("type","Acceleration")
        .append_child_value("device","Wiimote");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Gravity_Z")
        .append_child_value("type","Acceleration")
        .append_child_value("device","Wiimote");
        
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Pointing_X")
        .append_child_value("type","Direction")
        .append_child_value("device","Wiimote");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Pointing_Y")
        .append_child_value("type","Direction")
        .append_child_value("device","Wiimote");
        channels.append_child("channel")
        .append_child_value("label","Wiimote_Pointing_Z")
        .append_child_value("type","Direction")
        .append_child_value("device","Wiimote");
        
        // Some devices have additional channels
        switch (wm->exp.type) {
            case EXP_NUNCHUK:
            case EXP_MOTION_PLUS_NUNCHUK:
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_RawAccel_X")
                .append_child_value("type","Acceleration")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","raw");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_RawAccel_Y")
                .append_child_value("type","Acceleration")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","raw");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_RawAccel_Z")
                .append_child_value("type","Acceleration")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","raw");
                
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Pitch")
                .append_child_value("type","Orientation")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Yaw")
                .append_child_value("type","Orientation")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Roll")
                .append_child_value("type","Orientation")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_AbsolutePitch")
                .append_child_value("type","Orientation")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_AbsoluteRoll")
                .append_child_value("type","Orientation")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","degrees");
                
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Gravity_X")
                .append_child_value("type","Acceleration")
                .append_child_value("device","Nunchuk");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Gravity_Y")
                .append_child_value("type","Acceleration")
                .append_child_value("device","Nunchuk");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Gravity_Z")
                .append_child_value("type","Acceleration")
                .append_child_value("device","Nunchuk");
                
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Joystick_Ang")
                .append_child_value("type","Direction")
                .append_child_value("device","Nunchuk")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","Nunchuk_Joystick_Mag")
                .append_child_value("type","Magnitude")
                .append_child_value("device","Nunchuk");
                break;
                
            case EXP_CLASSIC:
                channels.append_child("channel")
                .append_child_value("label","ClassicController_JoystickLeft_Ang")
                .append_child_value("type","Direction")
                .append_child_value("device","ClassicController")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","ClassicController_JoystickLeft_Mag")
                .append_child_value("type","Magnitude")
                .append_child_value("device","ClassicController");
                channels.append_child("channel")
                .append_child_value("label","ClassicController_JoystickRight_Ang")
                .append_child_value("type","Direction")
                .append_child_value("device","ClassicController")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","ClassicController_JoystickRight_Mag")
                .append_child_value("type","Magnitude")
                .append_child_value("device","ClassicController");
                
                channels.append_child("channel")
                .append_child_value("label","ClassicController_LeftShoulderButton")
                .append_child_value("type","Magnitude")
                .append_child_value("device","ClassicController");
                channels.append_child("channel")
                .append_child_value("label","ClassicController_RightShoulderButton")
                .append_child_value("type","Magnitude")
                .append_child_value("device","ClassicController");
                break;
            
            case EXP_GUITAR_HERO_3:
                channels.append_child("channel")
                .append_child_value("label","GH3_Joystick_Ang")
                .append_child_value("type","Direction")
                .append_child_value("device","GH3")
                .append_child_value("unit","degrees");
                channels.append_child("channel")
                .append_child_value("label","GH3_Joystick_Mag")
                .append_child_value("type","Magnitude")
                .append_child_value("device","GH3");
                channels.append_child("channel")
                .append_child_value("label","GH3_WhammyBar")
                .append_child_value("type","Magnitude")
                .append_child_value("device","GH3");
                break;
            
            default:
                break;
        }
        if (wm->exp.type == EXP_MOTION_PLUS ||
            wm->exp.type == EXP_MOTION_PLUS_NUNCHUK)
        {
            channels.append_child("channel")
            .append_child_value("label","pitch_rate")
            .append_child_value("type", "Angular_Rate")
            .append_child_value("device","Motion+")
            .append_child_value("unit","degrees/sec");
            channels.append_child("channel")
            .append_child_value("label","yaw_rate")
            .append_child_value("type", "Angular_Rate")
            .append_child_value("device","Motion+")
            .append_child_value("unit","degrees/sec");
            channels.append_child("channel")
            .append_child_value("label","roll_rate")
            .append_child_value("type", "Angular_Rate")
            .append_child_value("device","Motion+")
            .append_child_value("unit","degrees/sec");
        }
    }
}
