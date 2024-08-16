
for dir in arduino-ide-board-package/libraries/Fri3dBadge2024/examples/*/; do
  echo "copying Fri3dBadge_pins.h to '$dir'"
  cp arduino-ide-board-package/variants/fri3d_2024_esp32s3/Fri3dBadge_pins.h "$dir"
done
