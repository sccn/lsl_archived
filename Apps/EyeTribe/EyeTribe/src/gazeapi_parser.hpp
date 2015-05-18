/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifndef _THEEYETRIBE_GAZEAPI_PARSER_H_
#define _THEEYETRIBE_GAZEAPI_PARSER_H_

#include "gazeapi_types.h"
#include <boost/property_tree/json_parser.hpp>


namespace gtl
{
    class Parser
    {
    public:
        static bool parse_description(std::string & description, boost::property_tree::ptree const & root);
        static bool parse_calib_result(CalibResult & calib_result, boost::property_tree::ptree const & root, bool & has_calib_result);
        static bool parse_status_code( GazeApiStatusCode & status_code, boost::property_tree::ptree const & root);
        static bool parse_server_state(ServerState & server_state, GazeData & gaze_data, CalibResult & calib_result, Screen & screen, boost::property_tree::ptree const & root, bool& has_gaze_data, bool& has_calib_result);
        static bool parse_category(GazeApiCategory & category, boost::property_tree::ptree const & root);
        static bool parse_request( GazeApiRequest & request, boost::property_tree::ptree const & root);
        static bool parse_point2d(Point2D & point, boost::property_tree::ptree const & object);
        static bool parse_eye(Eye & eye, boost::property_tree::ptree const & object);

    private:
        typedef boost::property_tree::ptree PTree;
        typedef boost::optional<PTree const &> OptionalPTree;

    };
}

#endif // _THEEYETRIBE_GAZEAPI_PARSER_H_
