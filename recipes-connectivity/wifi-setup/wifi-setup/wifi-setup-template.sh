#!/bin/bash

export SSID="__SSID__"
export PASS="__PASS__"
export IFACE=$(iw dev | awk '/Interface/ {print $2; exit}')
export CONF="/etc/wpa_supplicant/wpa_supplicant-${IFACE}.conf"

if [ -z "${SSID}" ]; then exit 1; fi 
if [ -z "${PASS}" ]; then exit 1; fi 
if [ -z "${IFACE}" ]; then exit 1; fi 

cat > ${CONF} <<str
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1

str

wpa_passphrase "${SSID}" "${PASS}" >> ${CONF}

cat > /etc/systemd/network/25-wlan.network <<str
[Match]
Name=${IFACE}

[Network]
DHCP=ipv4
str

rfkill unblock wifi

systemctl enable wpa_supplicant@${IFACE}.service

systemctl restart systemd-networkd.service
systemctl restart wpa_supplicant@${IFACE}.service

