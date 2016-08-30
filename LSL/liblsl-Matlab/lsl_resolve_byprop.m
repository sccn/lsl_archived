function streaminfos = lsl_resolve_byprop(lib,prop,value,minimum,timeout)
% [Streaminfos] = lsl_resolve_byprop(LibHandle,PropertyName,PropertyValue,Minimum,Timeout)
% Resolve streams of the network that have a particular value for a given property.
%
% If the goal is to resolve a specific stream, this method is preferred over resolving all streams 
% and then selecting the desired one.
%
% In:
%   LibHandle : handle to the API library, as obtained via lsl_loadlib();
%
%   PropertyName : The stream_info property that should have a specific value (e.g., 'name', 'type',
%                  'source_id', or 'desc/manufaturer').
%
%   PropertyValue : The string value that the property should have (such as 'EEG' if the PropertyName
%                   was 'type').
%
%   Minimum : Return at least this number of results. (default: 1)
%
%   Timeout : Optionally a timeout of the operation, in seconds (default: 5s). If the timeout 
%             expires, less than the desired number of streams (possibly none) will be returned.
%
% Out:
%   Streaminfos : A vector of matching stream info objects (excluding their meta-data), any of
%                 which can subsequently be used to open an inlet.
%
% Notes:
%   If you want to wait until a stream shows up it is usually a better idea to call resolve
%   in a loop with a few-second timeout than to set the timeout to infinite (since MATLAB would
%   otherwise become unresponsive).

if ~exist('minimum','var') || isempty(minimum)
    minimum = 1; end
if ~exist('timeout','var') || isempty(timeout)
    timeout = 5; end

% first do a raw resolve
tmpresults = lsl_resolve_byprop_(lib,prop,value,minimum,timeout);

% create an lsl_streaminfo instance for each result
streaminfos = cell(1,length(tmpresults));
for r=1:length(tmpresults)
    streaminfos{r} = lsl_streaminfo(lib,tmpresults(r)); end
