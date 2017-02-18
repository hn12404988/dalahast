function ws(url_array){
	this.url = [];
	this.com = [];
	this.callback_list = {};
	this.already_json = {};
	this.data = [];
	this.server = [];
	this.max_msg = 5;
	this.msg = 0;
	if(url_array.length==0){
		throw new Error("No url supplied");
	}
	for(var i=0;i<url_array.length;i++){
		this.url.push(url_array[i]);
		if(url_array[i]=="0" || url_array[i]==""){
			this.com.push("0");
		}
		else{
			this.com.push(new WebSocket(url_array[i]));
			this.com[i].addEventListener("message",function(event){
				var text = event.data;
				//console.log("ws: "+text);
				ws.msg--;
				if(ws.data.length>0){
					while(ws.msg<ws.max_msg){
						ws.com[ws.server[0]].send(ws.data[0]);
						ws.msg++;
						ws.data.splice(0,1);
						ws.server.splice(0,1);
						if(ws.data.length==0 || ws.server.length==0){
							ws.data = [];
							ws.server = [];
							break;
						}
					}
				}
				var i = parseInt(text[text.length-1]);
				text = text.substr(0,text.length-1);
				var obj;
				if(text[0]=='0'){
					console.log("ERROR in ws");
					text = text.substr(1);
					console.log(text);
					obj = false;
				}
				else{
					if(text=="1"){
						obj = true;
					}
					else{
						try{
							if(text[0]=='['){
								text = text.substr(1);
								text = text.substr(0,text.length-1);
								obj = text.split(",");
							}
							else if(text[0]=='{'){
								obj = JSON.parse(text);
							}
							else{
								obj = false;
							}
						}
						catch(err){
							console.log(err);
							obj = false;
							if(i>0 && i<10){
								if(ws.callback_list[i]){
									ws.callback_list[i](obj);
									delete ws.callback_list[i];
								}
							}
						}
					}
				}
				if(i>0 && i<10){
					if(ws.callback_list[i]){
						ws.callback_list[i](obj);
						delete ws.callback_list[i];
					}
				}
			},false);
			this.com[i].addEventListener("open",function(event){
				console.log("websocket open again");
				if(ws.data.length>0){
					while(ws.msg<ws.max_msg){
						ws.com[ws.server[0]].send(ws.data[0]);
						ws.msg++;
						ws.data.splice(0,1);
						ws.server.splice(0,1);
						if(ws.data.length==0 || ws.server.length==0){
							ws.data = [];
							ws.server = [];
							break;
						}
					}
				}
			},false);
			this.com[i].addEventListener("error",function(event){
				alert("No WebSocket Connection.");
			},false);
		}
	}
}

ws.prototype.check = function(i){
	if(this.com[i].readyState==1){
		return true;
	}
	else if(this.com[i].readyState==0){
		return false;
	}
	else if(this.com[i].readyState>1){
		this.com[i] = new WebSocket(this.url[i]);
		return false;
	}
}

ws.prototype.send = function(server,node,body,callback){
	/*
	  console.log("WS:");
	  console.log("location:"+location);
	  console.log(body);
	*/
	var send = "";
	this.already_json = {};
	if(typeof(body)=="object"){
		for(var ele in body){
			if(body[ele]!=undefined)
				body[ele] = body[ele].toString();
			if(body[ele][0]=="{" || body[ele][0]=="["){
				this.already_json[ele] = body[ele];
				delete body[ele];
			}
		}
		send = JSON.stringify(body);
		if(Object.keys(this.already_json).length>0){
			send = send.substr(0,send.length-1);
			for(var ele in this.already_json){
				send = send+",\""+ele+"\":"+this.already_json[ele];
			}
			send = send+"}";
		}
	}
	else{
		throw new Error("parameter 3 in ws.send is not object");
	}
	send = node+".socket"+send;
	if(callback){
		for(var index = 1;index<10;index++){
			if(this.callback_list[index]==undefined){
				break;
			}
		}
		if(index==10){
			throw new Error("ws runners are all busy");
			return;
		}
		this.callback_list[index] = callback;
		send = send+index;
	}
	else{
		send = send+"0";
	}
	if(this.check(server)==false){
		this.server.push(server);
		this.data.push(send);
	}
	else{
		if(this.msg<this.max_msg){
			this.com[server].send(send);
			this.msg++;
		}
		else{
			this.server.push(server);
			this.data.push(send);
		}
	}
}
