/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifndef _THEEYETRIBE_GAZEAPI_TYPES_H_
#define _THEEYETRIBE_GAZEAPI_TYPES_H_

#include <vector>
#include <cstring> // memcmp


namespace gtl
{
    enum GazeApiCategory
    {
        GAC_UNKNOWN,
        GAC_TRACKER,
        GAC_CALIBRATION,
        GAC_HEARTBEAT
    };

    enum GazeApiRequest
    {
        GAR_UNKNOWN,
        GAR_GET,
        GAR_SET,
        GAR_START,
        GAR_ABORT,
        GAR_CLEAR,
        GAR_POINTSTART,
        GAR_POINTEND
    };

    enum GazeApiStatusCode
    {
        GASC_UNKNOWN,
        GASC_OK,
        GASC_ERROR,
        GASC_CALIBRATION_CHANGE,
        GASC_DISPLAY_CHANGE,
        GASC_TRACKER_STATE_CHANGE
    };

    struct Point2D
    {
        float x;    ///< x coordinate
        float y;    ///< y coordinate
    };

    struct Eye
    {
        Point2D raw; ///< raw gaze coordinates in pixels
        Point2D avg; ///< smoothed coordinates in pix
        float psize;  ///<pupil size
        Point2D pcenter; ///< pupil coordinates normalized
    };

    struct GazeData
    {
        enum
        {
            GD_STATE_TRACKING_GAZE        = 1 << 0,
            GD_STATE_TRACKING_EYES        = 1 << 1,
            GD_STATE_TRACKING_PRESENCE    = 1 << 2,
            GD_STATE_TRACKING_FAIL        = 1 << 3,
            GD_STATE_TRACKING_LOST        = 1 << 4
        };

        int time; ///< timestamp
        bool fix; ///< is fixated?
        int state; ///< 32bit masked state integer
        Point2D raw; ///< raw gaze coordinates in pixels
        Point2D avg; ///< smoothed coordinates in pix
        Eye lefteye; ///< data for left eye
        Eye righteye; ///< data for right eye

        bool operator == (GazeData const & rhs) const
        {
            return 0 == memcmp(this, &rhs, sizeof(GazeData));
        }

        bool operator != (GazeData const & rhs) const
        {
            return 0 != memcmp(this, &rhs, sizeof(GazeData));
        }
    };

    struct CalibPoint
    {
        int state; ///< state of calibration point
        Point2D cp; ///< coordinates in pixels
        Point2D mecp; ///< mean estimated coords in pixels
        struct { float ad; float adl; float adr; } acd; ///< accuracy in degrees
        struct { float mep; float mepl; float mepr; } mepix ; ///< mean error in pixels
        struct { float asd; float asdl; float asdr; } asdp; ///< average std deviation in

        bool operator == (CalibPoint const & rhs) const
        {
            return 0 == memcmp(this, &rhs, sizeof(CalibPoint));
        }
    };

    struct CalibResult
    {
        bool result; ///< was the calibration successful?
        float deg;  ///< average error in degrees
        float degl; ///< average error in degs, left eye
        float degr;  ///< average error in degs, right eye
        std::vector<CalibPoint> calibpoints;

        bool operator == (CalibResult const & rhs) const
        {
            return result == rhs.result &&
                deg == rhs.deg &&
                degl == rhs.degl &&
                degr == rhs.degr &&
                calibpoints == rhs.calibpoints;
        }

        bool operator != (CalibResult const & rhs) const
        {
            return result != rhs.result ||
                deg != rhs.deg ||
                degl != rhs.degl ||
                degr != rhs.degr ||
                calibpoints != rhs.calibpoints;
        }

        void clear()
        {
            result = false;
            deg = 0.0f;
            degl = 0.0f;
            degr = 0.0f;
            calibpoints.clear();
        }
    };

    struct Screen
    {
        int screenindex;    ///< Screen index
        int screenresw;     ///< Screen resolution width in pixels
        int screenresh;     ///< Screen resolution height in pixels
        float screenpsyw;   ///< Screen physical width in meters
        float screenpsyh;   ///< Screen physical height in meters

        void set(int index, int resw, int resh, float psyw, float psyh)
        {
            screenindex = index;
            screenresw = resw;
            screenresh = resh;
            screenpsyw = psyw;
            screenpsyh = psyh;
        }

        bool operator == (Screen const & rhs) const
        {
            return 0 == memcmp(this, &rhs, sizeof(Screen));
        }

        bool operator != (Screen const & rhs) const
        {
            return 0 != memcmp(this, &rhs, sizeof(Screen));
        }
    };

    struct ServerState
    {
        enum
        {
            TRACKER_CONNECTED = 0,
            TRACKER_NOT_CONNECTED = 1,
            TRACKER_CONNECTED_BADFW = 2,
            TRACKER_CONNECTED_NOUSB3 = 3,
            TRACKER_CONNECTED_NOSTREAM = 4
        };

        bool push;
        int heartbeatinterval;
        int version;
        int trackerstate;
        int framerate;
        bool iscalibrated;
        bool iscalibrating;
    };
}

#endif ///_THEEYETRIBE_GAZEAPI_H_
