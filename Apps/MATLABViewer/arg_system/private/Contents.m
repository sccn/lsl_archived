% Property grid using JIDE implementation
% Copyright 2010 Levente Hunyadi
%
% Sample code
%   example_propertyeditor - Demonstrates how to use the property editor.
%   example_propertygrid   - Demonstrates how to use the property pane.
%   example_matrixeditor   - Demonstrates how to use the matrix editor.
%   SampleObject           - Sample object to test data persistence functions and user controls.
%   SampleNestedObject     - Sample nested object to test expandable properties.
%
% Property grid control and property metadata
%   PropertyEditor         - A property editor for custom objects.
%   PropertyGrid           - A property grid based on the JIDE grid implementation.
%   PropertyGridField      - Metadata on a single property.
%   PropertyType           - Type information on a property.
%
% Property grid utility classes
%   JidePropertyGridField  - A single JIDE property field in a JIDE property grid.
%   UIControl              - Root class for composite user controls.
%
% Matrix editor
%   MatrixEditor           - Displays a matrix in a pop-up window for convenient editing.
%
% Help services
%   helpdialog             - Displays a dialog to give help information on an object.
%   helptext               - Help text associated with a function, class, property or method.
%
% Java data marshaling
%   javaArrayList          - Converts a MatLab array into a java.util.ArrayList.
%   javaclass              - Return java.lang.Class instance for MatLab type.
%   javaStringArray        - Converts a MatLab cell array of strings into a java.lang.String array.
%
% String utility functions
%   strjoin                - Concatenates a cell array of strings.
%   strsplit               - Splits a string into a cell array of strings.
%   var2str                - Textual representation of any MatLab value.
%   strsetmatch            - Indicator of which elements of a universal set are in a particular set.
%
% Utility functions for structures and objects
%   arrayfilter            - Filter elements of array that meet a condition.
%   constructor            - Sets public properties of a MatLab object using a name-value list.
%   getclassfield          - Field value of each object in an array or cell array.
%   getdependentproperties - Publicly accessible dependent properties of an object.
%   nestedassign           - Assigns the given value to the named property of an object or structure.
%   nestedfetch            - Fetches the value of the named property of an object or structure.
%
% Utility functions for handle graphics objects
%   findobjuser            - Find handle graphics object with user data check.
%   objectEDT              - Auto-delegate Java object method calls to Event Dispatch Thread (EDT).
