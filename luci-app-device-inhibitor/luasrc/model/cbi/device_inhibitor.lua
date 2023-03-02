map = Map("device_inhibitor", "Tuya IoT")

section = map:section(NamedSection, "input_sct", "device_inhibitor", "Input")

flag = section:option(Flag, "enable", "Enable", "Enable program")

productid = section:option(Value, "productid", "Product ID", "This is found at the Tuya IoT website. Product -> (device) -> Device Development -> Activation & Verification.")
productid.datatype = "string"
productid.maxlength = 30

deviceid = section:option(Value, "deviceid", "Device ID", "This is found at the Tuya IoT website. Product -> (device) -> Device Development -> Activation & Verification.")
deviceid.datatype = "string"
deviceid.maxlength = 30

devicesecret = section:option(Value, "devicesecret", "Device Secret", "This is found at the Tuya IoT website. Product -> (device) -> Device Development -> Activation & Verification.")
devicesecret.datatype = "string"
devicesecret.maxlength = 30

return map
