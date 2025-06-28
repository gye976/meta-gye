IMAGE_INSTALL:append = " qtbase qtwayland cube connman"
IMAGE_INSTALL:append = " wayland weston" 
IMAGE_FSTYPES:append = " wic"

IMAGE_FEATURES:append = " ssh-server-openssh \
                          tools-debug \
                          tools-sdk \
                          dev-pkgs \
                          dbg-pkgs \
                          "


