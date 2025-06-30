include recipes-core/images/rpi-test-image.bb
COMPATIBLE_MACHINE = "^rpi$"

LICENSE ?= "CLOSED"

IMAGE_INSTALL:append = " qtbase cube-imu wifi-setup"

IMAGE_FSTYPES:append = " wic"

IMAGE_FEATURES:append = " ssh-server-openssh \
                          tools-debug \
                          dbg-pkgs \
                          "


