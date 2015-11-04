# example prcdedure for unstripping just one app
import lsl_get_deps as lgd
import os

# define the current operating system we want
op_sys = ["win32","win64", "OSX", "linux"]  # which os to unstrip -- win32 is the only one fully supported

# which apps are to be stripped/unstripped
apps = [
    "LabRecorder/src",
    "LabRecorder/src/pylsl",
    "LabRecorder/src/TestRecorder"]

# needed to obtain local copies of liblsl
lgd.dl_deps(lgd.libs, lgd.libs_d, op_sys)

# and the non liblsl-libraries for these apps
lgd.dl_deps(apps, lgd.apps_d, op_sys)

# bring the libs into selected apps
lgd.unstrip(apps, lgd.apps_d, lgd.apps_dir, op_sys)
