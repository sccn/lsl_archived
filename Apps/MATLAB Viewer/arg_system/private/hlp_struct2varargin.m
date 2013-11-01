function args = hlp_struct2varargin(struc,varargin)
% Convert a struct into a sequence of name-value pairs; inverse to hlp_varargin2struct.
% Args = hlp_struct2varargin(Struct,Options...)
%
% In:
%   Struct: a 1x1 structure array
%   Options: optional name-value pairs;
%            'suppress': suppress the output names listed in the following cell array
%            'rewrite' : rewrite some names in the struct, if present (executed after suppress and before restrict)
%                        specified as a cell array of {oldname,newname,oldname,newname, ....}
%            'restrict': restrict the output names to those listed in the following cell array
%
% Out:
%  Args: a list of name-value pairs
%
% Examples:
%   % in the given cell array of name-value pairs, remove occurrences of myarg1 and myarg2 parameters
%   hlp_struct2varargin(options,'suppress',{'myarg1','myarg2'});
%
%   % rewrite any occurrences of a 'channel' parameter into 'chns', and likewise for samplerate
%   hlp_struct2varargin(options,'rewrite',{'channels','chns','samplerate','srate'});
%
%   % restrict the given name-value pairs to those whose name is either test, arg1, or arg2.
%   hlp_struct2varargin(options,'restrict',{'test','arg1','arg2'});
%
% See also:
%   hlp_varargin2struct
%
%                               Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                               2010-03-28

opts = hlp_varargin2struct(varargin,'restrict',[],'suppress',[],'rewrite',[]);

fields = fieldnames(struc)';
data = struct2cell(struc)'; 
% suppress some of the original field names
if ~isempty(opts.suppress)
    [fields,I] = setdiff(fields,opts.suppress);
    data = data(I);
end
% rewrite some of the original field names into new field names
for c=1:2:length(opts.rewrite)
    fields(strcmp(fields,opts.rewrite{c})) = opts.rewrite(c+1); end
% restrict to a subset of old/new field names
if ~isempty(opts.restrict)
    [fields,I,J] = intersect(fields,opts.restrict);  %#ok<NASGU>
    data = data(I);
end

args = vertcat(fields,data); 
args = args(:)';
