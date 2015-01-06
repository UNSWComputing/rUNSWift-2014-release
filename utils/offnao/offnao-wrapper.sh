#!/bin/bash
CTC_DIR=@CTC_DIR@
if [[ -n "$CTC_DIR" ]]; then
  export QT_PLUGIN_PATH=$CTC_DIR/sysroot/usr/lib/qt4/plugins/
  export LD_LIBRARY_PATH=$CTC_DIR/sysroot/usr/lib/qt4/
fi
`dirname $0`/offnao.bin "$@"
