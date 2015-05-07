function streaminfos = lsl_resolve_all(lib,timeout)
% [Streaminfos] = lsl_resolve_all(LibHandle,Timeout)
% Resolve all streams on the network.
%
% This function returns all currently available streams from any outlet on the network.
% The network is usually the subnet specified at the local router, but may also include
% a multicast group of machines (given that the network supports it), or list of hostnames.
% These details may optionally be customized by the experimenter in a configuration file
% (see Configuration File in the documentation).
% This is the default mechanism used by the browsing programs and the recording program.
%
% In:
%   LibHandle : handle to the API library, as obtained via lsl_loadlib();
%
%   Timeout : The timeout for the operation, in seconds.
%
% Out
%   Streaminfos : A cell array of stream info objects (excluding their desc field), any of which can
%                 subsequently be used to open an inlet. The full info can be retrieved from the
%                 inlet.

if ~exist('timeout','var') || isempty(timeout) 
    timeout = 3.0; end

% first do a raw resolve
tmpresults = lsl_resolve_all_(lib,timeout);

% create an lsl_streaminfo instance for each result
streaminfos = cell(1,length(tmpresults));
for r=1:length(tmpresults)
    streaminfos{r} = lsl_streaminfo(lib,tmpresults(r)); end
