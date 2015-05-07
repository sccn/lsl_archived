classdef lsl_outlet < handle
    % A stream outlet.
    % Outlets are used to make streaming data (and the meta-data) available on the lab network.
    
    properties (Hidden)
        LibHandle = 0;       % this is a handle to the liblsl library (to call its functions)
        OutletHandle = 0;    % this is a handle to an lsl_outlet object within the library.
        SampleRate = 0;      % copy of the sample rate...
    end
    
    methods
        
        function self = lsl_outlet(info, chunksize, maxbuffered)
            % Outlet = lsl_outlet(Streaminfo, ChunkSize, MaxBuffered)
            % Establish a new stream outlet. This makes the stream discoverable.
            %
            % In:
            %   Streaminfo : The lsl_streaminfo object to use for creating this stream. Stays constant over the lifetime of the outlet.
            %
            %   ChunkSize :  Optionally the desired chunk granularity (in samples) for transmission. If 0,
            %                each push operation yields one chunk. Stream recipients can have this setting bypassed.
            %                (default: 0)
            %
            %   MaxBuffered : Optionally the maximum amount of data to buffer (in seconds if there is a nominal
            %                 sampling rate, otherwise x100 in samples). Stream recipients can elect to use 
            %                 a smaller (but not larger) buffer size than the one specified here.
            %                 (default: 360)
            %
            
            if ~exist('chunksize','var') || isempty(chunksize) chunksize = 0; end
            if ~exist('maxbuffered','var') || isempty(maxbuffered) maxbuffered = 360; end            
            self.LibHandle = info.LibHandle;
            self.SampleRate = info.nominal_srate();
            self.OutletHandle = lsl_create_outlet(info.LibHandle,info.InfoHandle,chunksize,maxbuffered);
        end
        
        
        function delete(self)
            % Destroy the outlet when it is deleted.
            % The outlet will no longer be discoverable after destruction and all paired inlets will
            % stop delivering data.
            
            lsl_destroy_outlet(self.LibHandle,self.OutletHandle);
        end
        
        
        function push_sample(self,sampledata,timestamp,pushthrough)
            % push_sample(SampleData,Timestamp,Pushthrough)
            % Push a sample into the outlet.
            %
            % In:
            %   SampleData : either a numeric vector (as double, converted implicitly) with as many 
            %                elements as the stream has channels, or a cell array of strings (if the 
            %                 stream is string-formatted), or a cell array of uint8 vectors to submit
            %                 variable-length binary data.
            %
            %   Timestamp : Optionally the capture time of the most recent sample, in agreement with lsl_local_clock(); 
            %               if 0, the current time is used. 
            %
            %   Pushthrough : Whether to push the chunk through to the receivers instead of buffering it with subsequent samples.
            %                   Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
            %                 (default: 1)

            if ~exist('timestamp','var') || isempty(timestamp) timestamp = 0; end
            if ~exist('pushthrough','var') || isempty(pushthrough) pushthrough = 1; end
            lsl_push_sample(self.LibHandle,self.OutletHandle,sampledata,timestamp,pushthrough);
        end

        
        function push_chunk(self,chunkdata,timestamps,pushthrough)
            % push_sample(ChunkData,Timestamps,Pushthrough)
            % Push a chunk into the outlet.
            %
            % In:
            %   ChunkData : Numeric matrix with one column for each sample that shall be
            %               transmitted. May also be empty.
            %
            %   Timestamps : Optionally the capture time of the most recent sample, in agreement with lsl_local_clock(); 
            %                if 0, the current time is used. The time stamps of other samples are automatically 
            %                derived based on the sampling rate of the stream, or replicated if the stream is irregular-rate 
            %                If this is a vector, it is assumed that it contains one time stamp for each sample. (default: 0)
            %
            %   Pushthrough : Whether to push the chunk through to the receivers instead of buffering it with subsequent samples.
            %                   Note that the chunk_size, if specified at outlet construction, takes precedence over the pushthrough flag.
            %                 (default: 1)

            if ~exist('timestamps','var') || isempty(timestamps) timestamps = 0; end
            if ~exist('pushthrough','var') || isempty(pushthrough) pushthrough = 1; end
            
            if timestamps == 0
                timestamps = lsl_local_clock(self.LibHandle); end

            if isscalar(timestamps)
                if self.SampleRate == 0
                    timestamps = [timestamps; -ones(size(chunkdata,2)-1,1)];
                else
                    % otherwise infer the timestamp of the first sample in the chunk and let the
                    % library deduce the timestamps for the remaining samples (using -1.0 as the
                    % stamps).
                    timestamps = [timestamps - (size(chunkdata,2)-1)/self.SampleRate; -ones(size(chunkdata,2)-1,1)];
                end
            end
                        
            lsl_push_chunk(self.LibHandle,self.OutletHandle,chunkdata,timestamps,pushthrough);
        end
        
        
        % === Miscellaneous Features ===

        function result = have_consumers(self)
            % Check whether consumers are currently registered.
            % While it does not hurt, there is technically no reason to push samples if there is no consumer.
            
            result = lsl_have_consumers(self.LibHandle,self.OutletHandle);
        end

        
        function success = wait_for_consumers(self,timeout)
            % Wait until some consumer shows up (without wasting resources).
            %
            % In:
            %   Timeout : Timeout for the operation. (default: 60)
            %
            % Out:
            %   Success : True if the wait was successful, false if the timeout expired.
            
            if ~exist('timeout','var') || isempty(timeout) timeout = 60; end
            success = lsl_wait_for_consumers(self.LibHandle,self.OutletHandle,timeout);
        end
        
        
        function result = info(self)
            % Retrieve the stream info provided by this outlet.
            % This is what was used to create the stream (and also has the Additional Network Information fields assigned).
            
            result = lsl_streaminfo(self.LibHandle,lsl_get_info(self.LibHandle,self.OutletHandle));
        end
        
        function h = get_libhandle(self)
            % get the library handle (e.g., to query the clock)
            h = self.LibHandle;
        end        
    end
end