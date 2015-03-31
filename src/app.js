var initialised = false;
var current_msg = 0;
var count_val=0;
var results = [];
Pebble.addEventListener("ready", function() {
    initialised = true;
});



function http_request(data,user_id) {
  var req = new XMLHttpRequest();
	var url = 'http://165.124.23.144:3000/pebble_data';
  req.open('POST', url, true);
	var unix = Math.round(+new Date()/1000);
req.setRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
//	console.log("Pebble token : " + Pebble.getAccountToken());
	//console.log('sending id : ' + user_id);
	req.send("id="+ user_id+ "&tstmp="+unix+"&token="+Pebble.getAccountToken() +"&data="+data);
 
}





Pebble.addEventListener('appmessage', function(e) {
	current_msg=e.payload.Count;
	console.log("Appmessage received:"+current_msg);
	if (current_msg == 1)
		console.log("Appmessage received: "+JSON.stringify(e.payload)); // show data just once
	var orgdata = e.payload.Data;
	var user_id = e.payload.PERSIST_KEY_LAUNCHES;
	
	var res = [];
	var s=0;
	for(var i = 0; i < orgdata.length; i += 2) {
		s++;
		
		var combined = ((orgdata[i+1] & 0xFF) << 8) | (orgdata[i] & 0xFF);
		if(combined >= 0x8000) 
			combined -= 0x10000;
		res.push(combined);
		
		
		if(s==3){
		var point = { x: res[0], y: res[1],z:res[2] };
			results.push(point);
			res =[];
			s=0;
			}
		
		
	}
		
	
	//console.log('results :' + JSON.stringify(results));
	if(count_val == 7){
	
	http_request(JSON.stringify(results),user_id);
	results=[];
	count_val=0;	
	}
		
	count_val++;

	/*
	var lost_msg = current_msg -1 - last_msg;
	Total_lost = Total_lost + lost_msg;
	if (lost_msg > 0)
		console.log("lost(now/total): "+ lost_msg+"/"+Total_lost+" recived/Previus: "+current_msg+"/"+ last_msg );
	last_msg = current_msg;  */
});