function [name,file] = hlp_getcaller(indirection)
% Find the name & file of the calling function.
% [Name,File] = hlp_getcaller(Indirection)
%
% In:
%   Indirection : optional level of indirection (default: 1)
%
% Out: 
%   Name: MATLAB name of the calling function, if any
%   File: file name of the calling function, if any
% 
% Example:
%   % in a function, get the name of the calling function (if any)
%   mycaller = hlp_getcaller;
%
%   % in a function, get the name of the function that called the calling function
%   mycallerscaller = hlp_getcaller(2);
%
% See also:
%   hlp_iscaller
%
%                               Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                               2010-04-15

try
    throw; %#ok<LTARG> % fast way to get an exception
catch context
    if ~exist('indirection','var')
        indirection = 1; end
    if length(context.stack) > indirection+1
        name = context.stack(indirection+2).name;
        file = context.stack(indirection+2).file;
    else
        name = [];
        file = [];
    end
end
