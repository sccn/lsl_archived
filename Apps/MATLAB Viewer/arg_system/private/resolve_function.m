function f = resolve_function(expr)
% resolve a scoped function, which is normally not possible in MATLAB

% thus, we have to use a crazy trick: arg_report gives us access to nested functions
ofs = strfind(expr,'::@');
f = arg_report('handle',str2func(expr(1:ofs-1)),{expr(ofs+3:end)});
if iscell(f)
    if isempty(f)
        error(['No function corresponding to the expression ' expr ' was found.']);
    else
        if length(f) > 1
            warnonce(['There are multiple functions with the name ' char(f{1}) '; returning the first one.']); end
        f = f{1};
    end
end

