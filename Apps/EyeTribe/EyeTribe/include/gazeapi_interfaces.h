/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifndef _THEEYETRIBE_GAZEAPI_INTERFACES_H_
#define _THEEYETRIBE_GAZEAPI_INTERFACES_H_

#include "gazeapi_types.h"


namespace gtl
{
    /** \class IGazeListener
     *  Callback interface with methods associated to Gaze Tracking.
     *  This interface should be implemented by classes that are to recieve live GazeData stream.
     */
    class IGazeListener
    {
    public:
        virtual ~IGazeListener() {}

        /** A notification call back indicating that a new GazeData frame is available. 
         * Implementing classes should update themselves accordingly if needed.
         * Register for updates through GazeApi::add_listener(IGazeListener & listener)
        
         \param[in] gazeData Latest GazeData frame processed by Tracker Server
         */
        virtual void on_gaze_data(gtl::GazeData const & gaze_data) = 0;
    };

    /** \class ICalibrationResultListener
     *  Callback interface with methods associated to the changes of calibration result.
     *  This interface should be implemented by classes that are to recieve only changes in calibration result
     *  and who are _not_ to perform the calibration process itself.
     */
    class ICalibrationResultListener
    {
    public:
        virtual ~ICalibrationResultListener() {}

        /** A notification call back indicating that state of calibration has changed. 
         * Implementing classes should update themselves accordingly if needed.
         * Register for updates through GazeApi::add_listener(ICalibrationResultListener & listener).
         *
         * \param[in] is_calibrated is the Tracker Server calibrated?
         * \param[in] calib_result if calibrated, the currently valid gtl::CalibResult
         */
        virtual void on_calibration_changed(bool is_calibrated, gtl::CalibResult const & calib_result) = 0;
    };

        
    /** \class ITrackerStateListener
     *  Callback interface with methods associated to the state of the physical Tracker device.
     *  This interface should be implemented by classes that are to recieve changes if the state of Tracker
     *  and handle these accordingly. This could be a class in the 'View' layer telling the user that a 
     *  Tracker has disconnected.
     */
    class ITrackerStateListener
    {
    public:
        virtual ~ITrackerStateListener() {}

        /** A notification call back indicating that state of connected Tracker device has changed. 
         *  Use this to detect if a tracker has been connected or disconnected.
         *  Implementing classes should update themselves accordingly if needed.
         *  Register for updates through GazeApi::add_listener(ITrackerStateListener & listener).
         *
         * \param[in] trackerState the current state of the physical Tracker device
         */
        virtual void on_tracker_connection_changed(int tracker_state) = 0;

        /** A notification call back indicating that main screen index has changed. 
         *  This is only relevant for multiscreen setups. Implementing classes should
         *  update themselves accordingly if needed.
         *  Register for updates through GazeApi::add_listener(ITrackerStateListener & listener).
         *
         *  \param[in] screen the new screen state
         */
        virtual void on_screen_state_changed(gtl::Screen const & screen) = 0;
    };

    /** \class ICalibrationProcessHandler
     *  Callback interface with methods associated to Calibration process.
     */
    class ICalibrationProcessHandler
    {
    public:
        virtual ~ICalibrationProcessHandler() {}

        /** Called when a calibration process has been started. */
        virtual void on_calibration_started() = 0;

        /** Called every time tracking of a single calibration points has completed.
         *
         * \param[in] progress'normalized' progress [0..1d]
         */
        virtual void on_calibration_progress(double progress) = 0;

        /** Called when all calibration points have been collected and calibration processing begins. */
        virtual void on_calibration_processing() = 0;

        /** Called when processing of calibration points and calibration as a whole has completed.
         * 
         * \param[in] is_calibrated is the Tracker Server calibrated?
         * \param[in] calib_result if calibrated, the currently valid gtl::CalibResult
         */
        virtual void on_calibration_result(bool is_calibrated, gtl::CalibResult const & calib_result) = 0;
    };
}

#endif // _THEEYETRIBE_GAZEAPI_INTERFACES_H_
