# bricks
Network services united ...

## About 

The goal of the brciks project is to abstract the microservices communication patterns behind generic interface where actual networking is implemented by the means of plugin. Using abstract API enables developer to replace the actual communication engine at any time at relatily low cost. But mostly important the whole networking service can be replaced with mock objects elimiatng actual netwokring layers which imporves drastically the testability of your services.

Current supported communication paradigms are :-
- Publish/Subscribe (aka producer/consumer).
- P2P (aka bidi, aka pair) 
- Request Response 

So bricks are interfaces + implementations. 

Following table summarizes currently available implemenation


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
- Modern OOP (prefer function object callbacks to opaque objects)
- Be as slim as possible (avoid managing maps, or any other persistent data-scructures by plugin implementors)
- Avoid introducing interface which is peculiar to certin implementation.
  
### Memory Handling

All brick object are created via factory objects and released via brick_t::release interface. This is to ensure that plugin implemntors can choose the allocation and deallocation mechanism of their own, similar to MS COM interfaces.

### xtree 
All specific communication engine peculiarities are handled by xtree data structures. Xtree is conceptually an XML in OOP, e.g. tree like data structures where nodes can be accessed by string path, have different propertis of predefined set of types  and can hold a buffer as a value. It is not bound to XML in any case in a way other than concept, but the only serializinf and de-0serilizing logic currently implmented in XML. It can be extended to any other format of preference.

Take for example the interface of plugin implementing the publish/subscribe interface.

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

The flexibility of the engine introduces the challenge of consistent thread model. So replaceing Kafka engine with ZeromMQ will not change the threading architecture of the your program. The solution that was chosen was to communciate with the engine via abstract queue of callbacks. If you would like to receive callback as result of topic subscription you need to supply queue when intiiating the plugin and the "pump" the queue fro the chosen thread. The callbacks in this case are promised to be called from the the same thread that pumping the queue. To pump the queue the helper class slector is provided, so the code should basically look that. 

```
while (!shutdown)
{
	selector->poll(); // callback will be called here
}
```

In general if queue is not supplied to plugin it will call callback directly from one of the implementation threads which basically means unprdicatbel threading model and not what was intended.

#in
