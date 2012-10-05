function [streams,fileheader] = load_xdf(filename,varargin)
% Import an XDF file.
% [Streams,FileHeader] = load_xdf(Filename, Options...)
%
% This is a simple MATLAB importer for mult-stream XDF (Extensible Data Format) recordings. All
% information covered by the XDF 1.0 specification is imported, plus any additional meta-data
% associated with streams or with the container file itself.
%
% See http://code.google.com/p/xdf/ for more information on XDF.
%
% In:
%   Filename : name of the file to import (*.xdf or *.xdfz)
%
%   Options... : a list of optional name-value arguments. The allowed names are listed in
%                the following: 
%
%                'OnChunk' : Function that is called for each chunk of data as it 
%                            is being retrieved from the file; the function is allowed to modify the
%                            data (for example, sub-sample it). The four input arguments are 1) the
%                            matrix of [#channels x #samples] values (either numeric or 2d cell
%                            array of strings), 2) the vector of unprocessed local time stamps (one
%                            per sample), 3) the info struct for the stream (same as the .info field
%                            in the final output, buth without the .effective_srate sub-field), and
%                            4) the scalar stream number (1-based integers). The three return values
%                            are 1) the (optionally modified) data, 2) the (optionally modified)
%                            time stamps, and 3) the (optionally modified) header.
%
% Out:
%   Streams : cell array of structs, one for each stream; the structs have the following content:
%             .time_series field: contains the stream's time series [#Channels x #Samples]
%                                 this matrix is of the type declared in .info.channel_format
%             .time_stamps field: contains the time stamps for each sample (synced across streams)
%
%             .info field: contains the meta-data of the stream (all values are strings)
%               .name: name of the stream
%               .type: content-type of the stream ('EEG','Events', ...)
%               .channel_format: value format ('int8','int16','int32','int64','float32','double64','string')
%               .nominal_srate: nominal sampling rate of the stream (as declared by the device);
%                               zero for streams with irregular sampling rate
%               .effective_srate: effective (measured) sampling rate of the stream, if regular
%                                 (otherwise omitted)
%               .desc: struct with any domain-specific meta-data declared for the stream; see
%                      www.xdf.org for the declared specifications
%
%             .segments field: struct array containing segment ranges for regularly sampled
%                              time series with breaks (not present if the stream is irregular)
%               .index_range: 1st and last index of the segment within the .time_series/.time_stamps
%                             arrays
%               .t_begin: time of the 1st sample in the segment, in seconds
%               .t_end: time of the last sample in the segment, in seconds
%               .duration: duration of the segment, in seconds
%               .num_samples: number of samples in the segment
%               .effective_srate: effective (i.e. measured) sampling rate within the segment
%
%   FileHeader : struct with file header contents in the .info field
%
% Examples:
%   % load the streams contained in a given XDF file
%   streams = load_xdf('C:\Recordings\myrecording.xdf')
%
% Notes:
%   A somewhat higher-quality clock drift post-processing is performed when the Statistics toolbox 
%   is present.
%
% License:
%     This file is covered by the BSD license.
%
%     Copyright (c) 2012, Christian Kothe
%     Portions Copyright (c) 2010, Wouter Falkena
%     All rights reserved.
%
%     Redistribution and use in source and binary forms, with or without
%     modification, are permitted provided that the following conditions are
%     met:
%
%         * Redistributions of source code must retain the above copyright
%           notice, this list of conditions and the following disclaimer.
%         * Redistributions in binary form must reproduce the above copyright
%           notice, this list of conditions and the following disclaimer in
%           the documentation and/or other materials provided with the distribution
%
%     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
%     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
%     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
%     ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
%     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
%     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
%     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
%     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
%     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
%     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
%     POSSIBILITY OF SUCH DAMAGE.
%
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2012-04-22
%
%                                Contains portions of xml2struct Copyright (c) 2010, Wouter Falkena,
%                                ASTI, TUDelft, 21-08-2010
%
%                                version 1.09

% handle options
opts = cell2struct(varargin(2:2:end),varargin(1:2:end),2);
if ~isfield(opts,'OnChunk')
    opts.OnChunk = @default_OnChunk; end

if ~exist(filename,'file')
    error(['The file "' filename '" does not exist.']); end
    
% uncompress if necessary
[p,n,x] = fileparts(filename);
if strcmp(x,'.xdfz')
    % idea for this type of approach by Michael Kleder
    import com.mathworks.mlwidgets.io.InterruptibleStreamCopier
    src = java.io.FileInputStream(filename);
    flt = java.util.zip.InflaterInputStream(src);
    filename = [p filesep n '_temp_uncompressed' x];
    dst = java.io.FileOutputStream(filename);
    copier = InterruptibleStreamCopier.getInterruptibleStreamCopier;
    copier.copyStream(flt,dst);
    dst.close();
    src.close();
end

% open the file (and make sure that we don't forget to close it)
f = fopen(filename,'r','ieee-le.l64');
closer = onCleanup(@()close_file(f,filename));

streams = {};               % cell array of returned streams (in the order of appearance in the file)
idmap = sparse(2^31-1,1);   % remaps stream id's onto indices in streams
temp = struct();            % struct array of temporary per-stream information
fileheader = struct();      % the file header

% check whether we have a working mex file for the inner loop
have_mex = exist('load_xdf_innerloop','file');
if ~have_mex
    disp('NOTE: apparently you are missing a compiled binary version of the inner loop code. Using the slow MATLAB code instead.'); end

% === read the file ===

% [MagicCode]
if ~strcmp(fread(f,4,'*char')','XDF:')
    error(['This is not a valid XDF file (' filename ').']); end

% for each chunk...
while 1
    % [NumLengthBytes], [Length]
    len = read_varlen_int(f);
    if ~len
        break; end
    % [Tag]
    switch fread(f,1,'uint16')
        % [Samples] chunk
        case 3
            try
                % [StreamId]
                id = idmap(fread(f,1,'uint32'));
                if have_mex
                    % read the chunk data at once
                    data = fread(f,len-6,'*uint8');
                    % run the mex kernel
                    [values,timestamps] = load_xdf_innerloop(data, temp(id).chns, temp(id).readfmt, temp(id).sampling_interval, temp(id).last_timestamp);
                    temp(id).last_timestamp = timestamps(end);
                else
                    % [NumSampleBytes], [NumSamples]
                    num = read_varlen_int(f);
                    % allocate space
                    timestamps = zeros(1,num);
                    if strcmp(temp(id).readfmt,'*string')
                        values = cell(temp(id).chns,num);
                    else
                        values = zeros(temp(id).chns,num);
                    end
                    % for each sample...
                    for s=1:num
                        % read or deduce time stamp
                        if fread(f,1,'*uint8')
                            timestamps(s) = fread(f,1,'double');
                        else
                            timestamps(s) = temp(id).last_timestamp + temp(id).sampling_interval;
                        end
                        % read the values
                        if strcmp(temp(id).readfmt,'*string')
                            for v = 1:size(values,1)
                                values{v,s} = fread(f,read_varlen_int(f),'*char')'; end
                        else
                            values(:,s) = fread(f,size(values,1),temp(id).readfmt);
                        end
                        temp(id).last_timestamp = timestamps(s);
                    end
                end
                % send through the OnChunk function
                [values,timestamps,streams{id}] = opts.OnChunk(values,timestamps,streams{id},id);
                % append to the time series...
                temp(id).time_series{end+1} = values;
                temp(id).time_stamps{end+1} = timestamps;
            catch e
                % an error occurred (perhaps a chopped-off file): emit a warning 
                % and return the file up to this point
                warning(e.identifier,e.message);
                break;                
            end
        % [StreamHeader] chunk
        case 2
            % [StreamId]
            streamid = fread(f,1,'uint32');
            id = length(streams)+1;
            idmap(streamid) = id;
            % [Content]
            header = parse_xml_struct(fread(f,len-6,'*char')');
            streams{id} = header;
            % generate a few temporary fields
            temp(id).chns = str2num(header.info.channel_count);
            temp(id).srate = str2num(header.info.nominal_srate);
            temp(id).last_timestamp = 0;
            temp(id).time_series = {};
            temp(id).time_stamps = {};
            temp(id).clock_times = [];
            temp(id).clock_values = [];
            if temp(id).srate > 0
                temp(id).sampling_interval = 1/temp(id).srate;
            else
                temp(id).sampling_interval = 0;
            end
            % fread parsing format for data values
            temp(id).readfmt = ['*' header.info.channel_format];
            if strcmp(temp(id).readfmt,'*double64')
                temp(id).readfmt = 'double'; end               
        % [StreamFooter] chunk
        case 6
            % [StreamId]
            id = idmap(fread(f,1,'uint32'));
            % [Content]
            footer = parse_xml_struct(fread(f,len-6,'*char')');
            streams{id} = hlp_superimposedata(footer,streams{id});
        % [FileHeader] chunk
        case 1            
            fileheader = parse_xml_struct(fread(f,len-2,'*char')');
        % [ClockOffset] chunk
        case 4
            % [StreamId]
            id = idmap(fread(f,1,'uint32'));
            % [CollectionTime]
            temp(id).clock_times(end+1) = fread(f,1,'double');
            % [OffsetValue]
            temp(id).clock_values(end+1) = fread(f,1,'double');
        otherwise
            % skip other chunks (Boundary, ...)
            fread(f,len-2,'*uint8');
    end
end


% === do some advanced post-processing on the result ===

for k=1:length(temp)
    % concatenate the signal
    temp(k).time_series = [temp(k).time_series{:}];
    temp(k).time_stamps = [temp(k).time_stamps{:}];
    if ~isempty(temp(k).time_stamps)
        try
            clock_times = temp(k).clock_times;
            clock_values = temp(k).clock_values;
            % calculate clock offset mapping
            try
                mapping = robustfit(clock_times,clock_values);
            catch %#ok<CTCH>
                try
                    mapping = regress(clock_values',[ones(length(clock_times),1) clock_times']);
                catch %#ok<CTCH>
                    mapping = clock_values / [ones(1,length(clock_times)); clock_times];
                end
            end
            % apply offset correction
            temp(k).time_stamps = temp(k).time_stamps + (mapping(1) + mapping(2)*temp(k).time_stamps);
        catch %#ok<CTCH>
            disp(['No clock offsets were available for stream "' streams{k}.info.name '"']);
        end
        
        % do some fix-ups if the time series is regularly sampled
        if temp(k).srate
            segment_threshold = [1,500]; % time that has to pass without data from a regularly sampled
            % stream to assume that the stream is fragmented into segments
            % (breaks shorter than this will be assumed to be time-stamping
            % glitches) in seconds and in samples (both thresholds have to
            % be exceeded)
            
            % check for segment breaks
            diffs = diff(temp(k).time_stamps);
            breaks_at = diffs > max(segment_threshold(1),segment_threshold(2)*temp(k).sampling_interval);
            if any(breaks_at)
                % turn the break mask into a cell array of [begin,end] index ranges
                tmp = find(breaks_at)';
                tmp = [tmp tmp+1]';
                tmp = [1 tmp(:)' length(breaks_at)];
                ranges = num2cell(reshape(tmp,2,[])',2);
            else
                ranges = {[1,length(temp(k).time_stamps)]};
            end
            
            % calculate some meta-data about the segments
            segments = repmat(struct(),1,length(ranges));
            for r=1:length(ranges)
                range = ranges{r};
                segments(r).num_samples = range(2)-range(1)+1;
                segments(r).index_range = range;
                % re-calculate the time stamps within the range (linearly interpolated)
                if segments(r).num_samples > 0
                    indices = segments(r).index_range(1):segments(r).index_range(2);
                    stamps = temp(k).time_stamps(indices);
                    try
                        mapping = regress(stamps',[ones(length(indices),1) indices']);
                    catch %#ok<CTCH>
                        mapping = stamps / [ones(1,length(indices)); indices];
                    end
                    temp(k).time_stamps(indices) = mapping(1) + mapping(2) * indices;
                end
                segments(r).t_begin = temp(k).time_stamps(range(1));
                segments(r).t_end = temp(k).time_stamps(range(2));
                segments(r).duration = segments(r).t_end - segments(r).t_begin;
                segments(r).effective_srate = segments(r).num_samples / segments(r).duration;
            end
            
            % calculate the weighted mean sampling rate over all segments
            temp(k).effective_rate = sum(bsxfun(@times,[segments.effective_srate],[segments.num_samples]/sum([segments.num_samples])));
            
            % transfer the information into the output structs
            streams{k}.info.effective_srate = temp(k).effective_rate;
            streams{k}.segments = segments;
        end
    end
    streams{k}.time_series = temp(k).time_series;
    streams{k}.time_stamps = temp(k).time_stamps;
end

end

% read a variable-length integer
function num = read_varlen_int(f)
try
    switch fread(f,1,'*uint8')
        case 1
            num = fread(f,1,'*uint8');
        case 4
            num = fread(f,1,'*uint32');
        case 8
            num = fread(f,1,'*uint64');
        otherwise
            error('Invalid variable-length integer encountered.');
    end
catch
    num = 0;
end
end


% close the file and delete temporary data
function close_file(f,filename)
fclose(f);
if strfind(filename,'_temp_uncompressed.xdf')
    delete(filename); end
end


% --- helper functions ---

% parse a simplified (attribute-free) subset of XML into a MATLAB struct
function result = parse_xml_struct(str)
import org.xml.sax.InputSource
import javax.xml.parsers.*
import java.io.*
tmp = InputSource();
tmp.setCharacterStream(StringReader(str));
result = parseChildNodes(xmlread(tmp));

    % this is part of xml2struct (slightly simplified)
    function [children,ptext] = parseChildNodes(theNode)
        % Recurse over node children.
        children = struct;
        ptext = [];
        if theNode.hasChildNodes
            childNodes = theNode.getChildNodes;
            numChildNodes = childNodes.getLength;
            for count = 1:numChildNodes
                theChild = childNodes.item(count-1);
                [text,name,childs] = getNodeData(theChild);
                if (~strcmp(name,'#text') && ~strcmp(name,'#comment'))
                    if (isfield(children,name))
                        if (~iscell(children.(name)))
                            children.(name) = {children.(name)}; end
                        index = length(children.(name))+1;
                        children.(name){index} = childs;
                        if(~isempty(text))
                            children.(name){index} = text; end
                    else
                        children.(name) = childs;
                        if(~isempty(text))
                            children.(name) = text; end
                    end
                elseif (strcmp(name,'#text'))
                    if (~isempty(regexprep(text,'[\s]*','')))
                        if (isempty(ptext))
                            ptext = text;
                        else
                            ptext = [ptext text];
                        end
                    end
                end
            end
        end
    end

    % this is part of xml2struct (slightly simplified)
    function [text,name,childs] = getNodeData(theNode)
        % Create structure of node info.
        name = char(theNode.getNodeName);
        if ~isvarname(name)
            name = regexprep(name,'[-]','_dash_');
            name = regexprep(name,'[:]','_colon_');
            name = regexprep(name,'[.]','_dot_');
        end
        [childs,text] = parseChildNodes(theNode);
        if (isempty(fieldnames(childs)))
            try
                text = char(theNode.getData);
            catch
            end
        end
    end
end


function [data,timestamps,header] = default_OnChunk(data,timestamps,header,streamnum)
    % nothing to do...
end

function res = hlp_superimposedata(varargin)
% Merge multiple partially populated data structures into one fully populated one.
% Result = hlp_superimposedata(Data1, Data2, Data3, ...)
%
% The function is applicable when you have cell arrays or structs/struct arrays with non-overlapping
% patterns of non-empty entries, where all entries should be merged into a single data structure
% which retains their original positions. If entries exist in multiple data structures at the same
% location, entries of later items will be ignored (i.e. earlier data structures take precedence).
%
% In:
%   DataK : a data structure that should be super-imposed with the others to form a single data
%           structure
%
% Out:
%   Result : the resulting data structure
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2011-08-19

% first, compactify the data by removing the empty items
compact = varargin(~cellfun('isempty',varargin));

% start with the last data structure, then merge the remaining data structures into it (in reverse
% order as this avoids having to grow arrays incrementally in typical cases)
res = compact{end};
for k=length(compact)-1:-1:1
    res = merge(res,compact{k}); end
end


% merge data structures A and B
function A = merge(A,B)

if iscell(A) && iscell(B)
    % make sure that both have the same number of dimensions
    if ndims(A) > ndims(B)
        B = grow_cell(B,size(A));
    elseif ndims(A) < ndims(B)
        A = grow_cell(A,size(B));
    end
    
    % make sure that both have the same size
    if all(size(B)==size(A))
        % we're fine
    elseif all(size(B)>=size(A))
        % A is a minor of B: grow A
        A = grow_cell(A,size(B));
    elseif all(size(A)>=size(B))
        % B is a minor of A: grow B
        B = grow_cell(B,size(A));
    else
        % A and B have mixed sizes... grow both as necessary
        M = max(size(A),size(B));
        A = grow_cell(A,M);
        B = grow_cell(B,M);
    end
    
    % find all non-empty elements in B
    idx = find(~cellfun(@(x)isequal(x,[]),B));
    
    if ~isempty(idx)
        % check if any of these is occupied in A
        clean = cellfun('isempty',A(idx));
        if ~all(clean)
            % merge all conflicting items recursively
            conflicts = idx(~clean);
            for k=conflicts(:)'
                A{k} = merge(A{k},B{k}); end
            % and transfer the rest
            if any(clean)
                A(idx(clean)) = B(idx(clean)); end
        else
            % transfer all to A
            A(idx) = B(idx);
        end
    end
elseif isstruct(A) && isstruct(B)
    % first make sure that both have the same fields
    fnA = fieldnames(A);
    fnB = fieldnames(B);
    if isequal(fnA,fnB)
        % we're fine
    elseif isequal(sort(fnA),sort(fnB))
        % order doesn't match -- impose A's order on B
        B = orderfields(B,fnA);
    elseif isempty(setdiff(fnA,fnB))
        % B has a superset of A's fields: add the remaining fields to A, and order them according to B
        remaining = setdiff(fnB,fnA);
        for fn = remaining'
            A(1).(fn{1}) = []; end
        A = orderfields(A,fnB);
    elseif isempty(setdiff(fnB,fnA))
        % A has a superset of B's fields: add the remaining fields to B, and order them according to A
        remaining = setdiff(fnA,fnB);
        for fn = remaining'
            B(1).(fn{1}) = []; end
        B = orderfields(B,fnA);
    else
        % A and B have incommensurable fields; add B's fields to A's fields, add A's fields to B's
        % and order according to A's fields
        remainingB = setdiff(fnB,fnA);
        for fn = remainingB'
            A(1).(fn{1}) = []; end
        remainingA = setdiff(fnA,fnB);
        for fn = remainingA'
            B(1).(fn{1}) = []; end
        B = orderfields(B,A);
    end
    
    % that being established, convert them to cell arrays, merge their cell arrays, and convert back to structs
    merged = merge(struct2cell(A),struct2cell(B));
    A = cell2struct(merged,fieldnames(A),1);
    
elseif isstruct(A) && ~isstruct(B)
    if ~isempty(B)
        error('One of the sub-items is a struct, and the other one is of a non-struct type.');
    else
        % we retain A
    end
elseif isstruct(B) && ~isstruct(A)
    if ~isempty(A)
        error('One of the sub-items is a struct, and the other one is of a non-struct type.');
    else
        % we retain B
        A = B;
    end
elseif iscell(A) && ~iscell(B)
    if ~isempty(B)
        error('One of the sub-items is a cell array, and the other one is of a non-cell type.');
    else
        % we retain A
    end
elseif iscell(B) && ~iscell(A)
    if ~isempty(A)
        error('One of the sub-items is a cell array, and the other one is of a non-cell type.');
    else
        % we retain B
        A = B;
    end
elseif isempty(A) && ~isempty(B)
    % we retain B
    A = B;
elseif isempty(B) && ~isempty(A)
    % we retain A
elseif ~isequal(A,B)
    % we retain A and warn about dropping B
    disp('Two non-empty (and non-identical) sub-elements occupied the same index; one was dropped. This warning will only be displayed once.');
end
end

% grow a cell array to accomodate a particular index
% (assuming that this index is not contained in the cell array yet)
function C = grow_cell(C,idx)
tmp = sprintf('%i,',idx);
eval(['C{' tmp(1:end-1) '} = [];']);
end
    
