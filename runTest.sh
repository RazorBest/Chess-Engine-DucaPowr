#!/bin/bash
cd src
xboard -variant 3check -fcp "make run" -scp "pulsar2009-9b-64 mxT-2" -tc 5 -inc 2 -autoCallFlag true -mg 4 -sgf partide.txt -reuseFirst false 