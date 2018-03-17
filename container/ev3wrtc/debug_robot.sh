gdb-multiarch --tui --quiet --directory=./src \
    --eval-command="target extended-remote $GDBREMOTE"\
    --eval-command="set remote exec-file wrtcrobot"\
    --eval-command="file ./bin/wrtcrobot"
