module("luci.controller.device_inhibitor_controller", package.seeall)

function index()
	entry({"admin", "services", "tuya-iot"}, cbi("device_inhibitor"), _("Tuya IoT"),105)
end
