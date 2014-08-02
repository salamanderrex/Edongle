// Bind event
var runtimeOrExtension = chrome.runtime && chrome.runtime.sendMessage ?
                         'runtime' : 'extension';
 var websocket;
 connect_webSocket();
$( document ).ready(function() {

	
	$("#save").click(function() {
		$("#warning")[0].innerHTML = "";
		$("#success")[0].innerHTML = "";
		if(!$("#username")[0].value || !$("#password")[0].value){
			$("#warning")[0].innerHTML = "Please input Sakai username & password!"
		} else {
			// Test validity of username & psw
			//var login_url = "http://sakai.umji.sjtu.edu.cn/portal/relogin";
			// Show wait.gif
			$("#wait-gif").css("display", "inline");
			send();
			//$.post(login_url, { eid: $("#username")[0].value, pw: $("#password")[0].value})
			.done(function(data) {
				if (data.indexOf('action="http://sakai.umji.sjtu.edu.cn/portal/relogin"') == -1){
					// Success, save it to local storage
					localStorage.setItem("username", $("#username")[0].value);
					localStorage.setItem("password", $("#password")[0].value);
					localStorage.setItem("checkInterval", $("#refreshInterval")[0].value);
					// localStorage.setItem("ifNotify", $("#showNotifications")[0].value);
					$("#wait-gif").css("display", "none");
					$("#success")[0].innerHTML = "Succeed! You can close this page now."
					chrome[runtimeOrExtension].sendMessage({message: "checkNew"}, function(response) {
						// Get response
					});
				} else{
					$("#wait-gif").css("display", "none");
					$("#warning")[0].innerHTML = "Login failed!"
				}
			});
		}
	});
});




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


	//window.open('http://sakai.umji.sjtu.edu.cn/');
}

 
function onClose(){
    alert('connect close')
    has_login=0;
}
function send(){
    websocket.send('jiehaoser');
}
function close(){
    websocket.close();
   
}


