# Call this script from the path corresponding to the new submodule

set -e

if [ "$#" -ne 2 ]; then
	printf 'Usage:\n\tcd submodule/path\n\tbackport_to_old_repo.sh [newrepopath] [commit id]\n\n'
	printf 'Example:\n\tcd old/LSL/liblsl\n\t../../backport_to_old_repo.sh ~/src/new abc123\n'
	exit;
fi

set -x

LSLSUBDIR=$(git rev-parse --show-prefix)
OLDREPOPATH=$(git rev-parse --show-toplevel)
NEWREPOPATH=$1
COMMIT=$2
git -C $NEWREPOPATH/$LSLSUBDIR show -p $COMMIT | git apply -3 -p1 --directory=$LSLSUBDIR -v
