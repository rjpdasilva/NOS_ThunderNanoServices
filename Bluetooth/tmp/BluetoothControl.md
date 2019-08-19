<!-- Generated automatically, DO NOT EDIT! -->
<a name="head.BluetoothControl_Plugin"></a>
# BluetoothControl Plugin

**Version: 1.0**

**Status: :black_circle::black_circle::black_circle:**

BluetoothControl plugin for Thunder framework.

### Table of Contents

- [Introduction](#head.Introduction)
- [Description](#head.Description)
- [Configuration](#head.Configuration)
- [Methods](#head.Methods)
- [Properties](#head.Properties)

<a name="head.Introduction"></a>
# Introduction

<a name="head.Scope"></a>
## Scope

This document describes purpose and functionality of the BluetoothControl plugin. It includes detailed specification of its configuration, methods and properties provided.

<a name="head.Case_Sensitivity"></a>
## Case Sensitivity

All identifiers on the interface described in this document are case-sensitive. Thus, unless stated otherwise, all keywords, entities, properties, relations and actions should be treated as such.

<a name="head.Acronyms,_Abbreviations_and_Terms"></a>
## Acronyms, Abbreviations and Terms

The table below provides and overview of acronyms used in this document and their definitions.

| Acronym | Description |
| :-------- | :-------- |
| <a name="acronym.API">API</a> | Application Programming Interface |
| <a name="acronym.HTTP">HTTP</a> | Hypertext Transfer Protocol |
| <a name="acronym.JSON">JSON</a> | JavaScript Object Notation; a data interchange format |
| <a name="acronym.JSON-RPC">JSON-RPC</a> | A remote procedure call protocol encoded in JSON |

The table below provides and overview of terms and abbreviations used in this document and their definitions.

| Term | Description |
| :-------- | :-------- |
| <a name="term.callsign">callsign</a> | The name given to an instance of a plugin. One plugin can be instantiated multiple times, but each instance the instance name, callsign, must be unique. |

<a name="head.References"></a>
## References

| Ref ID | Description |
| :-------- | :-------- |
| <a name="ref.HTTP">[HTTP](http://www.w3.org/Protocols)</a> | HTTP specification |
| <a name="ref.JSON-RPC">[JSON-RPC](https://www.jsonrpc.org/specification)</a> | JSON-RPC 2.0 specification |
| <a name="ref.JSON">[JSON](http://www.json.org/)</a> | JSON specification |
| <a name="ref.Thunder">[Thunder](https://github.com/WebPlatformForEmbedded/Thunder/blob/master/doc/WPE%20-%20API%20-%20WPEFramework.docx)</a> | Thunder API Reference |

<a name="head.Description"></a>
# Description

BluetoothControl gives you controll over what is displayed on screen.

The plugin is designed to be loaded and executed within the Thunder framework. For more information about the framework refer to [[Thunder](#ref.Thunder)].

<a name="head.Configuration"></a>
# Configuration

The table below lists configuration options of the plugin.

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| callsign | string | Plugin instance name (default: *BluetoothControl*) |
| classname | string | Class name: *BluetoothControl* |
| locator | string | Library name: *libWPEFrameworkBluetoothControl.so* |
| autostart | boolean | Determines if the plugin is to be started automatically along with the framework |

<a name="head.Methods"></a>
# Methods

The following methods are provided by the BluetoothControl plugin:

BluetoothControl interface methods:

| Method | Description |
| :-------- | :-------- |
| [pair](#method.pair) | Pair host with bluetooth device |
| [connect](#method.connect) | Connect with bluetooth device |
| [scan](#method.scan) | Scan environment for bluetooth devices |
| [stopscan](#method.stopscan) | Connect with bluetooth device |
| [unpair](#method.unpair) | Unpair host from a bluetooth device |
| [disconnect](#method.disconnect) | Disconnects host from bluetooth device |

<a name="method.pair"></a>
## *pair <sup>method</sup>*

Pair host with bluetooth device.

### Description

Use this function to request pairing with another bluetooth device. If device accepts pairing request, connection can be established

### Parameters

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| params | object |  |
| params.address | string | Address of the deveice to pair with |

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 22 | ```ERROR_UNKNOWN_KEY``` | Device not found |
| 1 | ```ERROR_GENERAL``` | Failed to pair |
| 4 | ```ERROR_ASYNC_ABORTED``` | Pairing aborted |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.pair", 
    "params": {
        "address": "11::22::33:44:55:66"
    }
}
```
#### Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": null
}
```
<a name="method.connect"></a>
## *connect <sup>method</sup>*

Connect with bluetooth device.

### Description

Use this function to connecto to bluetooth device. Devices must be paired beforehand

### Parameters

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| params | object |  |
| params.address | string | Address of the deveice to pair with |

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 22 | ```ERROR_UNKNOWN_KEY``` | Device not found |
| 1 | ```ERROR_GENERAL``` | Failed to connect |
| 4 | ```ERROR_ASYNC_ABORTED``` | Connecting aborted |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.connect", 
    "params": {
        "address": "11::22::33:44:55:66"
    }
}
```
#### Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": null
}
```
<a name="method.scan"></a>
## *scan <sup>method</sup>*

Scan environment for bluetooth devices.

### Description

Use this function to search for bluetooth devices that are advertising themselfs.

### Parameters

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| params | object |  |
| params?.lowenergy | boolean | <sup>*(optional)*</sup> Perform a scan in low-energy mode |
| params?.limited | boolean | <sup>*(optional)*</sup>  |
| params?.passive | boolean | <sup>*(optional)*</sup> Perform a passive scan if set to true. In passive mode host will only listen to advertisments. It won't send scan requests |
| params?.duration | number | <sup>*(optional)*</sup> Scanning duration in seconds |

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 1 | ```ERROR_GENERAL``` | Scanning failed |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.scan", 
    "params": {
        "lowenergy": true, 
        "limited": false, 
        "passive": false, 
        "duration": 10
    }
}
```
#### Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": null
}
```
<a name="method.stopscan"></a>
## *stopscan <sup>method</sup>*

Connect with bluetooth device.

### Description

Cancels ongoing scanning

### Parameters

This method takes no parameters.

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.stopscan"
}
```
#### Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": null
}
```
<a name="method.unpair"></a>
## *unpair <sup>method</sup>*

Unpair host from a bluetooth device.

### Description

Use this function to invalidate pairing between host and selected device.

### Parameters

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| params | object |  |
| params.address | string | Address of the deveice to pair with |

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 22 | ```ERROR_UNKNOWN_KEY``` | Device not found |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.unpair", 
    "params": {
        "address": "11::22::33:44:55:66"
    }
}
```
#### Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": null
}
```
<a name="method.disconnect"></a>
## *disconnect <sup>method</sup>*

Disconnects host from bluetooth device.

### Description

Use this function to disconnect from 

### Parameters

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| params | object |  |
| params.address | string | Address of the deveice to pair with |
| params?.reason | number | <sup>*(optional)*</sup> Code for reason od disconnection |

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 22 | ```ERROR_UNKNOWN_KEY``` | Device not found |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.disconnect", 
    "params": {
        "address": "11::22::33:44:55:66", 
        "reason": 0
    }
}
```
#### Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": null
}
```
<a name="head.Properties"></a>
# Properties

The following properties are provided by the BluetoothControl plugin:

BluetoothControl interface properties:

| Property | Description |
| :-------- | :-------- |
| [scanning](#property.scanning) <sup>RO</sup> | Tells if host is currently scanning for bluetooth devices |
| [advertising](#property.advertising) | Defines if device is advertising to be visible by other bluetooth devices |
| [device](#property.device) <sup>RO</sup> | Informations about devices found during scanning |

<a name="property.scanning"></a>
## *scanning <sup>property</sup>*

Provides access to the tells if host is currently scanning for bluetooth devices.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | boolean | Tells if host is currently scanning for bluetooth devices |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.scanning"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": false
}
```
<a name="property.advertising"></a>
## *advertising <sup>property</sup>*

Provides access to the defines if device is advertising to be visible by other bluetooth devices.

### Description

Use this property to set or get information whether the device will be visible to other devices

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | boolean | Defines if device is advertising to be visible by other bluetooth devices |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.advertising"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": false
}
```
#### Set Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.advertising", 
    "params": false
}
```
#### Set Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": "null"
}
```
<a name="property.device"></a>
## *device <sup>property</sup>*

Provides access to the informations about devices found during scanning. If index is supplied, only information about that device is returned.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | array | Informations about devices found during scanning. If index is supplied, only information about that device is returned |
| (property)[#] | object |  |
| (property)[#]?.address | string | <sup>*(optional)*</sup> Device address |
| (property)[#]?.name | string | <sup>*(optional)*</sup> Device name |
| (property)[#]?.lowenergy | boolean | <sup>*(optional)*</sup> Describes if device is running in low energy mode (BLE) |
| (property)[#]?.connected | boolean | <sup>*(optional)*</sup> Describes if device is connected to host |
| (property)[#]?.paired | boolean | <sup>*(optional)*</sup> Describes if device is paired with host |
| (property)[#]?.reason | integer | <sup>*(optional)*</sup> Code for reason of last disconnection |

> The *address* shall be passed as the index to the property, e.g. *BluetoothControl.1.device@11::22::33:44:55:66*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 22 | ```ERROR_UNKNOWN_KEY``` | Device not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "BluetoothControl.1.device@11::22::33:44:55:66"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": [
        {
            "address": "11::22::33:44:55:66", 
            "name": "iPhone8", 
            "lowenergy": false, 
            "connected": false, 
            "paired": false, 
            "reason": 0
        }
    ]
}
```
