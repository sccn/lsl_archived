function hlib = lsl_loadlib(binarypath,debugging,keep_persistent)
% Load the lab streaming layer library.
% [LibHandle] = lsl_loadlib(BinaryPath,DebugVersion)
%
% This operation loads the library, after which its functions (starting with lsl_) can be used.
%
% In:
%   BinaryPath : Optionally the path to the locations of the liblsl bin folder (default: relative
%                to this .m file).
%
%   DebugVersion : Optionally load the debug version of the library (default: false)
%
%   Persistent : keep the library handle around until shutdown (default: true)
%
% Out:
%   Handle : handle to the library
%            when the handle is deleted, the library will be unloaded
%
% Notes:
%   Do not delete this handle while you still have LSL objects (streaminfos, inlets, outlets)
%   alive.
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2012-03-05

if ~exist('binarypath','var') || isempty(binarypath)
    binarypath = [fileparts(mfilename('fullpath')) filesep 'bin']; end
if ~exist('debugging','var') || isempty(debugging)
    debugging = false; end
if ~exist('keep_persistent','var') || isempty(keep_persistent)
    keep_persistent = true; end
    
persistent lib;
if keep_persistent && ~isempty(lib)
    hlib = lib;
else   
    if ispc
        ext = '.dll';
    elseif ismac
        ext = '.dylib';
    elseif isunix
        ext = '.so';
    else
        error('Your operating system is not supported by this version of the lab streaming layer API.');
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

    dllpath = [binarypath filesep 'liblsl' bitness debug ext];

    if ~exist(dllpath,'file')
        error(['Apparently the file "' dllpath '" is missing on your computer. Cannot load the lab streaming layer.']); end

    % open the library and make sure that it gets auto-deleted when the handle is erased
    hlib = lsl_loadlib_(dllpath);
    hlib.on_cleanup = onCleanup(@()lsl_freelib_(hlib));
    
    if keep_persistent
        lib = hlib; end
end