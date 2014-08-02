
(function() {
  	var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
  	ga.src = 'https://ssl.google-analytics.com/ga.js';
  	var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
})();

//global variable
var websocket;
var receive_msg;
connect_webSocket();

var has_login=0;

function getDomainFromUrl(url){
     var host = "null";
     if(typeof url == "undefined" || null == url)
          url = window.location.href;
     var regex = /.*\:\/\/([^\/]*).*/;
     var match = url.match(regex);
     if(typeof match != "undefined" && null != match)
          host = match[1];
     return host;
}

function checkForValidUrl(tabId, changeInfo, tab) {
	console.log("has_login"+has_login);
	console.log("current web is "+getDomainFromUrl(tab.url).toLowerCase());
     if(getDomainFromUrl(tab.url).toLowerCase()=="sakai.umji.sjtu.edu.cn" && has_login==0) {
         //chrome.pageAction.show(tabId);
          console.log(new Date().getTime() + ": " + "you enter URL the sakai");
			chrome.browserAction.setBadgeBackgroundColor({color: "#000"});
			chrome.browserAction.setBadgeText({text: "O"});

			//of send
			send();
     }
};



chrome.tabs.onUpdated.addListener(checkForValidUrl);
// Bind event
var runtimeOrExtension = chrome.runtime && chrome.runtime.sendMessage ?
                         'runtime' : 'extension';

console.log(new Date().getTime() + ": " + "dongle start");

//checkConnection();
/*

function checkConnection(){
	// http://sakai.umji.sjtu.edu.cn/library/skin/neo-default/images/logo_inst.gif

	var Url = 'http://sakai.umji.sjtu.edu.cn/'
	console.log(new Date().getTime() + ": " + "Start check Connection");
	xmlHttp = new XMLHttpRequest(); 
    xmlHttp.onreadystatechange = ProcessRequest;
    xmlHttp.open( "GET", Url, true );
    xmlHttp.send( null );

    function ProcessRequest() 
	{
		console.log(new Date().getTime() + ": " + "Process Request");
		console.log(new Date().getTime() + ": " + "Ready State: " + xmlHttp.readyState);
		console.log(new Date().getTime() + ": " + "Status: " + xmlHttp.status);
		if (xmlHttp.readyState == 4 && xmlHttp.status == 200){
			console.log(new Date().getTime() + ": " + "Connection Success");
		//	checkNew();
		} else if (xmlHttp.readyState == 4 && xmlHttp.status == 0){
			console.log(new Date().getTime() + ": " + "Connection Fail");
			chrome.browserAction.setBadgeBackgroundColor({color: "#000"});
			chrome.browserAction.setBadgeText({text: "X"});
		}
	}
}
*/

function openSakai(){
	
	send();
	//var login_url = "http://sakai.umji.sjtu.edu.cn/portal/relogin";
	//	$.post(login_url, { eid: "5113709257", pw: "ZQUREX41PSP"})
//	while(receive_msg!=null || receive_msg==undefined);

	//	console.log("receive"+receive_msg);
	
	//window.open('http://sakai.umji.sjtu.edu.cn/','_blank');
}

chrome.browserAction.onClicked.addListener(openSakai);


function connect_webSocket(){
    try{
           var wsServer = 'ws://localhost:4567/';

 websocket = new WebSocket(wsServer);
    }catch(e){
        alert('error');
        return;
    }


websocket.binaryType = "arraybuffer";

websocket.onopen = onOpen;

websocket.onclose = onClose;

websocket.onmessage = onMessage;

websocket.onerror = onError;
 
}
function onOpen(){
  console.log("connect the web socket server succeed");
}
function onError(){
    alert('connect error')
}


function onMessage(evt) {

//var json = JSON.parse(evt.data);
console.log("onMessage handler"+evt.data);
receive_msg=evt.data;


//analysis data
var json=JSON.parse(evt.data);

var type=json.type;
var user;
var pw;
var web;
//7 is the lgoin info
if(type==7)
{
user=json.parameters[0].user;
pw=json.parameters[0].pw;
web=json.parameters[0].web;
}

else
{

}
	console.log("try loggiing ");

	var login_url = "http://sakai.umji.sjtu.edu.cn/portal/relogin";
	$.post(login_url, { eid: user, pw: pw})
	
	.done (function (){has_login=1;window.open('http://sakai.umji.sjtu.edu.cn/','_blank'); })
	
	
	//window.open('http://sakai.umji.sjtu.edu.cn/');
}

 
function onClose(){
    alert('connect close')
    has_login=0;
}
function send(){
    websocket.send('hello server ~ I am browser');
}
function close(){
    websocket.close();
    has_login=0;
}


