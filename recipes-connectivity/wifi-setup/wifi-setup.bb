inherit systemd

LICENSE = "CLOSED"

RDEPENDS:${PN}:append = "bash wpa-supplicant busybox-ptest"

WIFI_SSID ?= "GYE-m"
WIFI_PASSWORD ?= "rpdudqor6412!"

SRC_URI = "file://wifi-setup-template.sh \
           file://wifi-setup.service \
           "

S = "${WORKDIR}"

do_install() {
        install -d ${D}${bindir}
        install -d ${D}${sysconfdir}/wpa_supplicant

        sed -e "s/__SSID__/${WIFI_SSID}/g" -e "s/__PASS__/${WIFI_PASSWORD}/g" ${S}/wifi-setup-template.sh > \
                ${D}${bindir}/wifi-setup.sh
        chmod 0744 ${D}${bindir}/wifi-setup.sh

        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${S}/wifi-setup.service ${D}${systemd_system_unitdir}/
}

SYSTEMD_SERVICE:${PN} = "wifi-setup.service"
