/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#endif

#include "gazeapi.h"
#include "gazeapi_interfaces.h"
#include "gazeapi_types.h"

#include "gazeapi_observable.hpp"
#include "gazeapi_parser.hpp"
#include "gazeapi_socket.hpp"

#include <boost/thread.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <iostream>
#include <vector>

namespace gtl
{
    class CalibrationProxy
    {
    public:
        CalibrationProxy()
            : m_point_count(0)
            , m_processed_points(0)
            , m_is_calibrating(false)
        {}

        void start_calibration(size_t const point_count)
        {
            m_point_count = point_count;
            m_is_calibrating = true;
        }

        void point_start()
        {}

        void point_end()
        {
            m_processed_points++;
        }

        bool is_done() const
        {
            return m_processed_points == m_point_count;
        }

        bool is_calibrating() const
        {
            return m_is_calibrating;
        }

        double get_progress() const
        {
            return 0 == m_point_count ? 0.0 : (double)m_processed_points / (double)m_point_count;
        }

        void clear()
        {
            m_point_count = 0 ;
            m_processed_points = 0;
            m_is_calibrating = false;
        }

    private:
        size_t m_point_count;
        size_t m_processed_points;
        bool m_is_calibrating;
    };

    class IHeartbeatListener
    {
    public:
        virtual ~IHeartbeatListener() {}
        virtual void on_heartbeat() = 0;
    };

    // Responsible for sending heart beats once client has connected
    class Heartbeater : public Observable<IHeartbeatListener>
    {
    public:
        Heartbeater()
            : m_do_loop(false)
            , m_heartbeat_interval(1000)
        {}

        ~Heartbeater()
        {
            m_thread.join();
        }

        void set_interval(const int heartbeat_interval)
        {
            m_heartbeat_interval = heartbeat_interval;
        }

        void start()
        {
            m_thread = boost::thread(boost::bind(&Heartbeater::run, this));
        }

        void stop()
        {
            m_do_loop = false;
            m_thread.join();
        }

    private:
        void run()
        {
            m_do_loop = true;

            while (m_do_loop)
            {
                Observable<IHeartbeatListener>::ObserverVector const & observers = get_observers();

                for (size_t i = 0; i < observers.size(); ++i)
                {
                    observers[i]->on_heartbeat();
                }

                boost::this_thread::sleep(boost::posix_time::milliseconds(m_heartbeat_interval));
            }
        }

    private:
        bool                m_do_loop;
        boost::thread       m_thread;
        size_t              m_heartbeat_interval;
    };

    class GazeApi::Engine
        : public ISocketListener
        , public IHeartbeatListener
        , Observable<IGazeListener>
        , Observable<ICalibrationResultListener>
        , Observable<ITrackerStateListener>
        , Observable<ICalibrationProcessHandler>
    {
    public:
        using Observable<IGazeListener>::add_observer;
        using Observable<IGazeListener>::remove_observer;
        using Observable<ICalibrationResultListener>::add_observer;
        using Observable<ICalibrationResultListener>::remove_observer;
        using Observable<ITrackerStateListener>::add_observer;
        using Observable<ITrackerStateListener>::remove_observer;
        using Observable<ICalibrationProcessHandler>::add_observer;
        using Observable<ICalibrationProcessHandler>::remove_observer;

        Engine(bool verbose)
            : m_socket(verbose)
            , m_state(AS_STOPPED)
        {
            m_heartbeat.add_observer(*this);
            m_socket.add_observer(*this);
            m_sync_requests = 0;
        }

        virtual ~Engine()
        {
            m_heartbeat.remove_observer(*this);
            m_socket.remove_observer(*this);
            disconnect();
        }

        bool is_running() const
        {
            return m_state == AS_RUNNING;
        }

        // GazeApi support
        bool connect(bool push_mode, std::string const & port)
        {
            if (AS_STOPPED != m_state)
            {
                return false;
            }

            m_port = port;

            bool const success = m_socket.connect("127.0.0.1", m_port);

            if (success)
            {
                m_state = AS_RUNNING;

                memset(&m_server_proxy, 0, sizeof(ServerState));

                m_heartbeat.start();

                // Set version 1
                set_version(1);

                // Set push or pull mode
                set_push(push_mode);

                // retrieve current state
                get_tracker_state();
            }

            return success;
        }

        void disconnect()
        {
            if (m_state != AS_STOPPED)
            {
                m_heartbeat.stop();
                m_socket.disconnect();
                m_state = AS_STOPPED;
            }
        }

        void set_version(size_t const version)
        {
            std::stringstream ss;
            ss << "{\"category\":\"tracker\",\"request\":\"set\",\"values\":" << "{\"version\":" << version << "}}";
            send(ss.str());
        }

        void set_push(bool const enable)
        {
            std::stringstream ss;
            ss << "{\"category\":\"tracker\",\"request\":\"set\",\"values\":" << "{\"push\":" << (enable ? "true" : "false") <<  "}}";
            send(ss.str());
        }

        void set_screen(Screen const & screen)
        {
            std::stringstream ss;
            ss << "{\"category\":\"tracker\",\"request\":\"set\",\"values\":" << "{\"screenindex\":" << screen.screenindex << ",\"screenresw\":" << screen.screenresw << ",\"screenresh\":" << screen.screenresh << ",\"screenpsyw\":" << screen.screenpsyw << ",\"screenpsyh\":" << screen.screenpsyh << "}}";
            send(ss.str());
        }

        void get_screen(Screen & screen) const
        {
            m_screen_lock.lock();
            screen = m_screen;
            m_screen_lock.unlock();
        }

        void get_tracker_state()
        {
            // request everything
            std::stringstream ss;
            ss << "{\"category\":\"tracker\",\"request\":\"get\",\"values\":["
                << "\"push\","
                << "\"heartbeatinterval\","
                << "\"version\","
                << "\"trackerstate\","
                << "\"framerate\","
                << "\"iscalibrated\","
                << "\"iscalibrating\","
                << "\"calibresult\","
                << "\"frame\","
                << "\"screenindex\","
                << "\"screenresw\","
                << "\"screenresh\","
                << "\"screenpsyw\","
                << "\"screenpsyh\""
                << "]}";

            clear_sync_req(SR_TSTATE);
            send(ss.str());
            wait_sync_req(SR_TSTATE);
        }

        void get_frame(GazeData & gaze_data)
        {
            // If we are in push-mode, we already have the latest frame.
            // In pull mode we have to request the data

            if (!m_server_proxy.push)
            {
                clear_sync_req(SR_GAZEDATA);
                send("{\"category\":\"tracker\",\"request\":\"get\",\"values\":[\"frame\"]}");
                wait_sync_req(SR_GAZEDATA);
            }

            m_gaze_lock.lock();
            gaze_data = m_gaze_data;
            m_gaze_lock.unlock();
        }

        void get_calib_result(CalibResult & calib_result) const
        {
            m_calib_lock.lock();
            calib_result = m_calib_result;
            m_calib_lock.unlock();
        }

        ServerState const & get_server_state() const
        {
            return m_server_proxy;
        }

        bool calibration_start(int const point_count)
        {
            m_calibration_proxy.start_calibration(point_count);
            std::stringstream ss;
            ss << "{\"category\":\"calibration\",\"request\":\"start\",\"values\":{\"pointcount\":" << point_count << "}}";

            clear_sync_req(SR_CSTART);
            send(ss.str());
            return wait_sync_req(SR_CSTART);
        }

        void calibration_clear()
        {
            send("{\"category\":\"calibration\",\"request\":\"clear\"}");
        }

        void calibration_abort()
        {
            send("{\"category\":\"calibration\",\"request\":\"abort\"}");
        }

        void calibration_point_start(int const x, int const y)
        {
            std::stringstream ss;
            ss << "{\"category\":\"calibration\",\"request\":\"pointstart\",\"values\":{\"x\":" << x << ",\"y\":" << y << "}}";
            send(ss.str());
        }

        void calibration_point_end()
        {
            send("{\"category\":\"calibration\",\"request\":\"pointend\"}");
        }

        void on_message(std::string const & message)
        {
            try
            {
                parse(message);
            }
            catch (std::exception const & e)
            {
                e.what();
            }
        }

        void on_disconnected()
        {
            disconnect();
            // todo: try to reconnect here
        }

        void on_heartbeat()
        {
            send("{\"category\":\"heartbeat\"}");
        }

    private:

        struct Message
        {
            Message()
                : m_category(GAC_UNKNOWN), m_request(GAR_UNKNOWN), m_statuscode(GASC_ERROR)
            {}

            bool is (GazeApiCategory const & category) const
            {
                return m_category == category;
            }

            bool is (GazeApiRequest const & request) const
            {
                return m_request == request;
            }

            bool is (GazeApiStatusCode const & statuscode) const
            {
                return m_statuscode == statuscode;
            }

            bool is_notification() const
            {
                return is(GASC_CALIBRATION_CHANGE) || is(GASC_DISPLAY_CHANGE) || is(GASC_TRACKER_STATE_CHANGE);
            }

            GazeApiCategory m_category;
            GazeApiRequest m_request;
            GazeApiStatusCode m_statuscode;
            std::string m_description;
        };

    private:

        void send(std::string const & message)
        {
            if (m_state != AS_STOPPED)
            {
                m_socket.send(message);
            }
        }

        void parse (std::string const & json_message)
        {
            std::stringstream ss(json_message);
            boost::property_tree::ptree root;
            boost::property_tree::read_json(ss, root);

            Message message;

            if (!Parser::parse_category(message.m_category, root))
            {
                return; // Broken message, so just ignore
            }

            if (!Parser::parse_status_code(message.m_statuscode, root))
            {
                return; // Broken status code, so just ignore
            }

            // If message is broken or it's not a notification
            if (message.is(GASC_ERROR) && !message.is_notification())
            {
                // TODO: Maybe throw exception with description
                // Parser::parse_description(message.m_description, root);
                set_sync_req(SR_ERROR);
                return;
            }

            // If message is notification we do not care about the request-part
            if (message.is_notification())
            {
                switch(message.m_statuscode)
                {
                case GASC_CALIBRATION_CHANGE:
                    {
                        // initiate request for calibration data
                        send("{\"category\":\"tracker\",\"request\":\"get\",\"values\":[\"calibresult\",\"iscalibrated\",\"iscalibrating\"]}");
                        break;
                    }
                case GASC_DISPLAY_CHANGE:
                    {
                        // initiate request for display index
                        send("{\"category\":\"tracker\",\"request\":\"get\",\"values\":[\"screenindex\",\"screenresw\",\"screenresh\",\"screenpsyw\",\"screenpsyh\"]}");
                        break;
                    }
                case GASC_TRACKER_STATE_CHANGE:
                    {
                        // initiate request for tracker state
                        send("{\"category\":\"tracker\",\"request\":\"get\",\"values\":[\"trackerstate\"]}");
                        break;
                    }
                };
                return;
            }

            // If message is a heartbeat just ignore
            if (message.is(GAC_HEARTBEAT))
            {
                return; // Heartbeat reply, so just ignore
            }

            // If message is NOT a notification we need to parse the request as well
            if(!message.is_notification() && !Parser::parse_request(message.m_request, root))
            {
                return; // Broken request, so just ignore
            }

            if (message.is(GAC_TRACKER))
            {
                if (message.is(GAR_SET))
                {
                    return; // Everything went fine so we just return;
                }
                else if (message.is(GAR_GET))
                {
                    // Here we requested data, so we must process whatever is returned in "values"
                    bool has_gaze_data = false;
                    GazeData gaze_data;

                    bool has_calib_result = false;
                    CalibResult calib_result;

                    ServerState server_state = m_server_proxy;
                    Screen screen = m_screen;

                    if (!Parser::parse_server_state(server_state, gaze_data, calib_result, screen, root, has_gaze_data, has_calib_result))
                    {
                        return; // Parsing failed, so just return
                    }

                    if (has_gaze_data)
                    {
                        m_gaze_lock.lock();
                        m_gaze_data = gaze_data;
                        m_gaze_lock.unlock();
                        set_sync_req(SR_GAZEDATA);

                        // There was gaze data present, so
                        typedef Observable<IGazeListener> ObservableType;
                        ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                        for (size_t i = 0; i < observers.size(); ++i)
                        {
                            observers[i]->on_gaze_data(m_gaze_data);
                        }
                    }

                    if (has_calib_result)
                    {
                        m_calib_lock.lock();
                        m_calib_result = calib_result;
                        m_calib_lock.unlock();
                        set_sync_req(SR_CRESULT);

                        typedef Observable<ICalibrationResultListener> ObservableType;
                        ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                        for (size_t i = 0; i < observers.size(); ++i)
                        {
                            observers[i]-> on_calibration_changed(m_calib_result.result, m_calib_result);
                        }
                    }

                    if (screen != m_screen)
                    {
                        m_screen_lock.lock();
                        m_screen = screen;
                        m_screen_lock.unlock();
                        set_sync_req(SR_SCREEN);

                        typedef Observable<ITrackerStateListener> ObservableType;
                        ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                        for (size_t i = 0; i < observers.size(); ++i)
                        {
                            observers[i]->on_screen_state_changed(m_screen);
                        }
                    }

                    if (server_state.trackerstate != m_server_proxy.trackerstate)
                    {
                        m_server_proxy.trackerstate = server_state.trackerstate;

                        typedef Observable<ITrackerStateListener> ObservableType;
                        ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                        for (size_t i = 0; i < observers.size(); ++i)
                        {
                            observers[i]->on_tracker_connection_changed(m_server_proxy.trackerstate);
                        }
                    }

                    if (server_state.heartbeatinterval != 0)
                    {
                        m_heartbeat.set_interval(server_state.heartbeatinterval);
                    }

                    // Update everything
                    m_server_proxy = server_state;
                    set_sync_req(SR_TSTATE);
                }
                return;
            }

            if (message.is(GAC_CALIBRATION))
            {
                if (message.is(GAR_START))
                {
                    set_sync_req(SR_CSTART);

                    typedef Observable<ICalibrationProcessHandler> ObservableType;
                    ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                    for (size_t i = 0; i < observers.size(); ++i)
                    {
                        observers[i]->on_calibration_started();
                    }
                }

                if (message.is(GAR_POINTEND))
                {
                    m_calibration_proxy.point_end();

                    double progress = m_calibration_proxy.get_progress();

                    typedef Observable<ICalibrationProcessHandler> ObservableType;
                    ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                    for (size_t i = 0; i < observers.size(); ++i)
                    {
                        observers[i]->on_calibration_progress(progress);
                    }

                    CalibResult calib_result;
                    bool has_calib_result;
                    if (!Parser::parse_calib_result(calib_result, root, has_calib_result))
                    {
                        return;
                    }

                    if (has_calib_result)
                    {
                        if (calib_result.result)
                        {
                            m_calib_lock.lock();
                            m_calib_result = calib_result;
                            m_calib_lock.unlock();
                            m_calibration_proxy.clear();
                            set_sync_req(SR_CRESULT);
                        }

                        typedef Observable<ICalibrationProcessHandler> ObservableType;
                        ObservableType::ObserverVector const & observers = ObservableType::get_observers();

                        for (size_t i = 0; i < observers.size(); ++i)
                        {
                            observers[i]->on_calibration_result(calib_result.result, calib_result);
                        }
                    }
                }

                if (message.is(GAR_ABORT))
                {
                    m_calibration_proxy.clear();
                }

                if (message.is(GAR_CLEAR))
                {
                    m_calib_lock.lock();
                    m_calib_result.clear();
                    m_calib_lock.unlock();
                }
            }
        }

        typedef unsigned char  SyncRequest;

        inline void clear_sync_req(SyncRequest sr)
        {
            m_sync_lock.lock();
            m_sync_requests &= ~sr;
            m_sync_lock.unlock();
        }

        inline void set_sync_req(SyncRequest sr)
        {
            m_sync_lock.lock();
            m_sync_requests &= ~SR_ERROR;
            m_sync_requests |= sr;
            m_sync_lock.unlock();
        }

        inline bool has_sync_req(SyncRequest sr)
        {
            return (m_sync_requests & sr) == sr;
        }

        inline bool wait_sync_req(SyncRequest sr)
        {
            while((m_sync_requests & (sr | SR_ERROR)) == 0)
            {
                boost::this_thread::yield();
            }
            return (m_sync_requests & SR_ERROR) == 0;
        }

    private:

        enum ApiState { AS_STOPPED, AS_RUNNING, AS_ISCALIBRATING };
        enum { SR_ERROR = 1 << 0, SR_TSTATE = 1 << 1, SR_GAZEDATA = 1 << 2, SR_SCREEN = 1 << 3, SR_CSTART = 1 << 4, SR_CRESULT = 1 << 5 };

        Heartbeater             m_heartbeat;
        Socket                  m_socket;
        ApiState                m_state;
        CalibrationProxy        m_calibration_proxy;
        std::string             m_port;

        ServerState             m_server_proxy;
        GazeData                m_gaze_data;
        CalibResult             m_calib_result;
        Screen                  m_screen;
        SyncRequest             m_sync_requests;

        mutable boost::mutex    m_calib_lock;
        mutable boost::mutex    m_gaze_lock;
        mutable boost::mutex    m_screen_lock;
        mutable boost::mutex    m_sync_lock;
    };

    GazeApi::GazeApi(bool verbose)
        : m_engine(new Engine(verbose))
    {}

    GazeApi::~GazeApi()
    {}

    void GazeApi::add_listener(IGazeListener & listener)
    {
        m_engine->add_observer(listener);
    }

    void GazeApi::remove_listener(IGazeListener & listener)
    {
        m_engine->remove_observer(listener);
    }

    void GazeApi::add_listener(ICalibrationResultListener & listener)
    {
        m_engine->add_observer(listener);
    }

    void GazeApi::remove_listener(ICalibrationResultListener & listener)
    {
        m_engine->remove_observer(listener);
    }

    void GazeApi::add_listener(ITrackerStateListener & listener)
    {
        m_engine->add_observer(listener);
    }

    void GazeApi::remove_listener(ITrackerStateListener & listener)
    {
        m_engine->remove_observer(listener);
    }

    void GazeApi::add_listener(ICalibrationProcessHandler & listener)
    {
        m_engine->add_observer(listener);
    }

    void GazeApi::remove_listener(ICalibrationProcessHandler & listener)
    {
        m_engine->remove_observer(listener);
    }

    bool GazeApi::is_connected() const
    {
        return m_engine->is_running();
    }

    bool GazeApi::connect(bool push_mode)
    {
        return connect(push_mode, 6555);
    }

    bool GazeApi::connect(bool push_mode, unsigned short port)
    {
        std::stringstream ss;
        ss << port;
        return m_engine->connect(push_mode, ss.str());
    }

    void GazeApi::disconnect()
    {
        m_engine->disconnect();
    }

    void GazeApi::set_push(bool const enable)
    {
        m_engine->set_push(enable);
    }

    void GazeApi::set_screen(Screen const & screen)
    {
        m_engine->set_screen(screen);
    }

    void GazeApi::get_screen(Screen & screen) const
    {
        m_engine->get_screen(screen);
    }

    void GazeApi::get_frame(GazeData & gaze_data) const
    {
        m_engine->get_frame(gaze_data);
    }

    void GazeApi::get_calib_result(CalibResult & calib_result) const
    {
        m_engine->get_calib_result(calib_result);
    }

    ServerState const & GazeApi::get_server_state() const
    {
        return m_engine->get_server_state();
    }

    bool GazeApi::calibration_start(int const point_count)
    {
        return m_engine->calibration_start(point_count);
    }

    void GazeApi::calibration_clear()
    {
        m_engine->calibration_clear();
    }

    void GazeApi::calibration_abort()
    {
        m_engine->calibration_abort();
    }

    void GazeApi::calibration_point_start(int const x, int const y)
    {
        m_engine->calibration_point_start(x, y);
    }

    void GazeApi::calibration_point_end()
    {
        m_engine->calibration_point_end();
    }
} // namespace gtl


