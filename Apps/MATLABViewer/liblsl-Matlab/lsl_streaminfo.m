classdef lsl_streaminfo < handle
	% The stream_info object stores the declaration of a data stream.
	% Represents the following information:
	%  a) stream data format (#channels, channel format)
	%  b) core information (stream name, content type, sampling rate)
	%  c) optional meta-data about the stream content (channel labels, measurement units, etc.)
	%
	% Available to recipients of a stream and also written to disk when being recorded
	% (corresponds to the file header).

    properties
        LibHandle = 0;      % this is a handle to the liblsl library (to call its functions)
        InfoHandle = 0;     % this is a handle to an lsl_streaminfo object within the library.
    end
    
    methods
        
        function self = lsl_streaminfo(lib,name,type,channelcount,samplingrate,channelformat,sourceid)
            % Construct a new stream_info object.
            % StreamInfo = lsl_streaminfo(LibHandle,Name,Type,ChannelCount,SamplingRate,ChannelFormat,SourceId)
            %
            % Core stream information is specified here. Any remaining meta-data can be added later.
            %
            % In:
            % 	Name : Name of the stream. Describes the device (or product series) that this stream makes available
            %		   (for use by programs, experimenters or data analysts).
            %
            %   Type : Content type of the stream. Please see Table of Content Types in the documentation 
            %          for naming recommendations.
            %
            %   ChannelCount : Number of channels per sample. This stays constant for the lifetime of the stream.
            %
            %   NominalSrate : The sampling rate (in Hz) as advertised by the data source, 
            %                  if regular (otherwise set to IRREGULAR_RATE).
            %
            %   ChannelFormat : Format/type of each channel. If your channels have different formats, 
            %                   consider supplying multiple streams or use the largest type that can 
            %                   hold them all (such as cf_double64). The possible formats are:
            %                   'cf_float32', 'cf_double64', 'cf_string', 'cf_int32', 'cf_int16',
            %                   'cf_int8'
            %
            %   SourceId: Unique identifier of the source or device, if available (such as the serial number).
            %             Allows recipients to recover from failure even after the serving app or device crashes.
            %			  May in some cases also be constructed from device settings.
            
            self.LibHandle = lib;
            
            if ~exist('type','var') && ~ischar(name)
                % assume that the second argument is a handle
                self.InfoHandle = name;
            else                        
                % parse inputs
                if ~exist('channelcount','var') || isempty(channelcount) channelcount = 1; end
                if ~exist('samplingrate','var') || isempty(samplingrate) samplingrate = 0; end
                if ~exist('channelformat','var') || isempty(channelformat) channelformat = 'cf_float32'; end
                if ~exist('sourceid','var') || isempty(sourceid) sourceid = ''; end
                
                % translate channel formats
                if ischar(channelformat)
                    switch(channelformat)
                        case 'cf_float32'       % For up to 24-bit precision measurements in the appropriate physical unit
                            channelformat = 1;  % (e.g., microvolts). Integers from -16777216 to 16777216 are represented accurately.
                        case 'cf_double64'      % For universal numeric data as long as permitted by network & disk budget.
                            channelformat = 2;  % The largest representable integer is 53-bit.
                        case 'cf_string'        % For variable-length ASCII strings or data blobs, such as video frames,
                            channelformat = 3;  % complex event descriptions, etc.
                        case 'cf_int32'         % For high-rate digitized formats that require 32-bit precision. Depends critically on
                            channelformat = 4;  % meta-data to represent meaningful units. Useful for application event codes or other coded data.
                        case 'cf_int16'         % For very high rate signals (40Khz+) or consumer-grade audio
                            channelformat = 5;  % (for professional audio float is recommended).
                        case 'cf_int8'          % For binary signals or other coded data.
                            channelformat = 6;  % Not recommended for encoding string data.
                        case 'cf_int64'         % For future compatibility. Support for this type is not yet widespread in programming languages
                            channelformat = 7;  % and therefore not exposed in the API for all languages. Some builds of liblsl will not be able to
                                                % send or receive data of this type.
                        otherwise
                            error('Unsupported channel format.');
                    end
                end
                
                % create info object
                self.InfoHandle = lsl_create_streaminfo(lib,name,type,channelcount,samplingrate,channelformat,sourceid);
            end
        end
        
        
        function delete(self)
            % Destroy the streaminfo when it is deleted.
            lsl_destroy_streaminfo(self.LibHandle,self.InfoHandle);
        end
                
        
		% === Core Information ===
		% (these fields are assigned at construction)
        
        function result = name(self)
            % Get the name of the stream.
            % This is a human-readable name. For streams offered by device modules, it refers to the type of device or product series
            % that is generating the data of the stream. If the source is an application, the name may be a more generic or specific identifier.
            % Multiple streams with the same name can coexist, though potentially at the cost of
            % ambiguity (for the recording app or experimenter).
            
            result = lsl_get_name(self.LibHandle,self.InfoHandle);
        end
        

        function result = type(self)
            % Get the content type of the stream.
            % The content type is a short string such as "EEG", "Gaze" which describes the content carried by the channel (if known). 
            % If a stream contains mixed content this value need not be assigned but may instead be stored in the description of channel types.
            % To be useful to applications and automated processing systems using the recommended content types is preferred. 
            % See Table of Content Types in the documentation.
            
            result = lsl_get_type(self.LibHandle,self.InfoHandle);
        end
        
        
        function result = channel_count(self)
            % Get the number of channels of the stream.
            % A stream has at least one channel; the channel count stays constant for all samples.
            
            result = lsl_get_channel_count(self.LibHandle,self.InfoHandle);
        end
        
        
        function result = nominal_srate(self)
            % Get the sampling rate of the stream, according to the source (in Hz).
            % If a stream is irregularly sampled, this should be set to IRREGULAR_RATE.
            %
            % Note that no data will be lost even if this sampling rate is incorrect or if a device has temporary
            % hiccups, since all samples will be recorded anyway (except for those dropped by the device itself). However,
            % when the recording is imported into an application, a good importer may correct such errors more accurately
            % if the advertised sampling rate was close to the specs of the device.
            
            result = lsl_get_nominal_srate(self.LibHandle,self.InfoHandle);
        end
        
        
        function result = channel_format(self)
            % Get the channel format of the stream.
            % All channels in a stream have the same format. However, a device might offer multiple time-synched streams 
            % each with its own format.
            
            channel_format_strings = {'cf_float32','cf_double64','cf_string','cf_int32','cf_int16','cf_int8','cf_int64'};
            result = channel_format_strings{lsl_get_channel_format(self.LibHandle,self.InfoHandle)};
        end
        
        
        function result = source_id(self)
            % Get the unique identifier of the stream's source, if any.
            % The unique source (or device) identifier is an optional piece of information that, if available, allows that
            % endpoints (such as the recording program) can re-acquire a stream automatically once it is back online.
            
            result = lsl_get_source_id(self.LibHandle,self.InfoHandle);
        end

        
		% === Additional Network Info ===
		% (these fields are auto-assigned once the streaminfo is bound to an outlet)        
        
        function result = version(self)
            % Get the protocol version used to deliver the stream.

            result = lsl_get_version(self.LibHandle,self.InfoHandle);
        end
        

        function result = created_at(self)
            % Get the creation time stamp of the stream.
            % This is the time stamp when the stream was first created
            % (as determined via lsl_local_clock() on the providing machine).
            
            result = lsl_get_created_at(self.LibHandle,self.InfoHandle);
        end
        
        
        function result = uid(self)
            % Get the unique ID of the stream outlet (once assigned).
            % This is a unique identifier of the stream outlet, and is guaranteed to be different
            % across multiple instantiations of the same outlet (e.g., after a re-start).
            
            result = lsl_get_uid(self.LibHandle,self.InfoHandle);
        end
        
        
        function result = session_id(self)
            % Get the session ID for the given stream.
            % The session id is an optional human-assigned identifier of the recording session.
            % While it is rarely used, it can be used to prevent concurrent recording activitites
            % on the same sub-network (e.g., in multiple experiment areas) from seeing each other's streams
            % (assigned via a configuration file by the experimenter, see Configuration File in the docs).
            
            result = lsl_get_session_id(self.LibHandle,self.InfoHandle);
        end
        
        
        function result = desc(self)
            % Get access to the extended description of the stream.
            % It is highly recommended that at least the channel labels are described here.
            % See code examples in the documentation. Other information, such as amplifier settings,
            % measurement units if deviating from defaults, setup information, subject information, etc.,
            % can be specified here, as well. See Meta-Data Recommendations in the docs.
            
            result = lsl_xml_ptr(self.LibHandle,lsl_get_desc(self.LibHandle,self.InfoHandle));
        end
        
        
        function result = as_xml(self)
            % Retrieve the entire stream_info in XML format.
            % This yields an XML document (in string form) whose top-level element is <info>. The info element contains
            % one element for each field of the stream_info class, including:
            %  a) the core elements <name>, <type>, <channel_count>, <nominal_srate>, <channel_format>, <source_id>
            %  b) the misc elements <version>, <created_at>, <uid>, <session_id>, <v4address>, <v4data_port>, <v4service_port>, <v6address>, <v6data_port>, <v6service_port>
            %  c) the extended description element <desc> with user-defined sub-elements.
            
            result = lsl_get_xml(self.LibHandle,self.InfoHandle);
        end
        
    end
end