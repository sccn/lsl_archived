This folder contains a modified version of the PropertyGrid toolbox:
Major changes:
* PropertyType accepts [] for types that are 'scalar'.
* The PropertyGrid can be constructed from an argument specification (as created by the arg system).
* Unsupported Datatypes are automatically serialized into strings in the PropertyGrid (function handles, non-string cell arrays, structs)
* There are now properties with children whose value selects from different subtrees (e.g., a pulldown list to select from different children structures).
* An argument specification can be extracted from the PropertyGrid.
* The code has been more extensively commented.


The folder also contains a subset of the guifunc package of EEGLAB 10.0.1.0, including the files:
* supergui.m
* inputgui.m
* finputcheck.m

Lastly, there are a few functions from the BCILAB toolbox included:
* hlp_* (commonly-used helper functions)
* parse_help.m
* disp_once.m
* env_doc.m
