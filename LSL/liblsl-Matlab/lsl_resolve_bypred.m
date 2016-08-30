function streaminfos = lsl_resolve_bypred(lib,pred,minimum,timeout)
% [Streaminfos] = lsl_resolve_byprop(LibHandle,Predicate,Minimum,Timeout)
% Resolve streams of the network whose contents match a given predicate.
%
% Advanced query that allows to impose more conditions on the retrieved streams; the given string is
% an XPath 1.0 predicate for the <info> node (omitting the surrounding []'s), see also 
% http://en.wikipedia.org/w/index.php?title=XPath_1.0&oldid=474981951.
%
% In:
%   LibHandle : handle to the API library, as obtained via lsl_loadlib();
%
%   Predicate : The predicate string, e.g. 'name=''BioSemi''' or 
%               'type=''EEG'' and starts-with(name,''BioSemi'') and count(info/desc/channel)=32'
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
tmpresults = lsl_resolve_bypred_(lib,pred,minimum,timeout);

% create an lsl_streaminfo instance for each result
streaminfos = cell(1,length(tmpresults));
for r=1:length(tmpresults)
    streaminfos{r} = lsl_streaminfo(lib,tmpresults(r)); end
