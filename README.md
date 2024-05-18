
<div align="center">
<img alt="bricks logo" src="https://github.com/borisu/bricks/assets/7937935/63ad0a60-5d7b-40ff-89bf-748ba04bd2c7" width="400px"/>
</div>

# bricks

Set of C++ abstract interfaces and pluggable implementations for network services patterns.

❗The project is in early stages under active development not stable release is available and interfaces is under constant change. Help if you want. ❗

## About 

The goal of the brciks project is to abstract the microservices communication patterns. By ssing abstract interface which hides the actual network provider, developer is enabled to replace the actual communication engine at any time, use several engines in parallel in a uniform manner, avoid vendor lock-in, and mostly important replace networking services by mock objects by thus eliminating actual networking layers which improves drastically the testability of your services.

Current supported communication paradigms are :-
- Publish/Subscribe (aka producer/consumer).
- P2P (aka bidi, aka pair) 
- Request Response 

So bricks are mostly interfaces + POC implementations. 

Following table summarizes currently available plugins and their capabilities :-

<table>
  <tr>
    <th>Plugin</th>
    <th>Publish-Subscribe</th>
    <th>Request-Response</th>
    <th>Bidi</th>
  </tr>
  <tr>
    <td>Kafka Plugin</td>
    <td>V</td>
    <td></td>
    <td></td>
  </tr>
    <td>Oatpp Plugin</td>
    <td></td>
    <td>V</td>
    <td></td>
  </tr>
  </tr>
    <td>ZeroMQ Plugin</td>
    <td>V</td>
    <td></td>
    <td>V</td>
  </tr>
  </tr>
    <td>Mock(Jester( Plugin</td>
    <td>V</td>
    <td>V</td>
    <td>V</td>
  </tr>
</table>

## Plugin Interface Design 

Interface of the plugins was designed around following principles :-
- Modern OOP (prefer function object callbacks to opaque handles)
- Be as slim as possible (avoid managing maps, or any other persistent data-structures by plugin implementors)
- Avoid introducing interface which is peculiar to certain implementation (partitions which are present in Kafka but not so in ZeroMQ).
  
### Memory Handling

All brick object are created with the help of factory methods and released via brick_t::release interface. This is to ensure, that plugin implemntors can choose the allocation and deallocation mechanism of their own, similar to MS COM interfaces.

### XTree 
All specific communication engine peculiarities are handled by xtree data structures which passesd as optional object to most of the interface methods. Xtree is conceptually an XML in OOP, e.g. tree like data structures where nodes can be accessed by string path, have set of properties of predefined list of types and can hold a single char buffer as a value (text in XML). XTree is not bound to XML in a way other than concept, but naturtally the only serializion and de-serialization logic of XTree is implemented in XML. It theoretcially can be extended to any other format of preference.

An example of creating xtree directly from XML and getting its propertiy :-

```
brick_uptr<xtree_t> s_xt(
	create_xtree_from_xml(
		"<configuration>"
		" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
		" <property name = \"group.id\" value=\"mygroup\"/>"
		" <property name = \"auto.offset.reset\" value=\"latest\"/>"
		"</configuration>"
	));

auto value = s_xt->get_property_value_as_string(xp_t("/configuration", 0), "value").value();
```
### Threading Model 

The flexibility of the engines introduces the challenge of consistent thread model. In usch way that for example replacing Kafka engine with ZeromMQ engine will not change the threading architecture of your program. The solution is to communciate with the engine via abstract queue of simple callbacks. If you would like to receive callback as result of topic subscription for example, you have to supply queue object when intiiating the plugin and then "pump" the queue from the chosen thread. The callbacks in this case are promised to be called from the the same thread that is pumping the queue. Class selector is provided to automate the task of waiting on queue and invoking the callbacks :-

```
while (!shutdown)
{
	selector->poll(); // callback will be called here
}
```

In general if queue is not supplied to plugin, it will invoke callback directly from one of the implementation threads which basically means unpredicatble threading model and not what was intended.

### Plugins 

The example of using kafka plugin to publish topic should be self explicable. Paty attention to following important points:-

- It does not use selector because the plugin does not supply any delivery feedback. Indeed rdkafka which was used to implement the plugin has umerous callbacks including delivery confirmation, but at this time it is not implemented and also plugin design is built around minimal common denomiator of capabilities between different engines. For example in ZeroMq getting delivery feedback is not trivial so it cannot be mandatory for implementation.
- All specific implementation details are encapulated in xtree object. Each plugin should document the format of xtree it expects to receive.
- After plugin is created it should be minimally usable without speicifc xtree parameters.  

```
brick_uptr<xtree_t> publisher_xt(
	create_xtree_from_xml(
		"<bricks>"
		" <rdkafka>"
		"  <rdlog enabled=\"true\"/>"
		"  <producer name=\"producer1\">"
		"   <configuration>"
		"    <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
		"   </configuration>"
		"  </producer>"
		" </rdkafka>"
		"</bricks>"
	));
brick_uptr<publisher_plugin_t>  publisher(create_kafka_publisher());
publisher->init(selector->queue(),publisher_xt);
publisher->start()
publisher->register_topic("bricks.test.topic");
publisher->publish("bricks.test.topic", msg, strlen(msg))
```
For more examples of usage see bricks_ut project.

## How To Build

bricks uses vcpkg for most of the 3rd party packages. 

Follow the same proces on Windows and Linux. For Window you should either run cmake --build or use generated solution instead of make.

1. Ensure that cmake, developement tools, and git are available for Linux. For Windows, the build was tested with Visual Studio 2022 installed. Please be sure to install the gtest adapter extension to run bricks tests

2. Install vcpkg
   
Please follow the [link](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd) 
Ensure that you have VCPKG_ROOT variable set to the vcpkg installation directory and vcpkg is found on path
```
export VCPKG_ROOT=<vcpkg root>
```

3. Clone the bricks directory and init all submodules
```
git clone https://github.com/borisu/bricks.git
cd bricks
git submodule update --init --recursive
```

4. Build oatpp (regretfully the latest version is not in vcpkg repository yet)
```
cd <bricks>/3rdparty/modules/oatpp
mkdir build;cd build;
cmake .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON
make
```
5. Build bricks
```
cd <bricks>/src
mkdir build;cd build;cmake ..;make
```

6. Link your program to binary products

7. Enjoy :heart:
