require recipes-core/images/rpi-test-image.bb

IMAGE_INSTALL:append = " qtbase qtwayland cube connman"
DISTRO_FEATURES:append = " wayland"
CORE_IMAGE_EXTRA_INSTALL:append = " wayland weston" 

IMAGE_FSTYPES:append = " wic"

IMAGE_FEATURES:append = " ssh-server-openssh \
                          tools-sdk \
                          dev-pkgs \
                          dbg-pkgs \
                          "


