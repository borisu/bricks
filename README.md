
<div align="center">
<img alt="bricks logo" src="https://github.com/borisu/bricks/assets/7937935/63ad0a60-5d7b-40ff-89bf-748ba04bd2c7" width="400px"/>
</div>

# BRICKS

Set of C++ abstract interfaces and pluggable implementations for microservices network services patterns.

❗The project is in early stages under active development. Help, if you think it is valuable.❗



## About 

The goal of the bricks project is to suggest API framework and POC implementations for the microservices communication patterns. By using abstract interface which hides the actual provider, developer is enabled to replace the communication engine at any time, use several engines in parallel in a uniform manner, avoid vendor lock-in, and mostly important replace networking services by mock objects, which improves drastically the testability of your C++ network services applications.

Current supported communication paradigms are :-
- Publish/Subscribe (aka producer/consumer).
- P2P (a.k.a Bidi, a.k.a Pair) 
- Request/Response 

One intresting directions that abstraction is taking us is implementation of service on top of other services for example we can create request/response communication pattern on top of abstract publish/subscribe. That what is called meta plugin. Currently only one such plugin implemented pubsub_client and pubsub_server, both are tested with redis. 

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
    <td>libevent HTTP Plugin</td>
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
    <td>Redis Plugin</td>
    <td>V</td>
    <td>V(Meta)</td>
    <td></td>
  </tr>
   </tr>
    <td>Rabbit MQ Plugin</td>
    <td>V</td>
    <td></td>
    <td></td>
  </tr>

  </tr>
    <td>Mock(Jester( Plugin</td>
    <td>V</td>
    <td>V</td>
    <td>V</td>
  </tr>
</table>

## Example of Redis Sub Plugin 
```cpp
/* create configuration for plugin */
auto xt = create_xtree_from_xml(
	"<bricks>"
	" <redispp>"
	"  <subscriber name=\"redispp_server_subscriber\" >"
	"   <methods>"
	"    <init>"
	"     <connection url=\"tcp://127.0.0.1:6379\" />"
	"    </init>"
	"   </methods>"
	"  </subscriber>"
	" </redispp>"
	"</bricks>"
);
auto subscriber = create_redispp_subscriber(); 

/* !!! from here the code is generic and should not change for any other plugin !!! */
/* create callback queue selector and start poller thread */
auto cb_q = create_callback_queue();
auto selector = create_selector();
selector->init(cb_q);
auto poller = create_poller(10000, selector);

/* create callback for handling topic */
auto on_topic_cb = [&](const string& topic, buffer_t* buf, xtree_t* xt)
	{

		printf("Received msg on topic %s\n", topic.c_str());
		/* do something on topic update */
	};


subscriber->init(selector->queue(), on_topic_cb, xt);
auto rc = subscriber->subscribe("some.interesting.topic", xt);

/* proceed to application logic and don't forget to release memory */
```

## Plugin Interface Design 

Interface of the plugins was designed around following principles :-
- Modern OOP (prefer function object callbacks to opaque handles)
- Be as slim as possible (avoid managing maps, or any other persistent data-structures)
- Avoid introducing interface which is peculiar to certain implementation.
  
### Memory Handling

All brick object are created with the help of factory methods and released via brick_t::release interface. This is to ensure, that plugin implementors can choose the memory handling mechanisms of their own, similar to MS COM interfaces design.

### XTree 
All specific communication engine peculiarities are handled by xtree data structures which is passed as optional object to most of the interface methods. Xtree is conceptually an XML or alternatively hierarchical FS in OOP, e.g. tree like data structures where nodes can be accessed by string path, have set of properties of predefined list of types and can hold a single large char buffer as a value. XTree is not bound to XML in a way other than concept.

### Threading Model And Safety

The flexibility of the engines introduces the challenge of consistent thread model. In such way, that replacing Kafka engine with ZeromMQ engine will not change the threading architecture of your program. The solution is to communciate with the engine via abstract queue of simple callbacks. To receive callback as result of topic subscription for example, one has to supply queue object when initiating the plugin and then "pump" the queue from the chosen thread. The callbacks in this case are promised to be called from the the same thread that is pumping the queue. Classes poller and selector are  provided to automate the task of waiting on queue and invoking the callbacks. :-

```cpp
void
poller_impl_t::run()
{
	while (!shutdown)
	{
		selector->poll(milliseconds);
	}
}
```
All public plugin methods are thread safe

### Plugins 

Best way to learn about plugin usage is looking at their usage in bricks_ut project.

## How To Build

bricks uses vcpkg for the 3rd party packages. 

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

4. Build bricks
```
cd <bricks>/src
mkdir build;cd build;cmake ..;make
```

5. Link your program to binary products

6. Enjoy :heart:
