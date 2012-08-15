function arg_toworkspace(args)
% Copy the arguments in the given Struct into the workspace of the calling function. 
% arg_toworkspace(Struct)
%
% In:
%   Struct : an argument structure, as produced by arg_define
%
% Examples:
%   function myfunction(varargin)
%   opts = arg_define(varargin, ...
%               arg(...), ...
%               arg(...));
%
%   arg_toworkspace(opts);
%
% See also:
%   arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

% place the variables in the caller's workspace
for fn=fieldnames(args)'
    assignin('caller',fn{1},args.(fn{1})); end
