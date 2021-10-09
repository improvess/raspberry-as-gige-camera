#!/bin/bash
rmmod uvcvideo
modprobe uvcvideo nodrop=1 timeout=5000 quirks=0x80