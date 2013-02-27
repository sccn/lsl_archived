function env_doc(topic)
% Replacement for the 'doc' command (also works in deployed mode).
% env_doc(Topic)
%
% In:
%   Topic : topic to display documentation for (name of a function, directory on path, or function 
%           handle itself)
%
%                               Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                               2012-04-06

if isa(topic,'function_handle')
    as_string = char(topic);
    try
        if as_string(1) == '@'
            % if this is a class member function, use the class name as topic
            fn = functions(topic);
            tmp = struct2cell(fn.workspace{1});
            topic = class(tmp{1});
        else
            % otherwise use the string form of the function as topic
            topic = as_string;
        end
    catch
        disp('Cannot deduce help topic for the given function handle.');
        return;
    end
end

if ~isdeployed
    doc(topic);
else
    try
        % check if this is a file
        filepath = utl_whichfile(topic);
        docpath = strrep(strrep(filepath,env_translatepath('bcilab:/'),env_translatepath('bcilab:/build/docs/')),'.m','.html');
        env_web(docpath);
    catch
        try
            % treat it as a directory relative to BCILAB's root (e.g. code/arguments)
            docpath = env_translatepath(['bcilab:/build/docs/' topic '/index.html']);
            env_web(docpath);
        catch
            disp(['Could not retrieve documentation for "' topic '".']);
        end
    end
end 
