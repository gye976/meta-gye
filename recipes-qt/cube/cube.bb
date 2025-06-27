LICENSE = "CLOSED"

DEPENDS:append = " qtbase wayland"

SRC_URI = "file://sources/ \ 
          "

S = "${WORKDIR}/sources"

do_install:append () {
        install -d ${D}${bindir}
        install -m 0775 cube ${D}${bindir}
}

FILES_${PN}:append = "${bindir}/cube"

inherit qt6-qmake
