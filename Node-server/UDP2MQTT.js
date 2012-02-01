var sys = require('sys');
var net = require('net');
var mqtt = require('./mqtt');
var dgram = require("dgram");

var server = dgram.createSocket("udp4");
var client = new mqtt.MQTTClient(1883, '10.254.0.1', 'mirror'); 
    
client.addListener('sessionOpened', function(){
    sys.puts('sub to node mirror');
    client.subscribe('/mirror');
    client.publish('node', 'here is nodejs');
    
});

client.addListener('mqttData', function(topic, payload){
    sys.puts(topic+':'+payload);
        
});

server.on("message", function (msg, rinfo) {
  var buff = new Buffer(msg,'hex')
  
  reader_id = buff.toString('hex', start=4,end=6)
  timestamp = buff.toString('hex', start=12,end=16)
  proto = buff.toString('hex', start=16,end=17)
  oid = buff.toString('hex', start=17,end=19)
  flags = buff.toString('hex', start=19,end=20)
  strength = buff.toString('hex', start=20,end=21)
  oid_last_seen = buff.toString('hex', start=21,end=23)
  power_up_count = buff.toString('hex', start=23,end=25)
  reserved = buff.toString('hex', start=25,end=26)
  seq = buff.toString('hex', start=26,end=30)
  
  payload = '{"reader_id": "' + reader_id + '", "timestamp": "' + timestamp + 
  '", "proto": "' + proto + '", "oid": "' + oid + '", "flags": "' + flags + 
  '", "strength": "' + strength + '", "oid_last_seen": "' + oid_last_seen +
  '", "power_up_count": "' + power_up_count + '", "reserved": "' + reserved +
  '", "seq": "' + seq + '"}';
  client.publish('node',payload);
});

server.on("listening", function () {
  var address = server.address();
  console.log("server listening " +
      address.address + ":" + address.port);
});

server.bind(2342);
