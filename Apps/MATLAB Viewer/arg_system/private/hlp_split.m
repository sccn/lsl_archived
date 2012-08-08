function res = hlp_split(str,delims)
% Split a string according to some delimiter(s).
% Result = hlp_split(String,Delimiters)
%
% In:
%   String : a string (char vector)
%
%   Delimiters : a vector of delimiter characters (includes no special support for escape sequences)
%
% Out:
%   Result : a cell array of (non-empty) non-Delimiter substrings in String
%
% Examples:
%   % split a string at colons and semicolons; returns a cell array of four parts
%   hlp_split('sdfdf:sdfsdf;sfdsf;;:sdfsdf:',':;')
% 
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-11-05

pos = find(diff([0 ~sum(bsxfun(@eq,str(:)',delims(:)),1) 0]));
res = cell(~isempty(pos),length(pos)/2);
for k=1:length(res)
    res{k} = str(pos(k*2-1):pos(k*2)-1); end
