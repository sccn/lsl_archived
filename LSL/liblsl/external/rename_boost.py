#!/usr/bin/env/python
import os
def process(loc):
    for dir,subdirs,names in os.walk(loc):
        for name in names:
            if name.endswith('.hpp') or name.endswith('.cpp') or name.endswith('.ipp') or name.endswith('.inl'):
                path = os.path.join(dir, name)
                text = open(path,'r').read()
                text = text.replace('boost','lslboost')
                open(path,'w').write(text)
				
# first rename the directories 
os.rename('boost','lslboost')    # (note: if you're getting an error here you're probably not in the right directory or don't have the appropriate rights)
os.rename('libs','src')

# then recursively rename all occurrences of boost to lslboost
process('./lslboost')
process('./src')