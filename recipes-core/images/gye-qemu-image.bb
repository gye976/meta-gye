require recipes-graphics/images/core-image-weston.bb
COMPATIBLE_MACHINE = "^qemu"

IMAGE_INSTALL:append = " qtbase qtwayland cube"
IMAGE_INSTALL:append = " wayland weston" 

IMAGE_FEATURES:append = " \
                          tools-debug \
                          dbg-pkgs \
                          "


