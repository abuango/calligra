#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=KarbonFilterEffects.pot

source ../../kundo2_aware_xgettext.sh

kundo2_aware_xgettext $potfilename `find . -name \*.cpp -o -name \*.h`
