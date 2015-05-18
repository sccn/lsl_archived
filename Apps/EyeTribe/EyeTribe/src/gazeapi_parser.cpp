/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#include "gazeapi_parser.hpp"

#include <string>


namespace gtl
{
    /* static */ bool Parser::parse_description(std::string & description, boost::property_tree::ptree const & root)
    {
        boost::optional<std::string> desc = root.get_optional<std::string>("statusmessage");

        if (!desc)
        {
            return false;
        }

        description = *desc;

        return true;
    }

    /* static */ bool Parser::parse_calib_result(CalibResult & calib_result, boost::property_tree::ptree const & root, bool & has_calib_result)
    {
        OptionalPTree values = root.get_child_optional("values");
        has_calib_result = false;

        if (!values)
        {
            return true;
        }

        OptionalPTree calibresult = values->get_child_optional("calibresult");

        if(!calibresult)
        {
            return false;
        }

        calib_result.result = calibresult->get<bool>("result");
        calib_result.deg = calibresult->get<float>("deg");
        calib_result.degl = calibresult->get<float>("degl");
        calib_result.degr = calibresult->get<float>("degr");

        OptionalPTree calibpoints = calibresult->get_child_optional("calibpoints");

        if(!calibpoints)
        {
            return false;
        }

        std::vector<CalibPoint> & calibpoints_vector = calib_result.calibpoints;
        calibpoints_vector.reserve(calibpoints->size());
        boost::property_tree::ptree::const_iterator it = calibpoints->begin();
        boost::property_tree::ptree::const_iterator end = calibpoints->end();

        for (; it != end ; ++it)
        {
            CalibPoint calib_point;

            calib_point.state = it->second.get<int>("state");
            parse_point2d(calib_point.cp, it->second.get_child("cp"));
            parse_point2d(calib_point.mecp, it->second.get_child("mecp"));

            PTree const & acd = it->second.get_child("acd");
            calib_point.acd.ad = acd.get<float>("ad");
            calib_point.acd.adl = acd.get<float>("adl");
            calib_point.acd.adr = acd.get<float>("adr");

            PTree const & mepix = it->second.get_child("mepix");
            calib_point.mepix.mep = mepix.get<float>("mep");
            calib_point.mepix.mepl = mepix.get<float>("mepl");
            calib_point.mepix.mepr = mepix.get<float>("mepr");

            PTree const & asdp = it->second.get_child("asdp");
            calib_point.asdp.asd = asdp.get<float>("asd");
            calib_point.asdp.asdl = asdp.get<float>("asdl");
            calib_point.asdp.asdr = asdp.get<float>("asdr");

            calibpoints_vector.push_back(calib_point);
        }

        has_calib_result = true;
        return true;
    }

    /* static */ bool Parser::parse_status_code( GazeApiStatusCode & status_code, boost::property_tree::ptree const & root)
    {
        boost::optional <int> status = root.get_optional<int>("statuscode");

        if (!status)
        {
            return false;
        }

        status_code =
            *status == 200 ? GASC_OK :
            *status == 800 ? GASC_CALIBRATION_CHANGE :
            *status == 801 ? GASC_DISPLAY_CHANGE :
            *status == 802 ? GASC_TRACKER_STATE_CHANGE :
            GASC_ERROR;

        return *status != GASC_UNKNOWN;
    }

    /* static */ bool Parser::parse_server_state(ServerState & server_state, GazeData & gaze_data, CalibResult & calib_result, Screen & screen, boost::property_tree::ptree const & root, bool& has_gaze_data, bool& has_calib_result)
    {
        OptionalPTree values = root.get_child_optional("values");

        if (!values)
        {
            return false;
        }

        Parser::parse_calib_result(calib_result, root, has_calib_result);

        OptionalPTree frame = values->get_child_optional("frame");
        has_gaze_data = static_cast<bool>(frame);

        if (has_gaze_data)
        {
            gaze_data.time = frame->get<int>("time");
            gaze_data.fix = frame->get<bool>("fix");
            gaze_data.state  = frame->get<int>("state");
            parse_point2d(gaze_data.raw, frame->get_child("raw"));
            parse_point2d(gaze_data.avg, frame->get_child("avg"));
            parse_eye(gaze_data.lefteye, frame->get_child("lefteye"));
            parse_eye(gaze_data.righteye, frame->get_child("righteye"));
        }

        server_state.push               = values->get<bool>("push", server_state.push);
        server_state.heartbeatinterval  = values->get<int>("heartbeatinterval", server_state.heartbeatinterval);
        server_state.version            = values->get<int>("version", server_state.version);
        server_state.trackerstate       = values->get<int>("trackerstate", server_state.trackerstate);
        server_state.framerate          = values->get<int>("framerate", server_state.framerate);
        server_state.iscalibrated       = values->get<bool>("iscalibrated", server_state.iscalibrated);
        server_state.iscalibrating      = values->get<bool>("iscalibrating", server_state.iscalibrating);

        screen.screenindex = values->get<int>("screenindex", screen.screenindex);
        screen.screenresw  = values->get<int>("screenresw", screen.screenresw);
        screen.screenresh  = values->get<int>("screenresh", screen.screenresh);
        screen.screenpsyw  = values->get<float>("screenpsyw", screen.screenpsyw);
        screen.screenpsyh  = values->get<float>("screenpsyh", screen.screenpsyh);
        return true;
    }

    /* static */ bool Parser::parse_category(GazeApiCategory & category, boost::property_tree::ptree const & root)
    {
        boost::optional<std::string> cat = root.get_optional<std::string>("category");

        if (!cat)
        {
            return false;
        }

        category =
            *cat == "tracker" ? GAC_TRACKER :
            *cat == "calibration" ? GAC_CALIBRATION :
            *cat == "heartbeat" ? GAC_HEARTBEAT :
            GAC_UNKNOWN;

        return category != GAC_UNKNOWN;
    }

    /* static */ bool Parser::parse_request( GazeApiRequest & request, boost::property_tree::ptree const & root)
    {
        boost::optional<std::string> req = root.get_optional<std::string>("request");

        if (!req)
        {
            return false;
        }

        request =
            *req == "get" ? GAR_GET :
            *req == "set" ? GAR_SET :
            *req == "start" ? GAR_START :
            *req == "abort" ? GAR_ABORT :
            *req == "clear" ? GAR_CLEAR :
            *req == "pointstart" ? GAR_POINTSTART :
            *req == "pointend" ? GAR_POINTEND :
            GAR_UNKNOWN;

        return true;
    }

    /* static */ bool Parser::parse_point2d(Point2D & point, boost::property_tree::ptree const & object)
    {
        point.x = object.get<float>("x");
        point.y = object.get<float>("y");
        return true;
    }

    /* static */ bool Parser::parse_eye(Eye & eye, boost::property_tree::ptree const & object)
    {
        parse_point2d(eye.raw, object.get_child("raw"));
        parse_point2d(eye.avg, object.get_child("avg"));
        eye.psize = object.get<float>("psize");
        parse_point2d(eye.pcenter, object.get_child("pcenter"));
        return true;
    }



}
