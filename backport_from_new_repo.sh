# Call this script from the path corresponding to the new submodule
# Usage: backport_to_old_repo.sh [newrepopath] [commit id]

set -x
set -e

LSLSUBDIR=$(git rev-parse --show-prefix)
OLDREPOPATH=$(git rev-parse --show-toplevel)
NEWREPOPATH=$1
COMMIT=$2
git -C $NEWREPOPATH/$LSLSUBDIR show -p $COMMIT | git apply -p1 --directory=$LSLSUBDIR -v
