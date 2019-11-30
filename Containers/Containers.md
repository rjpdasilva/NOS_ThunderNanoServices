<!-- Generated automatically, DO NOT EDIT! -->
<a name="head.Process_Containers_Plugin"></a>
# Process Containers Plugin

**Version: 1.0**

**Status: :white_circle::white_circle::white_circle:**

Containers plugin for Thunder framework.

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

This document describes purpose and functionality of the Containers plugin. It includes detailed specification of its configuration, methods and properties provided.

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

The Containers plugin provides informations about process containers running on system.

The plugin is designed to be loaded and executed within the Thunder framework. For more information about the framework refer to [[Thunder](#ref.Thunder)].

<a name="head.Configuration"></a>
# Configuration

The table below lists configuration options of the plugin.

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| callsign | string | Plugin instance name (default: *Containers*) |
| classname | string | Class name: *Containers* |
| locator | string | Library name: *libWPEContainers.so* |
| autostart | boolean | Determines if the plugin is to be started automatically along with the framework |

<a name="head.Methods"></a>
# Methods

The following methods are provided by the Containers plugin:

Containers interface methods:

| Method | Description |
| :-------- | :-------- |
| [stop](#method.stop) | Stops a container |

<a name="method.stop"></a>
## *stop <sup>method</sup>*

Stops a container.

### Parameters

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| params | object |  |
| params.name | string | Name of container |

### Result

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| result | null | Always null |

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.stop", 
    "params": {
        "name": "ContainerName"
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

The following properties are provided by the Containers plugin:

Containers interface properties:

| Property | Description |
| :-------- | :-------- |
| [containers](#property.containers) <sup>RO</sup> | List of active containers |
| [networks](#property.networks) <sup>RO</sup> | List of network interfaces of the container |
| [memory](#property.memory) <sup>RO</sup> | Memory taken by container |
| [status](#property.status) <sup>RO</sup> | Operational status of the container |
| [cpu](#property.cpu) <sup>RO</sup> | CPU time |
| [logpath](#property.logpath) <sup>RO</sup> | Containers log |
| [configpath](#property.configpath) <sup>RO</sup> | Container's configuration |

<a name="property.containers"></a>
## *containers <sup>property</sup>*

Provides access to the list of active containers.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | array | List of names of all containers |
| (property)[#] | string |  |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.containers"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": [
        "ContainerName"
    ]
}
```
<a name="property.networks"></a>
## *networks <sup>property</sup>*

Provides access to the list of network interfaces of the container.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | array | List of all network interfaces related to the container |
| (property)[#] | object | Returns networks associated with the container |
| (property)[#]?.interface | string | <sup>*(optional)*</sup> Interface name |
| (property)[#]?.ips | array | <sup>*(optional)*</sup> List of ip addresses |
| (property)[#]?.ips[#] | string | <sup>*(optional)*</sup> IP address |

> The *name* shall be passed as the index to the property, e.g. *Containers.1.networks@ContainerName*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.networks@ContainerName"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": [
        {
            "interface": "veth3NF06K", 
            "ips": [
                "192.168.0.12"
            ]
        }
    ]
}
```
<a name="property.memory"></a>
## *memory <sup>property</sup>*

Provides access to the memory taken by container.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | object | Memory allocated by the container, in bytes |
| (property)?.allocated | number | <sup>*(optional)*</sup> Memory allocated by container |
| (property)?.resident | number | <sup>*(optional)*</sup> Resident memory of the container |
| (property)?.shared | number | <sup>*(optional)*</sup> Shared memory in the container |

> The *name* shall be passed as the index to the property, e.g. *Containers.1.memory@ContainerName*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.memory@ContainerName"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": {
        "allocated": 0, 
        "resident": 0, 
        "shared": 0
    }
}
```
<a name="property.status"></a>
## *status <sup>property</sup>*

Provides access to the operational status of the container.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | string | Status of the container (must be one of the following: *stopped*, *starting*, *running*, *aborting*, *stopping*) |

> The *name* shall be passed as the index to the property, e.g. *Containers.1.status@ContainerName*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.status@ContainerName"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": "stopped"
}
```
<a name="property.cpu"></a>
## *cpu <sup>property</sup>*

Provides access to the CPU time.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | object | CPU time spent on running the container, in nanoseconds |
| (property)?.total | number | <sup>*(optional)*</sup> CPU-time spent on container, in nanoseconds |
| (property)?.threads | array | <sup>*(optional)*</sup> Time spent on each cpu-thread, in nanoseconds |
| (property)?.threads[#] | number | <sup>*(optional)*</sup>  |

> The *name* shall be passed as the index to the property, e.g. *Containers.1.cpu@ContainerName*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.cpu@ContainerName"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": {
        "total": 2871287421, 
        "threads": [
            12314235
        ]
    }
}
```
<a name="property.logpath"></a>
## *logpath <sup>property</sup>*

Provides access to the containers log.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | string | logs from the container |

> The *name* shall be passed as the index to the property, e.g. *Containers.1.logpath@ContainerName*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.logpath@ContainerName"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": ""
}
```
<a name="property.configpath"></a>
## *configpath <sup>property</sup>*

Provides access to the container's configuration.

> This property is **read-only**.

### Value

| Name | Type | Description |
| :-------- | :-------- | :-------- |
| (property) | string | configuraiton of the container |

> The *name* shall be passed as the index to the property, e.g. *Containers.1.configpath@ContainerName*.

### Errors

| Code | Message | Description |
| :-------- | :-------- | :-------- |
| 2 | ```ERROR_UNAVAILABLE``` | Container not found |

### Example

#### Get Request

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "method": "Containers.1.configpath@ContainerName"
}
```
#### Get Response

```json
{
    "jsonrpc": "2.0", 
    "id": 1234567890, 
    "result": ""
}
```
