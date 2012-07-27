function hlib = lsl_loadlib
% Load the lab streaming layer library.
% [LibHandle] = lsl_loadlib
%
% This operation loads the library, after which its functions (starting with lsl_) can be used.
%
% Out:
%   Handle : handle to the library
%            when the handle is deleted, the library will be unloaded
%
% Notes:
%   Do not delete this handles while you still have LSL objects (streaminfos, inlets, outlets)
%   alive.
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2012-03-05


debugging = false; % set this to true to link against the debug version of the library

% find out which library path we need
basepath = [fileparts(mfilename('fullpath')) filesep];

if ispc
    ext = '.dll';
elseif ismac
    ext = '.dylib';
elseif isunix
    ext = '.so';
else
    error('Operating System not supported by this version of the lab streaming layer API.');
end

if strfind(computer,'64')
    bitness = '64';
else
    bitness = '32';
end
    
if debugging
    debug = '-debug';
else
    debug = '';
end

dllpath = [basepath 'bin' filesep 'liblsl' bitness debug ext];


if ~exist(dllpath,'file')
    error(['Apparently the file "' dllpath '" is missing on your computer. Cannot load the lab streaming layer.']); end

% open the library and make sure that it gets auto-deleted when the handle is erased
hlib = lsl_loadlib_(dllpath);
hlib.on_cleanup = onCleanup(@()lsl_freelib_(hlib));
