/* 
 * MQTT-WebClient example for Web-IO 4.0
*/
var hostname = "91.121.93.94";
var port = 8080;
var clientId = "webio4mqttexample";
clientId += new Date().getUTCMilliseconds();;

var subscriptionTemperaturaDomo1 = "hadeesc/Domo1/temperatura";
var subscriptionHumedadDomo1 = "hadeesc/Domo1/humedad";
var subscriptionAireDomo1 = "hadeesc/Domo1/aire";

var subscriptionTemperaturaDomo2 = "hadeesc/Domo2/temperatura";
var subscriptionHumedadDomo2 = "hadeesc/Domo2/humedad";
var subscriptionAireDomo2 = "hadeesc/Domo2/aire";

var subscriptionTemperaturaDomo3 = "hadeesc/Domo3/temperatura";
var subscriptionHumedadDomo3 = "hadeesc/Domo3/humedad";
var subscriptionAireDomo3 = "hadeesc/Domo3/aire";

mqttClient = new Paho.MQTT.Client(hostname, port, clientId);
mqttClient.onMessageArrived =  MessageArrived;
mqttClient.onConnectionLost = ConnectionLost;
Connect();


/*Initiates a connection to the MQTT broker*/
function Connect(){
	mqttClient.connect({
		onSuccess: Connected,
		onFailure: ConnectionFailed,	
	});
}

/*Callback for successful MQTT connection */
function Connected() {
  console.log("Connected");

  mqttClient.subscribe(subscriptionTemperaturaDomo1);
  mqttClient.subscribe(subscriptionHumedadDomo1);
  mqttClient.subscribe(subscriptionAireDomo1);

  mqttClient.subscribe(subscriptionTemperaturaDomo2);
  mqttClient.subscribe(subscriptionHumedadDomo2);
  mqttClient.subscribe(subscriptionAireDomo2);

  mqttClient.subscribe(subscriptionTemperaturaDomo3);
  mqttClient.subscribe(subscriptionHumedadDomo3);
  mqttClient.subscribe(subscriptionAireDomo3);
}

/*Callback for failed connection*/
function ConnectionFailed(res) {
	console.log("Connect failed:" + res.errorMessage);
}

/*Callback for lost connection*/
function ConnectionLost(res) {
	
  if (res.errorCode != 0) {
	console.log("Connection lost:" + res.errorMessage);
	Connect();
  }
}

/*Callback for incoming message processing */
function MessageArrived(message) {
	console.log(message.destinationName +" : " + message.payloadString);
	console.log(message.destinationName);
	var topico = message.destinationName.split("/");
	var variable = topico[2];
	var domo = topico[1];
	displayClass = message.payloadString;
	messagePayLoad = message.payloadString;

	if (topico.length == 3){
		console.log(displayClass);
		console.log("6");
		var ioname = topico[1] + "/" + topico[2];
		UpdateElement(ioname, displayClass);
		console.log("8");
		//insert_message(ioname, messagePayLoad);
	}


}
 
//////////////////////SQL///////////////
var mysql = require('mysql'); //https://www.npmjs.com/package/mysql
//Create Connection
var connection = mysql.createConnection({
	host: localhost,
	user: "root",
	password: "h4d33sc22",
	database: "hadeesc"
});

connection.connect(function(err) {
	if (err) throw err;
	//console.log("Database Connected!");
});

//insert a row into the tbl_messages table
function insert_message(topic, message_str) {

	var sql = "INSERT INTO ?? (??,??,??) VALUES (?,?,?)";
	
	// switcase for tipo de variable y domo
	var CURRENT_TIMESTAMP = Date.now();
		switch (topic) {
	  case 'Domo1/temperatura':
		console.log("8888");
		console.log(message_str);
		console.log(Date.now());
		var sql = "INSERT INTO temperatura_domo_1 (id, fecha, temperatura) VALUES ?";
		var values = [null, CURRENT_TIMESTAMP, message_str];
		//var params = ['temperatura_domo_1', 'id', 'fecha', 'temperatura', null, CURRENT_TIMESTAMP, message_str];
		console.log(params);
		break;
	  case 'Domo1/humedad':
		console.log("7777");
		console.log(message_str);
		var params = ['humedad_domo_1', 'id', 'fecha', 'humedad', null, CURRENT_TIMESTAMP, message_str];
		console.log(params);
		break;
	  case 'Domo1/aire':
		var params = ['aire_domo_1', 'id', 'fecha', 'aire', NULL, CURRENT_TIMESTAMP, message_str];
		break;
	  case 'Domo2/temperatura':
		var params = ['temperatura_domo_2', 'id', 'fecha', 'temperatura', NULL, CURRENT_TIMESTAMP, message_str];
		break;
	  case 'Domo2/humedad':
		var params = ['humedad_domo_2', 'id', 'fecha', 'humedad', NULL, CURRENT_TIMESTAMP, message_str];
		break;
	  case 'Domo2/aire':
		var params = ['aire_domo_2', 'id', 'fecha', 'aire', NULL, CURRENT_TIMESTAMP, message_str];
		break;
	  case 'Domo3/temperatura':
		
		console.log("6666");
		console.log(message_str);
		console.log(params);
		console.log(Date.now());
		var sql = "INSERT INTO temperatura_domo_3 (id, fecha, temperatura) VALUES ?";
		var values = [null, CURRENT_TIMESTAMP, message_str];
		
		//var params = ['temperatura_domo_3', 'id', 'fecha', 'temperatura', null, CURRENT_TIMESTAMP, message_str];
		console.log(params);
		break;
	  case 'Domo3/humedad':
		var params = ['humedad_domo_3', 'id', 'fecha', 'humedad', NULL, CURRENT_TIMESTAMP, message_str];
		break;
	  case 'Domo3/aire':
		var params = ['aire_domo_3', 'id', 'fecha', 'aire', NULL, CURRENT_TIMESTAMP, message_str];
		break;
	  default:
		console.log(`Sorry, we are out of ${topic}.`);
	}
	console.log("--------");
	//sql = mysql.format(sql, params);
	console.log("8888---------");	
	
	connection.query(sql, values, function (error, results) {
		if (error) throw error;
		console.log("Message added: " + message_str);
	}); 
};	

//split a string into an array of substrings
function extract_string(message_str) {
	var message_arr = message_str.split(","); //convert to array	
	return message_arr;
};	

//count number of delimiters in a string
var delimiter = ",";
function countInstances(message_str) {
	var substrings = message_str.split(delimiter);
	return substrings.length - 1;
};




