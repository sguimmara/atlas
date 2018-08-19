#!/bin/sh

astyle --style=allman -n --recursive "include/*.hpp" "src/*.cpp" \
                         --suffix=none \
                         --convert-tabs \
                         --indent-cases \
                         --indent-classes \
                         --indent-namespaces \
                         --indent-switches \
                         --indent=spaces \
                         --pad-header \
                         --pad-oper \
                         --unpad-paren
