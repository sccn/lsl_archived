% Displays a dialog to give help information on an object.
%
% Examples:
% helpdialog char
%    gives information of character arrays
% helpdialog plot
%    gives help on the plot command
% helpdialog(obj)
%    gives help on the MatLab object obj
%
% See also: helptext, msgbox

% Copyright 2008-2010 Levente Hunyadi
function helpdialog(obj)

if nargin < 1
    obj = 'helpdialog';
end

if ischar(obj)
    key = obj;
else
    key = class(obj);
end
title = [key ' - Quick help'];
text = helptext(key);
if isempty(text)
    text = {'No help available.'};
end
if 0  % standard MatLab message dialog box
    createmode = struct( ...
        'WindowStyle', 'replace', ...
        'Interpreter', 'none');
    msgbox(text, title, 'help', createmode);
else
    fig = figure( ...
        'MenuBar', 'none', ...
        'Name', title, ...
        'NumberTitle', 'off', ...
        'Position', [0 0 480 160], ...
        'Toolbar', 'none', ...
        'Visible', 'off', ...
        'ResizeFcn', @helpdialog_resize);

    % information icon
    icons = load('dialogicons.mat');
    icons.helpIconMap(256,:) = get(fig, 'Color');
    iconaxes = axes( ...
        'Parent', fig, ...
        'Units', 'pixels', ...
        'Tag', 'IconAxes');
    try
        iconimg = image('CData', icons.helpIconData, 'Parent', iconaxes);
        set(fig, 'Colormap', icons.helpIconMap);
    catch me
        delete(fig);
        rethrow(me)
    end
    if ~isempty(get(iconimg,'XData')) && ~isempty(get(iconimg,'YData'))
        set(iconaxes, ...
            'XLim', get(iconimg,'XData')+[-0.5 0.5], ...
            'YLim', get(iconimg,'YData')+[-0.5 0.5]);
    end
    set(iconaxes, ...
        'Visible', 'off', ...
        'YDir', 'reverse');
    
    % help text
    rgb = get(fig, 'Color');
    text = cellfun(@(line) helpdialog_html(line), text, 'UniformOutput', false);
    html = ['<html>' strjoin(sprintf('\n'), text) '</html>'];
    jtext = javax.swing.JLabel(html);
    jcolor = java.awt.Color(rgb(1), rgb(2), rgb(3));
    jtext.setBackground(jcolor);
    jtext.setVerticalAlignment(1);
    jscrollpane = javax.swing.JScrollPane(jtext, javax.swing.JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, javax.swing.JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    jscrollpane.getViewport().setBackground(jcolor);
    jscrollpane.setBorder(javax.swing.border.EmptyBorder(0,0,0,0));
    
    [jcontrol,jcontainer] = javacomponent(jscrollpane, [0 0 100 100]);
    set(jcontainer, 'Tag', 'HelpText');

    
    movegui(fig, 'center');  % center figure on screen
    set(fig, 'Visible', 'on');
end

function helpdialog_resize(fig, event) %#ok<INUSD>

position = getpixelposition(fig);
width = position(3);
height = position(4);

iconaxes = findobj(fig, 'Tag', 'IconAxes');
helptext = findobj(fig, 'Tag', 'HelpText');

bottom = 7*height/12;
set(iconaxes, 'Position', [12 bottom 51 51]);
set(helptext, 'Position', [75 12 width-75-12 height-24]);

function html = helpdialog_html(line)

preline = deblank(line);  % trailing spaces removed
line = strtrim(preline);  % leading spaces removed
leadingspace = repmat('&nbsp;', 1, numel(preline)-numel(line));  % add leading spaces as non-breaking space

ix = strfind(line, 'See also');
if ~isempty(ix)
    ix = ix(1) + numel('See also');
    line = [ line(1:ix-1) regexprep(line(ix:end), '(\w[\d\w]+)', '<a href="matlab:helpdialog $1">$1</a>') ];
end

html = ['<p>' leadingspace line '</p>'];