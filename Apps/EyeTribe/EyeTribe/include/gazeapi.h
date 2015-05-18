/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifndef _THEEYETRIBE_GAZEAPI_H_
#define _THEEYETRIBE_GAZEAPI_H_

#include <gazeapi_types.h>
#include <gazeapi_interfaces.h>

#include <memory>
#include <string>


namespace gtl
{

    /** \class GazeApi
    *   This is the main entry point into the GaziApi library for communicating and controlling The Eyetribe Tracker server
    */
    class GazeApi
    {
    public:
        /** GazeApi constructor.
         * Creates an instance of the GazeApi that can be used to connect to a server. 
         *
         * \param[in] verbose Whether to output all JSON-messages recieved and sent on the socket. 
         * When enabling verbose output, messages are output using std::cout.
         */
        explicit GazeApi(bool verbose = false);
        ~GazeApi();

        /** Add an IGazeListener to the GazeApi.
         *
         * \param[in] listener The IGazeListener listener to be added.
         * \sa remove_listener(IGazeListener & listener).
         */
        void add_listener(IGazeListener & listener);

        /** Remove an IGazeListener from the GazeApi.
         *
         * \param[in] listener The IGazeListener listener to be removed.
         * \sa add_listener(IGazeListener & listener).
         */
        void remove_listener(IGazeListener & listener);

        /** Add an ICalibrationResultListener to the GazeApi.
         *
         * \param[in] listener The ICalibrationResultListener listener to be added.
         * \sa remove_listener(ICalibrationResultListener & listener).
         */
        void add_listener(ICalibrationResultListener & listener);

        /** Remove an ICalibrationResultListener from the GazeApi.
         *
         * \param[in] listener The ICalibrationResultListener listener to be removed.
         * \sa add_listener(ICalibrationResultListener & listener).
         */
        void remove_listener(ICalibrationResultListener & listener);

        /** Add an ITrackerStateListener to the GazeApi.
         *
         * \param[in] listener The ITrackerStateListener listener to be added.
         * \sa remove_listener(ITrackerStateListener & listener).
         */
        void add_listener(ITrackerStateListener & listener);

        /** Remove an ITrackerStateListener from the GazeApi.
         *
         * \param[in] listener The ITrackerStateListener listener to be removed.
         * \sa add_listener(ITrackerStateListener & listener).
         */
        void remove_listener(ITrackerStateListener & listener);

        /** Add an ICalibrationProcessHandler to the GazeApi.
         *
         * \param[in] listener The ICalibrationProcessHandler listener to be added.
         * \sa remove_listener(ICalibrationProcessHandler & listener).
         */
        void add_listener(ICalibrationProcessHandler & listener);

        /** Remove an ICalibrationProcessHandler from the GazeApi.
         *
         * \param[in] listener The ICalibrationProcessHandler listener to be removed.
         * \sa add_listener(ICalibrationProcessHandler & listener).
         */
        void remove_listener(ICalibrationProcessHandler & listener);

        /** Query whether the client is connected the the server.
         *
         * \return bool True if connected, false if not.
         */
        bool is_connected() const;

        /** Connect to the server via default port.
         *
         * \param[in] push_mode connect using push mode? otherwice pull mode is activated.
         * \return bool True if connected, false if connection failed.
         */
        bool connect(bool push_mode);

        /** Connect to the server via specified port.
         *
         * \param[in] push_mode connect using push mode? otherwice pull mode is activated.
         * \param[in] port port number to connect to server on.
         * \return bool True if connected, false if connection failed.
         */
        bool connect(bool push_mode, unsigned short port);

        /** Disconnect from server. */
        void disconnect();

        /** Enable/disable server push of GazeData via call backs from IGazeListener interface.
         *
         * \param[in] enable True if enable, false if disable.
         */
        void set_push(bool const enable);

        /** Set screen parameters.
         *
         * \param[in] screen the Screen parameters to be set.
         */
        void set_screen(Screen const & screen);

        /** Get current used screen parameters.
         *
         * \param[out] screen the Screen parameters to be retrieved.
         */
        void get_screen(Screen & screen) const;

        /** Get current GazeData
         *
         * Attempts to retrieve the current valid GazeData. If push-mode is enabled gaze_data is filled with the latest valid GazeData.
         * If push-mode is disabled the method requests the latest GazeData from the server and fills out gaze_data when it is available.
         * 
         * In pull-mode this is a blocking call. The method does not terminate before a new GazeData has been delivered.
         * In push-mode the GazeApi will fire an IGazeListener::on_gaze_data(gtl::GazeData const & gaze_data) containing the latest GazeData.
         * 
         * \param[out] gaze_data valid GazeData if push-mode enabled, invalid otherwise.
         */
        void get_frame(GazeData & gaze_data) const;

        /** Get current valid calibration 
         *
         * \param[out] calib_result latest valid calibration result.
         */
        void get_calib_result(CalibResult & calib_result) const;

        /** Read the current server state.
         *
         * \returns ServerState the current server state.
         */
        ServerState const & get_server_state() const;

        /** Begin new calibration sesssion.
         *
         * \param[in] point_count The number of points to use for calibration.
         * \returns indication of the request processed okay.
         */
        bool calibration_start(int const point_count);

        /** Clear the current server calibration .
         *
         * Clears the current server calibration but does not affect an ongoing calibration session.
         */
        void calibration_clear();

        /** Abort the current calibration session.
         *
         * Aborts the current calibration session, but does not clear any valid calibration in the server
         */
        void calibration_abort();

        /** Begin calibration a new calibration point.
         *
         * \param[in] x x-coordinate of calibration point.
         * \param[in] y y-coordinate of calibration point.
         * \sa calibration_point_end.
        */
        void calibration_point_start(int const x, int const y);

        /** End current calibration point.
         * \sa calibration_point_start(int const x, int const y).
        */
        void calibration_point_end();

    private:
        GazeApi(GazeApi const & other);
        GazeApi & operator = (GazeApi const & other);

        class Engine;

#if __cplusplus <= 199711L
        std::auto_ptr<Engine> m_engine;
#else
        std::unique_ptr<Engine> m_engine;
#endif
    };

}

#endif
