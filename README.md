# bricks
Network services united ...

** About **

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




#in
